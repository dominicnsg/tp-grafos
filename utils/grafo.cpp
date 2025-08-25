#include "grafo.h"

using namespace std;

Grafo::Grafo(int V)
{
    this->V = V; // atribui o número de vértices
    adj = new list<int>[V]; // cria as listas
}

Grafo::~Grafo() // extra
{
    delete[] adj; // libera memória alocada
}

void Grafo::adicionarAresta(int v1, int v2)
{
    adj[v1].push_back(v2);  // adiciona vértice v2 à lista de vértices adjacentes de v1
}

void Grafo::imprimirGrafo() {
    cout << "Representacao do Grafo (Lista de Adjacencias):" << endl;
    for (int v = 0; v < V; ++v) {
        cout << "Vertice " << v << ": [ ";
        for (int vizinho : adj[v]) {
            cout << vizinho <<  " " ;
        }
        cout << "]" << endl;
    }
}