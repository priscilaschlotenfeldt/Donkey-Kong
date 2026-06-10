#include "raylib.h"
#include <stdio.h>
#include "opcoes.h"

// Estado local das opções
static float volumeMusica = 0.6f;
static float volumeEfeitosSonoros = 0.6f;
static int opcaoSelecionada = 0; // 0 = música, 1 = efeitos, 2 = voltar

void iniciarOpcoes(void)
{
    opcaoSelecionada = 0;
}

void atualizarOpcoes(MENU *menu, float dt)
{
    // navegação entre opções
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        opcaoSelecionada = (opcaoSelecionada + 1) % 3;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        opcaoSelecionada = (opcaoSelecionada - 1 + 3) % 3;

    // ajustar valores com esquerda/direita
    if (opcaoSelecionada == 0)
    {
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) volumeMusica += 0.01f;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) volumeMusica -= 0.01f;
        if (volumeMusica < 0.0f) volumeMusica = 0.0f;
        if (volumeMusica > 1.0f) volumeMusica = 1.0f;

        SetMusicVolume(menu->recursos.musicaMenu, volumeMusica);
    }
    else if (opcaoSelecionada == 1)
    {
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) volumeEfeitosSonoros += 0.01f;
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) volumeEfeitosSonoros -= 0.01f;
        if (volumeEfeitosSonoros < 0.0f) volumeEfeitosSonoros = 0.0f;
        if (volumeEfeitosSonoros > 1.0f) volumeEfeitosSonoros = 1.0f;
    }

    // atalhos: voltar com R ou ENTER quando Voltar estiver selecionado
    if (IsKeyPressed(KEY_R) ||
        (opcaoSelecionada == 2 && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))))
    {
        menu->telaAtual = TELA_MENU;
    }
}

static void DrawSlider(float x, float y, float width, float value, bool selected)
{
    Color bg = selected ? RED : RAYWHITE;
    DrawRectangle((int)x, (int)y, (int)width, 20, bg);
    DrawRectangle((int)x + 2, (int)y + 2, (int)((width - 4) * value), 16, BLACK);
}

void desenharOpcoes(const MENU *menu, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    ClearBackground(BLACK);

    const int titleSize = 36;
    const char *title = "OPÇÕES";
    DrawText(title, SCREEN_WIDTH / 2 - MeasureText(title, titleSize) / 2, 40, titleSize, WHITE);

    // instruções
    DrawText("Use W | S para selecionar, <- | -> para ajustar.", 40, 100, 18, WHITE);
    DrawText("Pressione ENTER ou R para voltar quando VOLTAR estiver selecionado.", 40, 124, 18, WHITE);

    // Música
    DrawText("Volume da Musica", 80, 180, 22, WHITE);
    DrawSlider(80, 210, 400, volumeMusica, opcaoSelecionada == 0);
    char buf[64];
    sprintf(buf, "%d%%", (int)(volumeMusica * 100));
    DrawText(buf, 500, 206, 22, WHITE);

    // Efeitos
    DrawText("Efeitos Sonoros", 80, 270, 22, WHITE);
    DrawSlider(80, 300, 400, volumeEfeitosSonoros, opcaoSelecionada == 1);
    sprintf(buf, "%d%%", (int)(volumeEfeitosSonoros * 100));
    DrawText(buf, 500, 296, 22, WHITE);

    // Botão desenhado na tela para voltar
    Rectangle btnVoltar = {20, SCREEN_HEIGHT - 70, 160, 40};
    Color cv = (opcaoSelecionada == 2) ? LIGHTGRAY : GRAY;
    if (CheckCollisionPointRec(menu->cursorMouse, btnVoltar))
        cv = LIGHTGRAY;

    DrawRectangleRec(btnVoltar, cv);
    DrawText("VOLTAR (R)", btnVoltar.x + 20, btnVoltar.y + 10, 20, BLACK);
}
