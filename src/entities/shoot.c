#include "entities/shoot.h"

// #define DEBUG

static u16** sht_indexes;
u16 sht_index;

u16 SHOOT_load_tiles(u16 ind) {
    u16 num_tiles;

    sht_index = ind;
	sht_indexes = SPR_loadAllFrames(&spr_bullet, ind, &num_tiles);

    return num_tiles;
}

u16 SHOOT_init(GameObject* obj, s16 x, s16 y, u8 type){
    GAMEOBJECT_init_no_pal(obj, &spr_bullet, x, y, 0, 0, PAL_ENEMY, sht_index);
    obj->speed_x = 0;
    if(type == ALIED_SHOOT) {
        obj->speed_y = F16(-SHOOT_SPEED);
    } else if(type == ENEMY_SHOOT){
        obj->speed_y = F16(SHOOT_SPEED);
    }
    obj->active = TRUE;

    obj->update = SHOOT_update;

    SPR_setAutoTileUpload(obj->sprite, FALSE);
    SPR_setFrameChangeCallback(obj->sprite, &SHOOT_frame_changed);

    SPR_setVisibility(obj->sprite, VISIBLE);

    return obj->sprite->definition->maxNumTile;
}

void SHOOT_update(GameObject* obj) {
    obj->x += obj->speed_x;
    obj->y += obj->speed_y;

    if(!obj->active) {
        // if object is not active, do nothing
        return;
    }
    GAMEOBJECT_update_boundbox(obj->x, obj->y, obj);

    if(F16_toInt(obj->y) > SCREEN_H){
        obj->active = FALSE; // remove shot
    }

    GAMEOBJECT_set_hwsprite_position(obj);
}

static void SHOOT_frame_changed(Sprite* sprite) {
    // get VRAM tile index for this animation of this sprite
    u16 tileIndex = sht_indexes[sprite->animInd][sprite->frameInd];
    // manually set tile index for the current frame (preloaded in VRAM)
    SPR_setVRAMTileIndex(sprite, tileIndex);
}