#include "AlgoritmoTarjan.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <stack>

using namespace std;

// Gerenciador de memória para os nós da Heap
static std::vector<struct HeapNode*> nodesAllocated;

// Nó da Skew Heap (Fila de Prioridade)
struct HeapNode {
    double val;          // Peso ajustado (chave)
    double lazy;         // Valor para propagação preguiçosa
    int u, v;            // Aresta (origem -> destino)
    int idOriginal;      // ID original da aresta para recuperação
    HeapNode *left, *right;

    HeapNode(double w, int _u, int _v, int _id) 
        : val(w), lazy(0), u(_u), v(_v), idOriginal(_id), left(nullptr), right(nullptr) {
            nodesAllocated.push_back(this);
        }
};

// Estruturas para reconstrução (Expansão dos ciclos) - CRÍTICO PARA A CORREÇÃO
struct ComponenteCiclo {
    int representante; // Qual nó (ou supernó) dentro do ciclo
    int edgeID;        // A aresta interna do ciclo que aponta para ele
};

struct CicloInfo {
    int superNo; 
    vector<ComponenteCiclo> componentes; 
};

// --- FUNÇÕES DE HEAP (SKEW HEAP) ---

void push_lazy(HeapNode* t) {
    if (!t || t->lazy == 0) return;
    t->val += t->lazy;
    if (t->left) t->left->lazy += t->lazy;
    if (t->right) t->right->lazy += t->lazy;
    t->lazy = 0;
}

HeapNode* merge(HeapNode* a, HeapNode* b) {
    push_lazy(a);
    push_lazy(b);
    if (!a) return b;
    if (!b) return a;
    
    if (a->val > b->val) swap(a, b);
    
    swap(a->left, a->right); 
    a->left = merge(b, a->left);
    return a;
}

HeapNode* push(HeapNode* root, double w, int u, int v, int id) {
    return merge(root, new HeapNode(w, u, v, id));
}

HeapNode* pop(HeapNode* root) {
    push_lazy(root);
    return merge(root->left, root->right);
}

// --- UNION-FIND LOCAL ---
// Mantido local para garantir que o representante seja controlado pelo algoritmo
struct DSU {
    vector<int> pai;
    DSU(int n) {
        pai.resize(n);
        for(int i=0; i<n; ++i) pai[i] = i;
    }
    int find(int i) {
        if(pai[i] == i) return i;
        return pai[i] = find(pai[i]);
    }
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if(root_i != root_j) pai[root_i] = root_j;
    }
};

// --- IMPLEMENTAÇÃO PRINCIPAL ---

