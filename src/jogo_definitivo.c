// Este programa possui as estruturas e funções principais para a funcionalidade do jogo Donkey Kong.

// Inclusão das bibliotecas em C e header
#include <raylib.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../include/menu.h"
#include "../include/ranking.h"
#include "../include/opcoes.h"

// Declaração das constantes
#define TAMANHO 21                  // Tamanho de renderização dos mapas
#define GRAVIDADE (-50 * TAMANHO)    // Gravidade do pulo do personagem
#define FORCA_DO_PULO (10 * TAMANHO) // Força que o pulo do personagem tem
#define LINHA 30                     // Tamanho da linha do mapa
#define COLUNA 30                    // Tamanho da coluna 30 do mapa
#define QUANTIDADE_MAPAS 3           // Quantidade de mapas a ser carregados

// Estrutura dos objetos
typedef struct objeto
{
    char tipo;  // Tipo do objeto a ser criado. 'Z', 'W'...
    int coluna; // Tamanho de coluna desse objeto
    int linha;  // Tamanho de linha do objeto
} OBJETO;

// Estrura do personagem
typedef struct personagem
{
    double coluna; // Tamanho de coluna do personagem
    double linha;  // Tamanho de linha do personagem
} PERSONAGEM;

// Estrutura do INIMIGO
typedef struct inimigo
{
    double coluna; // Tamanho de coluna do INIMIGO
    double linha;  // Tamanho de coluna do INIMIGO
    char mov;      // O movimento que ele realiza
} INIMIGO;

// Estrutura das imagens
typedef struct textura
{
    Texture2D Plataforma;
    Texture2D Plataformadupla;
    Texture2D Parede;
    Texture2D ParedePlata;
    Texture2D Escada;
    Texture2D Objetivo;
    Texture2D Safezone;
    Texture2D Spawnpoint;
} TEXTURA;

typedef enum resultado_jogo
{
    RESULTADO_SAIU,
    RESULTADO_DERROTA,
    RESULTADO_VITORIA
} RESULTADO_JOGO;

// Variáveis globais estaticas
static char mapa[LINHA][COLUNA + 3]; // Declaração da matriz 30x30 + 3 caracteres para rodar no Windows
static TEXTURA tex;                  // Declaração das imagens
static Texture2D texMarioCorrendo;   // Declaração da animação do personagem correndo
static Texture2D texMarioSubindo;    // Declaração da animação do personagem subindo
static Texture2D texMarioPulando;    // Declaração da animação do personagem pulando
static Texture2D texBarril;          // Declaração da animação do barril
static Texture2D texInimigo;         // Declaração da animação do inimigo
static Texture2D texBordaTelaJogo;   // Declaração da imagem da borda entre os mapas

static PERSONAGEM personagem;             // Declaração do personagem do jogador
static PERSONAGEM spawnpoint;        // Declaração do ponto de inicio de cada mapa
static PERSONAGEM objetivo;          // Declaração do objetivo de cada mapa (a porta)
static OBJETO parede[COLUNA][LINHA]; // Declaração da criação de objetos estáticos

static int chao = 0;
static int escada = 0;
static int invulnerabilidade = 60 * 3;
static double velocidadeY = 0;
static double velocidadeYb = 0;
static double tempo = 0;
static double tempoTotalJogo = 0;
static int morte = 0;
static int vidas = 5;
static int vmax = 29 * TAMANHO;
static bool pausado = false;

static INIMIGO inimigoMorto = {-100, -100, ' '};

static int pontos = 0;
static int contframes = 0;
static int marioDir = 0;

// Carrega todas as imagens do jogo ao iniciar
Texture2D CarregaTex(const char *Path)
{
    Texture2D tex = LoadTexture(Path); // Carrega diretamente para a GPU
    return tex;
}

// Calcula quanto o mapa do jogo precisa ser deslocado no eixo X para ficar centralizado na tela
static float deslocamentoXJogo(void)
{
    return ((float)GetScreenWidth() - (float)(COLUNA * TAMANHO)) / 2.0f;
}

// Calcula quanto o mapa do jogo precisa ser deslocado no eixo X para ficar centralizado na tela
static float deslocamentoYJogo(void)
{
    return ((float)GetScreenHeight() - (float)(LINHA * TAMANHO)) / 2.0f;
}

// Centraliza a tela no centro definidos pelas funções: deslocamentoYJogo e deslocamentoXJogo
static Rectangle centralizaTelaJogo(Rectangle destino)
{
    destino.x += deslocamentoXJogo();
    destino.y += deslocamentoYJogo();
    return destino;
}

// Desenha a borda do jogo
static void desenhaBordaTelaJogo(Texture2D fundo)
{
    if (fundo.id == 0)
        return;

    DrawTexturePro(
        fundo,
        (Rectangle){0.0f, 0.0f, (float)fundo.width, (float)fundo.height},
        (Rectangle){0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight()},
        (Vector2){0.0f, 0.0f},
        0.0f,
        WHITE);
}

// Desenha uma imagem estática redimensionada para o retângulo de destino
void desenhaTex(Texture2D tex, Rectangle destino)
{
    destino = centralizaTelaJogo(destino);

    DrawTexturePro(tex, (Rectangle){0, 0, (float)tex.width, (float)tex.height},
                   destino, (Vector2){0, 0}, 0.0f, WHITE);
}

