#ifndef GRAFODIRECIONADOPONDERADO_H
#define GRAFODIRECIONADOPONDERADO_H

#include "grafoDirecionado.h"
#include <vector>
#include <list>
#include <utility>

struct Aresta {
    int origem;
    int destino;
    double peso;
    int idOriginal; 
    
    Aresta(int u, int v, double p, int id = -1) 
        : origem(u), destino(v), peso(p), idOriginal(id) {}
};

class GrafoDirecionadoPonderado : public GrafoDirecionado
{
protected:
    std::vector<std::list<std::pair<int, double>>> adjPeso;
    std::vector<std::list<Aresta>> adjEntrada;
    std::vector<Aresta> todasArestas;

public:
    GrafoDirecionadoPonderado(int vertices);

    void adicionarAresta(int v1, int v2, double peso);
    
    using GrafoDirecionado::adicionarAresta; 

    void imprimirGrafo() const override;
    
    const std::list<Aresta>& getArestasEntrada(int v) const;
    const std::vector<Aresta>& getTodasArestas() const;
    
    void removerArestasEntrada(int v);
};

#endif