#include <iostream>
#include <chrono> // Para medir tempo
#include "AlgoritmoEdmonds.h"
#include "AlgoritmoKruskal.h"
#include "SegmentadorImagem.h"

using namespace std;
using namespace std::chrono;

void imprimirUso() {
    cout << "Uso: ./grafo_app <imagem_entrada> <metodo> <limiar>\n";
    cout << "Metodos: \n";
    cout << "  0: Edmonds (Arborescencia - Direcionado)\n";
    cout << "  1: Kruskal (MST - Nao Direcionado)\n";
    cout << "Exemplo: ./grafo_app foto.jpg 0 50.0\n";
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    if (argc < 4) {
        imprimirUso();
        return 1;
    }

    string inputPath = argv[1];
    int metodo = stoi(argv[2]);
    double limiar = stod(argv[3]);

    SegmentadorImagem seg;
    if (!seg.carregarImagem(inputPath)) return 1;

    // Medir tempo de criação do grafo
    auto start = high_resolution_clock::now();
    GrafoDirecionadoPonderado grafoDir = seg.criarGrafo(); 
    auto stop = high_resolution_clock::now();
    cout << "Grafo criado em: " << duration_cast<milliseconds>(stop - start).count() << "ms\n";

    GrafoDirecionadoPonderado resultado(0); // Placeholder

    if (metodo == 0) {
        cout << "--- Executando Edmonds (Direcionado) ---\n";
        AlgoritmoEdmonds edmonds;
        
        start = high_resolution_clock::now();
        resultado = edmonds.encontrarArborescenciaMinima(grafoDir, 0);
        stop = high_resolution_clock::now();
        
        cout << "Edmonds concluido em: " << duration_cast<milliseconds>(stop - start).count() << "ms\n";
        seg.salvarSegmentacao(resultado, "saida_edmonds.png", limiar);
    } 
    else {
        cout << "--- Executando Kruskal (Nao-Direcionado) ---\n";
        // Convertendo para não direcionado (simplesmente reinterpretando as arestas)
        // Criamos um grafo nao direcionado com os mesmos dados
        GrafoNaoDirecionadoPonderado grafoNaoDir(grafoDir.numVertices());
        for(const auto& a : grafoDir.getTodasArestas()) {
            // Adiciona apenas uma direção para evitar duplicidade no peso, 
            // a classe NaoDirecionado cuida da simetria lógica
            if(a.origem < a.destino) // Pequeno truque para add aresta 1 vez só
                 grafoNaoDir.adicionarAresta(a.origem, a.destino, a.peso);
        }

        AlgoritmoKruskal kruskal;
        
        start = high_resolution_clock::now();
        GrafoNaoDirecionadoPonderado mst = kruskal.encontrarMST(grafoNaoDir);
        stop = high_resolution_clock::now();
        
        cout << "Kruskal concluido em: " << duration_cast<milliseconds>(stop - start).count() << "ms\n";
        
        // Salvar (Cast para grafo direcionado para reaproveitar a função de salvar)
        // Isso funciona porque GrafoNaoDirecionado herda de Direcionado
        seg.salvarSegmentacao(mst, "saida_kruskal.png", limiar);
    }

    return 0;
}