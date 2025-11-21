#include <iostream>
#include <memory>

#include "grafoDirecionado.h"
#include "grafoNaoDirecionado.h"
#include "grafoDirecionadoPonderado.h"
#include "grafoNaoDirecionadoPonderado.h"
#include "AlgoritmoEdmonds.h"
#include "SegmentadorImagem.h"

using namespace std;

int main()
{
    // int V = 5;

    // std::cout << "--- Testando Grafo Não-Direcionado ---\n";
    // std::unique_ptr<IGrafo> grafoNaoDir = std::make_unique<GrafoNaoDirecionado>(V);
    // // Criar um grafo completo
    // for (int i = 0; i < V; ++i)
    //     for (int j = i + 1; j < V; ++j)
    //         grafoNaoDir->adicionarAresta(i, j);
    // grafoNaoDir->imprimirGrafo();

    // std::cout << "\n--- Testando Grafo Direcionado ---\n";
    // std::unique_ptr<IGrafo> grafoDir = std::make_unique<GrafoDirecionado>(V);
    // grafoDir->adicionarAresta(0, 1);
    // grafoDir->adicionarAresta(1, 2);
    // grafoDir->adicionarAresta(2, 3);
    // grafoDir->adicionarAresta(3, 4);
    // grafoDir->adicionarAresta(4, 0);
    // grafoDir->imprimirGrafo();

    // std::cout << "\n--- Testando Grafo Não-Direcionado Ponderado ---\n";
    // GrafoNaoDirecionadoPonderado grafoNaoDirPeso(V);
    // grafoNaoDirPeso.adicionarAresta(0, 1, 1.0);
    // grafoNaoDirPeso.adicionarAresta(0, 2, 2.0);
    // grafoNaoDirPeso.adicionarAresta(1, 3, 3.0);
    // grafoNaoDirPeso.adicionarAresta(2, 4, 4.0);
    // grafoNaoDirPeso.adicionarAresta(3, 4, 5.0);
    // grafoNaoDirPeso.imprimirGrafo();

    // std::cout << "\n--- Testando Grafo Direcionado Ponderado ---\n";
    // GrafoDirecionadoPonderado grafoDirPeso(V);
    // grafoDirPeso.adicionarAresta(0, 1, 2.5);
    // grafoDirPeso.adicionarAresta(1, 2, 1.2);
    // grafoDirPeso.adicionarAresta(2, 3, 3.0);
    // grafoDirPeso.adicionarAresta(3, 4, 0.8);
    // grafoDirPeso.adicionarAresta(4, 0, 4.1);
    // grafoDirPeso.imprimirGrafo();

    // std::cout << "\n--- Fim dos Testes ---\n";

    // int V = 4;
    // GrafoDirecionadoPonderado grafo(V);

    // // Exemplo clássico onde Dijkstra falha ou onde há ciclo
    // // 0 -> 1 (10)
    // // 0 -> 2 (10)
    // // 1 -> 3 (20)
    // // 2 -> 3 (20)
    // // 3 -> 1 (1)  <-- Ciclo entre 1 e 3 com peso baixo
    
    // // Raiz = 0
    // grafo.adicionarAresta(0, 1, 10.0);
    // grafo.adicionarAresta(0, 2, 10.0);
    // grafo.adicionarAresta(1, 3, 20.0);
    // grafo.adicionarAresta(2, 3, 20.0); // Outra opção para chegar em 3
    // grafo.adicionarAresta(3, 1, 1.0);  // Ciclo! (1->3->1) custo total 21
    
    // // Se fosse guloso simples sem tratar ciclo, poderia ficar preso ou não otimizar.
    // // Edmonds deve perceber que para manter o ciclo barato (1 <-> 3), 
    // // ele deve entrar no ciclo pelo lugar mais barato.
    
    // AlgoritmoEdmonds edmonds;
    // cout << "Calculando Arborescencia Minima (Raiz 0)...\n";
    
    // GrafoDirecionadoPonderado resultado = edmonds.encontrarArborescenciaMinima(grafo, 0);
    
    // resultado.imprimirGrafo();

    srand(time(NULL)); // Semente para cores aleatórias

    SegmentadorImagem seg;
    string input = "teste.png"; // COLOQUE UMA IMAGEM PEQUENA AQUI
    
    if (seg.carregarImagem(input)) {
        // 1. Converter Imagem -> Grafo
        GrafoDirecionadoPonderado grafo = seg.criarGrafo();

        // 2. Calcular Arborescência Mínima (Edmonds)
        AlgoritmoEdmonds edmonds;
        std::cout << "Rodando Edmonds (isso pode demorar)..." << std::endl;
        
        // Usamos o vértice 0 (canto superior esquerdo) como raiz
        GrafoDirecionadoPonderado arborescencia = edmonds.encontrarArborescenciaMinima(grafo, 0);

        // 3. Salvar Resultado Segmentado
        // Limiar de corte: arestas com diferença de cor > 30 serão cortadas
        seg.salvarSegmentacao(arborescencia, "resultado_segmentacao.png", 30.0);
        
        std::cout << "Imagem salva como resultado_segmentacao.png" << std::endl;
    } else {
        std::cout << "Nao foi possivel carregar a imagem." << std::endl;
    }

    return 0;
}
