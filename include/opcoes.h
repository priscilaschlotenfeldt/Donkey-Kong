#ifndef OPCOES_H
#define OPCOES_H

#include "menu.h"

void iniciarOpcoes(void);
void atualizarOpcoes(MENU *menu, float dt);
void desenharOpcoes(const MENU *menu, int SCREEN_WIDTH, int SCREEN_HEIGHT);

#endif // OPCOES_H
