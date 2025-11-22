#ifndef SEGMENTADORIMAGEM_H
#define SEGMENTADORIMAGEM_H

#include <string>
#include <vector>
#include <tuple>
#include "grafoDirecionadoPonderado.h"

class SegmentadorImagem {
public:
    SegmentadorImagem();
    ~SegmentadorImagem();

    bool carregarImagem(const std::string& caminho);

    GrafoDirecionadoPonderado criarGrafo();

    void salvarSegmentacao(const GrafoDirecionadoPonderado& arborescencia, const std::string& saida, double limiarCorte);
    
    void aplicarSuavizacao(); 

private:
    unsigned char* dadosImagem;
    int largura, altura, canais;
   
    std::vector<int> pixelParaSuperno;
    
    std::vector<std::tuple<int, int, int>> coresSupernos;

    double calcularDiferencaCor(int idx1, int idx2);
    double calcularDiferencaCorMedia(std::tuple<int,int,int> c1, std::tuple<int,int,int> c2);
    
    int getIndice(int x, int y);
};

#endif