#ifndef GRAFO_H
#define GRAFO_H

#include <iostream>
#include <list>
#include <vector>

using namespace std;


class Grafo
{
protected:
    int V;  // número de vértices
    list<int> *adj; // ponteiro para um array contendo as listas de adjacências

public:
    Grafo(int V);   // construtor
    virtual ~Grafo();   // destrutor para liberar memória
    void adicionarAresta(int v1, int v2);   // adiciona uma aresta no grafo
    void imprimirGrafo();   // nova função para imprimir o grafo
};

#endif