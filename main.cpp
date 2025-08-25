#include "utils/grafo.h"
#include <iostream>


using namespace std;

int main() {
    int V = 5; 
    Grafo grafo(V);

    // Criar um grafo completo (sem laços)
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i != j) {
                grafo.adicionarAresta(i, j);
                grafo.imprimirGrafo();
            }
        }
    }

    grafo.imprimirGrafo();

    cout << "Pressione ENTER para sair...";
    cin.get(); // espera o usuário apertar ENTER
    return 0;
}
