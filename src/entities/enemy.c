#include <genesis.h>
#include "enemy.h"
#include "engine/level.h"

// #define DEBUG

static u16** ufo_indexes;
static u16** rkt_indexes;
static u16** kaz_indexes;

////////////////////////////////////////////////////////////////////////////
// PRIVATE PROTOTYPES

// static void frame_changed(Sprite* sprite);

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

// u16 ENEMY_load_tiles(u16 ind) {
// 	u16 num_tiles;
// 	ufo_indexes = SPR_loadAllFrames(&spr_ufo, ind, &num_tiles);
//     ind += num_tiles;
//     kaz_indexes = SPR_loadAllFrames(&spr_kaz, ind, &num_tiles);
//     ind += num_tiles;
//     rkt_indexes = SPR_loadAllFrames(&spr_rkt, ind, &num_tiles);
	
//     return num_tiles;
// }

u16 ENEMY_init(GameObject* const obj, const MapObject* const mapobj, u16 ind) {
    // convert map coord to screen coord (and fix 1 tile offset y from Tiled)
	f32 x = F32_toInt(mapobj->x);
	f32 y = F32_toInt(mapobj->y) % SCREEN_H - 16;

    switch (mapobj->type) 
    {
    case ENEMY_UFO:
        ind += GAMEOBJECT_init(obj, &spr_ufo, x, y, 0, 0, PAL_ENEMY, ind);
        if(mapobj->x < 0){
            #ifdef DEBUG
            kprintf("going rigth");
            #endif
            obj->speed_x = mapobj->speed;
        }
        if(mapobj->x > 320){
            #ifdef DEBUG
            kprintf("going left");
            #endif
            obj->speed_x = -mapobj->speed;
        }
        break;
    case ENEMY_RKT:
        ind += GAMEOBJECT_init(obj, &spr_rkt, x, y, -4, -4, PAL_ENEMY, ind);
        obj->speed_y = -mapobj->speed;
        break;
    case ENEMY_KAZ:
        ind += GAMEOBJECT_init(obj, &spr_kaz, x, y, -4, -4, PAL_ENEMY, ind);
        obj->speed_y = -mapobj->speed;
        break;
    default:
        ind += GAMEOBJECT_init(obj, &spr_ufo, x, y, -4, -4, PAL_ENEMY, ind);
        kprintf("ERROR: MAPOBJECTS - unknow enemy type %d. Default to ENEMY_UFO.", mapobj->type);
        break;
    }
    // GAMEOBJECT_init(obj, &spr_ball, x, y, -4, -4, PAL_ENEMY, ind);
    // obj->speed_x = F16_mul(  cosFix16(mapobj->direction * 128), mapobj->speed );
    // obj->speed_y = F16_mul( -sinFix16(mapobj->direction * 128), mapobj->speed );

    // check enemy type and define behavior
    switch (mapobj->type) {
        case ENEMY_UFO:
            obj->update = ENEMY_ufo_update;
            // set phase 1 for UFO
            obj->phase = 1;
            break;
        case ENEMY_RKT:
            obj->update = ENEMY_rkt_update;
            break;
        case ENEMY_KAZ:
            obj->update = ENEMY_kaz_update;
            break;
        default:
            obj->update = ENEMY_ufo_update;
            kprintf("ERROR: MAPOBJECTS - unknow enemy type %d. Default to ENEMY_UFO.", mapobj->type);
        }
    obj->on_hit = ENEMY_on_hit;

    obj->type = mapobj->type;
    obj->timer = 0;

    // SPR_setAutoTileUpload(obj->sprite, FALSE);
    SPR_setVisibility(obj->sprite, VISIBLE);
    return ind;
    // SPR_setFrameChangeCallback(obj->sprite, &frame_changed);
}

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

