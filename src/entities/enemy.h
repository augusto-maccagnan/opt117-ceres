#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "resources.h"
#include "engine/gameobject.h"
#include "engine/mapobjects.h"
#include "entities/shoot.h"
#include "engine/hud.h"

#define ENEMY_UFO 0
#define ENEMY_RKT  1
#define ENEMY_KAZ  2

#define ENEMY_MAX_SPEED 3

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 ENEMY_load_tiles(u16 ind);
u16 ENEMY_init(GameObject* const obj, const MapObject* const mapobj, u16 ind);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void ENEMY_ufo_update(GameObject* obj);
void ENEMY_rkt_update(GameObject* obj);
void ENEMY_kaz_update(GameObject* obj);
void ENEMY_on_hit(GameObject* power_up, GameObject* enemy);

void POWERUP_update(GameObject* obj);
////////////////////////////////////////////////////////////////////////////
// PRIVATE MEMBERS

static void frame_changed(Sprite* sprite);
static void pwup_frame_changed(Sprite* sprite);

#endif