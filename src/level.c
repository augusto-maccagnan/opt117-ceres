#include <genesis.h>
#include "level.h"

// #define DEBUG

#define SCROLLING_SPEED 1

Map* map;
u8 collision_map[SCREEN_METATILES_W + OFFSCREEN_TILES*2][(SCREEN_METATILES_H * (NUMBER_OF_ROOMS)) + OFFSCREEN_TILES*2] = {0}; // screen collision map

// Top-Left screen position in map
u16 screen_x = 0;
u16 screen_y = (NUMBER_OF_ROOMS-1)*SCREEN_H; // 224

u8 collision_result;
u8 update_tiles_in_VDP = false;

u8 scroll_time = 0;

u32 items_table[NUMBER_OF_ROOMS*2] = {0}; 

////////////////////////////////////////////////////////////////////////////
// INIT

u16 LEVEL_init(u16 ind) {
	PAL_setPalette(PAL_MAP, level1_pal.data, DMA);
	VDP_loadTileSet(&level1_tiles, ind, DMA);
	map = MAP_create(&level1_map, BG_MAP, TILE_ATTR_FULL(PAL_MAP, FALSE, FALSE, FALSE, ind));

	MAP_scrollToEx(map, 0, (NUMBER_OF_ROOMS-1)*SCREEN_H, TRUE);
	
	LEVEL_generate_screen_collision_map(IDX_WALL_FIRST, IDX_WALL_LAST);
	
	ind += level1_tiles.numTile;

	return ind;
}
/**
 * Varre a estrutura do MAPA TMX e monta a matriz de colisão do espaço da tela.
 *   Paredes -> idx 1
 * 		Para otimizar os tiles de parede devem estar de forma contigua, na mesma "linha" do tileset
 *   Vazio   -> idx 0
 *   Demais  -> idx original do TMX
 */
