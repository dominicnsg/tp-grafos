#include "grafoDirecionado.h"

GrafoDirecionado::GrafoDirecionado(int vertices) : V(vertices)
{
    adj.resize(V);
}

GrafoDirecionado::~GrafoDirecionado() = default;

void GrafoDirecionado::adicionarAresta(int v1, int v2)
{
    if (v1 < V && v2 < V)
        adj[v1].push_back(v2);
}

int GrafoDirecionado::numVertices() const
{
    return V;
}

const std::list<int> &GrafoDirecionado::getVizinhos(int v) const
{
    return adj[v];
}

void GrafoDirecionado::imprimirGrafo() const
{
    std::cout << "Grafo Direcionado (Lista de Adjacencias):\n";
    for (int v = 0; v < V; ++v)
    {
        std::cout << "Vertice " << v << " -> [ ";
        for (int vizinho : adj[v])
            std::cout << vizinho << " ";
        std::cout << "]\n";
    }
}
