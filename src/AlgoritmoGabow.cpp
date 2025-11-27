#include "AlgoritmoGabow.h"
#include <vector>
#include <algorithm>
#include <stack>
#include <limits>

using namespace std;

// Nó do Skew Heap 
struct GabowNode {
    double val;       // Peso da aresta
    double lazy;      // Valor para propagação preguiçosa
    int u, v;         // Origem e Destino
    int idOriginal;   // Índice original
    GabowNode *left, *right;

    GabowNode(double w, int _u, int _v, int _id) 
        : val(w), lazy(0), u(_u), v(_v), idOriginal(_id), left(nullptr), right(nullptr) {}
};

// Gerenciador de memória
static std::vector<GabowNode*> nodePool;

GabowNode* novoNo(double w, int u, int v, int id) {
    GabowNode* node = new GabowNode(w, u, v, id);
    nodePool.push_back(node);
    return node;
}

// Propaga valor lazy
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
    if (a->val > b->val) swap(a, b); 
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


// Estrutura Union-Find
// DSU Local: Optou-se por uma implementacao dedicada ao inves da classe UnionFind.h
// pois o algoritmo de Gabow exige uma politica de uniao especifica (o novo supernoDEVE ser o pai), 
// o que e incompativel com a otimizacao 'Union by Rank' generica.
struct DSU {
    vector<int> pai;
    DSU(int n) {
        pai.resize(n);
        for(int i=0; i<n; ++i) pai[i] = i;
    }
    int find(int i) {
        return (pai[i] == i) ? i : (pai[i] = find(pai[i]));
    }
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if(root_i != root_j) pai[root_i] = root_j;
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

// Algoritmo Principal
GrafoDirecionadoPonderado AlgoritmoGabow::encontrarArborescenciaMinima(GrafoDirecionadoPonderado& grafo, int raiz) {
    int n = grafo.numVertices();
    const auto& todasArestas = grafo.getTodasArestas();
    
    // Limpa memória de execuções anteriores
    for(auto p : nodePool) delete p;
    nodePool.clear();

    // Inicialização das heaps
    vector<GabowNode*> queues(2 * n, nullptr); 
    
    for (size_t i = 0; i < todasArestas.size(); ++i) {
        const auto& aresta = todasArestas[i];
        if (aresta.destino == raiz || aresta.origem == aresta.destino) continue;
        queues[aresta.destino] = gabow_push(queues[aresta.destino], aresta.peso, aresta.origem, aresta.destino, (int)i);
    }

    DSU dsu(2 * n);
    vector<int> estado(2 * n, 0); // 0: novo, 1: ativo, 2: processado
    vector<int> arestaEntradaEscolhida(2 * n, -1); 
    vector<int> paiNaHierarquia(2 * n, -1); 
    stack<CicloInfo> pilhaCiclos;
    
    int numComponentes = n; 

    // Fase de Contração (Path Growing)
    for (int i = 0; i < n; ++i) {
        if (i == raiz) continue; 
        
        int u = dsu.find(i);
        if (estado[u] != 0) continue;

        int curr = u;
        while (estado[curr] != 2) {
            estado[curr] = 1; 

            // Remove auto-loops
            GabowNode* minNode = queues[curr];
            while (minNode && dsu.find(minNode->u) == curr) {
                queues[curr] = gabow_pop(queues[curr]);
                minNode = queues[curr];
            }

            if (!minNode) {
                estado[curr] = 2;
                break;
            }

            // Seleciona provisoriamente esta aresta
            arestaEntradaEscolhida[curr] = minNode->idOriginal;
            int origem = dsu.find(minNode->u);

            if (estado[origem] == 1) {
                // Ciclo detectado
                int novoSuperNo = numComponentes++;
                CicloInfo ciclo;
                ciclo.superNo = novoSuperNo;

                GabowNode* heapUniao = nullptr;
                
                // Funde componentes do ciclo
                int iter = curr;
                while (iter != origem) {
                    int edgeId = arestaEntradaEscolhida[iter];
                    ciclo.componentes.push_back({iter, edgeId});
                    paiNaHierarquia[iter] = novoSuperNo;
                    
                    // Merge do heap com ajuste lazy
                    GabowNode* h = queues[iter];
                    if(h) h->lazy -= todasArestas[edgeId].peso;
                    heapUniao = gabow_merge(heapUniao, h);
                    
                    dsu.unite(iter, novoSuperNo);
                    iter = dsu.find(todasArestas[edgeId].origem);
                }
                
                // Trata o nó de fechamento
                int edgeIdOrigem = minNode->idOriginal;
                ciclo.componentes.push_back({origem, edgeIdOrigem});
                paiNaHierarquia[origem] = novoSuperNo;
                
                GabowNode* h = queues[origem];
                if(h) h->lazy -= todasArestas[edgeIdOrigem].peso;
                heapUniao = gabow_merge(heapUniao, h);
                dsu.unite(origem, novoSuperNo);
                
                pilhaCiclos.push(ciclo);
                queues[novoSuperNo] = heapUniao;
                estado[novoSuperNo] = 1; 
                
                curr = novoSuperNo;
                
            } else {
                curr = origem;
            }
        }
        
        // Marca caminho como processado
        int temp = u;
        while (temp != -1 && estado[temp] == 1) {
            estado[temp] = 2;
            if (temp == raiz || arestaEntradaEscolhida[temp] == -1) break;
            
            int parent = dsu.find(todasArestas[arestaEntradaEscolhida[temp]].origem);
            if (parent == temp) break; 
            temp = parent;
        }
    }

    // Fase de Expansão
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
        
        // Resolve arestas internas
        for (const auto& comp : ciclo.componentes) {
            if (comp.representante == subComponenteEntrada) {
                arestaEntradaEscolhida[comp.representante] = arestaQueEntraNoSuperNo;
            } else {
                arestaEntradaEscolhida[comp.representante] = comp.edgeID;
            }
        }
    }

    // Monta o grafo de resultado
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
