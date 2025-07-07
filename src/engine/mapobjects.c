#include "mapobjects.h"
#include "engine/level.h"
#include "engine/utils.h"

#define DEBUG

// RoomEnemies lookup_table[MAX_ROOMS];
int curr_mapobj;

///////////////////////////////////////////////////////////////////////////
// MAP OBJECTS EXTRACTION

/**
 * Inits the loop to get the MapObjects from a room.
 * Returns the first MapObject in room.
 * To loop through the remaining, use LEVEL_loop_next().
 */
// MapObject* MAPOBJ_loop_init(const void* level_objects[], u16 n, u8 room) {
//     curr_mapobj = lookup_table[room].first;

//     // empy rooms have crossed indexes
//     if (lookup_table[room].first > lookup_table[room].last) {
//         return NULL;
//     }
//     return (MapObject*)level_objects[curr_mapobj];
// }

/**
 * Loops through room's MapObjects.
 * Returns the next MapObject in room.
 * To init the loop, use LEVEL_loop_init().
 */
// MapObject* MAPOBJ_loop_next(const void* level_objects[], u16 n, u8 room) {
//     curr_mapobj++;
//     if (curr_mapobj > lookup_table[room].last) {
//         return NULL;
//     }
//     if (curr_mapobj >= n) {
//         return NULL;
//     }
//     return (MapObject*)level_objects[curr_mapobj];
// }

/**
 * Returns the next MapObject in the level_objects
 * validating if the enemy is close enough to the active screen.
 */
MapObject* MAPOBJ_lookup_enemies(const void* level_objects[], u16 n){
    MapObject* obj = NULL;
    if(curr_mapobj >= n) {
        // if all objects were already processed, return NULL
        return NULL;
    }
    // check if current MapObject is vertically close to active screen
    // OBS: inverse index, because the level starts from bottom and ends on top
    // OBS2: level_objects is ordered by y position
    obj = (MapObject*)level_objects[n-curr_mapobj-1];
    if(obj->y >= F32(screen_y - SCREEN_H/4)) {
        obj->y = FIX32(F32_toInt(obj->y) % SCREEN_H + SCREEN_H);
        #ifdef DEBUG
        print_object_info(obj);
        #endif
        curr_mapobj++;
        return obj;   
    } else {
        // if y is out of screen, return NULL
        return NULL;
    }
}

void print_object_info(MapObject* obj){
    kprintf("MapObject: %d, x: %ld, y: %ld, type: %d, damage: %d", 
            curr_mapobj, F32_toInt(obj->x), F32_toInt(obj->y), obj->type, obj->damage);
}