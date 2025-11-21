#include "SegmentadorImagem.h"
#include <iostream>
#include <cmath>
#include <queue>
#include <map>

// Implementação das bibliotecas STB (apenas em UM arquivo .cpp)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

SegmentadorImagem::SegmentadorImagem() : dadosImagem(nullptr), largura(0), altura(0), canais(0) {}

SegmentadorImagem::~SegmentadorImagem() {
    if (dadosImagem) stbi_image_free(dadosImagem);
}

bool SegmentadorImagem::carregarImagem(const std::string& caminho) {
    // Força carregar com 3 canais (RGB)
    dadosImagem = stbi_load(caminho.c_str(), &largura, &altura, &canais, 3);
    if (!dadosImagem) {
        std::cerr << "Erro ao carregar imagem: " << caminho << std::endl;
        return false;
    }
    return true;
}

int SegmentadorImagem::getIndice(int x, int y) {
    return y * largura + x;
}

double SegmentadorImagem::calcularDiferencaCor(int idx1, int idx2) {
    // Cada pixel tem 3 bytes (R, G, B)
    int r1 = dadosImagem[idx1 * 3 + 0];
    int g1 = dadosImagem[idx1 * 3 + 1];
    int b1 = dadosImagem[idx1 * 3 + 2];

    int r2 = dadosImagem[idx2 * 3 + 0];
    int g2 = dadosImagem[idx2 * 3 + 1];
    int b2 = dadosImagem[idx2 * 3 + 2];

    // Distância Euclidiana
    return std::sqrt(std::pow(r1 - r2, 2) + std::pow(g1 - g2, 2) + std::pow(b1 - b2, 2));
}

GrafoDirecionadoPonderado SegmentadorImagem::criarGrafo() {
    int numVertices = largura * altura;
    GrafoDirecionadoPonderado grafo(numVertices);

    std::cout << "Criando grafo para imagem " << largura << "x" << altura 
              << " (" << numVertices << " vertices)...\n";

    for (int y = 0; y < altura; ++y) {
        for (int x = 0; x < largura; ++x) {
            int u = getIndice(x, y);

            // Vizinhos: (x+1, y), (x-1, y), (x, y+1), (x, y-1)
            int dx[] = {1, -1, 0, 0};
            int dy[] = {0, 0, 1, -1};

            for (int i = 0; i < 4; ++i) {
                int nx = x + dx[i];
                int ny = y + dy[i];

                if (nx >= 0 && nx < largura && ny >= 0 && ny < altura) {
                    int v = getIndice(nx, ny);
                    double peso = calcularDiferencaCor(u, v);
                    grafo.adicionarAresta(u, v, peso);
                }
            }
        }
    }
    return grafo;
}

void SegmentadorImagem::salvarSegmentacao(const GrafoDirecionadoPonderado& arborescencia, const std::string& saida, double limiarCorte) {
    // Lógica:
    // 1. Percorrer a arborescência.
    // 2. Remover arestas com peso > limiarCorte (isso separa a árvore em várias sub-árvores/componentes).
    // 3. Fazer uma busca (BFS/DFS) para identificar os componentes conexos.
    // 4. Pintar cada componente de uma cor aleatória.

    int n = largura * altura;
    std::vector<std::vector<int>> adj(n);
    const auto& todasArestas = arborescencia.getTodasArestas();

    // Reconstrói lista de adjacência apenas com arestas "baratas" (que formam as regiões homogêneas)
    for (const auto& aresta : todasArestas) {
        if (aresta.peso <= limiarCorte) {
            adj[aresta.origem].push_back(aresta.destino);
            // Como é arborescência (direcionada), consideramos a conexão "pai-filho"
            // Mas para pintar a região, podemos tratar como não direcionado na busca
            adj[aresta.destino].push_back(aresta.origem); 
        }
    }

    // BFS para encontrar componentes conexos
    std::vector<int> componente(n, -1);
    int numComponentes = 0;
    
    // Array de cores para os componentes (R, G, B)
    std::vector<std::tuple<unsigned char, unsigned char, unsigned char>> cores;

    for (int i = 0; i < n; ++i) {
        if (componente[i] == -1) {
            // Novo componente encontrado
            numComponentes++;
            // Gera cor aleatória para este componente
            cores.emplace_back(rand() % 255, rand() % 255, rand() % 255);

            std::queue<int> q;
            q.push(i);
            componente[i] = numComponentes - 1;

            while (!q.empty()) {
                int u = q.front();
                q.pop();

                for (int v : adj[u]) {
                    if (componente[v] == -1) {
                        componente[v] = numComponentes - 1;
                        q.push(v);
                    }
                }
            }
        }
    }

    std::cout << "Segmentacao concluida. Numero de regioes detectadas: " << numComponentes << "\n";

    // Gerar imagem de saída
    std::vector<unsigned char> imagemSaida(n * 3);
    for (int i = 0; i < n; ++i) {
        auto [r, g, b] = cores[componente[i]];
        imagemSaida[i * 3 + 0] = r;
        imagemSaida[i * 3 + 1] = g;
        imagemSaida[i * 3 + 2] = b;
    }

    stbi_write_png(saida.c_str(), largura, altura, 3, imagemSaida.data(), largura * 3);
}