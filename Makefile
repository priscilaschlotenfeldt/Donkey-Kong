# =============================================================================
# Makefile Cross-Platform — Donkey Kong
# Detecta automaticamente: Windows (MinGW), Linux
# =============================================================================

# ── Detecção do Sistema Operacional ──────────────────────────────────────────
ifeq ($(OS),Windows_NT)
    PLATFORM := Windows
else
    PLATFORM := Linux
endif

# ── Compilador ────────────────────────────────────────────────────────────────
CC = gcc

# ── Flags comuns ─────────────────────────────────────────────────────────────
CFLAGS = -Wall -std=c99 -g -Iinclude

# ── Flags e extensões por plataforma ─────────────────────────────────────────
ifeq ($(PLATFORM),Windows)
    # No Windows com MinGW, usamos a lib estática em lib/windows/
    LDFLAGS   = -Llib/windows -lraylib -lopengl32 -lgdi32 -lwinmm -lshell32
    EXT       = .exe
    MKDIR     = if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"
    MKDIR_OBJ = if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"
    RM        = rmdir /s /q
else
    # Linux — usa o raylib instalado no sistema (via pkg-config)
    # Se pkg-config não funcionar, troca por: -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    LDFLAGS   = $(shell pkg-config --libs raylib 2>/dev/null || echo "-lraylib -lGL -lm -lpthread -ldl -lrt -lX11")
    CFLAGS   += $(shell pkg-config --cflags raylib 2>/dev/null)
    EXT       =
    MKDIR     = mkdir -p $(BIN_DIR)
    MKDIR_OBJ = mkdir -p $(OBJ_DIR)
    RM        = rm -rf
endif

# ── Pastas ────────────────────────────────────────────────────────────────────
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# ── Alvos finais ──────────────────────────────────────────────────────────────
TARGET      = $(BIN_DIR)/main$(EXT)
GAME_TARGET = $(BIN_DIR)/jogo$(EXT)

# ── Fontes ────────────────────────────────────────────────────────────────────
SRCS      = $(SRC_DIR)/main.c $(SRC_DIR)/menu.c $(SRC_DIR)/opcoes.c
GAME_SRCS = $(SRC_DIR)/jogo_definitivo.c

OBJS      = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
GAME_OBJS = $(GAME_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# ── Regras ────────────────────────────────────────────────────────────────────
.PHONY: all clean run info

all: info $(TARGET) $(GAME_TARGET)

info:
	@echo ">> Compilando para: $(PLATFORM)"

$(TARGET): $(OBJS)
	$(MKDIR)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo ">> Menu compilado: $(TARGET)"

$(GAME_TARGET): $(GAME_OBJS)
	$(MKDIR)
	$(CC) $(GAME_OBJS) -o $(GAME_TARGET) $(LDFLAGS)
	@echo ">> Jogo compilado: $(GAME_TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(MKDIR_OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR) $(BIN_DIR)

run: all
ifeq ($(PLATFORM),Windows)
	$(BIN_DIR)\main$(EXT)
else
	./$(BIN_DIR)/main$(EXT)
endif