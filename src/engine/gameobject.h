#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <genesis.h>
#include "sprite_eng.h"
#include "globals.h"

typedef struct {
	s16 left;
	s16 right;
	s16 top;
	s16 bottom;
} BoundBox;

typedef struct GameObject {
	u8 active;
	Sprite* sprite;
	f16 x;
	f16 y;
	f16 next_x;
	f16 next_y;
	f16 speed_x;
	f16 speed_y;
	f16 speed;
	u8 w;
	u8 h;
	BoundBox box;
	s8 w_offset;
	s8 h_offset;
	u8 anim;
	u8 health;
	u16 dir;
	u8 timer;
	u8 damage;
	u8 phase;
	u8 type;

	struct GameObject* next;		// for the Linked Lists in OBJECTS POOL

	void (*update)(struct GameObject* obj);
	void (*on_hit)(struct GameObject* obj, u8 amount);
} GameObject;

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 GAMEOBJECT_init(GameObject* const obj, const SpriteDefinition* const sprite, s16 x, s16 y, s8 w_offset, s8 h_offset, u8 pal, u16 ind);
u16 GAMEOBJECT_init_no_pal(GameObject* const obj, const SpriteDefinition* const sprite, s16 x, s16 y, s8 w_offset, s8 h_offset, u8 pal, u16 ind);
////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

u8 GAMEOBJECT_check_collision(GameObject* obj1, GameObject* obj2);
void GAMEOBJECT_update_boundbox(f16 x, f16 y, GameObject* obj);
void GAMEOBJECT_clamp_screen(GameObject* obj);
void GAMEOBJECT_wrap_screen(GameObject* obj);
void GAMEOBJECT_bounce_off_screen(GameObject* obj);

inline void GAMEOBJECT_set_hwsprite_position(GameObject* obj) {
	GAMEOBJECT_update_boundbox(obj->x, obj->y, obj);
	SPR_setPosition(obj->sprite, obj->box.left + obj->w_offset, obj->box.top + obj->h_offset);
}

#endif // _STRUCTS_H_