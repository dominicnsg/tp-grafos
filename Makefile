# Compilador e Flags
CXX = g++
# Flags: C++17, Include, Warnings, Debug e AUMENTO DE PILHA (Essencial para Edmonds)
CXXFLAGS = -std=c++17 -Iinclude -Wall -g -O2 -Wl,--stack,67108864

# Nome do Executável
TARGET = grafo_app

# Diretórios
SRCDIR = src
OBJDIR = obj

# Encontra todos os .cpp e cria a lista de .o correspondentes
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))

# Regra Principal
all: $(TARGET)

# Regra de Linkagem (Cria o executável final)
$(TARGET): $(OBJECTS)
	@echo "Linkando o executavel $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Regra de Compilação (Transforma cada .cpp em um .o dentro de obj/)
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	@echo "Compilando $<..."
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Limpeza completa (remove objetos, executável e imagens geradas)
clean:
	@echo "Limpando arquivos temporarios..."
	rm -rf $(OBJDIR) $(TARGET) *.png

.PHONY: all clean