// Desenha uma imagem animada horizontal redimensionada para o retângulo de destino
void desenhaTexAnimacao(Texture2D tex, Rectangle destino, int contframes, int velocidade, bool inverte)
{
    int numFrames, frame;
    float frameTamanho;

    numFrames = tex.width / tex.height;
    if (numFrames <= 0)
        numFrames = 1;
    frame = (contframes / velocidade) % numFrames;
    frameTamanho = (float)tex.height;

    Rectangle origem = {(float)frame * frameTamanho, 0, frameTamanho, frameTamanho};
    if (inverte)
        origem.width = -origem.width;

    destino = centralizaTelaJogo(destino);

    DrawTexturePro(tex, origem, destino, (Vector2){0, 0}, 0.0f, WHITE);
}

// Retorna o retângulo de desenho padrão (TAMANHO x TAMANHO)
Rectangle GetRect(float x, float y)
{
    return (Rectangle){x, y, TAMANHO, TAMANHO};
}

// Desenha o chão 'Z' dos mapas
void desenhaChao(OBJETO *m)
{
    // Se o personagem esta em cima do quadrado, então teleporta ele para cima e torna o chão verdadeiro
    // Ajustado para ser simétrico e mais natural (personagem físico = 0.7 do TAMANHO)
    if (personagem.linha >= TAMANHO * (m->linha - 1) && personagem.linha <= TAMANHO * (m->linha - 0.3) && personagem.coluna <= TAMANHO * (m->coluna + 0.7) && personagem.coluna >= TAMANHO * (m->coluna - 0.7))
    {
        personagem.linha = TAMANHO * (m->linha - 1);
        chao = 1;
        velocidadeY = 0;
    }

    // Se o personagem tenta entrar por baixo da caixa, ele é teletransportado de volta pra baixo
    if (personagem.linha >= TAMANHO * (m->linha - 0.3) && personagem.linha <= TAMANHO * (m->linha - 0.2) && personagem.coluna <= TAMANHO * (m->coluna + 0.6667) && personagem.coluna >= TAMANHO * (m->coluna - 0.3334))
    {
        personagem.linha = TAMANHO * (m->linha);
    }
    // se ele tentar entrar na caixa pela esquerda ele volta pro lugar dele
    if (personagem.linha >= TAMANHO * (m->linha - 0.9999999) && personagem.linha <= TAMANHO * (m->linha + 0.5) && personagem.coluna <= TAMANHO * (m->coluna) && personagem.coluna >= TAMANHO * (m->coluna - 0.6667))
    {
        personagem.coluna = TAMANHO * (m->coluna - 0.6667);
    }

    // ao contrario, direita
    if (personagem.linha >= TAMANHO * (m->linha - 0.9999999) && personagem.linha <= TAMANHO * (m->linha + 0.5) && personagem.coluna <= TAMANHO * (m->coluna + 0.6667) && personagem.coluna >= TAMANHO * (m->coluna))
    {
        personagem.coluna = TAMANHO * (m->coluna + 0.6667);
    }
}

// Desenha as paredes 'W' dos mapas
void desenhaParede(OBJETO *m)
{
    // Se ele tentar entrar na caixa(texParede) pela esquerda ele volta pro lugar dele
    if (personagem.linha >= TAMANHO * (m->linha - 0.9999999) && personagem.linha <= TAMANHO * (m->linha + 0.5) && personagem.coluna <= TAMANHO * (m->coluna) && personagem.coluna >= TAMANHO * (m->coluna - 0.6667))
    {
        personagem.coluna = TAMANHO * (m->coluna - 0.6667);
    }
    // ao contrario, direita
    if (personagem.linha >= TAMANHO * (m->linha - 0.9999999) && personagem.linha <= TAMANHO * (m->linha + 0.5) && personagem.coluna <= TAMANHO * (m->coluna + 0.6667) && personagem.coluna >= TAMANHO * (m->coluna))
    {
        personagem.coluna = TAMANHO * (m->coluna + 0.6667);
    }
}

// Desenha as escadas 'H' dos mapas
void desenhaEscada(OBJETO *m)
{
    // espaço em que o personagem pode se movimentar conforme o jogador controlar
    if (personagem.linha >= TAMANHO * (m->linha - 1) && personagem.linha <= TAMANHO * (m->linha + 1) && personagem.coluna <= TAMANHO * (m->coluna + 0.6667) && personagem.coluna >= TAMANHO * (m->coluna - 0.6667))
    {
        escada = 1;
        chao = 0;
        velocidadeY = 0;
    }
}

// Desenha a zona segura (desenhaSafeZone) 'N' dos mapas
void desenhaSafeZone(OBJETO *m)
{
    // Se entrar nele, a posição some, mas salva o spawpoint nessa posição
    if ((personagem.linha >= TAMANHO * (m->linha - 1)) && (personagem.linha <= TAMANHO * (m->linha + 1)) && (personagem.coluna <= TAMANHO * (m->coluna + 0.7)) && personagem.coluna >= TAMANHO * (m->coluna - 0.7))
    {
        spawnpoint.linha = TAMANHO * m->linha;
        spawnpoint.coluna = TAMANHO * m->coluna;
        mapa[m->linha][m->coluna] = ' ';
    }
}

