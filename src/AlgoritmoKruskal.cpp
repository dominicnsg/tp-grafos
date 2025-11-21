#include "AlgoritmoKruskal.h"
#include <algorithm>
#include <vector>

GrafoNaoDirecionadoPonderado AlgoritmoKruskal::encontrarMST(GrafoNaoDirecionadoPonderado& grafo) {
    int V = grafo.numVertices();
    GrafoNaoDirecionadoPonderado mst(V);
    
    // 1. Pegar todas as arestas
    // Nota: Como o grafo é não direcionado, precisamos cuidar para não pegar duplicatas (0-1 e 1-0)
    // Mas para Kruskal simples, se tiver duplicada o UnionFind descarta, então tudo bem.
    std::vector<Aresta> arestas = grafo.getTodasArestas();

    // 2. Ordenar arestas por peso (Crescente)
    std::sort(arestas.begin(), arestas.end(), [](const Aresta& a, const Aresta& b) {
        return a.peso < b.peso;
    });

    // 3. Union-Find
    UnionFind uf(V);
    int arestasAdicionadas = 0;

    for (const auto& aresta : arestas) {
        if (uf.find(aresta.origem) != uf.find(aresta.destino)) {
            uf.unite(aresta.origem, aresta.destino);
            mst.adicionarAresta(aresta.origem, aresta.destino, aresta.peso);
            arestasAdicionadas++;
        }
    }

    return mst;
}