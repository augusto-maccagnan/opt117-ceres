#ifndef _LEVEL_H_
#define _LEVEL_H_

#include <genesis.h>
#include "globals.h"
#include "gameobject.h"
#include "resources.h"

#define NUMBER_OF_LEVELS 5

#define IDX_EMPTY 	   4
#define IDX_ITEM  	   8
#define IDX_ITEM_DONE  80
#define IDX_WALL_FIRST 0
#define IDX_WALL_LAST  3

// #define OFFSCREEN_TILES 3

#define COLLISION_LEFT   0b0001
#define COLLISION_RIGHT  0b0010
#define COLLISION_HORIZ  0b0011

#define COLLISION_TOP    0b0100
#define COLLISION_BOTTOM 0b1000
#define COLLISION_VERT   0b1100

extern Map* map;
extern u8 collision_map[SCREEN_METATILES_W][(SCREEN_METATILES_H * (MAX_ROOMS))]; // screen collision map

// screen cell position in map
extern u16 screen_x;
extern u16 screen_y;

extern u8 collision_result;
extern u8 update_tiles_in_VDP;

/*
  COLLECTED TILE ITEMS TABLE
  This table controls which item(s) was collected in each room (screen).
  - Each index is mapped to a room
  - In each position, each bit is mapped to a item in that room (order is from left to right, top to bottom)
  - IMPORTANT: this implementation only allows 64 items (64 bits) per room.
*/
extern u32 items_table[MAX_ROOMS*2]; 

////////////////////////////////////////////////////////////////////////////
// INITIALIZATION

u16 LEVEL_init(u16 ind, u8 rooms);
void LEVEL_generate_screen_collision_map(u8 first_index, u8 last_index, u8 rooms);

////////////////////////////////////////////////////////////////////////////
// GAME LOOP/LOGIC

inline u8 LEVEL_collision_result() {
	return collision_result;
}

inline u8 LEVEL_wallXY(s16 x, s16 y) {
	return collision_map[x/METATILE_W][y/METATILE_W] == 1;
}

inline u8 LEVEL_tileXY(s16 x, s16 y) {
	return collision_map[x/METATILE_W][y/METATILE_W];
}

inline u8 LEVEL_tileIDX(s16 metatile_x, s16 metatile_y) {
	return collision_map[metatile_x][metatile_y];
}

inline u16 LEVEL_mapIDX(s16 tile_x, s16 tile_y) {
	return (MAP_getTile(map, tile_x, tile_y) & 0x03FF);
}

inline void LEVEL_set_tileXY(s16 x, s16 y, u8 value) {
	collision_map[x/METATILE_W][y/METATILE_W] = value;
}

inline void LEVEL_set_tileIDX(s16 x, s16 y, u8 value) {
	collision_map[x][y] = value;
}

u8 LEVEL_check_wall(GameObject* obj);
void LEVEL_move_and_slide(GameObject* obj);

void LEVEL_remove_tile(s16 x, s16 y, u8 new_tile);
void LEVEL_register_items_collected(s8 room);
void LEVEL_restore_items(s8 room);

void LEVEL_scroll_update(s16 offset_x, s16 offset_y, u8 rooms);
void LEVEL_update_camera(GameObject* obj, u8 rooms);

inline void LEVEL_update_items_in_VDP() {
	if (update_tiles_in_VDP) {
		LEVEL_restore_items(screen_y/SCREEN_H * 3 + screen_x/SCREEN_W);
		update_tiles_in_VDP = false;
	}
}

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

void LEVEL_draw_collision_map(u8 rooms);
void LEVEL_draw_tile_map();

// DEBUG: change for map you want to draw
inline void LEVEL_draw_map(u8 rooms) {
	LEVEL_draw_collision_map(rooms);
	// LEVEL_draw_tile_map();
}

#endif // _LEVEL_H_