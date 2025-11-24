#ifndef ALGORITMOGABOW_H
#define ALGORITMOGABOW_H

#include "grafoDirecionadoPonderado.h"

class AlgoritmoGabow {
public:
    GrafoDirecionadoPonderado encontrarArborescenciaMinima(GrafoDirecionadoPonderado& grafo, int raiz);
};

#endif