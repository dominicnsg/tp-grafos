#include "grafoNaoDirecionadoPonderado.h"
#include <iostream>

GrafoNaoDirecionadoPonderado::GrafoNaoDirecionadoPonderado(int vertices)
    : GrafoDirecionadoPonderado(vertices) {}

void GrafoNaoDirecionadoPonderado::adicionarAresta(int v1, int v2, double peso)
{
    if (v1 < V && v2 < V)
    {
        adjPeso[v1].emplace_back(v2, peso);
        adjPeso[v2].emplace_back(v1, peso);
    }
}

void GrafoNaoDirecionadoPonderado::imprimirGrafo() const
{
    std::cout << "Grafo Direcionado Ponderado (Lista de Adjacencias):\n";
    for (int v = 0; v < V; ++v)
    {
        std::cout << "Vertice " << v << " -> [ ";
        for (const auto &par : adjPeso[v])
        {
            int vizinho = par.first;
            double peso = par.second;
            std::cout << "(" << vizinho << ", " << peso << ") ";
        }
        std::cout << "]\n";
    }
}