// #include "engine/globals.h"
#include "res/resources.h"
#include "engine/gameobject.h"

#define SHOOT_SPEED 8


u16 SHOOT_load_tiles(u16 ind);
u16 SHOOT_init(GameObject* obj, s16 x, s16 y, u8 type);
void SHOOT_update(GameObject* obj);


//PRIVATE
static void SHOOT_frame_changed(Sprite* sprite);