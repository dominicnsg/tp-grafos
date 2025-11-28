# Trabalho de Grafos: Segmentação de Imagens com Árvores Geradoras Mínimas

## Sobre o Projeto

Este projeto foi desenvolvido para a disciplina de **Teoria de Grafos e Computabilidade (2025/2)** do curso de Ciência da Computação da PUC Minas.

O objetivo principal deste trabalho é a aplicação prática de algoritmos de grafos para realizar a **segmentação de imagens**. O projeto implementa estruturas de dados para grafos direcionados e não direcionados e utiliza algoritmos de Árvore Geradora Mínima (MST) e Arborescência Geradora Mínima (Directed MST) para processar as imagens.

### Algoritmos Implementados
O projeto inclui a implementação e análise comparativa dos seguintes algoritmos:

* **Grafos Não Direcionados:**
    * Algoritmo de Kruskal
* **Grafos Direcionados (Arborescência):**
    * Algoritmo de Edmonds (Chu-Liu/Edmonds)
    * Algoritmo de Tarjan (1977)
    * Algoritmo de Gabow et al. (1986)

O sistema recebe imagens de entrada (ex: `grande.jpg`, `pequena.png`) e gera como saída imagens segmentadas (ex: `saida_edmonds.png`, `saida_kruskal.png`), permitindo visualizar a eficácia de cada método.

## Estrutura do Projeto

A organização das pastas e arquivos segue o padrão abaixo:

```text
.
├── grafo_app.exe           # Executável do projeto (Windows)
├── Makefile                # Script de compilação
├── README.md               # Documentação do projeto
├── include/                # Arquivos de cabeçalho (.h)
├── obj/                    # Objetos de compilação (.o)
├── Relatorio/              # Relatório técnico e fontes TeX
│   ├── Grafos.pdf
│   └── main.tex
├── src/                    # Código fonte (.cpp)
│   ├── AlgoritmoEdmonds.cpp
│   ├── AlgoritmoGabow.cpp
│   ├── AlgoritmoKruskal.cpp
│   ├── AlgoritmoTarjan.cpp
│   ├── main.cpp
│   ├── SegmentadorImagem.cpp
│   └── (Implementações de grafos direcionados e não direcionados)
└── (Imagens de entrada e saída na raiz do projeto)

## Como Compilar e Executar

### Linux/WSL
Certifique-se de ter as ferramentas de compilação C++ (`g++`) e o utilitário `make` instalados.
    ```bash
    sudo apt-get install build-essential.
    ```

1. Abra o terminal na raiz do projeto.
2. Execute o comando:
	```bash
	mingw32-make
	```
3. Como Executar

A execução é realizada via linha de comando. O executável gerado será `grafo_app` (Linux) ou `grafo_app.exe` (Windows).

### Sintaxe

./grafo_app <nome_da_imagem> <id_algoritmo> <limiar>

### 1. IDs dos Algoritmos

Utilize a tabela abaixo para escolher qual método executar no segundo argumento:

| ID | Algoritmo | Tipo            | Descrição                               |
|:--:|:----------|:----------------|:----------------------------------------|
| 0  | Edmonds   | Direcionado     | Lento. Usar apenas em imagens pequenas. |
| 1  | Kruskal   | Não-Direcionado | MST padrão.                             |
| 2  | Tarjan    | Direcionado     | Rápido / Otimizado.                     |
| 3  | Gabow     | Direcionado     | Rápido / Otimizado.                     |

### 2. Entendendo o Limiar (Parâmetro Sigma)

O terceiro argumento (<limiar>) define a sensibilidade do corte e controla o quão "agressiva" será a segmentação:

* Valores Baixos (ex: 0.005 a 0.01): Preservam muitos detalhes. A imagem final terá muitas regiões pequenas e cores variadas (segmentação fina).
* Valores Altos (ex: 0.06 a 0.15): Fundem regiões similares. A imagem final terá poucas regiões grandes e mais homogêneas (segmentação grossa).

> Regra Prática: Quanto maior o limiar, menos segmentada (mais abstrata) fica a imagem.
	```
##  Exemplos de Uso

Exemplo 1: Segmentação detalhada em imagem grande
Usa-se um limiar baixo para não perder detalhes finos (Algoritmo Gabow).
./grafo_app grande.jpg 3 0.006

Exemplo 2: Segmentação balanceada em imagem média
Usa-se um limiar intermediário para separar objetos do fundo.
./grafo_app media.jpg 3 0.06

Exemplo 3: Teste de validação (Edmonds)
Teste em imagem pequena devido à complexidade do algoritmo.
./grafo_app pequena.png 0 0.08

### Windows
No Windows, é necessário instalar o `make` manualmente:

1. Baixe o `make` para Windows em: [GnuWin32 Make](https://gnuwin32.sourceforge.net/packages/make.htm)
2. Adicione o diretório do executável `make.exe` à variável de ambiente `PATH` nas configurações de env var do Windows.
3. Abra o terminal (cmd ou PowerShell) na raiz do projeto.
4. Execute:
	```cmd
	mingw32-make
	./grafo_app <nome_da_imagem> <id_algoritmo> <parametro_sigma>
	(exemplo de uso: Executa o algoritmo 0 (Kruskal) na imagem media.jpg com limiar 0.075 -> ./grafo_app media.jpg 0 0.075)
	```

## Observações
- O projeto utiliza C++17.
- O executável gerado será `grafo_app` (Linux) ou `grafo_app.exe` (Windows).
- O código fonte está em `src/` e os headers em `include/`.
