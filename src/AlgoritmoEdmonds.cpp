#include "AlgoritmoEdmonds.h"
#include <limits>
#include <iostream>
#include <map>
#include <algorithm>

using namespace std;

GrafoDirecionadoPonderado AlgoritmoEdmonds::encontrarArborescenciaMinima(GrafoDirecionadoPonderado& grafo, int raiz) {
    // 1. Prepara os dados para a primeira chamada
    vector<int> vertices(grafo.numVertices());
    for(int i=0; i < grafo.numVertices(); ++i) vertices[i] = i;

    vector<Aresta> todasArestas = grafo.getTodasArestas();

    // 2. Chama o algoritmo recursivo
    list<Aresta> arborescenciaArestas = edmondsRecursivo(vertices, todasArestas, raiz);

    // 3. Constroi o grafo de resultado
    GrafoDirecionadoPonderado resultado(grafo.numVertices());
    for(const auto& aresta : arborescenciaArestas) {
        resultado.adicionarAresta(aresta.origem, aresta.destino, aresta.peso);
    }
    return resultado;
}

std::list<Aresta> AlgoritmoEdmonds::edmondsRecursivo(const std::vector<int>& vertices, 
                                                     const std::vector<Aresta>& arestas, 
                                                     int raiz) {
    
    // --- PASSO 1: Seleção Gulosa (Menor aresta de entrada para cada vértice) ---
    map<int, Aresta> menoresArestasEntrada;
    
    for (int v : vertices) {
        if (v == raiz) continue; // Raiz não precisa de aresta de entrada

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
        } else {
            // Se algum vértice (exceto raiz) não tem entrada, não existe arborescência completa.
            // Em segmentação de imagem, isso raramente acontece se o grafo for conexo.
            // Retornamos o que for possível ou lançamos erro.
        }
    }

    // --- PASSO 2: Detecção de Ciclos ---
    // Vamos verificar se as arestas escolhidas formam ciclos
    map<int, int> grupo; // Mapeia vertice -> ID do ciclo (ou -1 se não estiver em ciclo)
    int novoIdCiclo = -1;
    vector<int> cicloDetectado;

    map<int, int> visitado; // 0: não visitado, 1: visitando, 2: visitado
    map<int, int> pai; 

    // Inicializa estruturas
    for(int v : vertices) {
        grupo[v] = -1; 
        visitado[v] = 0;
    }

    for (int v : vertices) {
        if (v == raiz || visitado[v] == 2) continue;
        
        int atual = v;
        while (atual != raiz && visitado[atual] != 2) {
            visitado[atual] = 1; // Marcando como visitando caminho atual
            
            if (menoresArestasEntrada.find(atual) == menoresArestasEntrada.end()) break; // Sem pai

            int proximo = menoresArestasEntrada.at(atual).origem;
            
            if (visitado[proximo] == 1) {
                // CICLO ENCONTRADO! 'proximo' é onde o ciclo fecha
                novoIdCiclo = proximo; // Usamos um ID arbitrário dos nós do ciclo para representar o super-nó
                
                // Reconstrói o ciclo para a lista 'cicloDetectado'
                int temp = proximo;
                do {
                    cicloDetectado.push_back(temp);
                    grupo[temp] = novoIdCiclo; // Marca que este nó pertence ao super-nó
                    if(menoresArestasEntrada.find(temp) != menoresArestasEntrada.end())
                        temp = menoresArestasEntrada.at(temp).origem;
                    else break;
                } while (temp != proximo);
                
                goto ciclo_encontrado_break; // Sai dos loops aninhados
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

    // --- CASO BASE: Se não houver ciclos, retornamos as arestas escolhidas ---
    if (cicloDetectado.empty()) {
        list<Aresta> resultado;
        for (auto const& [v, aresta] : menoresArestasEntrada) {
            resultado.push_back(aresta);
        }
        return resultado;
    }

    // --- PASSO 3: Contração do Ciclo (Recursão) ---
    // Precisamos criar um novo conjunto de vértices e arestas
    
    // O 'novoIdCiclo' será o representante do super-nó
    vector<int> novosVertices;
    for(int v : vertices) {
        if (grupo[v] == -1) { 
            novosVertices.push_back(v); // Vértices fora do ciclo continuam iguais
        }
    }
    // Adiciona o representante do ciclo apenas uma vez
    bool representanteJaAdicionado = false;
    for(int v : novosVertices) if(v == novoIdCiclo) representanteJaAdicionado = true;
    if(!representanteJaAdicionado) novosVertices.push_back(novoIdCiclo);


    // Novas arestas (ajustadas)
    vector<Aresta> novasArestas;

    for (const auto& aresta : arestas) {
        int u = aresta.origem;
        int v = aresta.destino;
        
        // Caso 1: Aresta totalmente fora do ciclo
        if (grupo[u] == -1 && grupo[v] == -1) {
            novasArestas.push_back(aresta);
        }
        // Caso 2: Aresta saindo do ciclo para fora (u no ciclo -> v fora)
        else if (grupo[u] != -1 && grupo[v] == -1) {
            // Origem vira o super-nó
            Aresta nova = aresta;
            nova.origem = novoIdCiclo; 
            // Guardamos o ID original para recuperar depois
            // Nota: o campo idOriginal na struct Aresta é crucial aqui. 
            // Se você não tiver, use a própria aresta como referência.
            novasArestas.push_back(nova);
        }
        // Caso 3: Aresta entrando no ciclo de fora (u fora -> v no ciclo)
        else if (grupo[u] == -1 && grupo[v] != -1) {
            // Esta é a parte mágica de Edmonds/Chu-Liu:
            // Novo Peso = Peso Original - Peso da aresta que já existe no ciclo apontando para v
            double pesoCicloV = menoresArestasEntrada.at(v).peso;
            double novoPeso = aresta.peso - pesoCicloV;
            
            Aresta nova = aresta;
            nova.destino = novoIdCiclo; // Destino vira super-nó
            nova.peso = novoPeso;
            // Importante: guardamos qual vértice REAL do ciclo esta aresta atingia (v)
            // Podemos usar um campo auxiliar ou guardar a aresta original
            // Aqui, assumimos que a struct Aresta tem dados suficientes ou passamos a original
            novasArestas.push_back(nova);
        }
        // Caso 4: Aresta interna do ciclo (ignora, pois o ciclo foi colapsado)
    }

    // Chamada Recursiva
    int novaRaiz = (grupo[raiz] != -1) ? novoIdCiclo : raiz;
    list<Aresta> arborescenciaContraida = edmondsRecursivo(novosVertices, novasArestas, novaRaiz);

    // --- PASSO 4: Expansão (Desfazer o super-nó) ---
    list<Aresta> resultadoFinal;

    // Mapeia qual vértice do ciclo foi o "ponto de entrada" escolhido pela recursão
    int verticeEntradaCiclo = -1;
    Aresta arestaEntradaCicloOriginal = Aresta(-1,-1,0);

    for (const auto& aresta : arborescenciaContraida) {
        // Verifica se essa aresta aponta para o super-nó
        if (aresta.destino == novoIdCiclo) {
            // Precisamos recuperar a aresta original. 
            // Como modificamos o peso e destino, precisamos achar a original na lista 'arestas'
            // que corresponde a esta 'aresta' contraída.
            
            // Busca linear (pode ser otimizada com ID) para achar a original:
            // A original é aquela que:
            // 1. Tem o mesmo origem (aresta.origem)
            // 2. Entra em ALGUÉM do ciclo (grupo[original.destino] != -1)
            // 3. Tem o peso compatível com a fórmula (novoPeso = pesoOrig - pesoCiclo)
            
            for(const auto& original : arestas) {
                if (original.origem == aresta.origem && grupo[original.destino] != -1) {
                     double pesoCiclo = menoresArestasEntrada.at(original.destino).peso;
                     // Comparação com tolerância para float
                     if (abs((original.peso - pesoCiclo) - aresta.peso) < 1e-9) {
                         resultadoFinal.push_back(original);
                         verticeEntradaCiclo = original.destino; // Onde quebramos o ciclo
                         break;
                     }
                }
            }
        } 
        else if (aresta.origem == novoIdCiclo) {
             // Aresta saindo do super-nó. Recuperar original.
             // Original tem origem em algum lugar do ciclo e destino = aresta.destino
             for(const auto& original : arestas) {
                 if (grupo[original.origem] != -1 && original.destino == aresta.destino && 
                     abs(original.peso - aresta.peso) < 1e-9) {
                     resultadoFinal.push_back(original);
                     break;
                 }
             }
        }
        else {
            // Aresta que não tocou o ciclo, mantém
            resultadoFinal.push_back(aresta);
        }
    }

    // Adiciona as arestas internas do ciclo, EXCETO a que entra no verticeEntradaCiclo
    for (int v : cicloDetectado) {
        if (v != verticeEntradaCiclo) {
            resultadoFinal.push_back(menoresArestasEntrada.at(v));
        }
    }

    return resultadoFinal;
}