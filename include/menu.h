#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include "raylib.h"

typedef enum telas_de_jogo
{
    TELA_MENU,
    TELA_NOVO_JOGO,
    TELA_RANKING,
    TELA_OPCOES,
    TELA_SAIR
} TELAS_DO_JOGO;

typedef enum animacao
{
    ANIMACAO_CORRENDO,
    ANIMACAO_SUBINDO,
    ANIMACAO_DESCENDO
} ANIMACAO;

typedef struct recursos_menu
{
    Texture2D titulo;
    Texture2D background;
    Texture2D escada;
    Texture2D marioCorrendo;
    Texture2D marioEscalando;
    Texture2D barrilRolando;
    Texture2D iconDonkeyKong;
    Music musicaMenu;

} RECURSOS_MENU;

typedef struct menu
{
    RECURSOS_MENU recursos;
    TELAS_DO_JOGO telaAtual;
    ANIMACAO animacaoAtual;

    int botaoSelecionado;
    int frameInicial;
    int barrilFrame;
    int etapaDescida;
    float temporizadorFrame;
    float temporizadorEscalada;
    float temporizadorFrameBarril;
    Vector2 posicaoMario;
    Vector2 posicaoAlvoEscalada;
    Vector2 posicaoBarril;
    Vector2 cursorMouse;
    Rectangle btnJogar;
    Rectangle btnRanking;
    Rectangle btnOpcoes;
    Rectangle btnSair;
    Rectangle btnVoltar;
    Rectangle btnConfirmarSim;
    Rectangle btnConfirmarNao;

    bool confirmarSaida;
    TELAS_DO_JOGO telaAntesConfirmar;
    int confirmacaoSelecionada;

} MENU;

void iniciarMenu(MENU *menu, int SCREEN_WIDTH, int SCREEN_HEIGHT);
void carregarRecursosMenu(MENU *menu);
void descarregarRecursosMenu(MENU *menu);
void atualizarMenu(MENU *menu, float dt, int SCREEN_WIDTH, int SCREEN_HEIGHT);
void desenharMenu(const MENU *menu, int SCREEN_WIDTH, int SCREEN_HEIGHT);
void solicitarSaidaMenu(MENU *menu);

#endif 
