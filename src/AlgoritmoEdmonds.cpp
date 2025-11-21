#include "AlgoritmoEdmonds.h"
#include <limits>
#include <iostream>
#include <algorithm>

using namespace std;

GrafoDirecionadoPonderado AlgoritmoEdmonds::encontrarArborescenciaMinima(GrafoDirecionadoPonderado& grafo, int raiz) {
    // Mapeamento inicial: 0..V-1
    int V = grafo.numVertices();
    vector<int> vertices(V);
    for(int i=0; i < V; ++i) vertices[i] = i;

    vector<Aresta> todasArestas = grafo.getTodasArestas();

    // Recursão com vetores em vez de maps para economizar pilha
    list<Aresta> arborescenciaArestas = edmondsRecursivo(vertices, todasArestas, raiz);

    GrafoDirecionadoPonderado resultado(V);
    for(const auto& aresta : arborescenciaArestas) {
        resultado.adicionarAresta(aresta.origem, aresta.destino, aresta.peso);
    }
    return resultado;
}

std::list<Aresta> AlgoritmoEdmonds::edmondsRecursivo(const std::vector<int>& vertices, 
                                                     const std::vector<Aresta>& arestas, 
                                                     int raiz) {
    
    // Encontrar o maior ID para dimensionar os vetores (evita usar std::map)
    int maxId = -1;
    for(int v : vertices) if(v > maxId) maxId = v;
    // Se houver contrações, IDs podem ser maiores que vertices.size, então buscamos nas arestas também
    for(const auto& a : arestas) {
        if(a.origem > maxId) maxId = a.origem;
        if(a.destino > maxId) maxId = a.destino;
    }

    // Usamos vector em vez de map para performance e menos memória na stack
    // O índice do vetor é o ID do vértice
    vector<Aresta> menoresArestas(maxId + 1, Aresta(-1, -1, numeric_limits<double>::max()));
    vector<bool> temEntrada(maxId + 1, false);

    // --- PASSO 1: Seleção Gulosa ---
    for (const auto& aresta : arestas) {
        if (aresta.destino == aresta.origem) continue; // Ignora auto-loops
        if (aresta.destino == raiz) continue;          // Raiz não tem pai

        // Precisamos verificar se o destino está no conjunto de vértices atual
        // (Uma otimização seria usar um vetor booleano 'ativo')
        
        if (aresta.peso < menoresArestas[aresta.destino].peso) {
            menoresArestas[aresta.destino] = aresta;
            temEntrada[aresta.destino] = true;
        }
    }

    // --- PASSO 2: Detecção de Ciclos ---
    vector<int> grupo(maxId + 1, -1);
    vector<int> visitado(maxId + 1, 0); // 0: branco, 1: cinza, 2: preto
    
    int novoIdCiclo = -1;
    vector<int> cicloDetectado;

    for (int v : vertices) {
        if (v == raiz || visitado[v] == 2) continue;

        int atual = v;
        while (atual != raiz && visitado[atual] != 2) {
            visitado[atual] = 1;
            
            if (!temEntrada[atual]) break; // Sem pai, caminho morre

            int pai = menoresArestas[atual].origem;
            
            if (visitado[pai] == 1) {
                // CICLO DETECTADO
                novoIdCiclo = pai; // Usamos o ID do nó de fechamento como base, mas idealmente seria um novo ID único maior
                // Para garantir unicidade sem conflito, vamos pegar maxId + 1
                novoIdCiclo = maxId + 1;

                int temp = pai;
                bool first = true;
                while (temp != pai || first) {
                    first = false;
                    cicloDetectado.push_back(temp);
                    grupo[temp] = novoIdCiclo;
                    temp = menoresArestas[temp].origem;
                }
                goto ciclo_encontrado;
            }
            atual = pai;
        }
        
        // Marca caminho como finalizado (preto)
        atual = v;
        while (atual != raiz && visitado[atual] == 1) {
            visitado[atual] = 2;
            if(temEntrada[atual]) atual = menoresArestas[atual].origem;
            else break;
        }
    }

    ciclo_encontrado:;

    // --- CASO BASE: Sem ciclos ---
    if (cicloDetectado.empty()) {
        list<Aresta> resultado;
        for (int v : vertices) {
            if (v != raiz && temEntrada[v]) {
                resultado.push_back(menoresArestas[v]);
            }
        }
        return resultado;
    }

    // --- PASSO 3: Contração ---
    vector<int> novosVertices;
    bool cicloAdicionado = false;
    for(int v : vertices) {
        if (grupo[v] == -1) {
            novosVertices.push_back(v);
        } else if (!cicloAdicionado) {
            novosVertices.push_back(novoIdCiclo);
            cicloAdicionado = true;
        }
    }

    vector<Aresta> novasArestas;
    novasArestas.reserve(arestas.size()); // Otimização de heap

    for (const auto& aresta : arestas) {
        int u = aresta.origem;
        int v = aresta.destino;
        
        if (grupo[u] == -1 && grupo[v] == -1) { // Fora -> Fora
            novasArestas.push_back(aresta);
        }
        else if (grupo[u] != -1 && grupo[v] == -1) { // Ciclo -> Fora
            Aresta nova = aresta;
            nova.origem = novoIdCiclo;
            novasArestas.push_back(nova);
        }
        else if (grupo[u] == -1 && grupo[v] != -1) { // Fora -> Ciclo
            double pesoNoCiclo = menoresArestas[v].peso;
            Aresta nova = aresta;
            nova.destino = novoIdCiclo;
            nova.peso = aresta.peso - pesoNoCiclo;
        }
    }

    int novaRaiz = (grupo[raiz] != -1) ? novoIdCiclo : raiz;
    
    // Chamada Recursiva
    list<Aresta> arborescenciaContraida = edmondsRecursivo(novosVertices, novasArestas, novaRaiz);

    // --- PASSO 4: Expansão ---
    list<Aresta> resultadoFinal;
    int entradaCicloDestinoOriginal = -1;

    for (const auto& aresta : arborescenciaContraida) {
        if (aresta.destino == novoIdCiclo) {
            
            bool achou = false;
            for(const auto& original : arestas) {
                if (original.origem == aresta.origem && grupo[original.destino] != -1) {
                    double pesoCiclo = menoresArestas[original.destino].peso;
                    if (abs((original.peso - pesoCiclo) - aresta.peso) < 1e-5) {
                        resultadoFinal.push_back(original);
                        entradaCicloDestinoOriginal = original.destino;
                        achou = true;
                        break;
                    }
                }
            }
        }
        else if (aresta.origem == novoIdCiclo) {
            // Recuperar aresta que sai do ciclo (Ciclo -> Fora)
            for(const auto& original : arestas) {
                if (grupo[original.origem] != -1 && original.destino == aresta.destino && 
                    abs(original.peso - aresta.peso) < 1e-5) {
                    resultadoFinal.push_back(original);
                    break;
                }
            }
        }
        else {
            resultadoFinal.push_back(aresta);
        }
    }

    for (int v : cicloDetectado) {
        if (v != entradaCicloDestinoOriginal) {
            resultadoFinal.push_back(menoresArestas[v]);
        }
    }

    return resultadoFinal;
}