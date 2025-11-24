#ifndef ALGORITMOGABOW_H
#define ALGORITMOGABOW_H

#include "grafoDirecionadoPonderado.h"

class AlgoritmoGabow {
public:
    // Implementação do Algoritmo de Gabow (versão eficiente de Edmonds/Chu-Liu)
    // Utiliza Skew Heaps para seleção de arestas e DSU para contração de ciclos.
    GrafoDirecionadoPonderado encontrarArborescenciaMinima(GrafoDirecionadoPonderado& grafo, int raiz);
};

#endif