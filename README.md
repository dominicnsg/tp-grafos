# tp-grafos

## Sobre o Projeto
Este projeto é um framework para implementações de grafos em C++, incluindo:
- Grafos Direcionados
- Grafos Não Direcionados
- Grafos Ponderados
- Grafos Não Ponderados

O objetivo é fornecer uma base flexível para estudos e aplicações envolvendo diferentes tipos de grafos.

## Estrutura do Projeto

```
grafo_app           # Executável gerado após compilação
Makefile            # Script de build (compilação)
README.md           # Este arquivo
include/            # Headers das implementações de grafos
  grafo.h
  grafoDirecionado.h
  grafoNaoDirecionado.h
  grafoPonderado.h
  grafoNaoPonderado.h
src/                # Código fonte principal
  main.cpp
```

## Como Compilar e Executar

### Linux/WSL
Certifique-se de ter as ferramentas de compilação C++ (`g++`) e o utilitário `make` instalados.
    ```bash
    sudo apt-get install build-essential.
    ```

1. Abra o terminal na raiz do projeto.
2. Execute o comando:
	```bash
	make
	```
3. Execute o programa:
	```bash
	./grafo_app
	```

### Windows
No Windows, é necessário instalar o `make` manualmente:

1. Baixe o `make` para Windows em: [GnuWin32 Make](https://gnuwin32.sourceforge.net/packages/make.htm)
2. Adicione o diretório do executável `make.exe` à variável de ambiente `PATH` nas configurações de env var do Windows.
3. Abra o terminal (cmd ou PowerShell) na raiz do projeto.
4. Execute:
	```cmd
	make
	grafo_app.exe
	```

## Observações
- O projeto utiliza C++17.
- O executável gerado será `grafo_app` (Linux) ou `grafo_app.exe` (Windows).
- O código fonte está em `src/` e os headers em `include/`.