#include "gameobject.h"
#include "utils.h"

////////////////////////////////////////////////////////////////////////////
// INIT

u16 GAMEOBJECT_init(GameObject* const obj, const SpriteDefinition* const sprite, s16 x, s16 y, u8 pal, u16 ind) {
	obj->x = FIX16(x);
	obj->y = FIX16(y);
	obj->next_x = obj->x;
	obj->next_y = obj->y;
	obj->speed_x = 0;
	obj->speed_y = 0;
	obj->anim = 0;
	PAL_setPalette(pal, sprite->palette->data, DMA);
	
	obj->sprite = SPR_addSprite(sprite, x, y, TILE_ATTR_FULL(pal, FALSE, FALSE, 0, ind));
	obj->w = obj->sprite->definition->w;
	obj->h = obj->sprite->definition->h;
	
	return obj->sprite->definition->maxNumTile;
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

/*
inline void update_rect(GameObject* obj) {
	obj->rect.left  = obj->x;
	obj->rect.right = obj->x + obj->sprite->definition->w;
	obj->rect.top   = obj->y;
	obj->rect.bottom= obj->y + obj->sprite->definition->h;
}
*/

void GAMEOBJECT_update_boundbox(f16 x, f16 y, GameObject* obj) {
	obj->box.left  = fix16ToInt(x);
	obj->box.top   = fix16ToInt(y);
	obj->box.right = fix16ToInt(x) + obj->w;// - 1;
	obj->box.bottom= fix16ToInt(y) + obj->h;// - 1;
}

void GAMEOBJECT_clamp_screen(GameObject* obj) {
	obj->x = clamp(obj->x, 0, FIX16(SCREEN_W));
	obj->y = clamp(obj->y, 0, FIX16(SCREEN_H));
}

void GAMEOBJECT_wrap_screen(GameObject* obj) {
	// WRAP(obj->x, -fix16Div(obj->w, 2), FIX16(SCREEN_W) - obj->w/2)
	// WRAP(obj->y, -fix16Div(obj->h, 2), FIX16(SCREEN_H) - obj->h/2)
}

void GAMEOBJECT_bounce_off_screen(GameObject* obj) {
	// bounce off screen bounds
	if (obj->x < 0 || (obj->x + obj->sprite->definition->w) > SCREEN_W) {
		obj->speed_x = -obj->speed_x;
	}	

	if (obj->y < 0 || (obj->y + obj->sprite->definition->h) > SCREEN_H) {
		obj->speed_y = -obj->speed_y;
	}	
}