GrafoDirecionadoPonderado AlgoritmoTarjan::encontrarArborescenciaMinima(GrafoDirecionadoPonderado& grafo, int raiz) {
    int n = grafo.numVertices();
    const auto& todasArestas = grafo.getTodasArestas();
    
    // Limpeza de memória estática
    // Nota: Em um código de produção real, use smart pointers ou pool objects.
    // Aqui usamos vector estático para simplicidade conforme seu modelo.
    nodesAllocated.clear(); 

    // 1. Inicialização
    vector<HeapNode*> heaps(2 * n, nullptr); // 2*n para acomodar supernós
    
    for (size_t i = 0; i < todasArestas.size(); ++i) {
        const auto& aresta = todasArestas[i];
        if (aresta.destino == raiz || aresta.origem == aresta.destino) continue;
        heaps[aresta.destino] = push(heaps[aresta.destino], aresta.peso, aresta.origem, aresta.destino, (int)i);
    }

    DSU dsu(2 * n);
    vector<int> visitado(2 * n, -1);
    // Armazena o ID da aresta escolhida para entrar no componente i
    vector<int> arestaEntradaEscolhida(2 * n, -1); 
    vector<int> paiNaHierarquia(2 * n, -1); // Para rastrear quem está dentro de quem
    stack<CicloInfo> pilhaCiclos;
    
    int numComponentes = n; // IDs para novos supernós

    // 2. Fase de Contração
    // Diferente do Gabow (Path Growing), o Tarjan clássico itera sobre raízes arbitrárias
    for (int i = 0; i < n; ++i) {
        if (i == raiz) continue;

        int curr = dsu.find(i);
        
        // Enquanto não processamos este componente ou ele não tem pai definido
        while (visitado[curr] == -1 && curr != dsu.find(raiz)) {
            visitado[curr] = i; // Marca que estamos visitando 'curr' nesta iteração 'i'

            if (!heaps[curr]) {
                // Componente inalcançável ou esgotado
                break; 
            }

            // Pega a menor aresta de entrada
            HeapNode* minEdge = heaps[curr];
            // Remove auto-loops (arestas dentro do mesmo supernó)
            while (minEdge && dsu.find(minEdge->u) == curr) {
                heaps[curr] = pop(heaps[curr]);
                minEdge = heaps[curr];
            }

            if (!minEdge) break;

            // Escolhemos provisoriamente esta aresta
            arestaEntradaEscolhida[curr] = minEdge->idOriginal;
            
            int origem = dsu.find(minEdge->u);

            if (visitado[origem] == i) {
                // CICLO DETECTADO! (origem foi visitada nesta mesma iteração)
                int novoSuperNo = numComponentes++;
                CicloInfo ciclo;
                ciclo.superNo = novoSuperNo;
                
                HeapNode* heapUniao = nullptr;
                int iter = curr;
                
                // Percorre o ciclo para fundir heaps e salvar info para reconstrução
                while (true) {
                    // Salva info para expansão futura
                    int edgeId = arestaEntradaEscolhida[iter];
                    ciclo.componentes.push_back({iter, edgeId});
                    paiNaHierarquia[iter] = novoSuperNo;

                    // Merge do heap com lazy update
                    HeapNode* h = heaps[iter];
                    if (h) h->lazy -= todasArestas[edgeId].peso;
                    heapUniao = merge(heapUniao, h);

                    dsu.unite(iter, novoSuperNo);

                    if (iter == origem) break;
                    
                    // Avança para o próximo nó "para trás" no ciclo
                    iter = dsu.find(todasArestas[edgeId].origem);
                }

                // Finaliza supernó
                heaps[novoSuperNo] = heapUniao;
                pilhaCiclos.push(ciclo);
                
                // O processamento continua a partir do novo supernó
                curr = novoSuperNo;
                visitado[curr] = -1; // Reset para permitir que o supernó seja parte de ciclos maiores
                
            } else {
                // Não fechou ciclo, apenas avança
                curr = origem;
            }
        }
        
        // Limpeza de marcação (opcional para essa lógica, mas boa prática)
        // No Tarjan clássico, componentes processados são "comprimidos", 
        // mas aqui mantemos simples.
    }

    // 3. Fase de Expansão (Reconstrução da Árvore)
    while (!pilhaCiclos.empty()) {
        CicloInfo ciclo = pilhaCiclos.top();
        pilhaCiclos.pop();
        
        int superNo = ciclo.superNo;
        int arestaQueEntraNoSuperNo = arestaEntradaEscolhida[superNo];
        
        int subComponenteEntrada = -1;
        
        // Descobre qual nó real dentro do supernó recebe a aresta externa
        if (arestaQueEntraNoSuperNo != -1) {
            int destinoReal = todasArestas[arestaQueEntraNoSuperNo].destino;
            int temp = destinoReal;
            
            // Sobe na hierarquia até achar o filho direto deste supernó
            while (paiNaHierarquia[temp] != superNo && paiNaHierarquia[temp] != -1) {
                temp = paiNaHierarquia[temp];
            }
            subComponenteEntrada = temp;
        }
        
        // Distribui as arestas:
        // - O nó que recebe a aresta externa fica com ela.
        // - Todos os outros nós do ciclo ficam com suas arestas internas (do ciclo).
        for (const auto& comp : ciclo.componentes) {
            if (comp.representante == subComponenteEntrada) {
                arestaEntradaEscolhida[comp.representante] = arestaQueEntraNoSuperNo;
            } else {
                arestaEntradaEscolhida[comp.representante] = comp.edgeID;
            }
        }
    }

    // 4. Construção do Grafo Final
    GrafoDirecionadoPonderado resultado(n);
    for (int i = 0; i < n; ++i) {
        if (i == raiz) continue;
        
        int edgeID = arestaEntradaEscolhida[i];
        if (edgeID != -1) {
            const auto& aresta = todasArestas[edgeID];
            resultado.adicionarAresta(aresta.origem, aresta.destino, aresta.peso);
        }
    }

    // Limpeza de memória dos nós alocados
    for(auto p : nodesAllocated) delete p;
    nodesAllocated.clear();

    return resultado;
}
