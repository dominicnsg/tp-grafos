# Nome do compilador C++
CXX = g++

# Flags de compilação:
# -std=c++17: Usa o padrão C++17 (bom para unique_ptr, etc.)
# -Iinclude:  Diz ao compilador para procurar arquivos de header no diretório 'include'
# -Wall:      Ativa todos os avisos (warnings), uma ótima prática!
# -g:         Gera informações de debug
CXXFLAGS = -std=c++17 -Iinclude -Wall -g

# O nome do executável final
TARGET = grafo_app

# Diretório dos fontes
SRCDIR = src

# Encontra todos os arquivos .cpp no diretório de fontes
SOURCES = $(wildcard $(SRCDIR)/*.cpp)

# Regra principal: compila o alvo (TARGET)
all: $(TARGET)

# Regra para linkar e criar o executável final
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

# Regra para limpar os arquivos gerados
clean:
	rm -f $(TARGET)

# Phony targets não representam arquivos
.PHONY: all clean

#compilar e executar
run: all
	./$(TARGET)