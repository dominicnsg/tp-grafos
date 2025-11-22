#ifndef ALGORITMOTARJAN_H
#define ALGORITMOTARJAN_H

#include "grafoDirecionadoPonderado.h"
#include <vector>
#include <list>

class AlgoritmoTarjan {
public:
    GrafoDirecionadoPonderado encontrarArborescenciaMinima(GrafoDirecionadoPonderado& grafo, int raiz);
};

#endif