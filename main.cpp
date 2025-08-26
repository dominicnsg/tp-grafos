#include <iostream>
#include <list>
#include <vector>

using namespace std;

class Grafo {
protected:
    int V;              // número de vértices
    list<int> *adj;     // ponteiro para um array contendo as listas de adjacências

public:
    Grafo(int V);                       // construtor
    virtual void adicionarAresta(int v1, int v2); // adiciona uma aresta no grafo
    void imprimirGrafo();               // imprime o grafo
    virtual ~Grafo();                   // destrutor
};

// ---------- Implementação da classe base Grafo ----------

Grafo::Grafo(int V) {
    this->V = V;
    adj = new list<int>[V];
}

void Grafo::adicionarAresta(int v1, int v2) {
    adj[v1].push_back(v2);  // adiciona v2 à lista de adjacentes de v1
}

void Grafo::imprimirGrafo() {
    cout << "Representacao do Grafo (Lista de Adjacencias):" << endl;
    for (int v = 0; v < V; ++v) {
        cout << "Vertice " << v << ": [ ";
        for (int vizinho : adj[v]) {
            cout << vizinho << " ";
        }
        cout << "]" << endl;
    }
}

Grafo::~Grafo() {
    delete[] adj;
}

// ---------- Classe derivada para Grafo Não Direcionado ----------

class GrafoNaoDirecionado : public Grafo {
public:
    GrafoNaoDirecionado(int V) : Grafo(V) {}

    // Sobrescreve a função para adicionar aresta nos dois sentidos
    void adicionarAresta(int v1, int v2) override {
        adj[v1].push_back(v2);
        adj[v2].push_back(v1); // adiciona a aresta inversa
    }
};

// ---------- Função principal ----------

int main() {
    int V = 5;

    GrafoNaoDirecionado grafo(V);

    // Criar um grafo completo (sem laços)
    for (int i = 0; i < V; i++) {
        for (int j = i + 1; j < V; j++) { // só metade da matriz (evita duplicar)
            grafo.adicionarAresta(i, j);
        }
    }

    grafo.imprimirGrafo();

    return 0;
}
