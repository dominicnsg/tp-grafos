#include <iostream>
#include <list>
#include <vector>

using namespace std;

class Grafo
{
private:
    int V;  // número de vértices
    list<int> *adj; // ponteiro para um array contendo as listas de adjacências

public:
    Grafo(int V);   // construtor
    void adicionarAresta(int v1, int v2);   // adiciona uma aresta no grafo
    void imprimirGrafo();   // nova função para imprimir o grafo
};

Grafo::Grafo(int V)
{
    this->V = V; // atribui o número de vértices
    adj = new list<int>[V]; // cria as listas
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

int main() {
    int V = 5; 
    Grafo grafo(V);

    // Criar um grafo completo (sem laços)
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i != j) {
                grafo.adicionarAresta(i, j);
            }
        }
    }

    grafo.imprimirGrafo();

    return 0;
}
