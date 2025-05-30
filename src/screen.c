#include <genesis.h>
#include <maths.h>

#include "globals.h"
#include "utils.h"

// #define DEBUG

void SCREEN_over_update(){
    u8 death_time = 0;
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
        if(death_time < 120){
            death_time++;
        } else {
            if(key_pressed(JOY_1, BUTTON_A) || key_pressed(JOY_1, BUTTON_START)) {
                VDP_clearTextArea(0, 0, SCREEN_TILES_W, SCREEN_TILES_H);
                break;
            }
        }
		SPR_update();
		SYS_doVBlankProcess();
    }
    
}