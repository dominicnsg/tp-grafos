#ifndef ALGORITMOGABOW_H
#define ALGORITMOGABOW_H

#include "grafoDirecionadoPonderado.h"

class AlgoritmoGabow {
public:
    // Implementação baseada em Heaps Mescláveis (Skew Heaps)
    GrafoDirecionadoPonderado encontrarArborescenciaMinima(GrafoDirecionadoPonderado& grafo, int raiz);
};

#endif