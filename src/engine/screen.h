#ifndef SCRN_H
#define SCRN_H

#include <genesis.h>
#include "gameobject.h"
#include "resources.h"
#include "globals.h"
#include "utils.h"


void SCREEN_over_update();
void SCREEN_game_clear_update();
u8 SCREEN_level_transition(u8 level, u8 ind);
void SCREEN_menu();
#endif // SCRN_H