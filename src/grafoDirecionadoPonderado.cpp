#include "grafoDirecionadoPonderado.h"
#include <iostream>

GrafoDirecionadoPonderado::GrafoDirecionadoPonderado(int vertices) 
    : GrafoDirecionado(vertices) {
    adjPeso.resize(V);
    adjEntrada.resize(V);
}

void GrafoDirecionadoPonderado::adicionarAresta(int v1, int v2, double peso) {
    if (v1 < V && v2 < V) {
        adjPeso[v1].emplace_back(v2, peso);
        
        Aresta novaAresta(v1, v2, peso);
        adjEntrada[v2].push_back(novaAresta);
        todasArestas.push_back(novaAresta);
    }
}

void GrafoDirecionadoPonderado::imprimirGrafo() const {
    std::cout << "Grafo Direcionado Ponderado (Lista de Adjacencias):\n";
    for (int v = 0; v < V; ++v) {
        std::cout << "Vertice " << v << " -> [ ";
        for (const auto& par : adjPeso[v]) {
            int vizinho = par.first;
            double peso = par.second;
            std::cout << "(" << vizinho << ", " << peso << ") ";
        }
        std::cout << "]\n";
    }
}

const std::list<Aresta>& GrafoDirecionadoPonderado::getArestasEntrada(int v) const {
    return adjEntrada[v];
}

const std::vector<Aresta>& GrafoDirecionadoPonderado::getTodasArestas() const {
    return todasArestas;
}

void GrafoDirecionadoPonderado::removerArestasEntrada(int v) {
    if(v < V) {
        adjEntrada[v].clear();
    }
}