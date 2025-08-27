#include <iostream>
#include <memory>

#include "grafoDirecionado.h"
#include "grafoNaoDirecionado.h"
#include "grafoDirecionadoPonderado.h"
#include "grafoNaoDirecionadoPonderado.h"

using namespace std;

int main()
{
    int V = 5;

    std::cout << "--- Testando Grafo Não-Direcionado ---\n";
    std::unique_ptr<IGrafo> grafoNaoDir = std::make_unique<GrafoNaoDirecionado>(V);
    // Criar um grafo completo
    for (int i = 0; i < V; ++i)
        for (int j = i + 1; j < V; ++j)
            grafoNaoDir->adicionarAresta(i, j);
    grafoNaoDir->imprimirGrafo();

    std::cout << "\n--- Testando Grafo Direcionado ---\n";
    std::unique_ptr<IGrafo> grafoDir = std::make_unique<GrafoDirecionado>(V);
    grafoDir->adicionarAresta(0, 1);
    grafoDir->adicionarAresta(1, 2);
    grafoDir->adicionarAresta(2, 3);
    grafoDir->adicionarAresta(3, 4);
    grafoDir->adicionarAresta(4, 0);
    grafoDir->imprimirGrafo();

    std::cout << "\n--- Testando Grafo Não-Direcionado Ponderado ---\n";
    GrafoNaoDirecionadoPonderado grafoNaoDirPeso(V);
    grafoNaoDirPeso.adicionarAresta(0, 1, 1.0);
    grafoNaoDirPeso.adicionarAresta(0, 2, 2.0);
    grafoNaoDirPeso.adicionarAresta(1, 3, 3.0);
    grafoNaoDirPeso.adicionarAresta(2, 4, 4.0);
    grafoNaoDirPeso.adicionarAresta(3, 4, 5.0);
    grafoNaoDirPeso.imprimirGrafo();

    std::cout << "\n--- Testando Grafo Direcionado Ponderado ---\n";
    GrafoDirecionadoPonderado grafoDirPeso(V);
    grafoDirPeso.adicionarAresta(0, 1, 2.5);
    grafoDirPeso.adicionarAresta(1, 2, 1.2);
    grafoDirPeso.adicionarAresta(2, 3, 3.0);
    grafoDirPeso.adicionarAresta(3, 4, 0.8);
    grafoDirPeso.adicionarAresta(4, 0, 4.1);
    grafoDirPeso.imprimirGrafo();

    std::cout << "\n--- Fim dos Testes ---\n";

    return 0;
}
