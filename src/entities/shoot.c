#include "entities/shoot.h"

static u16** sht_indexes;
u16 sht_index;

u16 SHOOT_load_tiles(u16 ind) {
    u16 num_tiles;

    sht_index = ind;
	sht_indexes = SPR_loadAllFrames(&spr_bullet, ind, &num_tiles);

    return num_tiles;
}

u16 SHOOT_init(GameObject* obj, s16 x, s16 y){
    GAMEOBJECT_init_no_pal(obj, &spr_ufo, x, y, 0, 0, PAL_ENEMY, sht_index);
    obj->speed_x = 0;
    obj->speed_y = F16(SHOOT_SPEED);
    obj->active = TRUE;

    SPR_setAutoTileUpload(obj->sprite, FALSE);
    SPR_setFrameChangeCallback(obj->sprite, &frame_changed);

    SPR_setVisibility(obj->sprite, VISIBLE);

    return obj->sprite->definition->maxNumTile;
}

void SHOOT_update(GameObject* obj) {

}

static void frame_changed(Sprite* sprite) {
    // get VRAM tile index for this animation of this sprite
    u16 tileIndex = sht_indexes[sprite->animInd][sprite->frameInd];

    // manually set tile index for the current frame (preloaded in VRAM)
    SPR_setVRAMTileIndex(sprite, tileIndex);
}