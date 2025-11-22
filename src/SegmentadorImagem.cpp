#include "SegmentadorImagem.h"
#include "UnionFind.h"
#include <iostream>
#include <cmath>
#include <queue>
#include <map>
#include <set>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

SegmentadorImagem::SegmentadorImagem() : dadosImagem(nullptr), largura(0), altura(0), canais(0) {}

SegmentadorImagem::~SegmentadorImagem() {
    if (dadosImagem) stbi_image_free(dadosImagem);
}

bool SegmentadorImagem::carregarImagem(const std::string& caminho) {
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
    int r1 = dadosImagem[idx1 * 3 + 0];
    int g1 = dadosImagem[idx1 * 3 + 1];
    int b1 = dadosImagem[idx1 * 3 + 2];

    int r2 = dadosImagem[idx2 * 3 + 0];
    int g2 = dadosImagem[idx2 * 3 + 1];
    int b2 = dadosImagem[idx2 * 3 + 2];

    return std::sqrt(std::pow(r1 - r2, 2) + std::pow(g1 - g2, 2) + std::pow(b1 - b2, 2));
}

double SegmentadorImagem::calcularDiferencaCorMedia(std::tuple<int,int,int> c1, std::tuple<int,int,int> c2) {
    auto [r1, g1, b1] = c1;
    auto [r2, g2, b2] = c2;
    return std::sqrt(std::pow(r1 - r2, 2) + std::pow(g1 - g2, 2) + std::pow(b1 - b2, 2));
}

void SegmentadorImagem::aplicarSuavizacao() {
    if (!dadosImagem) return;
    std::vector<unsigned char> dadosSuavizados(largura * altura * 3);
    int dx[] = {0, 1, -1, 0, 0}; 
    int dy[] = {0, 0, 0, 1, -1}; 
    
    for (int y = 0; y < altura; ++y) {
        for (int x = 0; x < largura; ++x) {
            long long r=0, g=0, b=0;
            int count = 0;
            for (int i = 0; i < 5; ++i) {
                int nx = x + dx[i], ny = y + dy[i];
                if (nx >= 0 && nx < largura && ny >= 0 && ny < altura) {
                    int idx = getIndice(nx, ny) * 3;
                    r += dadosImagem[idx]; g += dadosImagem[idx+1]; b += dadosImagem[idx+2];
                    count++;
                }
            }
            int idx = getIndice(x, y) * 3;
            dadosSuavizados[idx] = r/count; dadosSuavizados[idx+1] = g/count; dadosSuavizados[idx+2] = b/count;
        }
    }
    std::copy(dadosSuavizados.begin(), dadosSuavizados.end(), dadosImagem);
}

