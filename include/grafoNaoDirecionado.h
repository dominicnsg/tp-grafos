#pragma once
#include "grafoDirecionado.h"

class GrafoNaoDirecionado : public GrafoDirecionado {
public:
    explicit GrafoNaoDirecionado(int vertices);
    void adicionarAresta(int v1, int v2) override;
    void imprimirGrafo() const override;
};
