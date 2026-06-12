#ifdef _WIN32//isso aqui olha se o seu compilador é do padrão win
#define WIN32_LEAN_AND_MEAN//daqui para baixo é só para não haver nenhum conflito entre coisas do win e do raylib
#define NOGDI
#define NOUSER
#include <windows.h>
#include <shellapi.h>
#else
#include <stdlib.h>
#endif

#include <raylib.h>// colocar "" se não for
#include "menu.h"
#include "opcoes.h"

#define FRAMES_MARIO_CORRENDO 4
#define FRAMES_MARIO_ESCALANDO 3
#define FRAMES_BARRIL 4
#define FRAMES_DURACAO 0.1f
#define DURACAO_ESCALAR 8.0f
#define VELOCIDADE_MARIO_MENU 150.0f
#define VELOCIDADE_SUBIR 100.0f
#define VELOCIDADE_DESCER 100.0f
#define VELOCIDADE_BARRIL 140.0f
#define INICIO_X_SUBIR 760.0f
#define TOPO_X_ESCADA 25.0f
#define TOPO_Y_ESCADA 5.0f


void iniciarMenu(MENU *menu, int screenWidth, int screenHeight)
{
    menu->telaAtual = TELA_MENU;
    menu->animacaoAtual = ANIMACAO_CORRENDO;
    menu->botaoSelecionado = 0;
    menu->frameInicial = 0;
    menu->barrilFrame = 0;
    menu->etapaDescida = 0;
    menu->temporizadorFrame = 0.0f;
    menu->temporizadorEscalada = 0.0f;
    menu->temporizadorFrameBarril = 0.0f;
    menu->posicaoMario = (Vector2){100.0f, 538.0f};
    menu->posicaoAlvoEscalada = (Vector2){50.0f, 538.0f};
    menu->posicaoBarril = (Vector2){-200.0f, 538.0f};
    menu->cursorMouse = (Vector2){0.0f, 0.0f};
    menu->btnJogar = (Rectangle){screenWidth / 2.0f - 100.0f, 240.0f, 200.0f, 50.0f};
    menu->btnRanking = (Rectangle){screenWidth / 2.0f - 100.0f, 300.0f, 200.0f, 50.0f};
    menu->btnOpcoes = (Rectangle){screenWidth / 2.0f - 100.0f, 360.0f, 200.0f, 50.0f};
    menu->btnSair = (Rectangle){screenWidth / 2.0f - 100.0f, 420.0f, 200.0f, 50.0f};
    menu->btnVoltar = (Rectangle){20.0f, 20.0f, 100.0f, 40.0f};
}

void carregarRecursosMenu(MENU *menu)
{
    menu->recursos.titulo = LoadTexture("assets/images/menu/logoMenu.png");
    menu->recursos.background = LoadTexture("assets/images/menu/backgroundMenu.png");
    menu->recursos.escada = LoadTexture("assets/images/cenario/escadaMaior.png");
    menu->recursos.marioCorrendo = LoadTexture("assets/images/sprites/spriteMarioCorrendo.png");
    menu->recursos.marioEscalando = LoadTexture("assets/images/sprites/spriteMarioSubindo.png");
    menu->recursos.barrilRolando = LoadTexture("assets/images/sprites/spriteBarrilRolando.png");
    menu->recursos.iconDonkeyKong = LoadTexture("assets/images/menu/donkeyKongIcon.png");

    menu->recursos.musicaMenu = LoadMusicStream("assets/audio/DKThemeMenu.mp3");
    PlayMusicStream(menu->recursos.musicaMenu);
    SetMusicVolume(menu->recursos.musicaMenu, 0.6f);
}

void descarregarRecursosMenu(MENU *menu)
{
    UnloadTexture(menu->recursos.titulo);
    UnloadTexture(menu->recursos.background);
    UnloadTexture(menu->recursos.escada);
    UnloadTexture(menu->recursos.marioCorrendo);
    UnloadTexture(menu->recursos.marioEscalando);
    UnloadTexture(menu->recursos.barrilRolando);
    UnloadTexture(menu->recursos.iconDonkeyKong);

    StopMusicStream(menu->recursos.musicaMenu);
    UnloadMusicStream(menu->recursos.musicaMenu);
}

