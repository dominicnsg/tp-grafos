#pragma once

#include "grafo.h"

// Grafo Direcionado usando Lista de Adjacências.
class GrafoDirecionado : public IGrafo {
protected:
    int V; 
    std::vector<std::list<int>> adj; 

public:
    explicit GrafoDirecionado(int vertices) : V(vertices) {
        adj.resize(V); 
    }

    ~GrafoDirecionado() override = default;

    void adicionarAresta(int v1, int v2) override {
        if (v1 < V && v2 < V) {
            adj[v1].push_back(v2);
        }
    }

    int numVertices() const override {
        return V;
    }

    const std::list<int>& getVizinhos(int v) const override {
        return adj[v];
    }

    void imprimirGrafo() const override {
        std::cout << "Grafo Direcionado (Lista de Adjacencias):" << std::endl;
        for (int v = 0; v < V; ++v) {
            std::cout << "Vertice " << v << " -> [ ";
            for (int vizinho : adj[v]) {
                std::cout << vizinho << " ";
            }
            std::cout << "]" << std::endl;
        }
    }
};