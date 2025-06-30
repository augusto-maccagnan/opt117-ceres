#ifndef _HUD_H
#define _HUD_H

#include <genesis.h>
#include "globals.h"
#include "gameobject.h"
#include "resources.h"

extern u16 player_score;

void HUD_update_health(u8 value);

void HUD_score(u8 value);
void HUD_update_score();

u16 HUD_init(u16 ind);

#endif