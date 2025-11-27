#include "AlgoritmoTarjan.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <stack>

using namespace std;

// Gerenciador de memória
static std::vector<struct HeapNode*> nodesAllocated;

// Nó da Skew Heap
struct HeapNode {
    double val;          // Peso ajustado
    double lazy;         // Valor para propagação preguiçosa
    int u, v;            // Aresta
    int idOriginal;      // ID original para recuperação
    HeapNode *left, *right;

    HeapNode(double w, int _u, int _v, int _id) 
        : val(w), lazy(0), u(_u), v(_v), idOriginal(_id), left(nullptr), right(nullptr) {
            nodesAllocated.push_back(this);
        }
};

// Estruturas para reconstrução
struct ComponenteCiclo {
    int representante; 
    int edgeID;        
};

struct CicloInfo {
    int superNo; 
    vector<ComponenteCiclo> componentes; 
};

// Funções de Heap (Skew Heap)

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

// Union-Find Local
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

// Implementação Principal
GrafoDirecionadoPonderado AlgoritmoTarjan::encontrarArborescenciaMinima(GrafoDirecionadoPonderado& grafo, int raiz) {
    int n = grafo.numVertices();
    const auto& todasArestas = grafo.getTodasArestas();
    
    // Limpeza de memória
    nodesAllocated.clear(); 

    // Inicialização das heaps
    vector<HeapNode*> heaps(2 * n, nullptr); 
    
    for (size_t i = 0; i < todasArestas.size(); ++i) {
        const auto& aresta = todasArestas[i];
        if (aresta.destino == raiz || aresta.origem == aresta.destino) continue;
        heaps[aresta.destino] = push(heaps[aresta.destino], aresta.peso, aresta.origem, aresta.destino, (int)i);
    }

    DSU dsu(2 * n);
    vector<int> visitado(2 * n, -1);
    vector<int> arestaEntradaEscolhida(2 * n, -1); 
    vector<int> paiNaHierarquia(2 * n, -1); 
    stack<CicloInfo> pilhaCiclos;
    
    int numComponentes = n; 

    // Fase de Contração
    for (int i = 0; i < n; ++i) {
        if (i == raiz) continue;

        int curr = dsu.find(i);
        
        // Processa componentes ainda não resolvidos
        while (visitado[curr] == -1 && curr != dsu.find(raiz)) {
            visitado[curr] = i; 

            if (!heaps[curr]) {
                break; // Componente inalcançável
            }

            // Remove auto-loops
            HeapNode* minEdge = heaps[curr];
            while (minEdge && dsu.find(minEdge->u) == curr) {
                heaps[curr] = pop(heaps[curr]);
                minEdge = heaps[curr];
            }

            if (!minEdge) break;

            // Seleciona provisoriamente esta aresta
            arestaEntradaEscolhida[curr] = minEdge->idOriginal;
            
            int origem = dsu.find(minEdge->u);

            if (visitado[origem] == i) {
                // Ciclo detectado
                int novoSuperNo = numComponentes++;
                CicloInfo ciclo;
                ciclo.superNo = novoSuperNo;
                
                HeapNode* heapUniao = nullptr;
                int iter = curr;
                
                // Funde heaps do ciclo e salva info
                while (true) {
                    int edgeId = arestaEntradaEscolhida[iter];
                    ciclo.componentes.push_back({iter, edgeId});
                    paiNaHierarquia[iter] = novoSuperNo;

                    // Merge com lazy update
                    HeapNode* h = heaps[iter];
                    if (h) h->lazy -= todasArestas[edgeId].peso;
                    heapUniao = merge(heapUniao, h);

                    dsu.unite(iter, novoSuperNo);

                    if (iter == origem) break;
                    
                    // Avança no ciclo
                    iter = dsu.find(todasArestas[edgeId].origem);
                }

                // Finaliza supernó
                heaps[novoSuperNo] = heapUniao;
                pilhaCiclos.push(ciclo);
                
                curr = novoSuperNo;
                visitado[curr] = -1; // Permite revisitar o supernó
                
            } else {
                // Sem ciclo, avança
                curr = origem;
            }
        }
    }

    // Fase de Expansão
    while (!pilhaCiclos.empty()) {
        CicloInfo ciclo = pilhaCiclos.top();
        pilhaCiclos.pop();
        
        int superNo = ciclo.superNo;
        int arestaQueEntraNoSuperNo = arestaEntradaEscolhida[superNo];
        
        int subComponenteEntrada = -1;
        
        // Encontra o sub-componente real dentro do super-nó
        if (arestaQueEntraNoSuperNo != -1) {
            int destinoReal = todasArestas[arestaQueEntraNoSuperNo].destino;
            int temp = destinoReal;
            
            while (paiNaHierarquia[temp] != superNo && paiNaHierarquia[temp] != -1) {
                temp = paiNaHierarquia[temp];
            }
            subComponenteEntrada = temp;
        }
        
        // Distribui as arestas internas e externas
        for (const auto& comp : ciclo.componentes) {
            if (comp.representante == subComponenteEntrada) {
                arestaEntradaEscolhida[comp.representante] = arestaQueEntraNoSuperNo;
            } else {
                arestaEntradaEscolhida[comp.representante] = comp.edgeID;
            }
        }
    }

    // Construção do Grafo Final
    GrafoDirecionadoPonderado resultado(n);
    for (int i = 0; i < n; ++i) {
        if (i == raiz) continue;
        
        int edgeID = arestaEntradaEscolhida[i];
        if (edgeID != -1) {
            const auto& aresta = todasArestas[edgeID];
            resultado.adicionarAresta(aresta.origem, aresta.destino, aresta.peso);
        }
    }

    // Limpeza de memória
    for(auto p : nodesAllocated) delete p;
    nodesAllocated.clear();

    return resultado;
}
