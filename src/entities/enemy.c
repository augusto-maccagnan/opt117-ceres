#include <genesis.h>
#include "enemy.h"
#include "engine/level.h"
#include "entities/player.h"

// #define DEBUG

static u16** ufo_indexes;
static u16** rkt_indexes;
static u16** kaz_indexes;

u16 ufo_index;
u16 rkt_index;
u16 kaz_index;

////////////////////////////////////////////////////////////////////////////
// PRIVATE PROTOTYPES

// static void frame_changed(Sprite* sprite);

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 ENEMY_load_tiles(u16 ind) {
    u16 num_tiles, aux;
    aux = ind;
    ufo_index = ind;
	ufo_indexes = SPR_loadAllFrames(&spr_ufo, ind, &num_tiles);
    ind += num_tiles;

    kaz_index = ind;
    kaz_indexes = SPR_loadAllFrames(&spr_kaz, ind, &num_tiles);
    ind += num_tiles;

    rkt_index = ind;
    rkt_indexes = SPR_loadAllFrames(&spr_rkt, ind, &num_tiles);

    return ind - aux;
}

u16 ENEMY_init(GameObject* const obj, const MapObject* const mapobj, u16 ind) {
	f32 x = F32_toInt(mapobj->x);
    f32 y = F32(0);
    switch (mapobj->type) 
    {
    case ENEMY_UFO:
        y = F32_toInt(mapobj->y) % SCREEN_H - 16;
        GAMEOBJECT_init_no_pal(obj, &spr_ufo, x, y, 0, -4, PAL_ENEMY, ufo_index);
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
        y = screen_y- F32_toInt(mapobj->y) - SCREEN_H/3;
        GAMEOBJECT_init_no_pal(obj, &spr_rkt, x, y, 0, -4, PAL_ENEMY, rkt_index);
        obj->speed_y = mapobj->speed;
        break;
    case ENEMY_KAZ:
        y = screen_y - F32_toInt(mapobj->y) - SCREEN_H/3;
        GAMEOBJECT_init_no_pal(obj, &spr_kaz, x, y, 0, -4, PAL_ENEMY, kaz_index);
        obj->speed_y = mapobj->speed;
        obj->speed = mapobj->speed;
        break;
    default:
        GAMEOBJECT_init_no_pal(obj, &spr_rkt, x, y, 0, -4, PAL_ENEMY, rkt_index);
        kprintf("ERROR: MAPOBJECTS - unknow enemy type %d. Default to ENEMY_RKT.", mapobj->type);
        break;
    }

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
    obj->damage = mapobj->damage;
    obj->sprite->data = mapobj->type;

    SPR_setAutoTileUpload(obj->sprite, FALSE);
    SPR_setFrameChangeCallback(obj->sprite, &frame_changed);

    SPR_setVisibility(obj->sprite, VISIBLE);
    return ind;
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
            // kprintf("UFO should shoot!");
            obj->timer = 0;
        }
    }

    GAMEOBJECT_set_hwsprite_position(obj);
}

void ENEMY_rkt_update(GameObject* obj) {
    obj->x += obj->speed_x;
    obj->y += obj->speed_y;
    
    if(F16_toInt(obj->y) + obj->h > SCREEN_H){
        obj->active = FALSE;
    }

    GAMEOBJECT_update_boundbox(obj->x, obj->y, obj);
    GAMEOBJECT_set_hwsprite_position(obj);
}
// f16 angle;
void ENEMY_kaz_update(GameObject* obj) {
    obj->x += obj->speed_x;
    obj->y += obj->speed_y;
    // kprintf("x:%d y:%d speed_x:%d", F16_toInt(obj->x), F16_toInt(obj->y), F16_toInt(obj->speed_x));
    if(F16_toInt(obj->y) > 0 && F16_toInt(obj->y) < SCREEN_H){
        // start seeking player
        obj->speed_x = F16_mul(F16_cos(F16_atan2(obj->y - player.y, obj->x - player.x)), -obj->speed);
    }

    if(F16_toInt(obj->y) + obj->h > SCREEN_H){
        obj->active = FALSE;
    }

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

static void frame_changed(Sprite* sprite) {
    // get VRAM tile index for this animation of this sprite
    u16 tileIndex;
    switch (sprite->data)
    {
    case ENEMY_UFO:
        tileIndex = ufo_indexes[sprite->animInd][sprite->frameInd];
        break;
    case ENEMY_RKT:
        tileIndex = rkt_indexes[sprite->animInd][sprite->frameInd];
        break;
    case ENEMY_KAZ:
        tileIndex = kaz_indexes[sprite->animInd][sprite->frameInd];
        break;
    default:
        return;
        break;
    }
    // u16 tileIndex = ball_indexes[sprite->animInd][sprite->frameInd];
    // manually set tile index for the current frame (preloaded in VRAM)
    SPR_setVRAMTileIndex(sprite, tileIndex);
}