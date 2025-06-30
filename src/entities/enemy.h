#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "resources.h"
#include "engine/gameobject.h"
#include "engine/mapobjects.h"

#define ENEMY_UFO 0
#define ENEMY_RKT  1
#define ENEMY_KAZ  2

#define ENEMY_MAX_SPEED 3

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 ENEMY_load_tiles(u16 ind);
void ENEMY_init(GameObject* const obj, const MapObject* const mapobj, u16 ind);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void ENEMY_ufo_update(GameObject* obj);
void ENEMY_rkt_update(GameObject* obj);
void ENEMY_kaz_update(GameObject* obj);
void ENEMY_on_hit(GameObject* obj, u8 amount);

////////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBERS



#endif