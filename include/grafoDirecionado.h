#pragma once
#include "grafo.h"

class GrafoDirecionado : public IGrafo {
protected:
    int V;
    std::vector<std::list<int>> adj;

public:
    explicit GrafoDirecionado(int vertices);
    ~GrafoDirecionado() override;

    void adicionarAresta(int v1, int v2) override;
    int numVertices() const override;
    const std::list<int>& getVizinhos(int v) const override;
    void imprimirGrafo() const override;
};
