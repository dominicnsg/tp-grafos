#include "AlgoritmoGabow.h"
#include <vector>
#include <algorithm>
#include <stack>
#include <limits>

using namespace std;

// --- ESTRUTURAS AUXILIARES ---

// Nó do Skew Heap 
struct GabowNode {
    double val;       // Peso da aresta (pode sofrer lazy update)
    double lazy;      // Valor a ser subtraído dos filhos (lazy propagation)
    int u, v;         // Origem e Destino da aresta
    int idOriginal;   // Índice da aresta na lista original
    GabowNode *left, *right;

    GabowNode(double w, int _u, int _v, int _id) 
        : val(w), lazy(0), u(_u), v(_v), idOriginal(_id), left(nullptr), right(nullptr) {}
};

// Gerenciador de memória simples para evitar leaks durante a execução
static std::vector<GabowNode*> nodePool;

GabowNode* novoNo(double w, int u, int v, int id) {
    GabowNode* node = new GabowNode(w, u, v, id);
    nodePool.push_back(node);
    return node;
}

// Propagação do valor 'lazy'
void gabow_push_lazy(GabowNode* t) {
    if (!t || t->lazy == 0) return;
    t->val += t->lazy;
    if (t->left) t->left->lazy += t->lazy;
    if (t->right) t->right->lazy += t->lazy;
    t->lazy = 0;
}

// Fusão de dois Skew Heaps
GabowNode* gabow_merge(GabowNode* a, GabowNode* b) {
    gabow_push_lazy(a);
    gabow_push_lazy(b);
    if (!a) return b;
    if (!b) return a;
    if (a->val > b->val) swap(a, b); // Mantém a propriedade de Min-Heap
    swap(a->left, a->right);
    a->left = gabow_merge(b, a->left);
    return a;
}

GabowNode* gabow_push(GabowNode* root, double w, int u, int v, int id) {
    return gabow_merge(root, novoNo(w, u, v, id));
}

GabowNode* gabow_pop(GabowNode* root) {
    gabow_push_lazy(root);
    return gabow_merge(root->left, root->right);
}

// Estrutura Union-Find (DSU) para gerenciar componentes e super-nós
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

// Estruturas para reconstrução (Expansão dos ciclos)
struct ComponenteCiclo {
    int representante; // O representante DSU deste sub-componente no momento da contração
    int edgeID;        // A aresta interna do ciclo que entra neste componente
};

struct CicloInfo {
    int superNo; // O ID do novo super-nó criado
    vector<ComponenteCiclo> componentes; // Lista dos componentes que formaram o ciclo
};

// --- ALGORITMO PRINCIPAL ---

