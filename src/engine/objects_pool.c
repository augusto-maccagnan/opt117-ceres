#include "objects_pool.h"
#include "engine/utils.h"

/////////////////////////////////////////////////////////////////////
// LINKED LIST OP

static void LINKEDLIST_add(GameObject** list, GameObject* obj) {
    obj->next = *list;
    *list = obj;
}

static GameObject* LINKEDLIST_remove(GameObject** list, GameObject* obj) {
    GameObject* p = *list;
    GameObject* prev = NULL;
    
    while (p) {
        if (p == obj) break;
        prev = p;
        p = p->next;
    }
    if (!p) {                   // list is empty or obj not found
        return NULL;
    }
    if (!prev) {                // p is head
        *list = p->next;
    } else {
        prev->next = p->next;   // p is not head
    }

    return p;
}

static GameObject* LINKEDLIST_remove_first(GameObject** list) {
    if (*list) {                 // if not empty
        GameObject* obj = *list; // first list element
        *list = obj->next;
        return obj;
    }
    return NULL;
}

static void LINKEDLIST_print(GameObject* list) {
    kprintf("LINKED LIST: ");
    while (list) {
        text_add_int(list->dir);
        list = list->next;
    }
    text_print_and_clear();
}

// static void print_array(GameObject* array, u8 n) {
//     KLog("GAMEOBJECT ARRAY:");
//     for (u8 i = 0; i < n; ++i) {
//         text_add_int(array[i].dir = i); // just for testing
//     }
//     text_print_and_clear();
// }

/////////////////////////////////////////////////////////////////////
// OBJECTS POOL OP

/**
 * Inits GameObject pool from array.
 * It uses two linked lists inside the array to manage free and active (used) enemies.
 * OBS: Fields GameObject::active and GameObject::next are modified by this function. * 
 */
void OBJPOOL_init(ObjectsPool* pool, GameObject* array, u8 n) {
    pool->active = NULL;    // empty list
    pool->free = array;     // first array object
    pool->curr_active = NULL;

    // init array's linked list
    for (u8 i = 0; i < n-1; ++i) {
        array->active = FALSE;
        array->dir = i;     // ID for testing
        array->next = (array+1);
        array++;
    }
    array->active = FALSE;
    array->dir = n-1;       // ID for testing
    array->next = NULL;

    LINKEDLIST_print(pool->free);
    LINKEDLIST_print(pool->active);
}


/**
 * Removes all objects from active list, adding them into the free list.
 * OBS: Calls SPR_releaseSprite() and sets active=FALSE for every previously active obj.
 */
void OBJPOOL_clear(ObjectsPool* pool) {
    GameObject* obj = OBJPOOL_loop_init(pool);
    while (obj) {
        SPR_releaseSprite(obj->sprite);
        obj->active = FALSE;
        GameObject* next = obj->next;
        LINKEDLIST_remove(&pool->active, obj);
        LINKEDLIST_add(&pool->free, obj);
        obj = next;
    }
}

/**
 * Removes specific GameObject from active list, making it available to reuse (through free list).
 * OBS: Calls SPR_releaseSprite() and sets active=FALSE for every previously active obj.
 */
void OBJPOOL_release(ObjectsPool* pool, GameObject* obj) {
    LINKEDLIST_remove(&pool->active, obj);
    LINKEDLIST_add(&pool->free, obj);
    
    SPR_releaseSprite(obj->sprite);
    obj->active = FALSE;
}

/**
 * Returns the next available GameObject from pool or NULL if there is none.
 * OBS: It doesn't set GameObject::active to TRUE.
 */
GameObject* OBJPOOL_get_available(ObjectsPool* pool) {
    if (pool->free) {
        GameObject* obj = LINKEDLIST_remove_first(&pool->free);
        obj->active = TRUE;
        LINKEDLIST_add(&pool->active, obj);
        return obj;
    }

    return NULL;
}

/**
 * Inits the loop into the active objects list.
 * Returns the first active object.
 * To loop through the active objects, use OBJPOOL_loop_next().
 */
GameObject* OBJPOOL_loop_init(ObjectsPool* pool) {
    pool->curr_active = pool->active;
    return pool->curr_active;
}

/**
 * Loops through pool's active GameObjects.
 * Returns the next active object or NULL.
 * To init the loop, use OBJPOOL_loop_init().
 */
GameObject* OBJPOOL_loop_next(ObjectsPool* pool) {
    // restarts Linked List loop
    if (pool->curr_active) {
        pool->curr_active = pool->curr_active->next;
    }

    return pool->curr_active;
}