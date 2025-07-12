#include "screen.h"

// #define DEBUG

void SCREEN_over_update(){
    // update the screen
    #ifdef DEBUG
    kprintf("GAME OVER SCREEN\n");
    #endif
    VDP_setTextPlane(BG_BACKGROUND);
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    VDP_setVerticalScroll(BG_BACKGROUND, 0);
    VDP_drawText("GAME OVER", SCREEN_METATILES_H/2, SCREEN_METATILES_W/2 + 1);
    VDP_drawText("Press ACTION to restart", SCREEN_METATILES_H/2, SCREEN_METATILES_W/2 + 3);
    SPR_reset();
    while(true) {
        update_input();
        // wait for the player to press the action button
        if(key_pressed(JOY_1, BUTTON_A) || key_pressed(JOY_1, BUTTON_START)) {
            VDP_clearTextArea(0, 0, SCREEN_TILES_W, SCREEN_TILES_H);
            break;
        }
		SPR_update();
		SYS_doVBlankProcess();
    }
    game_state = GAME_INIT;
}

void SCREEN_game_clear_update(){
    #ifdef DEBUG
    kprintf("GAME CLEAR SCREEN\n");
    #endif
    VDP_clearPlane(WINDOW, TRUE);
    VDP_clearPlane(BG_BACKGROUND, TRUE);
    VDP_setWindowVPos(FALSE, SCREEN_TILES_H);
    PAL_setPalette(PAL_BACKGROUND, menu_screen.palette->data, DMA);
    VDP_drawImageEx(WINDOW, &menu_screen, TILE_ATTR_FULL(PAL_BACKGROUND, 0, 0, 0, TILE_USER_INDEX), 0, 0, TRUE, DMA);

    VDP_setTextPlane(WINDOW);
    VDP_setTextPalette(PAL_BACKGROUND);
	VDP_setBackgroundColor(PAL_BACKGROUND*16+11);

    VDP_drawText("Game Clear !", SCREEN_METATILES_W/2 + 4, SCREEN_METATILES_H/2 + 7);
    VDP_drawText("> Continue", SCREEN_METATILES_W/2 + 3, SCREEN_METATILES_H/2 + 9);

    VDP_drawText("Desenvolvimento: Augusto", 2, SCREEN_METATILES_H * 2 - 7);
    VDP_drawText("                 M. Silvano", 2, SCREEN_METATILES_H * 2 - 6);
    VDP_drawText("Design:          Augusto", 2, SCREEN_METATILES_H * 2 - 5);
    VDP_drawText("Arte:            Augusto", 2, SCREEN_METATILES_H * 2 - 4);
    VDP_drawText("Som:             A. Preradovic", 2, SCREEN_METATILES_H * 2 - 3);
    VDP_drawText("                 Z. Rubenstein", 2, SCREEN_METATILES_H * 2 - 2);
    while(true) {
        update_input();
        // wait for the player to press the action button
        if(key_pressed(JOY_1, BUTTON_A) || key_pressed(JOY_1, BUTTON_START)) {
            VDP_clearTextArea(0, 0, SCREEN_TILES_W, SCREEN_TILES_H);
            break;
        }
        SPR_update();
        SYS_doVBlankProcess();
    }
    game_state = GAME_INIT;
}

u8 SCREEN_level_transition(u8 level, u8 ind){
    #ifdef DEBUG
    kprintf("LEVEL %d TRANSITION SCREEN\n", level);
    #endif
    u8 num_tiles = 0;
    VDP_clearPlane(WINDOW, TRUE);
    VDP_setWindowVPos(FALSE, SCREEN_TILES_H);
    switch (level) {
    case 1:
        PAL_setPalette(PAL_BACKGROUND, level1_transition.palette->data, DMA);
        VDP_drawImageEx(WINDOW, &level1_transition, TILE_ATTR_FULL(PAL_BACKGROUND, 1, 0, 0, ind), 0, 0, TRUE, DMA);
        num_tiles = level1_transition.tileset->numTile;
        break;
    case 2:
        VDP_drawImageEx(WINDOW, &level2_transition, TILE_ATTR_FULL(PAL_BACKGROUND, 1, 0, 0, ind), 0, 0, TRUE, DMA);
        num_tiles = level2_transition.tileset->numTile;
        break;
    case 3:
        VDP_drawImageEx(WINDOW, &level3_transition, TILE_ATTR_FULL(PAL_BACKGROUND, 1, 0, 0, ind), 0, 0, TRUE, DMA);
        num_tiles = level3_transition.tileset->numTile;
        break;
    case 4:
        VDP_drawImageEx(WINDOW, &level4_transition, TILE_ATTR_FULL(PAL_BACKGROUND, 1, 0, 0, ind), 0, 0, TRUE, DMA);
        num_tiles = level4_transition.tileset->numTile;
        break;
    case 5:
        VDP_drawImageEx(WINDOW, &level5_transition, TILE_ATTR_FULL(PAL_BACKGROUND, 1, 0, 0, ind), 0, 0, TRUE, DMA);
        num_tiles = level5_transition.tileset->numTile;
        break;
    default:
        VDP_drawText("UNKNOWN LEVEL", SCREEN_METATILES_H/2, SCREEN_METATILES_W/2 + 1);
        break;
    }
    return num_tiles;
}

void SCREEN_menu(){
    // update the screen
    #ifdef DEBUG
    kprintf("MENU SCREEN\n");
    #endif
    VDP_clearPlane(WINDOW, TRUE);
    VDP_setWindowVPos(FALSE, SCREEN_TILES_H);
    PAL_setPalette(PAL_BACKGROUND, menu_screen.palette->data, DMA);
    VDP_drawImageEx(WINDOW, &menu_screen, TILE_ATTR_FULL(PAL_BACKGROUND, 1, 0, 0, TILE_USER_INDEX), 0, 0, TRUE, DMA);
    
    VDP_setTextPalette(PAL_BACKGROUND);
	VDP_setBackgroundColor(PAL_BACKGROUND*16+12);
	// PAL_setColor(PAL_BACKGROUND*16+15, RGB24_TO_VDPCOLOR(0x00EE00));	// text fg, pal index 15
	// PAL_setColor(PAL_BACKGROUND*16+6,  RGB24_TO_VDPCOLOR(0x494949));	// text bg, index of the whiter color in palette
    VDP_setTextPlane(WINDOW);
    VDP_drawText("> Start Game", SCREEN_METATILES_W/2 + 3, SCREEN_METATILES_H/2 + 8);

    VDP_drawText("Desenvolvimento: Augusto", 2, SCREEN_METATILES_H * 2 - 7);
    VDP_drawText("                 M. Silvano", 2, SCREEN_METATILES_H * 2 - 6);
    VDP_drawText("Design:          Augusto", 2, SCREEN_METATILES_H * 2 - 5);
    VDP_drawText("Arte:            Augusto", 2, SCREEN_METATILES_H * 2 - 4);
    VDP_drawText("Som:             A. Preradovic", 2, SCREEN_METATILES_H * 2 - 3);
    VDP_drawText("                 Z. Rubenstein", 2, SCREEN_METATILES_H * 2 - 2);
    while(true) {
        update_input();
        // wait for the player to press the action button
        if(key_pressed(JOY_1, BUTTON_A) || key_pressed(JOY_1, BUTTON_START)) {
            VDP_clearTextArea(0, 0, SCREEN_TILES_W, SCREEN_TILES_H);
            break;
        }
        SPR_update();
        SYS_doVBlankProcess();
    }
    SPR_reset();
    VDP_clearPlane(BG_BACKGROUND, TRUE);
    game_state = GAME_INIT;
}