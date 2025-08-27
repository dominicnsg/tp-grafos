#pragma once
#include "grafoDirecionado.h"
#include <utility> // std::pair

//TODO TIRAR NOMESPACE

class GrafoDirecionadoPonderado : public GrafoDirecionado {
protected:
    // Lista de adjacências com peso: (vizinhos, peso)
    std::vector<std::list<std::pair<int, double>>> adjPeso;

public:
    explicit GrafoDirecionadoPonderado(int vertices);

    void adicionarAresta(int v1, int v2, double peso);
    void imprimirGrafo() const override;
};
