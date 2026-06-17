#ifdef _WIN32               // isso aqui olha se o seu compilador é do padrão win
#define WIN32_LEAN_AND_MEAN // daqui para baixo é só para não haver nenhum conflito entre coisas do win e do raylib
#define NOGDI
#define NOUSER
#include <windows.h>
#include <shellapi.h>
#else
#include <stdlib.h>
#endif
#include <stdio.h>

#include <raylib.h> // colocar "" se não for
#include "../include/menu.h"
#include "../include/opcoes.h"
#include "../include/jogo_definitivo.h"
#include "../include/ranking.h"

#define FRAMES_MARIO_CORRENDO 4
#define FRAMES_MARIO_ESCALANDO 3
#define FRAMES_BARRIL 4
#define FRAMES_DURACAO 0.1f
#define DURACAO_ESCALAR 10.0f
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
    menu->posicaoMario = (Vector2){100.0f, 615.0f};
    menu->posicaoAlvoEscalada = (Vector2){50.0f, 616.0f};
    menu->posicaoBarril = (Vector2){-200.0f, 538.0f};
    menu->cursorMouse = (Vector2){0.0f, 0.0f};
    menu->btnJogar = (Rectangle){screenWidth / 2.0f - 100.0f, 280.0f, 200.0f, 50.0f};
    menu->btnRanking = (Rectangle){screenWidth / 2.0f - 100.0f, 340.0f, 200.0f, 50.0f};
    menu->btnOpcoes = (Rectangle){screenWidth / 2.0f - 100.0f, 400.0f, 200.0f, 50.0f};
    menu->btnSair = (Rectangle){screenWidth / 2.0f - 100.0f, 460.0f, 200.0f, 50.0f};
    menu->btnVoltar = (Rectangle){20.0f, 20.0f, 130.0f, 40.0f};
    menu->btnConfirmarSim = (Rectangle){screenWidth / 2.0f - 130.0f, screenHeight / 2.0f + 35.0f, 110.0f, 40.0f};
    menu->btnConfirmarNao = (Rectangle){screenWidth / 2.0f + 20.0f, screenHeight / 2.0f + 35.0f, 110.0f, 40.0f};
    menu->confirmarSaida = false;
    menu->telaAntesConfirmar = TELA_MENU;
    menu->confirmacaoSelecionada = 1;
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
    SetMusicVolume(menu->recursos.musicaMenu, obterVolumeMusica());
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
        menu->temporizadorEscalada += variacaoTempo;
        menu->posicaoMario.y -= VELOCIDADE_SUBIR * variacaoTempo;
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
                menu->posicaoMario.x -= VELOCIDADE_MARIO_MENU * variacaoTempo;
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
            if (menu->posicaoMario.y < 616.0f)
            {
                menu->posicaoMario.y += VELOCIDADE_DESCER * variacaoTempo;
                if (menu->posicaoMario.y > 616.0f)
                    menu->posicaoMario.y = 616.0f;
            }

            if (menu->posicaoMario.y == 616.0f)
            {
                menu->animacaoAtual = ANIMACAO_CORRENDO;
                menu->frameInicial = 0;
                menu->etapaDescida = 0;
            }
        }
    }

    menu->temporizadorFrame += variacaoTempo;
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

    menu->temporizadorFrameBarril += variacaoTempo;
    if (menu->temporizadorFrameBarril >= 0.04f)
    {
        menu->temporizadorFrameBarril = 0.0f;
        menu->barrilFrame = (menu->barrilFrame + 1) % FRAMES_BARRIL;
    }

    menu->posicaoBarril.x += VELOCIDADE_BARRIL * variacaoTempo;
    if (menu->posicaoBarril.x > SCREEN_WIDTH + 100.0f)
    {
        menu->posicaoBarril.x = -250.0f;
    }
}

static void iniciarNovoJogo(MENU *menu)
{
    bool sairPrograma = executarJogo();
    menu->telaAtual = sairPrograma ? TELA_SAIR : TELA_MENU;
}

void solicitarSaidaMenu(MENU *menu)
{
    if (menu->confirmarSaida)
        return;

    menu->confirmarSaida = true;
    menu->telaAntesConfirmar = menu->telaAtual;
    menu->confirmacaoSelecionada = 1; // começa em NÃO para evitar saída acidental
}

