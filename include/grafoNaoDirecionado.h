#pragma once

#include "grafoDirecionado.h"

// unica mudança é na lógica de adicionar arestas.
class GrafoNaoDirecionado : public GrafoDirecionado {
public:
    explicit GrafoNaoDirecionado(int vertices) : GrafoDirecionado(vertices) {}

    void adicionarAresta(int v1, int v2) override {
        if (v1 < V && v2 < V) {
            adj[v1].push_back(v2);
            adj[v2].push_back(v1);
        }
    }

    void imprimirGrafo() const override {
        std::cout << "Grafo Nao-Direcionado (Lista de Adjacencias):" << std::endl;
        GrafoDirecionado::imprimirGrafo();
    }
};