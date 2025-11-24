#include "AlgoritmoGabow.h"
#include <vector>
#include <algorithm>
#include <stack>
#include <queue>
#include <limits>

using namespace std;

// Aresta para uso na Priority Queue
struct ArestaGabow {
    double peso;
    int u, v;       
    int idOriginal; 

    // Operador invertido para funcionar como Min-Heap
    bool operator>(const ArestaGabow& outra) const {
        return peso > outra.peso;
    }
};

// Heap baseada em STL com suporte a Lazy Update
struct HeapSTL {
    priority_queue<ArestaGabow, vector<ArestaGabow>, greater<ArestaGabow>> pq;
    double lazyOffset = 0; 

    bool empty() const { return pq.empty(); }

    ArestaGabow top() {
        ArestaGabow a = pq.top();
        a.peso += lazyOffset; // Reconstrói o valor real
        return a;
    }

    void pop() {
        pq.pop();
    }

    void push(ArestaGabow a) {
        a.peso -= lazyOffset; 
        pq.push(a);
    }

    // Fusão Small-to-Large (menor para maior)
    void merge(HeapSTL& other) {
        if (other.pq.size() > pq.size()) {
            swap(pq, other.pq);
            swap(lazyOffset, other.lazyOffset);
        }
    
        while (!other.pq.empty()) {
            ArestaGabow a = other.top(); 
            other.pop();
            push(a); 
        }
    }
};

// Estrutura Union-Find
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

// Estruturas de controle de ciclos
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
    
    // Inicializa heaps para cada vértice (e espaços extras para supernós)
    vector<HeapSTL*> queues(2 * n);
    for(int i = 0; i < 2*n; ++i) queues[i] = new HeapSTL();

    for (size_t i = 0; i < todasArestas.size(); ++i) {
        const auto& aresta = todasArestas[i];
        if (aresta.destino == raiz || aresta.origem == aresta.destino) continue;
        queues[aresta.destino]->push({aresta.peso, aresta.origem, aresta.destino, (int)i});
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
            while (!queues[curr]->empty()) {
                ArestaGabow top = queues[curr]->top();
                if (dsu.find(top.u) == curr) {
                    queues[curr]->pop();
                } else {
                    break;
                }
            }

            if (queues[curr]->empty()) {
                estado[curr] = 2;
                break;
            }

            ArestaGabow minAresta = queues[curr]->top();
            arestaEntradaEscolhida[curr] = minAresta.idOriginal;
            
            int origem = dsu.find(minAresta.u);

            if (estado[origem] == 1) {
                // Ciclo detectado
                int novoSuperNo = numComponentes++;
                CicloInfo ciclo;
                ciclo.superNo = novoSuperNo;

                // Funde componentes do ciclo
                int iter = curr;
                while (iter != origem) {
                    int edgeId = arestaEntradaEscolhida[iter];
                    ciclo.componentes.push_back({iter, edgeId});
                    paiNaHierarquia[iter] = novoSuperNo;
                    
                    // Atualiza Lazy e funde heaps
                    queues[iter]->lazyOffset -= todasArestas[edgeId].peso;
                    queues[novoSuperNo]->merge(*queues[iter]);
                    
                    dsu.unite(iter, novoSuperNo);
                    iter = dsu.find(todasArestas[edgeId].origem);
                }

                // Trata o nó de fechamento
                int edgeIdOrigem = minAresta.idOriginal;
                ciclo.componentes.push_back({origem, edgeIdOrigem});
                paiNaHierarquia[origem] = novoSuperNo;
                
                queues[origem]->lazyOffset -= todasArestas[edgeIdOrigem].peso;
                queues[novoSuperNo]->merge(*queues[origem]);
                dsu.unite(origem, novoSuperNo);

                pilhaCiclos.push(ciclo);
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
        int arestaExterna = arestaEntradaEscolhida[superNo];
        int subComponenteEntrada = -1;
        
        if (arestaExterna != -1) {
            int destino = todasArestas[arestaExterna].destino;
            int temp = destino;
            while (paiNaHierarquia[temp] != superNo && paiNaHierarquia[temp] != -1) {
                temp = paiNaHierarquia[temp];
            }
            subComponenteEntrada = temp;
        }
        
        for (const auto& comp : ciclo.componentes) {
            if (comp.representante == subComponenteEntrada) {
                arestaEntradaEscolhida[comp.representante] = arestaExterna;
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
    
    // Limpeza de memória
    for(auto p : queues) delete p;

    return resultado;
}