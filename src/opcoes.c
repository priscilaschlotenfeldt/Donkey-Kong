#include <raylib.h>
#include <stdio.h>
#include "../include/opcoes.h"

// Estado local das opções
static float volumeMusica = 0.6f;
static float volumeEfeitosSonoros = 0.6f;
static bool musicaLigada = true;
static int opcaoSelecionada = 0; // 0 = música, 1 = efeitos, 2 = liga/desliga, 3 = voltar, 4 = sair

float obterVolumeMusica(void)
{
    return musicaLigada ? volumeMusica : 0.0f;
}

float obterVolumeEfeitosSonoros(void)
{
    return volumeEfeitosSonoros;
}

bool obterMusicaLigada(void)
{
    return musicaLigada;
}

void iniciarOpcoes(void)
{
    opcaoSelecionada = 0;
}

static bool Clicou(Rectangle botao, Vector2 mouse)
{
    return CheckCollisionPointRec(mouse, botao) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void limitarVolume(float *volume)
{
    if (*volume < 0.0f) *volume = 0.0f;
    if (*volume > 1.0f) *volume = 1.0f;
}

void definirVolumeMusica(float volume)
{
    volumeMusica = volume;
    limitarVolume(&volumeMusica);
}

void definirVolumeEfeitosSonoros(float volume)
{
    volumeEfeitosSonoros = volume;
    limitarVolume(&volumeEfeitosSonoros);
}

void definirMusicaLigada(bool ligada)
{
    musicaLigada = ligada;
}

void atualizarOpcoes(MENU *menu, float dt)
{
    (void)dt;

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        opcaoSelecionada = (opcaoSelecionada + 1) % 5;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        opcaoSelecionada = (opcaoSelecionada - 1 + 5) % 5;

    if (opcaoSelecionada == 0)
    {
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) volumeMusica += 0.01f;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) volumeMusica -= 0.01f;
        limitarVolume(&volumeMusica);
    }
    else if (opcaoSelecionada == 1)
    {
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) volumeEfeitosSonoros += 0.01f;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) volumeEfeitosSonoros -= 0.01f;
        limitarVolume(&volumeEfeitosSonoros);
    }

    if (opcaoSelecionada == 2 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)))
        musicaLigada = !musicaLigada;

    if (Clicou((Rectangle){80, 210, 400, 20}, menu->cursorMouse))
    {
        volumeMusica = (menu->cursorMouse.x - 80.0f) / 400.0f;
        limitarVolume(&volumeMusica);
        opcaoSelecionada = 0;
    }

    if (Clicou((Rectangle){80, 300, 400, 20}, menu->cursorMouse))
    {
        volumeEfeitosSonoros = (menu->cursorMouse.x - 80.0f) / 400.0f;
        limitarVolume(&volumeEfeitosSonoros);
        opcaoSelecionada = 1;
    }

    Rectangle btnMusica = {80, 380, 230, 40};
    Rectangle btnVoltar = {80, 470, 190, 45};
    Rectangle btnSair = {300, 470, 190, 45};

    if (Clicou(btnMusica, menu->cursorMouse))
    {
        musicaLigada = !musicaLigada;
        opcaoSelecionada = 2;
    }

    if (Clicou(btnVoltar, menu->cursorMouse) ||
        (opcaoSelecionada == 3 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))) ||
        IsKeyPressed(KEY_R))
    {
        menu->telaAtual = TELA_MENU;
    }

    if (Clicou(btnSair, menu->cursorMouse) ||
        (opcaoSelecionada == 4 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))))
    {
        solicitarSaidaMenu(menu);
    }

    SetMusicVolume(menu->recursos.musicaMenu, obterVolumeMusica());
}

static void DrawSlider(float x, float y, float width, float value, bool selected)
{
    Color bg = selected ? RED : RAYWHITE;
    DrawRectangle((int)x, (int)y, (int)width, 20, bg);
    DrawRectangle((int)x + 2, (int)y + 2, (int)((width - 4) * value), 16, BLACK);
}

static void DrawButton(Rectangle rect, const char *texto, bool selected, Vector2 mouse)
{
    bool hover = CheckCollisionPointRec(mouse, rect);
    Color cor = (selected || hover) ? LIGHTGRAY : GRAY;
    DrawRectangleRec(rect, cor);
    DrawRectangleLinesEx(rect, 2, selected ? RED : DARKGRAY);
    DrawText(texto,
             (int)(rect.x + rect.width / 2.0f - MeasureText(texto, 20) / 2.0f),
             (int)(rect.y + 12.0f),
             20,
             BLACK);
}

void desenharOpcoes(const MENU *menu, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    ClearBackground(BLACK);

    const int titleSize = 36;
    const char *title = "OPÇÕES";
    DrawText(title, SCREEN_WIDTH / 2 - MeasureText(title, titleSize) / 2, 40, titleSize, WHITE);

    DrawText("Use W/S para selecionar, <-/-> para ajustar e ENTER para confirmar.", 40, 100, 18, WHITE);
    DrawText("ESC abre a confirmação de saída.", 40, 124, 18, WHITE);

    DrawText("Volume da Música", 80, 180, 22, WHITE);
    DrawSlider(80, 210, 400, volumeMusica, opcaoSelecionada == 0);
    char buf[64];
    sprintf(buf, "%d%%", (int)(volumeMusica * 100));
    DrawText(buf, 500, 206, 22, WHITE);

    DrawText("Efeitos Sonoros", 80, 270, 22, WHITE);
    DrawSlider(80, 300, 400, volumeEfeitosSonoros, opcaoSelecionada == 1);
    sprintf(buf, "%d%%", (int)(volumeEfeitosSonoros * 100));
    DrawText(buf, 500, 296, 22, WHITE);

    char textoMusica[64];
    sprintf(textoMusica, "Música: %s", musicaLigada ? "LIGADA" : "DESLIGADA");
    DrawButton((Rectangle){80, 380, 230, 40}, textoMusica, opcaoSelecionada == 2, menu->cursorMouse);

    DrawButton((Rectangle){80, 470, 190, 45}, "VOLTAR AO MENU", opcaoSelecionada == 3, menu->cursorMouse);
    DrawButton((Rectangle){300, 470, 190, 45}, "SAIR", opcaoSelecionada == 4, menu->cursorMouse);
}