void LEVEL_generate_screen_collision_map(u8 first_wall, u8 last_wall) {
/*
    IMPORTANTE
	Os indices dos TILES sao os mesmos do tileset, embora os tiles no VDP sejam 8x8.
	Logo, precisa considerar que os METATILES 16x16 serao separados em 4 tiles e colocados
	na VRAM continuamento de acordo com as linhas do tileset.
*/

/*
	Alterando para construir a matriz de colisão do mapa todo,
	e ir acompanhando a tela conforme o scroll do jogador.
*/
	s16 start_x = screen_x/METATILE_W; // find screen top-left cell in map
	// s16 start_y = screen_y/METATILE_W;
	s16 start_y = 0;
	
	u8 col_x = 0;
	u8 col_y = 0;
	kprintf("x: %d, y: %d", (start_x + SCREEN_METATILES_W), start_y + (SCREEN_METATILES_H * (NUMBER_OF_ROOMS)));
	// x: 0 -> 320
	for (u16 x = start_x; x < start_x + SCREEN_METATILES_W; ++x) {
		// y: 0 -> 224 * NUMBER_OF_ROOMS
		for (u16 y = start_y; y < start_y + (SCREEN_METATILES_H * (NUMBER_OF_ROOMS)); ++y) {
			u16 tile_index = LEVEL_mapIDX(x*(METATILE_W/8), y*(METATILE_W/8));
			
			// kprintf("tile: %d", tile_index);

			if (tile_index == IDX_EMPTY) {										// empty -> 0
				// collision_map[col_x + OFFSCREEN_TILES][col_y + OFFSCREEN_TILES] = 0;
				LEVEL_set_tileIDX(col_x, col_y, 0);
			} else if (tile_index >= first_wall && tile_index <= last_wall) {	// wall  -> 1
				// collision_map[col_x + OFFSCREEN_TILES][col_y + OFFSCREEN_TILES] = 1;
				LEVEL_set_tileIDX(col_x, col_y, 1);
			} else {															// others-> tmx idx
				// collision_map[col_x + OFFSCREEN_TILES][col_y + OFFSCREEN_TILES] = tile_index;								
				LEVEL_set_tileIDX(col_x, col_y, tile_index);
			}
			col_y++;
		}
		col_y = 0;
		col_x++;
	}	
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

// GameObject box must be updated before calling this function
// OBS: this function only checks for objects that are multiple of 16 pixels
u8 LEVEL_check_wall(GameObject* obj) {
	for (u16 x = obj->box.left; x <= obj->box.right ; x += METATILE_W) {
		for (u16 y = obj->box.top; y <= obj->box.bottom; y += METATILE_W) {
			if (LEVEL_wallXY(x, y) != 0)
				return true;
		}
	}
	return false;
}

/**
 * Checks and resolves wall collisions. *  
 * OBS:
 * - To access collision result, use LEVEL_collision_result()
 */ 
void LEVEL_move_and_slide(GameObject* obj) {
	collision_result = 0;
	GAMEOBJECT_update_boundbox(obj->next_x, obj->y, obj);

	/*
	+---------+  <- right,top
	|         |     				} up to 16 px
	|         |  <- right,top+h/2
	|         |      				} up to 16 px
	+---------+  <- right, bottom
	*/
	if (obj->speed_x > 0) {				// moving right
		if (LEVEL_wallXY(obj->box.right, obj->box.top + screen_y) || 
	    	LEVEL_wallXY(obj->box.right, obj->box.top + screen_y + obj->h/2) || 
			LEVEL_wallXY(obj->box.right, obj->box.bottom - 1 + screen_y)) {
				obj->next_x = FIX16(obj->box.right/METATILE_W * METATILE_W - obj->w);
				collision_result |= COLLISION_RIGHT;
		}
	}
	/*
	    left, top ->  +---------+
	                  |         |    } up to 16 px
	left, top+h/2 ->  |         |
	                  |         |    } up to 16 px
	 left, bottom ->  +---------+
	*/
	else 
	if (obj->speed_x < 0) {			// moving left
		if (LEVEL_wallXY(obj->box.left, obj->box.top + screen_y) || 
			LEVEL_wallXY(obj->box.left, obj->box.top + screen_y + obj->h/2) || 
			LEVEL_wallXY(obj->box.left, obj->box.bottom - 1 + screen_y)) {
				obj->next_x = FIX16((obj->box.left/METATILE_W + 1) * METATILE_W);
				collision_result |= COLLISION_LEFT;
		}
	}

	GAMEOBJECT_update_boundbox(obj->next_x, obj->next_y, obj);
	/*
	         left+w/2,
	           top
    left,top    |    right,top
	       |    |    |
 	       +---------+ 
	       |         | 
	       |         | 
	       |         | 
	       +---------+ 
	*/
	if (obj->speed_y < 0) {        // moving up
		if (LEVEL_wallXY(obj->box.left,  obj->box.top + screen_y) || 
			LEVEL_wallXY(obj->box.left + obj->w/2, obj->box.top + screen_y) || 
			LEVEL_wallXY(obj->box.right-1, obj->box.top + screen_y)) {
				obj->next_y = FIX16((obj->box.top/METATILE_W + 1) * METATILE_W);
				collision_result |= COLLISION_TOP;
		}
    }
	/*
	         left+w/2,
	           top
    left,top    |    right,top
	       |    |    |
 	       +---------+ 
	       |         | 
	       |         | 
	       |         | 
	       +---------+ 
	*/
	else
	if (obj->speed_y > 0) {   // moving down
		if (LEVEL_wallXY(obj->box.left,  obj->box.bottom + screen_y) || 
			LEVEL_wallXY(obj->box.left + obj->w/2, obj->box.bottom + screen_y) || 
			LEVEL_wallXY(obj->box.right-1, obj->box.bottom + screen_y)) {
				obj->next_y = FIX16((obj->box.bottom/METATILE_W) * METATILE_W - obj->h);
				collision_result |= COLLISION_BOTTOM;
		}
    }
}

void LEVEL_remove_tile(s16 x, s16 y, u8 new_index) {
	// use 8x8 position in 16x16 collision vector
	LEVEL_set_tileXY(x, y, new_index);

	// find the position of the first 8x8 tile corresponding to the 16x16 tile
	x = (x + screen_x) / METATILE_W * (METATILE_W / 8);
	y = (y + screen_y) / METATILE_W * (METATILE_W / 8);

	VDP_setTileMapXY(BG_MAP, TILE_ATTR_FULL(PAL_MAP, 0, 0, 0, 0), x, y);
	VDP_setTileMapXY(BG_MAP, TILE_ATTR_FULL(PAL_MAP, 0, 0, 0, 0), x+1, y);
	VDP_setTileMapXY(BG_MAP, TILE_ATTR_FULL(PAL_MAP, 0, 0, 0, 0), x, y+1);
	VDP_setTileMapXY(BG_MAP, TILE_ATTR_FULL(PAL_MAP, 0, 0, 0, 0), x+1, y+1);

	#ifdef DEBUG
	LEVEL_draw_map();
	#endif
}

/**
 * Register the item in ITEMS TABLE.
 * Scans the ROOM map by 16x16 tiles, until it finds the corresponding bit in the table.
 * @param x The item x position.
 * @param y The item y position.
 */
void LEVEL_register_items_collected(s8 room) {
	/*
	When an item is colected, it's tiles are removed from MAP and the collision_map
	is set as 80 (item collected mark).
	Then, when exiting a room, the code below search for 8 (IDX_ITEM) or 80 (IDX_ITEM_DONE) in the collision_map,
	scanning through every item in the room (from left to right, top to bottom) and building a BIT MAP (64 bits total) 
	of every item status in the room (1 = item, 0 = collected item).
	*/
	u32 mask = 0x80000000; // 2147483648
	u8 offset = 0;
	u8 count = 0;

	#ifdef DEBUG
	kprintf(" ");
	kprintf("REGISTER ITEMS COLLECTED from room %d", room);	
	#endif
	
	// loop through COLLISION MAP (16x16 metatiles)
	for (u8 tile_y = 0; tile_y < SCREEN_METATILES_H; ++tile_y) {
		for (u8 tile_x = 0; tile_x < SCREEN_METATILES_W; ++tile_x) {
			
			// does it have any item here?
			u8 map_index = LEVEL_tileIDX(tile_x, tile_y);
			if ((map_index == IDX_ITEM) || (map_index == IDX_ITEM_DONE)) {
				++count;
				
				if (map_index == IDX_ITEM) {
					items_table[room*2 + offset] &= ~mask;	// clear flag
				} else {
					items_table[room*2 + offset] |= mask;	// set flag
				}
				#ifdef DEBUG
				kprintf("Item found %d, %d -> %d", tile_x, tile_y, map_index);
				#endif

				// if we already shifted all bits for the first byte, prepare for the second
				if (mask == 0x0001) { 
					mask = 0x80000000;
					offset = 1;
				} else {
					mask = mask >> 1;
				}
			}
		}
	}
	#ifdef DEBUG
	kprintf("mask: %08lX%08lX, items: %d", items_table[room*2], items_table[room*2 + 1], count);
	#endif
}

void LEVEL_restore_items(s8 room) {
	/*
	When the player enters a room, the screen is scrolled and the SGDK retores all item tiles in the room.
	Then, the code below search for 8 (IDX_ITEM) in the SGDK map,
	scanning through every item in the room (from left to right, top to bottom). 
	When a item tile is found (IDX_ITEM), its checked against the items BIT MAP (64 bits in total) to
	find if the item is there or is collected.
	*/
	u32 mask = 0x80000000; // 2147483648
	u8 offset = 0;
	u8 count = 0;

	u16 map_offset_x = screen_x / 8;
	u16 map_offset_y = screen_y / 8;

	#ifdef DEBUG
	kprintf(" ");
	kprintf("RESTORE ITEMS into room %d", room);
	#endif
	
	// loop through TILED MAP (8x8 tiles)
	for (u8 tile_y = 0; tile_y < SCREEN_TILES_H; tile_y += 2) {
		for (u8 tile_x = 0; tile_x < SCREEN_TILES_W; tile_x += 2) {
			
			// does it have any item here?
			u8 map_index = LEVEL_mapIDX(map_offset_x + tile_x, map_offset_y + tile_y);

			if (map_index == IDX_ITEM) {
				++count;

				// Is BIT MAP 0? Remove item from map and collision map
				if (items_table[room*2 + offset] & mask) {
					#ifdef DEBUG
					kprintf("Item removed %d, %d -> %d %lX", tile_x/2, tile_y/2, map_index, items_table[room*2 + offset]);
					#endif
					LEVEL_remove_tile(tile_x * 8, tile_y * 8, IDX_ITEM_DONE);
				}
				
				#ifdef DEBUG
				kprintf("Item found %d, %d -> %d", tile_x/2, tile_y/2, map_index);
				#endif

				// if we already shifted all bits for the first byte, prepare for the second
				if (mask == 0x0001) { 
					mask = 0x80000000;
					offset = 1;
				} else {
					mask = mask >> 1;
				}
			}
		}
	}
	#ifdef DEBUG
	kprintf("mask: %08lX%08lX, items: %d", items_table[room*2], items_table[room*2 + 1], count);
	#endif
}

void LEVEL_scroll_update(s16 offset_x, s16 offset_y) {
	// register items in current room
	// LEVEL_register_items_collected(screen_y/SCREEN_H * 3 + screen_x/SCREEN_W);
	
	// move to next room and generate collision map
	screen_x += offset_x;
	screen_y += offset_y;
	if(screen_y == 0){
		screen_y = (NUMBER_OF_ROOMS-1)*SCREEN_H;
	}
	MAP_scrollTo(map, screen_x, screen_y);

	update_tiles_in_VDP = true;

	#ifdef DEBUG
	LEVEL_draw_map();
	#endif
}

void LEVEL_update_camera(GameObject* obj) {
	scroll_time++;
	// if(scroll_time%2 == 0){
		LEVEL_scroll_update(0, -SCROLLING_SPEED);
	// }
}

////////////////////////////////////////////////////////////////////////////
// DRAWING AND FX

void LEVEL_draw_collision_map() {
    VDP_setTextPlane(BG_B);

	for (u8 tile_x = 0; tile_x < SCREEN_METATILES_W; ++tile_x) {
		for (u8 tile_y = 0; tile_y < SCREEN_METATILES_H; ++tile_y) {
				
				s16 index = LEVEL_tileIDX(tile_x, tile_y);
				if (index != 0) {
					intToStr(index, text, 1);
					VDP_drawText(text, tile_x * METATILE_W/8, tile_y * METATILE_W/8);
				} else {
					VDP_drawText("  ", tile_x * METATILE_W/8, tile_y * METATILE_W/8);
				}
		}
	}
}

void LEVEL_draw_tile_map() {
    VDP_setTextPlane(BG_B);

	u16 map_offset_x = screen_x / 8;
	u16 map_offset_y = screen_y / 8;

	for (u8 tile_x = 0; tile_x < SCREEN_METATILES_W; ++tile_x) {
		for (u8 tile_y = 0; tile_y < SCREEN_METATILES_H; ++tile_y) {

			s16 index = LEVEL_mapIDX(map_offset_x + tile_x*METATILE_W/8, map_offset_y + tile_y*METATILE_W/8);
			if (index != 10) {
				intToStr(index, text, 1);
				VDP_drawText(text, tile_x * METATILE_W/8, tile_y * METATILE_W/8);
			} else {
				VDP_drawText("  ", tile_x * METATILE_W/8, tile_y * METATILE_W/8);
			}
		}
	}
}