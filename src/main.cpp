#include <iostream>
#include <chrono> // Para medir tempo
#include <stdexcept> // Necessário para tratamento de erros (try/catch)
#include "AlgoritmoEdmonds.h"
#include "AlgoritmoKruskal.h"
#include "SegmentadorImagem.h"
#include "AlgoritmoTarjan.h"

using namespace std;
using namespace std::chrono;

void imprimirUso() {
    cout << "Uso: ./grafo_app <imagem_entrada> <metodo> <limiar>\n";
    cout << "Metodos: \n";
    cout << "  0: Edmonds (Arborescencia - Direcionado)\n";
    cout << "  1: Kruskal (MST - Nao Direcionado)\n";
    cout << "  2: Tarjan (Arborescencia - Direcionado)\n";
    cout << "Exemplo: ./grafo_app teste.png 0 0.05\n";
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    // 1. Bloco de segurança principal (Try-Catch)
    try {
        // Validação básica de quantidade de argumentos
        if (argc < 4) {
            imprimirUso();
            return 1;
        }

        string inputPath = argv[1];
        int metodo;
        double limiar;

        // 2. Validação de conversão de números
        try {
            metodo = stoi(argv[2]);
            limiar = stod(argv[3]);
        } catch (...) {
            throw runtime_error("Argumentos invalidos. 'metodo' deve ser inteiro e 'limiar' deve ser numero (double).");
        }

        SegmentadorImagem seg;
        
        // Carregamento da imagem
        if (!seg.carregarImagem(inputPath)) {
            throw runtime_error("Nao foi possivel carregar a imagem: " + inputPath);
        }

        // 3. Pré-processamento (IMPORTANTE: Suavização antes de criar o grafo)
        cout << "Aplicando suavizacao (blur) para reduzir ruido...\n";
        seg.aplicarSuavizacao();

        // Medir tempo de criação do grafo
        cout << "Criando Grafo de Superpixels...\n";
        auto start = high_resolution_clock::now();
        GrafoDirecionadoPonderado grafoDir = seg.criarGrafo(); 
        auto stop = high_resolution_clock::now();
        cout << "Grafo criado em: " << duration_cast<milliseconds>(stop - start).count() << "ms\n";

        GrafoDirecionadoPonderado resultado(0); // Placeholder

        // 4. Seleção do Método
        if (metodo == 0) {
            cout << "--- Executando Edmonds (Direcionado) ---\n";
            AlgoritmoEdmonds edmonds;
            
            start = high_resolution_clock::now();
            resultado = edmonds.encontrarArborescenciaMinima(grafoDir, 0);
            stop = high_resolution_clock::now();
            
            cout << "Edmonds concluido em: " << duration_cast<milliseconds>(stop - start).count() << "ms\n";
            seg.salvarSegmentacao(resultado, "saida_edmonds.png", limiar);
        } 
        else if(metodo == 1) {
            cout << "--- Executando Kruskal (Nao-Direcionado) ---\n";
            // Conversão para não direcionado
            GrafoNaoDirecionadoPonderado grafoNaoDir(grafoDir.numVertices());
            for(const auto& a : grafoDir.getTodasArestas()) {
                if(a.origem < a.destino) 
                     grafoNaoDir.adicionarAresta(a.origem, a.destino, a.peso);
            }

            AlgoritmoKruskal kruskal;
            
            start = high_resolution_clock::now();
            GrafoNaoDirecionadoPonderado mst = kruskal.encontrarMST(grafoNaoDir);
            stop = high_resolution_clock::now();
            
            cout << "Kruskal concluido em: " << duration_cast<milliseconds>(stop - start).count() << "ms\n";
            
            // Salvar (Cast implícito funciona pois herda de GrafoDirecionado)
            seg.salvarSegmentacao(mst, "saida_kruskal.png", limiar);

        } else if (metodo == 2) {
            cout << "--- Executando Tarjan (Otimizado) ---\n";
            AlgoritmoTarjan tarjan;
            
            start = high_resolution_clock::now();
            resultado = tarjan.encontrarArborescenciaMinima(grafoDir, 0);
            stop = high_resolution_clock::now();
            
            cout << "Tarjan concluido em: " << duration_cast<milliseconds>(stop - start).count() << "ms\n";
            seg.salvarSegmentacao(resultado, "saida_tarjan.png", limiar);
        } else {
            throw runtime_error("Metodo invalido. Escolha 0, 1 ou 2.");
        }

    } catch (const std::exception& e) {
        // Captura qualquer erro lançado acima e imprime de forma limpa
        std::cerr << "\n[ERRO CRITICO]: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}