#include "AlgoritmoTarjan.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <memory>

using namespace std;

// Estruturas internas
static std::vector<struct HeapNode*> nodesAllocated;

// Nó da Skew Heap (Fila de Prioridade)
struct HeapNode {
    double val;          // Peso ajustado
    double lazy;         // Valor para propagação preguiçosa
    int u, v;            // Aresta
    int idOriginal;      // Referência para a aresta original
    HeapNode *left, *right;

    HeapNode(double w, int _u, int _v, int _id) 
        : val(w), lazy(0), u(_u), v(_v), idOriginal(_id), left(nullptr), right(nullptr) {
            nodesAllocated.push_back(this);
        }
};

// Aplica o valor lazy nos nós
void push_lazy(HeapNode* t) {
    if (!t || t->lazy == 0) return;
    t->val += t->lazy;
    if (t->left) t->left->lazy += t->lazy;
    if (t->right) t->right->lazy += t->lazy;
    t->lazy = 0;
}

// Funde duas heaps mantendo a propriedade de min-heap
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

// Union-Find
// DSU Local: Assim como no algoritmo de Gabow, optou-se por uma DSU dedicada.
// O gerenciamento das filas de prioridade (vetor 'heaps') depende estritamente
// do identificador do representante do conjunto. O uso de Union-By-Rank generico
// poderia alterar o representante imprevisivelmente, quebrando o mapeamento das heaps.
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
    
    // Inicialização das Heaps
    vector<HeapNode*> heaps(n, nullptr);
    const auto& todasArestas = grafo.getTodasArestas();
    
    vector<Aresta> arestasOriginais = todasArestas; 
    for(size_t i=0; i<arestasOriginais.size(); ++i) arestasOriginais[i].idOriginal = i;

    // Preenche heaps ignorando loops e arestas para a raiz
    for (const auto& aresta : arestasOriginais) {
        if (aresta.destino == raiz) continue; 
        if (aresta.origem == aresta.destino) continue; 
        
        heaps[aresta.destino] = push(heaps[aresta.destino], aresta.peso, aresta.origem, aresta.destino, aresta.idOriginal);
    }

    DSU dsu(n);
    vector<int> visitado(n, -1);
    vector<HeapNode*> arestaEscolhida(n, nullptr);
    
    int raiz_dsu = dsu.find(raiz);
    visitado[raiz] = raiz; 

    // Fase de Contração
    for (int i = 0; i < n; ++i) {
        if (visitado[dsu.find(i)] != -1) continue;

        int curr = i;
        while (visitado[dsu.find(curr)] == -1) {
            int u = dsu.find(curr);
            visitado[u] = i; 

            if (!heaps[u]) {
                break; // Componente sem entrada
            }

            // Seleciona a menor aresta e remove auto-loops
            HeapNode* minEdge = heaps[u];
            while (minEdge && dsu.find(minEdge->u) == dsu.find(minEdge->v)) {
                heaps[u] = pop(heaps[u]);
                minEdge = heaps[u];
            }
            
            if (!minEdge) break; 

            arestaEscolhida[u] = minEdge;
            
            int v_origem = dsu.find(minEdge->u);
            
            // Verifica se formou ciclo
            if (visitado[v_origem] == i) {
                // Ciclo encontrado: contração
                int cicloNode = v_origem;
                HeapNode* novaHeap = nullptr;
                
                // Funde as heaps do ciclo
                while (true) {
                    if (heaps[cicloNode]) {
                        heaps[cicloNode]->lazy -= arestaEscolhida[cicloNode]->val;
                    }
                    
                    novaHeap = merge(novaHeap, heaps[cicloNode]);
                    
                    int proximo = dsu.find(arestaEscolhida[cicloNode]->u);
                    if (cicloNode == v_origem && proximo == v_origem) break; 
                    
                    dsu.unite(cicloNode, v_origem); 
                    
                    if (dsu.find(proximo) == dsu.find(v_origem)) break; 
                    cicloNode = proximo;
                }
                
                // Atualiza o super-nó
                int superNo = dsu.find(v_origem);
                heaps[superNo] = novaHeap;
                
                curr = superNo;
                visitado[superNo] = -1; // Permite revisitar o super-nó
            } else {
                // Sem ciclo, avança
                curr = v_origem;
            }
        }
    }

    // Fase de Construção do Resultado
    GrafoDirecionadoPonderado resultado(n);
    
    for(int i=0; i<n; ++i) {
        if (i == raiz) continue;
        
        // Adiciona as arestas selecionadas nos heaps finais
        if (arestaEscolhida[i]) {
             int u = arestaEscolhida[i]->u;
             int v = arestaEscolhida[i]->v;
             double w = arestasOriginais[arestaEscolhida[i]->idOriginal].peso;
             
             resultado.adicionarAresta(u, v, w);
        }
    }
    
    // Limpeza de memória
    for(auto node : nodesAllocated) {
        delete node;
    }
    nodesAllocated.clear();

    return resultado;
}
