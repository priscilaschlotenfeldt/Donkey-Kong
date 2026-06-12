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
ifeq ($(origin CC),default)
    CC = gcc
endif

# ── Flags comuns ─────────────────────────────────────────────────────────────
CFLAGS = -Wall -std=c99 -g -Iinclude

# ── Flags e extensões por plataforma ─────────────────────────────────────────
ifeq ($(PLATFORM),Windows)
    SHELL     = cmd.exe
    # No Windows com MinGW, usamos a lib estática em lib/windows/ e forçamos link estático
    LDFLAGS   = -static -Llib/windows -lraylib -lopengl32 -lgdi32 -lwinmm -lshell32
    EXT       = .exe
    MKDIR_OBJ = if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"
    RM        = rmdir /s /q
else
    # Linux — usa o raylib instalado no sistema (via pkg-config)
    LDFLAGS   = $(shell pkg-config --libs raylib 2>/dev/null || echo "-lraylib -lGL -lm -lpthread -ldl -lrt -lX11")
    CFLAGS   += $(shell pkg-config --cflags raylib 2>/dev/null)
    EXT       =
    MKDIR_OBJ = mkdir -p $(OBJ_DIR)
    RM        = rm -rf
endif

# ── Pastas ────────────────────────────────────────────────────────────────────
SRC_DIR = src
OBJ_DIR = obj

# ── Alvos finais ──────────────────────────────────────────────────────────────
TARGET      = main$(EXT)
GAME_TARGET = jogo$(EXT)

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
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo ">> Menu compilado: $(TARGET)"

$(GAME_TARGET): $(GAME_OBJS)
	$(CC) $(GAME_OBJS) -o $(GAME_TARGET) $(LDFLAGS)
	@echo ">> Jogo compilado: $(GAME_TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(MKDIR_OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
ifeq ($(PLATFORM),Windows)
	@if exist $(OBJ_DIR) $(RM) $(OBJ_DIR)
	@if exist main$(EXT) del /q main$(EXT)
	@if exist jogo$(EXT) del /q jogo$(EXT)
else
	@rm -rf $(OBJ_DIR) main jogo
endif

run: all
ifeq ($(PLATFORM),Windows)
	.\main$(EXT)
else
	./main$(EXT)
endif