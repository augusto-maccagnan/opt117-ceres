#ifndef _MAP_OBJECTS_H_
#define _MAP_OBJECTS_H_

#include <genesis.h>
#include "engine/gameobject.h"

// enemies room table
typedef struct {
	u16 first;
	u16 last;
} RoomEnemies;

typedef struct {
	u16 room;
    f32 x;
	f32 y;
    u16 direction;
    f16 speed;
    u8 type;
} MapObject;

extern RoomEnemies lookup_table[MAX_ROOMS];
extern int curr_mapobj;

void MAPOBJ_init_mapobjects(const void* level_objects[], u16 n);

MapObject* MAPOBJ_loop_init(const void* level_objects[], u16 n, u8 room);
MapObject* MAPOBJ_loop_next(const void* level_objects[], u16 n, u8 room);

#endif