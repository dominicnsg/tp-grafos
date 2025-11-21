#pragma once
#include "grafoNaoDirecionadoPonderado.h"
#include "UnionFind.h"

class AlgoritmoKruskal {
public:
    // Retorna um grafo contendo a MST
    GrafoNaoDirecionadoPonderado encontrarMST(GrafoNaoDirecionadoPonderado& grafo);
};