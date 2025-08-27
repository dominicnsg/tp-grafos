#pragma once
#include "grafoDirecionadoPonderado.h"

class GrafoNaoDirecionadoPonderado : public GrafoDirecionadoPonderado {
public:
    explicit GrafoNaoDirecionadoPonderado(int vertices);

    void adicionarAresta(int v1, int v2, double peso);
    void imprimirGrafo() const override;
};
