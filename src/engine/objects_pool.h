#ifndef _OBJECTS_POOL_
#define _OBJECTS_POOL_

#include <genesis.h>
#include "engine/gameobject.h"

typedef struct {
    GameObject* active;	     // head of active objects (used) linked list
	GameObject* free;		 // head of available objects linked list
    GameObject* curr_active; // loop through active list
} ObjectsPool;


void OBJPOOL_init(ObjectsPool* pool, GameObject* array, u8 n);
void OBJPOOL_clear(ObjectsPool* pool);

void OBJPOOL_release(ObjectsPool* pool, GameObject* obj);
GameObject* OBJPOOL_get_available(ObjectsPool* pool);
GameObject* OBJPOOL_loop_init(ObjectsPool* pool);
GameObject* OBJPOOL_loop_next(ObjectsPool* pool);

#endif