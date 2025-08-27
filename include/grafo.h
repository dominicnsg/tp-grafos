#pragma once
#include <vector>
#include <list>
#include <iostream>

class IGrafo {
public:
    virtual ~IGrafo() = default;
    virtual void adicionarAresta(int v1, int v2) = 0; 
    virtual int numVertices() const = 0;
    virtual void imprimirGrafo() const = 0;
    virtual const std::list<int>& getVizinhos(int v) const = 0;
};