static void atualizarAnimacaoMenu(MENU *menu, float variacaoTempo, int SCREEN_WIDTH)
{
    if (menu->animacaoAtual == ANIMACAO_CORRENDO)
    {
        menu->posicaoMario.x += VELOCIDADE_MARIO_MENU * variacaoTempo;
        if (menu->posicaoMario.x >= INICIO_X_SUBIR)
        {
            menu->posicaoMario.x = INICIO_X_SUBIR;
            menu->animacaoAtual = ANIMACAO_SUBINDO;
            menu->frameInicial = 0;
            menu->temporizadorEscalada = 0.0f;
        }
    }
    else if (menu->animacaoAtual == ANIMACAO_SUBINDO)
    {
        menu->temporizadorEscalada +=variacaoTempo;
        menu->posicaoMario.y -= VELOCIDADE_SUBIR *variacaoTempo;
        if (menu->temporizadorEscalada >= DURACAO_ESCALAR)
        {
            menu->animacaoAtual = ANIMACAO_DESCENDO;
            menu->frameInicial = 0;
            menu->temporizadorEscalada = 0.0f;
        }
    }
    else if (menu->animacaoAtual == ANIMACAO_DESCENDO)
    {
        if (menu->etapaDescida == 0)
        {
            if (menu->posicaoMario.x > menu->posicaoAlvoEscalada.x)
            {
                menu->posicaoMario.x -= VELOCIDADE_MARIO_MENU *variacaoTempo;
                if (menu->posicaoMario.x < menu->posicaoAlvoEscalada.x)
                    menu->posicaoMario.x = menu->posicaoAlvoEscalada.x;
            }

            if (menu->posicaoMario.x == menu->posicaoAlvoEscalada.x)
            {
                menu->posicaoMario.x = TOPO_X_ESCADA;
                menu->posicaoMario.y = TOPO_Y_ESCADA;
                menu->etapaDescida = 1;
            }
        }
        else if (menu->etapaDescida == 1)
        {
            if (menu->posicaoMario.y < 538.0f)
            {
                menu->posicaoMario.y += VELOCIDADE_DESCER *variacaoTempo;
                if (menu->posicaoMario.y > 538.0f)
                    menu->posicaoMario.y = 538.0f;
            }

            if (menu->posicaoMario.y == 538.0f)
            {
                menu->animacaoAtual = ANIMACAO_CORRENDO;
                menu->frameInicial = 0;
                menu->etapaDescida = 0;
            }
        }
    }

    menu->temporizadorFrame +=variacaoTempo;
    if (menu->temporizadorFrame >= FRAMES_DURACAO)
    {
        menu->temporizadorFrame = 0.0f;
        menu->frameInicial++;
        if (menu->animacaoAtual == ANIMACAO_CORRENDO)
        {
            if (menu->frameInicial >= FRAMES_MARIO_CORRENDO)
                menu->frameInicial = 0;
        }
        else
        {
            if (menu->frameInicial < 1)
                menu->frameInicial = 1;
            if (menu->frameInicial > 2)
                menu->frameInicial = 1;
        }
    }

    menu->temporizadorFrameBarril +=variacaoTempo;
    if (menu->temporizadorFrameBarril >= 0.04f)
    {
        menu->temporizadorFrameBarril = 0.0f;
        menu->barrilFrame = (menu->barrilFrame + 1) % FRAMES_BARRIL;
    }

    menu->posicaoBarril.x += VELOCIDADE_BARRIL *variacaoTempo;
    if (menu->posicaoBarril.x > SCREEN_WIDTH + 100.0f)
    {
        menu->posicaoBarril.x = -550.0f;
    }
}

static void iniciarNovoJogo(MENU *menu)
{
    (void)menu;
#ifdef _WIN32
    ShellExecuteA(NULL, "open", "jogo.exe", NULL, NULL, 1);
#else
    system("./jogo &");
#endif
}