static bool ClicouBotao(Rectangle botao, Vector2 mouse)
{
    return CheckCollisionPointRec(mouse, botao) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void AtualizarConfirmacaoSaida(MENU *menu)
{
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
        menu->confirmacaoSelecionada = 1 - menu->confirmacaoSelecionada;

    if (CheckCollisionPointRec(menu->cursorMouse, menu->btnConfirmarSim))
        menu->confirmacaoSelecionada = 0;
    else if (CheckCollisionPointRec(menu->cursorMouse, menu->btnConfirmarNao))
        menu->confirmacaoSelecionada = 1;

    bool confirmou = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) ||
                     IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    if (confirmou)
    {
        if (menu->confirmacaoSelecionada == 0 &&
            (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || ClicouBotao(menu->btnConfirmarSim, menu->cursorMouse)))
        {
            menu->telaAtual = TELA_SAIR;
        }
        else if (menu->confirmacaoSelecionada == 1 &&
                 (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || ClicouBotao(menu->btnConfirmarNao, menu->cursorMouse)))
        {
            menu->confirmarSaida = false;
            menu->telaAtual = menu->telaAntesConfirmar;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        menu->confirmarSaida = false;
        menu->telaAtual = menu->telaAntesConfirmar;
    }
}

static void HandleMenuSelection(MENU *menu)
{
    if (CheckCollisionPointRec(menu->cursorMouse, menu->btnJogar))
        menu->botaoSelecionado = 0;
    else if (CheckCollisionPointRec(menu->cursorMouse, menu->btnRanking))
        menu->botaoSelecionado = 1;
    else if (CheckCollisionPointRec(menu->cursorMouse, menu->btnOpcoes))
        menu->botaoSelecionado = 2;
    else if (CheckCollisionPointRec(menu->cursorMouse, menu->btnSair))
        menu->botaoSelecionado = 3;

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        menu->botaoSelecionado = (menu->botaoSelecionado + 1) % 4;

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        menu->botaoSelecionado = (menu->botaoSelecionado - 1 + 4) % 4;

    bool confirmar = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) ||
                     IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    if (!confirmar)
        return;

    if (menu->botaoSelecionado == 0)
        iniciarNovoJogo(menu);
    else if (menu->botaoSelecionado == 1)
        menu->telaAtual = TELA_RANKING;
    else if (menu->botaoSelecionado == 2)
        menu->telaAtual = TELA_OPCOES;
    else if (menu->botaoSelecionado == 3)
        solicitarSaidaMenu(menu);
}

static bool ShouldReturnToMenu(const MENU *menu)
{
    return ClicouBotao(menu->btnVoltar, menu->cursorMouse);
}