// Desenha o inimigo 'E' dos mapas
void desenhaInimigo(INIMIGO *m, Texture2D *tex)
{
    // pisou matou
    if (personagem.linha >= (m->linha - (TAMANHO * 1.2)) && personagem.linha <= (m->linha - (TAMANHO * 0.5)) && personagem.coluna <= (m->coluna + TAMANHO * 0.7) && personagem.coluna >= (m->coluna - TAMANHO * 0.7))
    {
        velocidadeY = (FORCA_DO_PULO * 0.65);
        *m = inimigoMorto; // inimigo morto == constante, quando é pisado, ele é teleportado para fora de onde o mapa mostra
        pontos += 1000;     // acrescenta pontos
    }

    // encostou morreu
    else if ((invulnerabilidade < 0) && personagem.linha >= (m->linha - (TAMANHO * 0.8)) && personagem.linha <= (m->linha + (TAMANHO * 0.5)) && personagem.coluna <= (m->coluna + (TAMANHO * 0.7)) && personagem.coluna >= (m->coluna - (TAMANHO * 0.7)))
    {
        vidas--;
        personagem = spawnpoint;
        invulnerabilidade = (60 * 3); // frames por segundo vezes 3, 3 segundos invuneravel
    }
    // mov dir ou troca
    // pergunta se o movimento é para x lado, ve se tem chao ou parede, se tiver ele muda de direção, se não segue em frente tal qual um robo aspirador

    if (!pausado)
    {
        if (m->mov == 'D')
        {
            int c = (int)(m->coluna / TAMANHO);
            int l = (int)(m->linha / TAMANHO);
            if (mapa[l][c + 1] == ' ' && (mapa[l + 1][c + 1] == 'Z' || mapa[l + 1][c + 1] == 'Y'))
            {
                m->coluna += (0.05 * TAMANHO);
            }
            else
                m->mov = 'E';
        }
        // mov esq ou troca
        // mesma coisa do de cima
        else if (m->mov == 'E')
        {
            int c = (int)(m->coluna / TAMANHO);
            int l = (int)(m->linha / TAMANHO);
            if ((m->coluna > 0) && mapa[l][c] == ' ' && (mapa[l + 1][c] == 'Z' || mapa[l + 1][c] == 'Y'))
            {
                m->coluna -= (0.05 * TAMANHO);
            }
            else
                m->mov = 'D';
        }
    }

    // desenha
    bool inverte = (m->mov == 'E');
    desenhaTexAnimacao(*tex, GetRect(m->coluna, m->linha), contframes, 10, inverte);
}

// Desenha os objetos do jogo (parede, chão, etc.)
void desenhaObjeto(OBJETO *m, TEXTURA *tex)
{
    if (m->tipo == ' ')
    {
        // precisa fazer nada
    }
    // se for chao
     else if (m->tipo == 'X')
    {
        desenhaTex(tex->Plataformadupla, (Rectangle){m->coluna * TAMANHO, m->linha * TAMANHO, TAMANHO, TAMANHO});
        desenhaChao(m);
    }
    else if (m->tipo == 'Y')
    {
        desenhaTex(tex->ParedePlata, (Rectangle){m->coluna * TAMANHO, m->linha * TAMANHO, TAMANHO, TAMANHO});
        desenhaChao(m);
    }
    else if (m->tipo == 'Z')
    {
        desenhaTex(tex->Plataforma, (Rectangle){m->coluna * TAMANHO, m->linha * TAMANHO, TAMANHO, TAMANHO});
        desenhaChao(m);
    }
    // se for parede
    else if (m->tipo == 'W')
    {
        desenhaTex(tex->Parede, (Rectangle){m->coluna * TAMANHO, m->linha * TAMANHO, TAMANHO, TAMANHO});
        desenhaParede(m);
    }
    // se for escada
    else if (m->tipo == 'D' || m->tipo == 'H' || m->tipo == 'S')
    {

        desenhaTex(tex->Escada, (Rectangle){m->coluna * TAMANHO, m->linha * TAMANHO, TAMANHO, TAMANHO});
        desenhaEscada(m);
    }
    // se for o objetivo
    else if (m->tipo == 'F')
    {

        desenhaTex(tex->Objetivo, (Rectangle){m->coluna * TAMANHO, m->linha * TAMANHO, TAMANHO, TAMANHO});
        objetivo.coluna = m->coluna * TAMANHO;
        objetivo.linha = m->linha * TAMANHO;
    }
    // se for uma safezone
    else if (m->tipo == 'N')
    {
        desenhaTex(tex->Safezone, (Rectangle){m->coluna * TAMANHO, m->linha * TAMANHO, TAMANHO, TAMANHO});
        desenhaSafeZone(m);
    }
}