static void HandleMenuSelection(MENU *menu)
{
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
    {
        menu->botaoSelecionado = (menu->botaoSelecionado + 1) % 4;
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
    {
        menu->botaoSelecionado = (menu->botaoSelecionado - 1 + 4) % 4;
    }

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
    {
        if (menu->botaoSelecionado == 0)
            iniciarNovoJogo(menu);
        else if (menu->botaoSelecionado == 1)
            menu->telaAtual = TELA_RANKING;
        else if (menu->botaoSelecionado == 2)
            menu->telaAtual = TELA_OPCOES;
        else if (menu->botaoSelecionado == 3)
            menu->telaAtual = TELA_SAIR;
    }
}

static bool ShouldReturnToMenu(const MENU *menu)
{
    return IsKeyPressed(KEY_ESCAPE) ||
           (CheckCollisionPointRec(menu->cursorMouse, menu->btnVoltar) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}

void atualizarMenu(MENU *menu, float variacaoTempo, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    menu->cursorMouse = GetMousePosition();

    switch (menu->telaAtual)
    {
    case TELA_MENU:
        atualizarAnimacaoMenu(menu, variacaoTempo, SCREEN_WIDTH);
        HandleMenuSelection(menu);
        break;

    case TELA_NOVO_JOGO:
    case TELA_RANKING:
        if (ShouldReturnToMenu(menu))
            menu->telaAtual = TELA_MENU;
        break;

    case TELA_OPCOES:
        atualizarOpcoes(menu, variacaoTempo);
        break;

    default:
        break;
    }
}

static void DrawBackButton(const MENU *menu)
{
    Color colorBack = CheckCollisionPointRec(menu->cursorMouse, menu->btnVoltar) ? LIGHTGRAY : GRAY;
    DrawRectangleRec(menu->btnVoltar, colorBack);
    DrawText("VOLTAR", menu->btnVoltar.x + 15.0f, menu->btnVoltar.y + 10.0f, 18, BLACK);
}

static void desenharMenuButtons(const MENU *menu)
{
    const float buttonIconScale = 2.4f;
    const int buttonFontSize = 30;
    const float buttonIconSpacing = 4.0f;

    struct
    {
        const char *text;
        Rectangle rect;
        bool selected;
    } buttons[4] = {
        {"NOVO JOGO", menu->btnJogar, menu->botaoSelecionado == 0},
        {"RANKING", menu->btnRanking, menu->botaoSelecionado == 1},
        {"OPÇÕES", menu->btnOpcoes, menu->botaoSelecionado == 2},
        {"SAIR", menu->btnSair, menu->botaoSelecionado == 3}};

    for (int i = 0; i < 4; i++)
    {
        bool selected = buttons[i].selected;
        Color textColor = selected ? RED : WHITE;
        DrawRectangleRec(buttons[i].rect, BLACK);

        if (selected)
        {
            DrawTextureEx(menu->recursos.iconDonkeyKong,
                          (Vector2){buttons[i].rect.x - (menu->recursos.iconDonkeyKong.width * buttonIconScale) - buttonIconSpacing,
                                    buttons[i].rect.y + (buttons[i].rect.height / 2.0f) - (menu->recursos.iconDonkeyKong.height * buttonIconScale) / 2.0f},
                          0.0f, buttonIconScale, WHITE);
        }

        DrawText(buttons[i].text,
                 buttons[i].rect.x + (buttons[i].rect.width / 2.0f) - (MeasureText(buttons[i].text, buttonFontSize) / 2.0f) + (selected ? 10 : 0),
                 buttons[i].rect.y + 15.0f,
                 buttonFontSize,
                 textColor);
    }
}

static void desenharMenuScreen(const MENU *menu, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    float scale = 0.4f;
    float tituloX = SCREEN_WIDTH / 2.0f - (menu->recursos.titulo.width * scale) / 2.0f;
    DrawTextureEx(menu->recursos.titulo, (Vector2){tituloX, 30.0f}, 0.0f, scale, WHITE);

    float backgroundScale = 1.0f;
    float backgroundX = SCREEN_WIDTH / 2.0f - (menu->recursos.background.width * backgroundScale) / 2.0f;
    DrawTextureEx(menu->recursos.background, (Vector2){backgroundX, 605.0f}, 0.0f, backgroundScale, WHITE);

    float escadaScale = 0.4f;
    float escadaXLeft = SCREEN_WIDTH / 2.5f - (menu->recursos.escada.width * escadaScale) / 1.12f;
    DrawTextureEx(menu->recursos.escada, (Vector2){escadaXLeft, -93.0f}, 0.0f, escadaScale, WHITE);

    float escadaXRight = SCREEN_WIDTH / 0.945f - (menu->recursos.escada.width * escadaScale) / 2.5f;
    DrawTextureEx(menu->recursos.escada, (Vector2){escadaXRight, -93.0f}, 0.0f, escadaScale, WHITE);

    Rectangle srcBarrel = {menu->barrilFrame * (menu->recursos.barrilRolando.width / FRAMES_BARRIL), 0.0f,
                           menu->recursos.barrilRolando.width / FRAMES_BARRIL, (float)menu->recursos.barrilRolando.height};
    float barrelScale = 4.5f;
    Rectangle destBarrel = {menu->posicaoBarril.x, menu->posicaoBarril.y - menu->recursos.barrilRolando.height * barrelScale,
                            srcBarrel.width * barrelScale, menu->recursos.barrilRolando.height * barrelScale};
    DrawTexturePro(menu->recursos.barrilRolando, srcBarrel, destBarrel, (Vector2){0.0f, -85.0f}, 0.0f, WHITE);

    if (menu->animacaoAtual == ANIMACAO_CORRENDO)
    {
        Rectangle srcMario = {menu->frameInicial * (menu->recursos.marioCorrendo.width / FRAMES_MARIO_CORRENDO), 0.0f,
                              menu->recursos.marioCorrendo.width / FRAMES_MARIO_CORRENDO, (float)menu->recursos.marioCorrendo.height};
        Rectangle destMario = {menu->posicaoMario.x, menu->posicaoMario.y,
                               srcMario.width * 4.2f, srcMario.height * 4.2f};
        DrawTexturePro(menu->recursos.marioCorrendo, srcMario, destMario, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
    }
    else
    {
        Rectangle srcMario = {menu->frameInicial * (menu->recursos.marioEscalando.width / FRAMES_MARIO_ESCALANDO), 0.0f,
                              menu->recursos.marioEscalando.width / FRAMES_MARIO_ESCALANDO, (float)menu->recursos.marioEscalando.height};
        Rectangle destMario = {menu->posicaoMario.x, menu->posicaoMario.y,
                               srcMario.width * 4.2f, srcMario.height * 4.2f};
        DrawTexturePro(menu->recursos.marioEscalando, srcMario, destMario, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
    }

    desenharMenuButtons(menu);
}

static void DrawPlaceholderScreen(const MENU *menu, const char *text, Color background, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    ClearBackground(background);
    DrawText(text, SCREEN_WIDTH / 2 - MeasureText(text, 30) / 2, SCREEN_HEIGHT / 2, 30, WHITE);
    DrawBackButton(menu);
}

void desenharMenu(const MENU *menu, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    switch (menu->telaAtual)
    {
    case TELA_MENU:
        desenharMenuScreen(menu, SCREEN_WIDTH, SCREEN_HEIGHT);
        break;

    case TELA_NOVO_JOGO:
        DrawPlaceholderScreen(menu, "O JOGO ESTÁ RODANDO!", DARKBLUE, SCREEN_WIDTH, SCREEN_HEIGHT);
        break;

    case TELA_OPCOES:
        desenharOpcoes(menu, SCREEN_WIDTH, SCREEN_HEIGHT);
        break;

    case TELA_RANKING:
        DrawPlaceholderScreen(menu, "TELA DE RANKING", DARKPURPLE, SCREEN_WIDTH, SCREEN_HEIGHT);
        break;

    default:
        break;
    }
}
