#include "global.h"
#include "res/resources.h"
#include "engine/gameobject.h"

#define SHOOT_SPEED 8

#define ALIED_SHOOT 0
#define ENEMY_SHOOT 1


u16 SHOOT_load_tiles(u16 ind);
u16 SHOOT_init(GameObject* obj, s16 x, s16 y);
void SHOOT_update(GameObject* obj);


//PRIVATE
static void frame_changed(Sprite* sprite);