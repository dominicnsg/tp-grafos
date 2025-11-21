#ifndef GRAFONAO_DIRECIONADO_PONDERADO_H
#define GRAFONAO_DIRECIONADO_PONDERADO_H

#include "grafoDirecionadoPonderado.h"

class GrafoNaoDirecionadoPonderado : public GrafoDirecionadoPonderado {
public:
    explicit GrafoNaoDirecionadoPonderado(int vertices);

    using GrafoDirecionadoPonderado::adicionarAresta; 

    void adicionarAresta(int v1, int v2, double peso);
    void imprimirGrafo() const override;
};

#endif