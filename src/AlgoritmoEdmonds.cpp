#include "AlgoritmoEdmonds.h"
#include <limits>
#include <iostream>
#include <map>
#include <algorithm>

using namespace std;

GrafoDirecionadoPonderado AlgoritmoEdmonds::encontrarArborescenciaMinima(GrafoDirecionadoPonderado& grafo, int raiz) {
    vector<int> vertices(grafo.numVertices());
    for(int i=0; i < grafo.numVertices(); ++i) vertices[i] = i;

    vector<Aresta> todasArestas = grafo.getTodasArestas();

    // Chamada do método recursivo
    list<Aresta> arborescenciaArestas = edmondsRecursivo(vertices, todasArestas, raiz);

    // Monta o grafo de resultado
    GrafoDirecionadoPonderado resultado(grafo.numVertices());
    for(const auto& aresta : arborescenciaArestas) {
        resultado.adicionarAresta(aresta.origem, aresta.destino, aresta.peso);
    }
    return resultado;
}

std::list<Aresta> AlgoritmoEdmonds::edmondsRecursivo(const std::vector<int>& vertices, const std::vector<Aresta>& arestas, int raiz) {

    // Caso base: grafo vazio ou apenas a raiz
    if (vertices.empty() || (vertices.size() == 1 && vertices[0] == raiz)) {
        return list<Aresta>();
    }
    
    // 1. Seleção Gulosa: escolhe a menor aresta de entrada para cada vértice
    map<int, Aresta> menoresArestasEntrada;
    
    for (int v : vertices) {
        if (v == raiz) continue; 

        Aresta menorAresta(-1, -1, numeric_limits<double>::max());
        bool encontrou = false;

        for (const auto& aresta : arestas) {
            if (aresta.destino == v && aresta.origem != v) { // Ignora auto-loops
                if (aresta.peso < menorAresta.peso) {
                    menorAresta = aresta;
                    encontrou = true;
                }
            }
        }

        if (encontrou) {
            menoresArestasEntrada.insert({v, menorAresta});
        }
    }

    // 2. Detecção de Ciclos
    map<int, int> grupo; // Mapeia vértice -> ID do ciclo
    int novoIdCiclo = -1;
    vector<int> cicloDetectado;

    map<int, int> visitado; 
    
    for(int v : vertices) {
        grupo[v] = -1; 
        visitado[v] = 0;
    }

    for (int v : vertices) {
        if (v == raiz || visitado[v] == 2) continue;
        
        int atual = v;
        while (atual != raiz && visitado[atual] != 2) {
            visitado[atual] = 1; 
            
            if (menoresArestasEntrada.find(atual) == menoresArestasEntrada.end()) break; 

            int proximo = menoresArestasEntrada.at(atual).origem;
            
            if (visitado[proximo] == 1) {
                // Ciclo encontrado
                novoIdCiclo = proximo; 
                
                int temp = proximo;
                do {
                    cicloDetectado.push_back(temp);
                    grupo[temp] = novoIdCiclo; // Marca vértice como parte do super-nó
                    if(menoresArestasEntrada.find(temp) != menoresArestasEntrada.end())
                        temp = menoresArestasEntrada.at(temp).origem;
                    else break;
                } while (temp != proximo);
                
                goto ciclo_encontrado_break; 
            }
            atual = proximo;
        }
        
        // Marca caminho como finalizado
        atual = v;
        while(atual != raiz && visitado[atual] == 1) {
            visitado[atual] = 2;
            if(menoresArestasEntrada.count(atual)) atual = menoresArestasEntrada.at(atual).origem;
            else break;
        }
    }

    ciclo_encontrado_break:;

    // Se não houver ciclos, retorna a seleção atual
    if (cicloDetectado.empty()) {
        list<Aresta> resultado;
        for (auto const& [v, aresta] : menoresArestasEntrada) {
            resultado.push_back(aresta);
        }
        return resultado;
    }

    // 3. Contração do Ciclo (Recursão)
    vector<int> novosVertices;
    for(int v : vertices) {
        if (grupo[v] == -1) { 
            novosVertices.push_back(v); 
        }
    }
    
    // Adiciona o super-nó
    bool representanteJaAdicionado = false;
    for(int v : novosVertices) if(v == novoIdCiclo) representanteJaAdicionado = true;
    if(!representanteJaAdicionado) novosVertices.push_back(novoIdCiclo);


    // Ajusta as arestas para o novo grafo contraído
    vector<Aresta> novasArestas;

    for (const auto& aresta : arestas) {
        int u = aresta.origem;
        int v = aresta.destino;
        
        if (grupo[u] == -1 && grupo[v] == -1) {
            // Aresta fora do ciclo (mantém)
            novasArestas.push_back(aresta);
        }
        else if (grupo[u] != -1 && grupo[v] == -1) {
            // Saindo do ciclo -> vai para fora
            Aresta nova = aresta;
            nova.origem = novoIdCiclo; 
            novasArestas.push_back(nova);
        }
        else if (grupo[u] == -1 && grupo[v] != -1) {
            // Entrando no ciclo (atualiza o peso)
            double pesoCicloV = menoresArestasEntrada.at(v).peso;
            double novoPeso = aresta.peso - pesoCicloV;
            
            Aresta nova = aresta;
            nova.destino = novoIdCiclo; 
            nova.peso = novoPeso;
            novasArestas.push_back(nova);
        }
    }

    // Chama recursão
    int novaRaiz = (grupo[raiz] != -1) ? novoIdCiclo : raiz;
    list<Aresta> arborescenciaContraida = edmondsRecursivo(novosVertices, novasArestas, novaRaiz);

    // 4. Expansão (Recupera o grafo original)
    list<Aresta> resultadoFinal;
    int verticeEntradaCiclo = -1;

    for (const auto& aresta : arborescenciaContraida) {
        if (aresta.destino == novoIdCiclo) {
            // Recupera a aresta original que entra no ciclo
            for(const auto& original : arestas) {
                if (original.origem == aresta.origem && grupo[original.destino] != -1) {
                      double pesoCiclo = menoresArestasEntrada.at(original.destino).peso;
                      if (abs((original.peso - pesoCiclo) - aresta.peso) < 1e-9) {
                          resultadoFinal.push_back(original);
                          verticeEntradaCiclo = original.destino; 
                          break;
                      }
                }
            }
        } 
        else if (aresta.origem == novoIdCiclo) {
             // Recupera a aresta original que sai do ciclo
             for(const auto& original : arestas) {
                 if (grupo[original.origem] != -1 && original.destino == aresta.destino && 
                     abs(original.peso - aresta.peso) < 1e-9) {
                     resultadoFinal.push_back(original);
                     break;
                 }
             }
        }
        else {
            // Aresta não relacionada ao ciclo
            resultadoFinal.push_back(aresta);
        }
    }

    // Adiciona arestas internas do ciclo (quebra o loop)
    for (int v : cicloDetectado) {
        if (v != verticeEntradaCiclo) {
            resultadoFinal.push_back(menoresArestasEntrada.at(v));
        }
    }

    return resultadoFinal;
}