GrafoDirecionadoPonderado AlgoritmoGabow::encontrarArborescenciaMinima(GrafoDirecionadoPonderado& grafo, int raiz) {
    int n = grafo.numVertices();
    const auto& todasArestas = grafo.getTodasArestas();
    
    // Limpeza de memória estática de execuções anteriores
    for(auto p : nodePool) delete p;
    nodePool.clear();

    // 1. Inicialização
    vector<GabowNode*> queues(2 * n, nullptr); // 2*n pois criaremos super-nós
    
    for (size_t i = 0; i < todasArestas.size(); ++i) {
        const auto& aresta = todasArestas[i];
        if (aresta.destino == raiz || aresta.origem == aresta.destino) continue;
        queues[aresta.destino] = gabow_push(queues[aresta.destino], aresta.peso, aresta.origem, aresta.destino, (int)i);
    }

    DSU dsu(2 * n);
    vector<int> estado(2 * n, 0); // 0: não visitado, 1: visitando (no caminho), 2: processado
    vector<int> arestaEntradaEscolhida(2 * n, -1); // ID da aresta escolhida para entrar no componente i
    vector<int> paiNaHierarquia(2 * n, -1); // Para rastrear a árvore de super-nós na expansão
    stack<CicloInfo> pilhaCiclos;
    
    int numComponentes = n; // Contador para gerar IDs de super-nós

    // 2. Fase de Contração (Path Growing)
    for (int i = 0; i < n; ++i) {
        if (i == raiz) continue; 
        
        int u = dsu.find(i);
        if (estado[u] != 0) continue;

        int curr = u;
        while (estado[curr] != 2) {
            estado[curr] = 1; // Marcado como 'no caminho atual'

            // Obter a aresta de menor custo entrando em 'curr' (vinda de fora do componente)
            GabowNode* minNode = queues[curr];
            while (minNode && dsu.find(minNode->u) == curr) {
                // Remove self-loops (arestas dentro do mesmo super-nó)
                queues[curr] = gabow_pop(queues[curr]);
                minNode = queues[curr];
            }

            if (!minNode) {
                // Sem arestas de entrada, não é possível conectar este componente
                estado[curr] = 2;
                break;
            }

            // Escolhemos provisoriamente esta aresta
            arestaEntradaEscolhida[curr] = minNode->idOriginal;
            int origem = dsu.find(minNode->u);

            if (estado[origem] == 1) {
                // CICLO DETECTADO! (origem já está no caminho atual)
                int novoSuperNo = numComponentes++;
                CicloInfo ciclo;
                ciclo.superNo = novoSuperNo;

                GabowNode* heapUniao = nullptr;
                
                int iter = curr;
                while (iter != origem) {
                    // A aresta que entra em 'iter'
                    int edgeId = arestaEntradaEscolhida[iter];
                    ciclo.componentes.push_back({iter, edgeId});
                    paiNaHierarquia[iter] = novoSuperNo;
                    
                    // Merge do heap, aplicando o ajuste lazy (w' = w - w_ciclo)
                    GabowNode* h = queues[iter];
                    if(h) h->lazy -= todasArestas[edgeId].peso;
                    heapUniao = gabow_merge(heapUniao, h);
                    
                    dsu.unite(iter, novoSuperNo);
                    
                    iter = dsu.find(todasArestas[edgeId].origem);
                }
                
                int edgeIdOrigem = minNode->idOriginal;
                ciclo.componentes.push_back({origem, edgeIdOrigem});
                paiNaHierarquia[origem] = novoSuperNo;
                
                GabowNode* h = queues[origem];
                if(h) h->lazy -= todasArestas[edgeIdOrigem].peso;
                heapUniao = gabow_merge(heapUniao, h);
                dsu.unite(origem, novoSuperNo);
                
                // Finaliza criação do super-nó
                pilhaCiclos.push(ciclo);
                queues[novoSuperNo] = heapUniao;
                estado[novoSuperNo] = 1; // Super-nó continua no caminho ativo
                
                // O próximo passo do loop continua a partir do super-nó
                curr = novoSuperNo;
                
            } else {
                // Não fechou ciclo, apenas estende o caminho
                curr = origem;
            }
        }
        
        // Backtracking para marcar como processado (estado 2)
        int temp = u;
        while (temp != -1 && estado[temp] == 1) {
            estado[temp] = 2;
            if (temp == raiz || arestaEntradaEscolhida[temp] == -1) break;
            
            int parent = dsu.find(todasArestas[arestaEntradaEscolhida[temp]].origem);
            if (parent == temp) break; 
            temp = parent;
        }
    }

    // 3. Fase de Expansão (Reconstrução)
    while (!pilhaCiclos.empty()) {
        CicloInfo ciclo = pilhaCiclos.top();
        pilhaCiclos.pop();
        
        int superNo = ciclo.superNo;
        int arestaQueEntraNoSuperNo = arestaEntradaEscolhida[superNo];
        
        int subComponenteEntrada = -1;
        
        if (arestaQueEntraNoSuperNo != -1) {
            int destinoReal = todasArestas[arestaQueEntraNoSuperNo].destino;
            
            int temp = destinoReal;
            while (paiNaHierarquia[temp] != superNo && paiNaHierarquia[temp] != -1) {
                temp = paiNaHierarquia[temp];
            }
            subComponenteEntrada = temp;
        }
        
        // Resolve as arestas para os componentes internos
        for (const auto& comp : ciclo.componentes) {
            if (comp.representante == subComponenteEntrada) {
                arestaEntradaEscolhida[comp.representante] = arestaQueEntraNoSuperNo;
            } else {
                arestaEntradaEscolhida[comp.representante] = comp.edgeID;
            }
        }
    }

    // 4. Construção do Grafo Resultado
    GrafoDirecionadoPonderado resultado(n);
    for (int i = 0; i < n; ++i) {
        if (i == raiz) continue;
        int edgeID = arestaEntradaEscolhida[i];
        if (edgeID != -1) {
            const auto& aresta = todasArestas[edgeID];
            resultado.adicionarAresta(aresta.origem, aresta.destino, aresta.peso);
        }
    }

    return resultado;
}