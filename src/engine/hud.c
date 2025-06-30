#include "hud.h"

u16 player_score;

u16 HUD_init(u16 ind) {
	VDP_setTextPlane(WINDOW);
	
	// by default, text uses the last color of first palette
	VDP_setTextPalette(PAL_BACKGROUND);
	VDP_setBackgroundColor(PAL_BACKGROUND*16+7);
	PAL_setColor(PAL_BACKGROUND*16+15, RGB24_TO_VDPCOLOR(0x00EE00));	// text fg, pal index 15
	PAL_setColor(PAL_BACKGROUND*16+6,  RGB24_TO_VDPCOLOR(0x494949));	// text bg, index of the whiter color in palette
	
	// VDP_setWindowHPos(FALSE, 0);
	// Freezes the first 3 rows of background tiles (it won't scroll)
	VDP_setWindowVPos(FALSE, HUD_TILES);
	
	VDP_drawImageEx(WINDOW, &img_hud, TILE_ATTR_FULL(PAL_BACKGROUND, 1, 0, 0, ind), 0, 0, FALSE, DMA);
	ind += img_hud.tileset->numTile;
	
	VDP_drawText("HEALTH|----------|  SCORE 1337 UP ----", 1, 0);
	HUD_score(0);

    return ind;
}

void HUD_update_health(u8 value) {
	char health[PLAYER_MAX_HEALTH+1] = "          ";
	for (u8 i = 0; i < value; i++) {
		health[i] = '-';
	}
	VDP_drawText(health, 8, 0);
}

void HUD_score(u8 value) {
	player_score += value;
	HUD_update_score();
}

void HUD_update_score() {
	char score[5];
	intToStr(player_score, score, 4);
	VDP_drawText(score, 27, 0);
}
