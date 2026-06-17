#ifndef OPCOES_H
#define OPCOES_H

#include "menu.h"
#include <stdbool.h>

void iniciarOpcoes(void);
void atualizarOpcoes(MENU *menu, float dt);
void desenharOpcoes(const MENU *menu, int SCREEN_WIDTH, int SCREEN_HEIGHT);

float obterVolumeMusica(void);
float obterVolumeEfeitosSonoros(void);
bool obterMusicaLigada(void);
void definirVolumeMusica(float volume);
void definirVolumeEfeitosSonoros(float volume);
void definirMusicaLigada(bool ligada);

#endif // OPCOES_H
