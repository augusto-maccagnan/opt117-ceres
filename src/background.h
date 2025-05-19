#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

#include <genesis.h>
#include "globals.h"
#include "gameobject.h"
#include "resources.h"

// parallax scroling
#define PLANE_W FIX16(320)

extern fix16 offset_pos[SCREEN_TILES_H];
extern fix16 offset_speed[SCREEN_TILES_H];
extern s16 values[SCREEN_TILES_H];

void set_offset_speed(u8 start, u8 len, fix16 speed);
u16 BACKGROUND_init(u16 ind);
void BACKGROUND_update();

#endif