GrafoDirecionadoPonderado SegmentadorImagem::criarGrafo() {
    int numPixels = largura * altura;
    
    UnionFind uf(numPixels);
    double limiarAgrupamento = 15.0; // Pixels com dif < 15 serão o mesmo nó lógica

    std::cout << "Agrupando pixels similares (Superpixels)...\n";
    
    for (int y = 0; y < altura; ++y) {
        for (int x = 0; x < largura; ++x) {
            int u = getIndice(x, y);
            int vizinhosX[] = {x + 1, x};
            int vizinhosY[] = {y, y + 1};

            for(int i=0; i<2; ++i) {
                int nx = vizinhosX[i], ny = vizinhosY[i];
                if (nx < largura && ny < altura) {
                    int v = getIndice(nx, ny);
                    if (calcularDiferencaCor(u, v) < limiarAgrupamento) {
                        uf.unite(u, v);
                    }
                }
            }
        }
    }

    // 2. CALCULAR CORES MÉDIAS DOS SUPERPIXELS
    std::map<int, int> rootToId;
    int nextId = 0;
    
    // Estruturas temporárias para soma de cores
    std::vector<long long> sumR(numPixels, 0), sumG(numPixels, 0), sumB(numPixels, 0);
    std::vector<int> countPixel(numPixels, 0);

    pixelParaSuperno.assign(numPixels, -1);

    for(int i=0; i<numPixels; ++i) {
        int root = uf.find(i);
        if (rootToId.find(root) == rootToId.end()) {
            rootToId[root] = nextId++;
        }
        int superNoId = rootToId[root];
        pixelParaSuperno[i] = superNoId;

        // Acumula cor
        sumR[superNoId] += dadosImagem[i*3+0];
        sumG[superNoId] += dadosImagem[i*3+1];
        sumB[superNoId] += dadosImagem[i*3+2];
        countPixel[superNoId]++;
    }

    int numSupernos = nextId;
    coresSupernos.resize(numSupernos);
    for(int i=0; i<numSupernos; ++i) {
        if(countPixel[i] > 0)
            coresSupernos[i] = {sumR[i]/countPixel[i], sumG[i]/countPixel[i], sumB[i]/countPixel[i]};
    }

    std::cout << "Grafo Reduzido: " << numPixels << " pixels -> " << numSupernos << " supernos.\n";

    // 3. CONSTRUIR O GRAFO LÓGICO
    GrafoDirecionadoPonderado grafo(numSupernos);
    const double MAX_PESO = 441.67;

    std::set<std::pair<int,int>> arestasAdicionadas;

    for (int y = 0; y < altura; ++y) {
        for (int x = 0; x < largura; ++x) {
            int uPixel = getIndice(x, y);
            int uSuper = pixelParaSuperno[uPixel];

            int dx[] = {1, 0}; // Direita e Baixo
            int dy[] = {0, 1};

            for(int i=0; i<2; ++i) {
                int nx = x + dx[i], ny = y + dy[i];
                if (nx < largura && ny < altura) {
                    int vPixel = getIndice(nx, ny);
                    int vSuper = pixelParaSuperno[vPixel];

                    if (uSuper != vSuper) {

                        if (arestasAdicionadas.find({uSuper, vSuper}) == arestasAdicionadas.end()) {
                            double peso = calcularDiferencaCorMedia(coresSupernos[uSuper], coresSupernos[vSuper]);
                            double pesoNorm = peso / MAX_PESO;
                            
                            grafo.adicionarAresta(uSuper, vSuper, pesoNorm);
                            grafo.adicionarAresta(vSuper, uSuper, pesoNorm); // Bidirecional no grafo base
                            
                            arestasAdicionadas.insert({uSuper, vSuper});
                            arestasAdicionadas.insert({vSuper, uSuper});
                        }
                    }
                }
            }
        }
    }

    return grafo;
}


void SegmentadorImagem::salvarSegmentacao(const GrafoDirecionadoPonderado& arborescencia, const std::string& saida, double limiarCorte) {
    // O grafo 'arborescencia' agora contém SUPERNOS, não pixels.
    int numSupernos = arborescencia.numVertices();
    
    // 1. Busca conexa no grafo de supernos
    std::vector<std::vector<int>> adj(numSupernos);
    for (const auto& aresta : arborescencia.getTodasArestas()) {
        if (aresta.peso <= limiarCorte) {
            adj[aresta.origem].push_back(aresta.destino);
            adj[aresta.destino].push_back(aresta.origem);
        }
    }

    std::vector<int> compSuperno(numSupernos, -1);
    int numComponentes = 0;
    std::vector<std::tuple<unsigned char, unsigned char, unsigned char>> coresComponentes;

    for (int i = 0; i < numSupernos; ++i) {
        if (compSuperno[i] == -1) {
            numComponentes++;
            // Cor aleatória para o componente final
            coresComponentes.emplace_back(rand() % 255, rand() % 255, rand() % 255);
            
            std::queue<int> q;
            q.push(i);
            compSuperno[i] = numComponentes - 1;
            
            while(!q.empty()){
                int u = q.front(); q.pop();
                for(int v : adj[u]){
                    if(compSuperno[v] == -1){
                        compSuperno[v] = numComponentes - 1;
                        q.push(v);
                    }
                }
            }
        }
    }

    std::cout << "Segmentacao final: " << numComponentes << " regioes.\n";

    // 2. Pintar a imagem original mapeando Pixel -> Superno -> Componente -> Cor
    std::vector<unsigned char> imagemSaida(largura * altura * 3);
    
    for(int i=0; i < largura * altura; ++i) {
        int superNoId = pixelParaSuperno[i];
        // Se por algum acaso o pixel não tiver superno (erro), pinta de preto
        if (superNoId == -1) continue; 
        
        int componenteId = compSuperno[superNoId];
        // Se o superno ficou isolado ou algo assim
        if (componenteId == -1) componenteId = 0;

        auto [r, g, b] = coresComponentes[componenteId];
        imagemSaida[i * 3 + 0] = r;
        imagemSaida[i * 3 + 1] = g;
        imagemSaida[i * 3 + 2] = b;
    }

    stbi_write_png(saida.c_str(), largura, altura, 3, imagemSaida.data(), largura * 3);
}