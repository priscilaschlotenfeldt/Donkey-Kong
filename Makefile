# Nome dos executáveis finais
TARGET = bin/main
GAME_TARGET = bin/jogo

# Compilador e flags
CC = gcc
CFLAGS = -Wall -std=c99 -Iinclude
# Para Linux, o Raylib geralmente precisa de: -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Pastas
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Compila apenas os arquivos-fonte ativos do projeto
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/menu.c $(SRC_DIR)/opcoes.c 
GAME_SRCS = $(SRC_DIR)/jogo_definitivo.c

# Define os arquivos .o correspondentes dentro da pasta obj/
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
GAME_OBJS = $(GAME_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Regra padrão
all: $(TARGET) $(GAME_TARGET)

# Regra para linkar os objetos e gerar o executável do menu
$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Regra para linkar os objetos e gerar o executável do mapa/jogo
$(GAME_TARGET): $(GAME_OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $(GAME_OBJS) -o $(GAME_TARGET) $(LDFLAGS)

# Regra para compilar os arquivos .c em arquivos .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para limpar os arquivos gerados (limpeza de build)
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Regra para compilar e rodar logo em seguida
run: all
	./bin/main