static bool PontoClicado(Rectangle botao, Vector2 mouse)
{
    return CheckCollisionPointRec(mouse, botao) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void DesenhaBotaoJogo(Rectangle botao, const char *texto, bool selecionado, Vector2 mouse)
{
    Color cor = (selecionado || CheckCollisionPointRec(mouse, botao)) ? LIGHTGRAY : GRAY;
    DrawRectangleRec(botao, cor);
    DrawRectangleLinesEx(botao, 2, selecionado ? RED : DARKGRAY);
    DrawText(texto,
             (int)(botao.x + botao.width / 2.0f - MeasureText(texto, 20) / 2.0f),
             (int)(botao.y + botao.height / 2.0f - 10.0f),
             20,
             BLACK);
}

// Desenha confirmação de saida
static void desenhaConfirmarSaida(Vector2 mouse, int selecionado, Rectangle btnSim, Rectangle btnNao)
{
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    DrawRectangle(0, 0, sw, sh, (Color){0, 0, 0, 170});

    Rectangle telaSair = {sw / 2.0f - 230.0f, sh / 2.0f - 95.0f, 460.0f, 190.0f};
    DrawRectangleRec(telaSair, (Color){25, 25, 25, 245});
    DrawRectangleLinesEx(telaSair, 3, RED);

    const char *txt = "Deseja mesmo sair?";
    DrawText(txt, sw / 2 - MeasureText(txt, 28) / 2, (int)telaSair.y + 35, 28, WHITE);

    DesenhaBotaoJogo(btnSim, "SIM", selecionado == 0, mouse);
    DesenhaBotaoJogo(btnNao, "NÃO", selecionado == 1, mouse);
}

static int AtualizaPopupSairJogo(Vector2 mouse, int *selecionado, Rectangle btnSim, Rectangle btnNao)
{
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
        *selecionado = 1 - *selecionado;

    if (CheckCollisionPointRec(mouse, btnSim))
        *selecionado = 0;
    else if (CheckCollisionPointRec(mouse, btnNao))
        *selecionado = 1;

    if (IsKeyPressed(KEY_ESCAPE))
        return 0;

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
        return (*selecionado == 0) ? 1 : 0;

    if (PontoClicado(btnSim, mouse))
        return 1;
    if (PontoClicado(btnNao, mouse))
        return 0;

    return -1;
}

static void DesenhaSliderFinal(float x, float y, float largura, float valor, bool selecionado)
{
    DrawRectangle((int)x, (int)y, (int)largura, 20, selecionado ? RED : RAYWHITE);
    DrawRectangle((int)x + 2, (int)y + 2, (int)((largura - 4) * valor), 16, BLACK);
}

static void LimitaFloat01(float *valor)
{
    if (*valor < 0.0f)
        *valor = 0.0f;
    if (*valor > 1.0f)
        *valor = 1.0f;
}

static bool MostrarTelaFinalERegistrar(RESULTADO_JOGO resultado)
{
    if (resultado == RESULTADO_SAIU)
        return false;

    typedef enum tela_final
    {
        FINAL_NOME,
        FINAL_VOLTAR,
        FINAL_OPCOES
    } TELA_FINAL;

    TELA_FINAL tela = FINAL_NOME;
    char nome[TAM_NOME_PLACAR] = "";
    int tamanhoNome = 0;
    int opcaoSelecionada = 1;
    int opcaoOpcoes = 0;
    bool confirmarSaida = false;
    int popupSelecionado = 1;
    bool placarSalvo = false;

    Rectangle btnSalvar = {GetScreenWidth() / 2.0f - 110.0f, 420.0f, 220.0f, 45.0f};
    Rectangle btnOpcoes = {GetScreenWidth() / 2.0f - 270.0f, 455.0f, 160.0f, 45.0f};
    Rectangle btnVoltar = {GetScreenWidth() / 2.0f - 80.0f, 455.0f, 160.0f, 45.0f};
    Rectangle btnSair = {GetScreenWidth() / 2.0f + 110.0f, 455.0f, 160.0f, 45.0f};
    Rectangle btnVoltarOpcoes = {GetScreenWidth() / 2.0f - 100.0f, 500.0f, 200.0f, 45.0f};
    Rectangle btnSim = {GetScreenWidth() / 2.0f - 130.0f, GetScreenHeight() / 2.0f + 35.0f, 110.0f, 40.0f};
    Rectangle btnNao = {GetScreenWidth() / 2.0f + 20.0f, GetScreenHeight() / 2.0f + 35.0f, 110.0f, 40.0f};

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        if (confirmarSaida)
        {
            int resposta = AtualizaPopupSairJogo(mouse, &popupSelecionado, btnSim, btnNao);
            if (resposta == 1)
                return true;
            if (resposta == 0)
                confirmarSaida = false;
        }
        else
        {
            if (IsKeyPressed(KEY_ESCAPE))
            {
                confirmarSaida = true;
                popupSelecionado = 1;
            }

            if (tela == FINAL_NOME)
            {
                int tecla = GetCharPressed();
                while (tecla > 0)
                {
                    if (tecla >= 32 && tecla <= 125 && tamanhoNome < TAM_NOME_PLACAR - 1)
                    {
                        nome[tamanhoNome] = (char)tecla;
                        tamanhoNome++;
                        nome[tamanhoNome] = '\0';
                    }
                    tecla = GetCharPressed();
                }

                if (IsKeyPressed(KEY_BACKSPACE) && tamanhoNome > 0)
                {
                    tamanhoNome--;
                    nome[tamanhoNome] = '\0';
                }

                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || PontoClicado(btnSalvar, mouse))
                {
                    if (tamanhoNome == 0)
                        strcpy(nome, "JOGADOR");

                    placarSalvo = salvarPontuacaoRanking(nome, pontos, (float)tempoTotalJogo) != 0;
                    tela = FINAL_VOLTAR;
                    opcaoSelecionada = 1;
                }
            }
            else if (tela == FINAL_VOLTAR)
            {
                if (CheckCollisionPointRec(mouse, btnOpcoes))
                    opcaoSelecionada = 0;
                else if (CheckCollisionPointRec(mouse, btnVoltar))
                    opcaoSelecionada = 1;
                else if (CheckCollisionPointRec(mouse, btnSair))
                    opcaoSelecionada = 2;

                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
                    opcaoSelecionada = (opcaoSelecionada - 1 + 3) % 3;
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
                    opcaoSelecionada = (opcaoSelecionada + 1) % 3;

                bool confirmar = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
                if (confirmar)
                {
                    if (opcaoSelecionada == 0 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || PontoClicado(btnOpcoes, mouse)))
                        tela = FINAL_OPCOES;
                    else if (opcaoSelecionada == 1 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || PontoClicado(btnVoltar, mouse)))
                        return false;
                    else if (opcaoSelecionada == 2 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || PontoClicado(btnSair, mouse)))
                    {
                        confirmarSaida = true;
                        popupSelecionado = 1;
                    }
                }
            }
            else if (tela == FINAL_OPCOES)
            {
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
                    opcaoOpcoes = (opcaoOpcoes + 1) % 4;
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
                    opcaoOpcoes = (opcaoOpcoes - 1 + 4) % 4;

                float volMusica = obterVolumeMusica();
                float volEfeitos = obterVolumeEfeitosSonoros();
                bool musica = obterMusicaLigada();

                if (opcaoOpcoes == 0)
                {
                    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
                        volMusica += 0.01f;
                    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
                        volMusica -= 0.01f;
                    LimitaFloat01(&volMusica);
                    definirVolumeMusica(volMusica);
                }
                else if (opcaoOpcoes == 1)
                {
                    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
                        volEfeitos += 0.01f;
                    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
                        volEfeitos -= 0.01f;
                    LimitaFloat01(&volEfeitos);
                    definirVolumeEfeitosSonoros(volEfeitos);
                }

                if (PontoClicado((Rectangle){180, 250, 400, 20}, mouse))
                {
                    volMusica = (mouse.x - 180.0f) / 400.0f;
                    LimitaFloat01(&volMusica);
                    definirVolumeMusica(volMusica);
                    opcaoOpcoes = 0;
                }

                if (PontoClicado((Rectangle){180, 340, 400, 20}, mouse))
                {
                    volEfeitos = (mouse.x - 180.0f) / 400.0f;
                    LimitaFloat01(&volEfeitos);
                    definirVolumeEfeitosSonoros(volEfeitos);
                    opcaoOpcoes = 1;
                }

                if ((opcaoOpcoes == 2 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))) ||
                    PontoClicado((Rectangle){180, 400, 250, 40}, mouse))
                {
                    definirMusicaLigada(!musica);
                    opcaoOpcoes = 2;
                }

                if ((opcaoOpcoes == 3 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))) ||
                    PontoClicado(btnVoltarOpcoes, mouse))
                {
                    tela = FINAL_VOLTAR;
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (tela == FINAL_NOME)
        {
            const char *titulo = (resultado == RESULTADO_VITORIA) ? "VITÓRIA!" : "DERROTA!";
            Color corTitulo = (resultado == RESULTADO_VITORIA) ? GREEN : RED;
            DrawText(titulo, GetScreenWidth() / 2 - MeasureText(titulo, 46) / 2, 95, 46, corTitulo);

            char info[128];
            sprintf(info, "Pontos: %d   Tempo: %.2fs", pontos, tempoTotalJogo);
            DrawText(info, GetScreenWidth() / 2 - MeasureText(info, 24) / 2, 170, 24, WHITE);

            const char *textoNome = "Digite seu nome para salvar no ranking:";
            DrawText(textoNome, GetScreenWidth() / 2 - MeasureText(textoNome, 22) / 2, 240, 22, WHITE);
            Rectangle caixaNome = {GetScreenWidth() / 2.0f - 180.0f, 285.0f, 360.0f, 50.0f};
            DrawRectangleRec(caixaNome, RAYWHITE);
            DrawRectangleLinesEx(caixaNome, 3, RED);
            DrawText(nome, (int)caixaNome.x + 12, (int)caixaNome.y + 14, 22, BLACK);

            const char *ajuda = "ENTER salva. Backspace apaga.";
            DrawText(ajuda, GetScreenWidth() / 2 - MeasureText(ajuda, 18) / 2, 350, 18, LIGHTGRAY);
            DesenhaBotaoJogo(btnSalvar, "SALVAR PLACAR", false, mouse);
        }
        else if (tela == FINAL_VOLTAR)
        {
            const char *titulo = placarSalvo ? "PLACAR SALVO!" : "NÃO FOI POSSÍVEL SALVAR O PLACAR";
            DrawText(titulo, GetScreenWidth() / 2 - MeasureText(titulo, 34) / 2, 120, 34, placarSalvo ? GREEN : RED);

            char info[160];
            sprintf(info, "Nome: %s   Pontos: %d   Tempo: %.2fs", nome, pontos, tempoTotalJogo);
            DrawText(info, GetScreenWidth() / 2 - MeasureText(info, 22) / 2, 200, 22, WHITE);

            const char *pergunta = "O que deseja fazer agora?";
            DrawText(pergunta, GetScreenWidth() / 2 - MeasureText(pergunta, 24) / 2, 300, 24, WHITE);
            DesenhaBotaoJogo(btnOpcoes, "OPÇÕES", opcaoSelecionada == 0, mouse);
            DesenhaBotaoJogo(btnVoltar, "VOLTAR MENU", opcaoSelecionada == 1, mouse);
            DesenhaBotaoJogo(btnSair, "SAIR", opcaoSelecionada == 2, mouse);
        }
        else if (tela == FINAL_OPCOES)
        {
            const char *titulo = "OPÇÕES";
            DrawText(titulo, GetScreenWidth() / 2 - MeasureText(titulo, 38) / 2, 80, 38, WHITE);
            DrawText("Use W/S para selecionar e <-/-> para ajustar.", 150, 150, 20, WHITE);

            DrawText("Volume da Música", 180, 220, 22, WHITE);
            DesenhaSliderFinal(180, 250, 400, obterVolumeMusica(), opcaoOpcoes == 0);

            DrawText("Efeitos Sonoros", 180, 310, 22, WHITE);
            DesenhaSliderFinal(180, 340, 400, obterVolumeEfeitosSonoros(), opcaoOpcoes == 1);

            char textoMusica[64];
            sprintf(textoMusica, "Música: %s", obterMusicaLigada() ? "LIGADA" : "DESLIGADA");
            DesenhaBotaoJogo((Rectangle){180, 400, 250, 40}, textoMusica, opcaoOpcoes == 2, mouse);
            DesenhaBotaoJogo(btnVoltarOpcoes, "VOLTAR", opcaoOpcoes == 3, mouse);
        }

        if (confirmarSaida)
            desenhaConfirmarSaida(mouse, popupSelecionado, btnSim, btnNao);

        EndDrawing();
    }

    return true;
}

