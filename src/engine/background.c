#include "background.h"

// parallax scroling
fix16 offset_pos[SCREEN_TILES_W/2] = {0}; // 320 px /8 = 40 tiles /2 = 20 columns
fix16 offset_speed[SCREEN_TILES_W/2] = {0};
s16 values[SCREEN_TILES_W/2] = {0};

////////////////////////////////////////////////////////////////////////////
// INIT

u16 BACKGROUND_init(u16 ind) {
	VDP_setPlaneSize(64, 64, TRUE);

	VDP_drawImageEx(BG_BACKGROUND, &img_background, TILE_ATTR_FULL(PAL_BACKGROUND, 0, 0, 0, ind), 0, 0, TRUE, DMA);
	
	VDP_setScrollingMode(HSCROLL_TILE , VSCROLL_COLUMN);
	
	f16 speed = FIX16(-0.6);
	// 5; 4; 3; 2; 1; 0
	// 19; 18; 17; 16; 15; 14
	for (u8 i = 5; i < 255; --i) {
		set_offset_speed(i, 1, speed);
		set_offset_speed((SCREEN_TILES_W/2)-i-1, 1, speed);
		if(!(i%3)) {
			speed += FIX16(-0.2);
		}
	}
    set_offset_speed(5, 9, FIX16(-0.6));
	
	return img_background.tileset->numTile;
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

void BACKGROUND_update() {
	for (u8 i = 0; i < SCREEN_TILES_W/2; i++) {
		offset_pos[i] += offset_speed[i];
		values[i] = F16_toInt(offset_pos[i]);
	}
	VDP_setVerticalScrollTile(BG_BACKGROUND, 0, values, SCREEN_TILES_W/2, DMA);
}

void set_offset_speed(u8 start, u8 len, fix16 speed) {
	if (start+len-1 >= SCREEN_TILES_W/2) {
		return;
	}
	for (u8 i = start; i <= start+len-1; i++) {
		offset_speed[i] = speed;
	}
}
