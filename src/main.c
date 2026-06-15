#include "../include/menu.h"
#include "../include/opcoes.h"
#include "../include/jogo_definitivo.h"

#define SCREEN_WIDTH 850
#define SCREEN_HEIGHT 650


int main(void)
{

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Donkey Kong - Menu");
    SetTargetFPS(60);
    
    InitAudioDevice();

    MENU menu;

    iniciarMenu(&menu, SCREEN_WIDTH, SCREEN_HEIGHT);
    carregarRecursosMenu(&menu);
    iniciarOpcoes();

    while (!WindowShouldClose() && menu.telaAtual != TELA_SAIR)
    {
        float variacaoTempo = GetFrameTime();
        atualizarMenu(&menu,variacaoTempo, SCREEN_WIDTH, SCREEN_HEIGHT);

        BeginDrawing();
        ClearBackground(BLACK);
        desenharMenu(&menu, SCREEN_WIDTH, SCREEN_HEIGHT);
        EndDrawing();

        UpdateMusicStream(menu.recursos.musicaMenu);
    }

    descarregarRecursosMenu(&menu);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
