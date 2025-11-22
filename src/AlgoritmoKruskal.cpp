#include "AlgoritmoKruskal.h"
#include <algorithm>
#include <vector>

GrafoNaoDirecionadoPonderado AlgoritmoKruskal::encontrarMST(GrafoNaoDirecionadoPonderado& grafo) {
    int V = grafo.numVertices();
    GrafoNaoDirecionadoPonderado mst(V);
    
    // Obtém todas as arestas
    std::vector<Aresta> arestas = grafo.getTodasArestas();

    // Ordena as arestas por peso crescente
    std::sort(arestas.begin(), arestas.end(), [](const Aresta& a, const Aresta& b) {
        return a.peso < b.peso;
    });

    // Inicializa Union-Find
    UnionFind uf(V);
    int arestasAdicionadas = 0;

    for (const auto& aresta : arestas) {
        // Verifica se a aresta forma ciclo
        if (uf.find(aresta.origem) != uf.find(aresta.destino)) {
            uf.unite(aresta.origem, aresta.destino);
            mst.adicionarAresta(aresta.origem, aresta.destino, aresta.peso);
            arestasAdicionadas++;
        }
    }

    return mst;
}