bool executarJogo(void)
{
    // 2. Carrega as texturas APENAS UMA VEZ
    // Elas serão redimensionadas automaticamente para TAMANHO x TAMANHO pela função CarregaTex

    printf("1 - Texturas carregadas\n");
    fflush(stdout);
    texMarioCorrendo = CarregaTex("assets/images/sprites/spriteMarioCorrendo.png");
    texMarioSubindo = CarregaTex("assets/images/sprites/spriteMarioSubindo.png");
    texMarioPulando = CarregaTex("assets/images/sprites/spriteMarioPulando.png");
    texInimigo = CarregaTex("assets/images/sprites/spriteInimigoFogo.png");
    texBarril = CarregaTex("assets/images/sprites/spriteBarrilRolando.png");

    texBordaTelaJogo = CarregaTex("assets/images/cenario/bordaTelaJogo.png");

    tex = (TEXTURA){
        CarregaTex("assets/images/cenario/plataformaRosaUnico.png"),
        CarregaTex("assets/images/cenario/plataformadupla.png"),
        CarregaTex("assets/images/cenario/paredeBlocoUnico.png"),
        CarregaTex("assets/images/cenario/platafotmaRozacomparede.png"),
        CarregaTex("assets/images/cenario/escadaMenor.png"),
        CarregaTex("assets/images/cenario/porta.png"),
        CarregaTex("assets/images/sprites/spriteBandeiraSavePoint.png"),
        CarregaTex("assets/images/sprites/spriteBandeiraSavePointConquistado.png")};

    printf("5 - Abrindo mapas\n");
    fflush(stdout);
    srand(time(NULL));

    FILE *arqmapas[QUANTIDADE_MAPAS];
    arqmapas[0] = fopen("mapas/mapa_1.txt", "r");
    arqmapas[1] = fopen("mapas/mapa_2.txt", "r");
    arqmapas[2] = fopen("mapas/mapa_3.txt", "r");
    // arqmapas[3] = fopen("mapas/mapa_teste.txt", "r");
    // arqmapas[4] = fopen("mapas/mapa_teste.txt", "r");

    for (int k = 0; k < QUANTIDADE_MAPAS; k++)
    {
        if (arqmapas[k] == NULL)
        {
            printf("Erro ao abrir o arquivo do mapa %d!\n", k);
            return false;
        }
    }
    printf("6 - Mapas OK\n");
    fflush(stdout);

    morte = 0;
    vidas = 5;
    pontos = 0;
    tempoTotalJogo = 0.0;
    velocidadeY = 0;
    velocidadeYb = 0;
    pausado = false;

    RESULTADO_JOGO resultadoJogo = RESULTADO_SAIU;
    bool sairPrograma = false;

    printf("7 - Entrando no loop principal\n");
    fflush(stdout);
    // 3. Loop dos mapas
    for (int i = 0; ((i < QUANTIDADE_MAPAS) && !morte); i++)
    {
        for (int j = 0; j < LINHA; j++)
        {
            fgets(mapa[j], COLUNA + 3, arqmapas[i]);
        }

        int quant_inimigos = 0;
        INIMIGO inimigos[51] = {};

        for (int l = 0; l < LINHA; l++)
        {
            for (int c = 0; c < COLUNA; c++)
            {
                if (mapa[l][c] == 'P')
                {
                    spawnpoint.coluna = c * TAMANHO;
                    spawnpoint.linha = l * TAMANHO;
                    mapa[l][c] = ' ';
                }
                else if (mapa[l][c] == 'E')
                {
                    inimigos[quant_inimigos].coluna = c * TAMANHO;
                    inimigos[quant_inimigos].linha = l * TAMANHO;
                    if (!(quant_inimigos % 2))
                        inimigos[quant_inimigos].mov = 'D';
                    else
                        inimigos[quant_inimigos].mov = 'E';
                    mapa[l][c] = ' ';
                    quant_inimigos++;
                }
            }
        }

        personagem = spawnpoint;
        int probabilidade = 2;
        PERSONAGEM bomba[1000] = {};
        bool confirmarSaidaJogo = false;
        int popupSelecionado = 1;
        Rectangle btnSimPopup = {GetScreenWidth() / 2.0f - 130.0f, GetScreenHeight() / 2.0f + 35.0f, 110.0f, 40.0f};
        Rectangle btnNaoPopup = {GetScreenWidth() / 2.0f + 20.0f, GetScreenHeight() / 2.0f + 35.0f, 110.0f, 40.0f};

        // 4. Loop da fase (do-while)
        do
        {
            tempo = GetFrameTime();
            PERSONAGEM mario_referencial = personagem;
            Vector2 mouseJogo = GetMousePosition();

            if (!confirmarSaidaJogo && (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose()))
            {
                confirmarSaidaJogo = true;
                popupSelecionado = 1;
            }

            if (confirmarSaidaJogo)
            {
                int respostaPopup = AtualizaPopupSairJogo(mouseJogo, &popupSelecionado, btnSimPopup, btnNaoPopup);
                if (respostaPopup == 1)
                {
                    sairPrograma = true;
                    resultadoJogo = RESULTADO_SAIU;
                    morte = 1;
                    break;
                }
                else if (respostaPopup == 0)
                {
                    confirmarSaidaJogo = false;
                }
            }

            if (!confirmarSaidaJogo && IsKeyPressed(KEY_TAB))
                pausado = !pausado;

            if (!pausado && !confirmarSaidaJogo)
            {
                if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
                {
                    personagem.coluna += (0.16 * TAMANHO);
                    marioDir = 0;
                }
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
                {
                    personagem.coluna -= (0.16 * TAMANHO);
                    marioDir = 1;
                }
                if ((IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) && (escada == 1))
                    personagem.linha -= (0.06 * TAMANHO);
                if ((IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) && (escada == 1))
                    personagem.linha += (0.06 * TAMANHO);
                if ((IsKeyDown(KEY_SPACE)) && (chao == 1))
                    velocidadeY = FORCA_DO_PULO;

                if ((chao == 0) && (escada == 0))
                    velocidadeY += GRAVIDADE * tempo;

                personagem.linha -= velocidadeY * tempo;

                if (velocidadeY < -vmax)
                    velocidadeY = -vmax;
                if (velocidadeYb < -vmax)
                    velocidadeYb = -vmax;

                invulnerabilidade--;
                probabilidade++;
                velocidadeYb += GRAVIDADE * tempo;
                contframes++;
                tempoTotalJogo += tempo;
            }

            // Reposicionamento se sair das bordas
            if (personagem.coluna >= TAMANHO * (COLUNA - 0.6667))
                personagem.coluna = TAMANHO * (COLUNA - 0.6667);
            else if (personagem.coluna <= TAMANHO * (-0.3334))
                personagem.coluna = TAMANHO * (-0.3334);

            chao = 0;
            escada = 0;

            BeginDrawing();
            ClearBackground(BLACK);
            desenhaBordaTelaJogo(texBordaTelaJogo);

            // 1. Desenha Cenário
            for (int l = 0; l < LINHA; l++)
            {
                for (int c = 0; c < COLUNA; c++)
                {
                    parede[c][l].tipo = mapa[l][c];
                    parede[c][l].coluna = c;
                    parede[c][l].linha = l;
                    desenhaObjeto(&parede[c][l], &tex);
                }
            }

            // 2. Desenha Inimigos
            bool pausadoOriginal = pausado;
            if (confirmarSaidaJogo)
                pausado = true;

            for (int k = 0; k < quant_inimigos; k++)
            {
                desenhaInimigo(&inimigos[k], &texInimigo);
            }

            // 3. Lógica e Desenho de Bombas
            if (i != (QUANTIDADE_MAPAS - 1))
                for (int x = 0; x < 999; x++)
                {
                    if (probabilidade < ((x + 1) * (66 * (QUANTIDADE_MAPAS - 1 - i))))
                        break;
                    else if (!(probabilidade % ((x + 1) * (66 * (QUANTIDADE_MAPAS - 1 - i)))))
                    {
                        bomba[x].coluna = (rand() % (COLUNA * TAMANHO));
                        bomba[x].linha = (2 * TAMANHO);
                    }
                }
            else
                for (int x = 2; x < 999; x++)
                {
                    if (!(probabilidade % ((x + 1) * (60 * (QUANTIDADE_MAPAS - i)))))
                    {
                        bomba[x].coluna = (rand() % (COLUNA * TAMANHO));
                        bomba[x].linha = (3 * TAMANHO);
                        if (!pausado)
                            velocidadeYb = 10 * TAMANHO;
                    }
                }

            if (!(probabilidade % (1000 * (66 * (QUANTIDADE_MAPAS - i)))))
            {
                bomba[999].coluna = personagem.coluna;
                bomba[999].linha = (2 * TAMANHO);
                probabilidade = 0;
            }

            for (int z = 0; z < 1000; z++)
            {
                if (!pausado)
                    bomba[z].linha -= velocidadeYb * tempo * 0.7;

                desenhaTexAnimacao(texBarril, GetRect(bomba[z].coluna, bomba[z].linha), contframes, 10, false);

                if ((invulnerabilidade < 0) && (personagem.linha >= (bomba[z].linha - TAMANHO)) && (personagem.linha <= (bomba[z].linha + TAMANHO)) && (personagem.coluna <= (bomba[z].coluna + (TAMANHO * 0.7))) && personagem.coluna >= (bomba[z].coluna - (TAMANHO * 0.7)))
                {
                    personagem = spawnpoint;
                    vidas--;
                    invulnerabilidade = (60 * 3);
                }
            }

            pausado = pausadoOriginal;

            desenhaTex(tex.Spawnpoint, GetRect(spawnpoint.coluna, spawnpoint.linha));

            // 4. Desenha personagem
            Rectangle marioRect = GetRect(personagem.coluna, personagem.linha);
            if (chao && personagem.coluna > mario_referencial.coluna) // andando para a direita
                desenhaTexAnimacao(texMarioCorrendo, marioRect, contframes, 10, false);
            else if (chao && personagem.coluna < mario_referencial.coluna) // andando para a esquerda
                desenhaTexAnimacao(texMarioCorrendo, marioRect, contframes, 10, true);
            else if (chao && personagem.coluna == mario_referencial.coluna) // parado
                desenhaTexAnimacao(texMarioCorrendo, marioRect, 0, 1, marioDir == 1);
            else if (escada && personagem.linha != mario_referencial.linha) // subindo ou descendo a escada
                desenhaTexAnimacao(texMarioSubindo, marioRect, contframes, 10, false);
            else if (escada && personagem.linha == mario_referencial.linha) // parado na escada
                desenhaTexAnimacao(texMarioSubindo, marioRect, 0, 1, false);
            else if (!chao && !escada && personagem.coluna > mario_referencial.coluna) // pulando para a direita
                desenhaTexAnimacao(texMarioPulando, marioRect, contframes, 10, false);
            else if (!chao && !escada && personagem.coluna < mario_referencial.coluna) // pulando para a esquerda
                desenhaTexAnimacao(texMarioPulando, marioRect, contframes, 10, true);
            else // pulando sem se mover
                desenhaTexAnimacao(texMarioPulando, marioRect, contframes, 10, marioDir == 1);

            char hud[128];
            sprintf(hud, "Vidas: %d   Pontos: %d   Tempo: %.1fs", vidas, pontos, tempoTotalJogo);
            DrawText(hud, (int)deslocamentoXJogo() + 10, (int)deslocamentoYJogo() + 10, 18, WHITE);

            if (pausado && !confirmarSaidaJogo)
            {
                const char *txtPausa = "PAUSADO - TAB para continuar";
                DrawText(txtPausa, GetScreenWidth() / 2 - MeasureText(txtPausa, 24) / 2, TAMANHO * LINHA * 9 / 17, 24, YELLOW);
                
            }

            if (confirmarSaidaJogo)
                desenhaConfirmarSaida(mouseJogo, popupSelecionado, btnSimPopup, btnNaoPopup);

            EndDrawing();

            // 5. Lógica de saída/morte/passagem de fase USANDO BREAK
            if (vidas < 1)
            {
                resultadoJogo = RESULTADO_DERROTA;
                morte = 1;
                break;
            }

            if ((personagem.linha >= (objetivo.linha - TAMANHO)) && (personagem.linha <= (objetivo.linha + (TAMANHO * 0.5))) && (personagem.coluna <= (objetivo.coluna + (TAMANHO * 0.6667))) && personagem.coluna >= (objetivo.coluna - (TAMANHO * 0.3334)))
            {
                pontos += (3000 * (i + 1));

                if (i == (QUANTIDADE_MAPAS - 1))
                {
                    resultadoJogo = RESULTADO_VITORIA;
                    morte = 1;
                }

                break;
            }

        } while (!morte);
    }

    // 6. Limpeza final
    for (int k = 0; k < QUANTIDADE_MAPAS; k++)
        if (arqmapas[k])
            fclose(arqmapas[k]);

    UnloadTexture(texMarioCorrendo);
    UnloadTexture(texMarioSubindo);
    UnloadTexture(texMarioPulando);
    UnloadTexture(texInimigo);
    UnloadTexture(texBarril);
    if (texBordaTelaJogo.id != 0)
        UnloadTexture(texBordaTelaJogo);
    UnloadTexture(tex.Plataforma);
    UnloadTexture(tex.Plataformadupla);
    UnloadTexture(tex.Parede);
    UnloadTexture(tex.ParedePlata);
    UnloadTexture(tex.Escada);
    UnloadTexture(tex.Objetivo);
    UnloadTexture(tex.Safezone);
    UnloadTexture(tex.Spawnpoint);


    if (!sairPrograma && (resultadoJogo == RESULTADO_VITORIA || resultadoJogo == RESULTADO_DERROTA))
        sairPrograma = MostrarTelaFinalERegistrar(resultadoJogo);

    return sairPrograma;
}