#ifndef SEGMENTADORIMAGEM_H
#define SEGMENTADORIMAGEM_H

#include <string>
#include <vector>
#include "grafoDirecionadoPonderado.h"

class SegmentadorImagem {
public:
    SegmentadorImagem();
    ~SegmentadorImagem();

    // 1. Carrega imagem do disco
    bool carregarImagem(const std::string& caminho);

    // 2. Converte os pixels em um grafo direcionado
    // Cada pixel é um nó. Arestas conectam aos 4 vizinhos. Peso = Diferença de cor.
    GrafoDirecionadoPonderado criarGrafo();

    // 3. Salva a imagem segmentada baseada na Arborescência
    // Recebe a arborescência gerada pelo Edmonds e um limiar (threshold) de corte
    void salvarSegmentacao(const GrafoDirecionadoPonderado& arborescencia, const std::string& saida, double limiarCorte);

private:
    unsigned char* dadosImagem; // Array de pixels (R, G, B, R, G, B...)
    int largura, altura, canais;

    // Função auxiliar: Distância Euclidiana RGB
    double calcularDiferencaCor(int idx1, int idx2);
    // Função auxiliar: Coordenada (x,y) para Índice Linear
    int getIndice(int x, int y);
};

#endif