#ifndef ALGORITMOEDMONDS_H
#define ALGORITMOEDMONDS_H

#include "grafoDirecionadoPonderado.h"
#include <vector>
#include <list>

class AlgoritmoEdmonds {
public:
    GrafoDirecionadoPonderado encontrarArborescenciaMinima(GrafoDirecionadoPonderado& grafo, int raiz);

private:

    std::list<Aresta> edmondsRecursivo(const std::vector<int>& verticesAtuais, 
                                       const std::vector<Aresta>& arestasDisponiveis, 
                                       int raizAtual);
};

#endif