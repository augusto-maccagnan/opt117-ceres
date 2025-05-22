#include <genesis.h>
#include <maths.h>

#include "player.h"
#include "level.h"
#include "utils.h"
#include "hud.h"

GameObject player;

////////////////////////////////////////////////////////////////////////////
// INIT

u16 PLAYER_init(u16 ind) {
	ind += GAMEOBJECT_init(&player, &spr_ship, SCREEN_W/2-12, SCREEN_H/2-12, PAL_PLAYER, ind);
	player.health = PLAYER_MAX_HEALTH;
	return ind;
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

void PLAYER_update() {
	// input
	PLAYER_get_input_dir8();
	
	// movement
	player.next_x = player.x + player.speed_x;
	player.next_y = player.y + player.speed_y;
	
	LEVEL_move_and_slide(&player);
	
	player.x = player.next_x;
	player.y = player.next_y;
	
	// if (LEVEL_collision_result()) {
	// 	PAL_setColor(3, RGB24_TO_VDPCOLOR(0xFF0000));
	// } else {
	// 	PAL_setColor(3, RGB24_TO_VDPCOLOR(0x00FF00));
	// }
	
	GAMEOBJECT_update_boundbox(player.x, player.y, &player);
	if (LEVEL_tileXY(player.box.left + player.w/2, player.box.top + player.h/2) == IDX_ITEM) {
		HUD_gem_collected(1);
		LEVEL_remove_tile(player.box.left + player.w/2, player.box.top + player.h/2, IDX_ITEM_DONE);
	}

	// GAMEOBJECT_wrap_screen(&player);
	GAMEOBJECT_clamp_screen(&player);
	
	// update VDP/SGDK
	SPR_setPosition(player.sprite, fix16ToInt(player.x), fix16ToInt(player.y));
	SPR_setAnim(player.sprite, player.anim);
}

/**
 * Get player input and set ship speed with:
 * - Constant speed
 * - Always moving (doesn't stop when dpad released)
 * - Four directions, mutually exclusive
 */
void PLAYER_get_input_dir4() {
	if (key_down(JOY_1, BUTTON_RIGHT)) {
		player.speed_x = PLAYER_SPEED;
		player.speed_y = 0;
		player.anim = 0;
	}
	else 
	if (key_down(JOY_1, BUTTON_LEFT)) {
		player.speed_x = -PLAYER_SPEED;
		player.speed_y = 0;
		player.anim = 4;
	}
	else
	if (key_down(JOY_1, BUTTON_UP)) {
		player.speed_x = 0;
		player.speed_y = -PLAYER_SPEED;
		player.anim = 2;
	}
	else
	if (key_down(JOY_1, BUTTON_DOWN)) {
		player.speed_x = 0;
		player.speed_y = PLAYER_SPEED;
		player.anim = 6;
	} 
}

/**
 * Get player input and set ship speed with:
 * - Constant speed
 * - Stop when dpad released
 * - Eight directions + fix for diagonals
 */
void PLAYER_get_input_dir8() {
	player.speed_x = 0;
	player.speed_y = 0;

	/* ANIM DIRECTIONS
		      2
		   3     1
		4           0
		   5     7
		      6
	*/

	if (key_down(JOY_1, BUTTON_RIGHT)) {
		player.speed_x = PLAYER_SPEED;
		player.anim = 0;

		if (key_down(JOY_1, BUTTON_UP)) {
			player.speed_x =  PLAYER_SPEED45;
			player.speed_y = -PLAYER_SPEED45;
			player.anim = 1;
			return;
		} 
		else
		if (key_down(JOY_1, BUTTON_DOWN)) {
			player.speed_x =  PLAYER_SPEED45;
			player.speed_y =  PLAYER_SPEED45;
			player.anim = 7;
			return;
		}
	} 
	else 
	if (key_down(JOY_1, BUTTON_LEFT)) {
		player.speed_x = -PLAYER_SPEED;
		player.anim = 4;

		if (key_down(JOY_1, BUTTON_UP)) {
			player.speed_x = -PLAYER_SPEED45;
			player.speed_y = -PLAYER_SPEED45;
			player.anim = 3;
			return;
		}
		else
		if (key_down(JOY_1, BUTTON_DOWN)) {
			player.speed_x = -PLAYER_SPEED45;
			player.speed_y =  PLAYER_SPEED45;
			player.anim = 5;
			return;
		}
	}

	if (key_down(JOY_1, BUTTON_UP)) {
		player.speed_y = -PLAYER_SPEED;
		player.anim = 2;
	}
	else
	if (key_down(JOY_1, BUTTON_DOWN)) {
		player.speed_y = PLAYER_SPEED;
		player.anim = 6;
	} 
}
