#include <iostream>
#include <memory>
#include "../include/grafoNaoDirecionado.h"
#include "../include/grafoDirecionado.h"

using namespace std;

int main()
{
    int V = 5;

    cout << "--- Testando Grafo Nao-Direcionado ---" << endl;
    unique_ptr<IGrafo> grafoCompleto = make_unique<GrafoNaoDirecionado>(V);
    // Criar um grafo completo
    for (int i = 0; i < V; i++)
    {
        for (int j = i + 1; j < V; j++)
        {
            grafoCompleto->adicionarAresta(i, j);
        }
    }
    grafoCompleto->imprimirGrafo();

    cout << "\n--- Testando Grafo Direcionado ---" << endl;
    unique_ptr<IGrafo> grafoCiclo = make_unique<GrafoDirecionado>(V);
    grafoCiclo->adicionarAresta(0, 1);
    grafoCiclo->adicionarAresta(1, 2);
    grafoCiclo->adicionarAresta(2, 3);
    grafoCiclo->adicionarAresta(3, 4);
    grafoCiclo->adicionarAresta(4, 0);
    grafoCiclo->imprimirGrafo();

    cout << "\n--- ======================== ---" << endl;
    return 0;
}