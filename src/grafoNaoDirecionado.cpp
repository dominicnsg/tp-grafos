#include "grafoNaoDirecionado.h"

GrafoNaoDirecionado::GrafoNaoDirecionado(int vertices) : GrafoDirecionado(vertices) {}

void GrafoNaoDirecionado::adicionarAresta(int v1, int v2)
{
    if (v1 < V && v2 < V)
    {
        adj[v1].push_back(v2);
        adj[v2].push_back(v1);
    }
}

void GrafoNaoDirecionado::imprimirGrafo() const
{
    std::cout << "Grafo Nao-Direcionado (Lista de Adjacencias):\n";
    for (int v = 0; v < V; ++v)
    {
        std::cout << "Vertice " << v << " -> [ ";
        for (int vizinho : adj[v])
            std::cout << vizinho << " ";
        std::cout << "]\n";
    }
}