void atualizarMenu(MENU *menu, float variacaoTempo, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    (void)SCREEN_HEIGHT;
    menu->cursorMouse = GetMousePosition();

    if (menu->confirmarSaida)
    {
        AtualizarConfirmacaoSaida(menu);
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        solicitarSaidaMenu(menu);
        return;
    }

    SetMusicVolume(menu->recursos.musicaMenu, obterVolumeMusica());

    switch (menu->telaAtual)
    {
    case TELA_MENU:
        atualizarAnimacaoMenu(menu, variacaoTempo, SCREEN_WIDTH);
        HandleMenuSelection(menu);
        break;

    case TELA_NOVO_JOGO:
        printf("CHEGUEI NO CASE TELA_NOVO_JOGO\n");
        fflush(stdout);

        if (executarJogo())
            menu->telaAtual = TELA_SAIR;
        else
            menu->telaAtual = TELA_MENU;

        printf("VOLTEI DO JOGO\n");
        fflush(stdout);
        break;

    case TELA_RANKING:
        if (ShouldReturnToMenu(menu))
            menu->telaAtual = TELA_MENU;
        if (ClicouBotao(menu->btnSair, menu->cursorMouse))
            solicitarSaidaMenu(menu);
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
    DrawTextureEx(menu->recursos.titulo, (Vector2){tituloX, 50.0f}, 0.0f, scale, WHITE);

    float backgroundScale = 1.0f;
    float backgroundX = SCREEN_WIDTH / 2.0f - (menu->recursos.background.width * backgroundScale) / 2.0f;
    DrawTextureEx(menu->recursos.background, (Vector2){backgroundX, 680.0f}, 0.0f, backgroundScale, WHITE);

    float escadaScale = 0.4f;
    float escadaXLeft = SCREEN_WIDTH / 2.5f - (menu->recursos.escada.width * escadaScale) / 1.12f;
    DrawTextureEx(menu->recursos.escada, (Vector2){escadaXLeft, -18.0f}, 0.0f, escadaScale, WHITE);

    float escadaXRight = SCREEN_WIDTH / 0.945f - (menu->recursos.escada.width * escadaScale) / 2.5f;
    DrawTextureEx(menu->recursos.escada, (Vector2){escadaXRight, -18.0f}, 0.0f, escadaScale, WHITE);

    Rectangle srcBarrel = {menu->barrilFrame * (menu->recursos.barrilRolando.width / FRAMES_BARRIL), 0.0f,
                           menu->recursos.barrilRolando.width / FRAMES_BARRIL, (float)menu->recursos.barrilRolando.height};
    float barrelScale = 4.5f;
    Rectangle destBarrel = {menu->posicaoBarril.x, menu->posicaoBarril.y - menu->recursos.barrilRolando.height * barrelScale,
                            srcBarrel.width * barrelScale, menu->recursos.barrilRolando.height * barrelScale};
    DrawTexturePro(menu->recursos.barrilRolando, srcBarrel, destBarrel, (Vector2){0.0f, -160.0f}, 0.0f, WHITE);

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

static void DrawButtonText(Rectangle rect, const char *texto, bool selected, Vector2 mouse)
{
    Color cor = (selected || CheckCollisionPointRec(mouse, rect)) ? LIGHTGRAY : GRAY;
    DrawRectangleRec(rect, cor);
    DrawRectangleLinesEx(rect, 2, selected ? RED : DARKGRAY);
    DrawText(texto,
             (int)(rect.x + rect.width / 2.0f - MeasureText(texto, 18) / 2.0f),
             (int)(rect.y + rect.height / 2.0f - 9.0f),
             18,
             BLACK);
}

static void DrawRankingScreen(const MENU *menu, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    ClearBackground(DARKPURPLE);

    const char *titulo = "RANKING - TOP 10";
    DrawText(titulo, SCREEN_WIDTH / 2 - MeasureText(titulo, 36) / 2, 45, 36, WHITE);

    DrawText("POS", 110, 120, 22, YELLOW);
    DrawText("NOME", 190, 120, 22, YELLOW);
    DrawText("PONTOS", 470, 120, 22, YELLOW);
    DrawText("TEMPO", 620, 120, 22, YELLOW);

    TIPO_PLACAR ranking[TOP_RANKING];
    int quantidade = carregarRanking(ranking, TOP_RANKING);

    if (quantidade == 0)
    {
        const char *msg = "Nenhum placar salvo ainda.";
        DrawText(msg, SCREEN_WIDTH / 2 - MeasureText(msg, 24) / 2, 250, 24, WHITE);
    }
    else
    {
        for (int i = 0; i < quantidade && i < TOP_RANKING; i++)
        {
            char linha[128];
            int y = 165 + i * 38;

            sprintf(linha, "%02d", i + 1);
            DrawText(linha, 115, y, 22, WHITE);

            DrawText(ranking[i].nome, 190, y, 22, WHITE);

            sprintf(linha, "%d", ranking[i].pontos);
            DrawText(linha, 480, y, 22, WHITE);

            sprintf(linha, "%.2fs", ranking[i].tempo);
            DrawText(linha, 620, y, 22, WHITE);
        }
    }

    DrawButtonText(menu->btnVoltar, "VOLTAR", false, menu->cursorMouse);
    DrawButtonText(menu->btnSair, "SAIR", false, menu->cursorMouse);
}

static void DrawPopupConfirmacaoSaida(const MENU *menu, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 160});

    Rectangle popup = {SCREEN_WIDTH / 2.0f - 230.0f, SCREEN_HEIGHT / 2.0f - 95.0f, 460.0f, 190.0f};
    DrawRectangleRec(popup, (Color){30, 30, 30, 245});
    DrawRectangleLinesEx(popup, 3, RED);

    const char *pergunta = "Deseja mesmo sair?";
    DrawText(pergunta,
             SCREEN_WIDTH / 2 - MeasureText(pergunta, 28) / 2,
             (int)popup.y + 35,
             28,
             WHITE);

    DrawButtonText(menu->btnConfirmarSim, "SIM", menu->confirmacaoSelecionada == 0, menu->cursorMouse);
    DrawButtonText(menu->btnConfirmarNao, "NÃO", menu->confirmacaoSelecionada == 1, menu->cursorMouse);
}

void desenharMenu(const MENU *menu, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    switch (menu->telaAtual)
    {
    case TELA_MENU:
        desenharMenuScreen(menu, SCREEN_WIDTH, SCREEN_HEIGHT);
        break;

    case TELA_OPCOES:
        desenharOpcoes(menu, SCREEN_WIDTH, SCREEN_HEIGHT);
        break;

    case TELA_RANKING:
        DrawRankingScreen(menu, SCREEN_WIDTH, SCREEN_HEIGHT);
        break;

    default:
        break;
    }

    if (menu->confirmarSaida)
        DrawPopupConfirmacaoSaida(menu, SCREEN_WIDTH, SCREEN_HEIGHT);
}
