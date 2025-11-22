#include "AlgoritmoGabow.h"
#include <vector>
#include <algorithm>

using namespace std;

// Estrutura Skew Heap
struct GabowNode;
static std::vector<GabowNode*> gabowNodesAllocated;

struct GabowNode {
    double val;
    double lazy;
    int u, v;
    int idOriginal;
    GabowNode *left, *right;

    GabowNode(double w, int _u, int _v, int _id) 
        : val(w), lazy(0), u(_u), v(_v), idOriginal(_id), left(nullptr), right(nullptr) {
            gabowNodesAllocated.push_back(this);
    }
};

// Propaga o valor lazy para os filhos
void gabow_push_lazy(GabowNode* t) {
    if (!t || t->lazy == 0) return;
    t->val += t->lazy;
    if (t->left) t->left->lazy += t->lazy;
    if (t->right) t->right->lazy += t->lazy;
    t->lazy = 0;
}

// Funde duas heaps
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
    return gabow_merge(root, new GabowNode(w, u, v, id));
}

GabowNode* gabow_pop(GabowNode* root) {
    gabow_push_lazy(root);
    return gabow_merge(root->left, root->right);
}

// Union-Find
struct DSU_Gabow {
    vector<int> pai;
    DSU_Gabow(int n) {
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

// Algoritmo Principal
GrafoDirecionadoPonderado AlgoritmoGabow::encontrarArborescenciaMinima(GrafoDirecionadoPonderado& grafo, int raiz) {
    int n = grafo.numVertices();
    gabowNodesAllocated.clear();

    // Inicializa heaps para cada vértice
    vector<GabowNode*> queues(n, nullptr);
    const auto& todasArestas = grafo.getTodasArestas();
    
    vector<Aresta> arestasOriginais = todasArestas; 
    for(size_t i=0; i<arestasOriginais.size(); ++i) arestasOriginais[i].idOriginal = i;

    for (const auto& aresta : arestasOriginais) {
        if (aresta.destino == raiz || aresta.origem == aresta.destino) continue;
        queues[aresta.destino] = gabow_push(queues[aresta.destino], aresta.peso, aresta.origem, aresta.destino, aresta.idOriginal);
    }

    DSU_Gabow dsu(n);
    vector<int> estado(n, 0); // 0: não visitado, 1: visitando, 2: processado
    vector<GabowNode*> arestaEntrada(n, nullptr);
    
    int raiz_set = dsu.find(raiz);
    estado[raiz] = 2; 

    // Processa vértices ainda não incluídos na arborescência
    for (int i = 0; i < n; ++i) {
        if (estado[dsu.find(i)] != 0) continue;

        int curr = dsu.find(i);
        while (estado[curr] != 2) {
            estado[curr] = 1; 
            
            if (!queues[curr]) {
                estado[curr] = 2; 
                break;
            }

            // Seleciona a menor aresta e remove auto-loops
            GabowNode* minEdge = queues[curr];
            while (minEdge && dsu.find(minEdge->u) == dsu.find(minEdge->v)) {
                queues[curr] = gabow_pop(queues[curr]);
                minEdge = queues[curr];
            }

            if (!minEdge) {
                estado[curr] = 2;
                break;
            }

            arestaEntrada[curr] = minEdge;
            int u_origem = dsu.find(minEdge->u);

            if (estado[u_origem] == 1) {
                // Ciclo detectado: contrai o ciclo fundindo as heaps
                int cicloNode = u_origem;
                GabowNode* novaQueue = nullptr;
                
                do {
                    // Ajusta pesos (lazy) e funde as heaps
                    if (queues[cicloNode]) {
                        queues[cicloNode]->lazy -= arestaEntrada[cicloNode]->val;
                    }
                    novaQueue = gabow_merge(novaQueue, queues[cicloNode]);
                    
                    int next = dsu.find(arestaEntrada[cicloNode]->u);
                    dsu.unite(cicloNode, u_origem);
                    cicloNode = next;
                } while (cicloNode != u_origem); 
                
                // Atualiza a heap do novo super-nó
                int superNo = dsu.find(u_origem);
                queues[superNo] = novaQueue;
                
                curr = superNo;
            } else {
                // Sem ciclo, segue o caminho
                curr = u_origem;
            }
        }
        
        // Marca caminho como processado
        int temp = dsu.find(i);
        while(estado[temp] == 1) {
            estado[temp] = 2;
            if(arestaEntrada[temp]) temp = dsu.find(arestaEntrada[temp]->u);
            else break;
        }
    }

    // Reconstrói o grafo com as arestas selecionadas
    GrafoDirecionadoPonderado resultado(n);
    for(int i=0; i<n; ++i) {
        if (i == raiz) continue;

        if (arestaEntrada[i]) {
             int u = arestaEntrada[i]->u;
             int v = arestaEntrada[i]->v;
             double w = arestasOriginais[arestaEntrada[i]->idOriginal].peso;
             resultado.adicionarAresta(u, v, w);
        }
    }

    // Limpeza de memória
    for(auto node : gabowNodesAllocated) delete node;
    gabowNodesAllocated.clear();
    
    return resultado;
}