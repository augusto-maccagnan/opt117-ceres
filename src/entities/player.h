#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <genesis.h>
#include "engine/gameobject.h"
#include "resources.h"

#define ANIM_VERTICAL    0
#define ANIM_HORIZONTAL  1

#define SHOOT_DELAY 10
#define MAX_SHOTS 15
#define SHOOT_SPEED 8
#define IMMUNITY_TIME 15

// #define PLAYER_SPEED45 FIX16(0.707)

extern GameObject player;
extern GameObject shots[MAX_SHOTS];
extern u8 shoot_count;

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 PLAYER_init(u16 ind);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void PLAYER_update();

void PLAYER_get_input_dir8();
void PLAYER_get_input_dir4();

/*
* Checks collision with level tiles.
* HAS TO BE CALLED AFTER LEVEL_move_and_slide()!
*/
void PLAYER_collision();
void PLAYER_damage(u8 damage);
void PLAYER_immunity_update();

// Shooting logic
void PLAYER_shoot();
void SHOTS_update();
void SHOOT_collision(GameObject* shot);

//DEBUGGING
void DEBUG_collision_map(s16 x, s16 y);

#endif // _PLAYER_H_