void ENEMY_ufo_update(GameObject* obj) {
    if(!obj->active) {
        // if object is not active, do nothing
        return;
    }
    #ifdef DEBUG
    kprintf("x:%d y:%d", F16_toInt(obj->x), F16_toInt(obj->y));
    #endif
    obj->x += obj->speed_x;
    obj->y += obj->speed_y;
    #ifdef DEBUG
    kprintf("upd_x:%d", F16_toInt(obj->x));
    #endif
    f16 aux_speed_x;

    GAMEOBJECT_update_boundbox(obj->x, obj->y, obj);
    
    switch (obj->phase)
    {
    case 1:// phase 1: entering screen
        #ifdef DEBUG
        kprintf("phase 1");
        #endif
        // wait till enemy is getting to the middle of sreen to activate phase 2
        if(F16_toInt(obj->x) > SCREEN_W/3 && F16_toInt(obj->x) < (2 * SCREEN_W/3)){
            obj->phase = 2;
        }
        break;
    case 2:// phase 2: bounce of first hit side
        #ifdef DEBUG
        kprintf("phase 2");
        #endif
        aux_speed_x = obj->speed_x;
        GAMEOBJECT_bounce_off_screen(obj);
        // activate phase 3 as enemy bounces off screen
        if(aux_speed_x != obj->speed_x){
            #ifdef DEBUG
            kprintf("phase 2 - bounce off screen");
            #endif
            obj->phase = 3;
        }
        break;
    case 3:// phase 3: go off-screen and despawn
        // wait till ufo completly outside off screen and despawns
        #ifdef DEBUG
        kprintf("phase 3");
        #endif
        if(F16_toInt(obj->x) < -obj->w || F16_toInt(obj->x) > (SCREEN_W + obj->w)){
            #ifdef DEBUG
            kprintf("phase 3 - despawn");
            #endif
            obj->active = false;
            return;
        }
        break;
    default:
        // case data isn't setted yet, set phase 1 on first execution
        obj->phase = 1;
        break;
    }

    // only shoots if is not off screen
    if(!(obj->x < F16(0 - obj->w) || obj->x > F16(SCREEN_W + obj->w))){
        // shooting timer for UFO, every 1 second
        obj->timer++;
        if(obj->timer % 60 == 0){
            // UFO_shoot() || SHOT_init(x, y, speed_x, speed_y)
            // UFO_shoot();
            kprintf("UFO should shoot!");
            obj->timer = 0;
        }
    }

    GAMEOBJECT_set_hwsprite_position(obj);
}

void ENEMY_rkt_update(GameObject* obj) {
    obj->x += obj->speed_x;
    obj->y += obj->speed_y;
    
    GAMEOBJECT_update_boundbox(obj->x, obj->y, obj);
    GAMEOBJECT_set_hwsprite_position(obj);
}

void ENEMY_kaz_update(GameObject* obj) {
    obj->x += obj->speed_x;
    obj->y += obj->speed_y;
    
    GAMEOBJECT_update_boundbox(obj->x, obj->y, obj);
    GAMEOBJECT_set_hwsprite_position(obj);
}

void ENEMY_on_hit(GameObject* obj, u8 amount) {
    KLog("Enemy hit!");
}

// Much worst performer...
// void ENEMY_update2(GameObject* obj) {
//     obj->next_x = obj->x + obj->speed_x;
//     obj->next_y = obj->y + obj->speed_y;

//     LEVEL_move_and_slide(obj);

//     obj->x = obj->next_x;
//     obj->y = obj->next_y;

//     if (LEVEL_collision_result() & COLLISION_BOTTOM || LEVEL_collision_result() & COLLISION_TOP) {
//         obj->speed_y = -obj->speed_y;
//     }
//     if (LEVEL_collision_result() & COLLISION_LEFT || LEVEL_collision_result() & COLLISION_RIGHT) {
//         obj->speed_x = -obj->speed_x;
//     }

//     GAMEOBJECT_set_hwsprite_position(obj);
//     GAMEOBJECT_bounce_off_screen(obj);
// }

////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

// static void frame_changed(Sprite* sprite) {
//     // get VRAM tile index for this animation of this sprite
//     u16 tileIndex = ball_indexes[sprite->animInd][sprite->frameInd];
	
//     // manually set tile index for the current frame (preloaded in VRAM)
//     SPR_setVRAMTileIndex(sprite, tileIndex);
// }