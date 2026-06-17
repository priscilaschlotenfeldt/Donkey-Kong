#include "../include/menu.h"
#include "../include/opcoes.h"
#include "../include/jogo_definitivo.h"

#define SCREEN_WIDTH 850
#define SCREEN_HEIGHT 730


int main(void)
{

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Donkey Kong - Menu");
    SetTargetFPS(60);
    SetExitKey(0);
    
    InitAudioDevice();

    MENU menu;

    iniciarMenu(&menu, SCREEN_WIDTH, SCREEN_HEIGHT);
    carregarRecursosMenu(&menu);
    iniciarOpcoes();

    while (menu.telaAtual != TELA_SAIR)
    {
        if (WindowShouldClose())
            solicitarSaidaMenu(&menu);

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
