#include <genesis.h>
#include <maths.h>

#include "player.h"
#include "level.h"
#include "utils.h"
#include "hud.h"

#define DEBUG

#define SHOOT_DELAY 10
#define MAX_SHOTS 10
#define SHOOT_SPEED 8

GameObject player;
u8 shoot_timer = 0;
u8 shoot_count = 0;
GameObject shots[MAX_SHOTS];

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
	shoot_timer++;
	if(shoot_timer == MAX_U8 - 1){
		shoot_timer = SHOOT_DELAY + 1;
	}
	// input
	PLAYER_get_input_dir8();
	
	// movement
	player.next_x = player.x + player.speed_x;
	player.next_y = player.y + player.speed_y;
	
	LEVEL_move_and_slide(&player);
	
	// player movement
	player.x = player.next_x;
	player.y = player.next_y;
	
	GAMEOBJECT_update_boundbox(player.x, player.y, &player);
	if (LEVEL_tileXY(player.box.left + player.w/2, player.box.top + player.h/2) == IDX_ITEM) {
		HUD_gem_collected(1);
		LEVEL_remove_tile(player.box.left + player.w/2, player.box.top + player.h/2, IDX_ITEM_DONE);
	}

	// GAMEOBJECT_wrap_screen(&player);
	GAMEOBJECT_clamp_screen(&player);
	
	// SHOOT_collision(&player, shots, MAX_SHOTS);
	#ifdef DEBUG
	for(int i = 0; i < MAX_SHOTS; i++) {
		if (shots[i].sprite != NULL) {
			kprintf("Shot %d: x=%d, y=%d, speed_x=%d, speed_y=%d\n", i, fix16ToInt(shots[i].x),
				fix16ToInt(shots[i].y), fix16ToInt(shots[i].speed_x), fix16ToInt(shots[i].speed_y));
		}
	}
	#endif
	// update bullets
	for(int i = 0; i < MAX_SHOTS; i++) {
		if (shots[i].sprite != NULL) {
			shots[i].y = shots[i].y + shots[i].speed_y;
			// check if bullet is inside screen
			if(shots[i].x > 0 && shots[i].x < FIX16(SCREEN_W) ||
			   shots[i].y > 0 && shots[i].y < FIX16(SCREEN_H)) {
				kprintf("true\n");
				SPR_setPosition(shots[i].sprite, fix16ToInt(shots[i].x), fix16ToInt(shots[i].y));
			} else {
				kprintf("false\n");
				// SPR_releaseSprite(shots[i].sprite);
				// shots[i].sprite = NULL;
				// shoot_count--;
			}
		}
	}
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

	if(key_pressed(JOY_1, BUTTON_A)) {
		//shoot logic here

		// validate minimun delay between shots
		if(shoot_timer > SHOOT_DELAY) {
			PLAYER_shoot();
			shoot_timer = 0;
		}
	}
}

void PLAYER_shoot() {
	// validate shots array size
	if (shoot_count < MAX_SHOTS) {
		// find an empty shot slot
		for (u8 i = 0; i < MAX_SHOTS; i++) {
			if (shots[i].sprite == NULL) {
				shots[i].sprite = SPR_addSprite(&spr_bullet, fix16ToInt(player.x + player.w/2 - 4), fix16ToInt(player.y + player.h/2 - 4), TILE_ATTR(PAL_PLAYER, 0, FALSE, FALSE));
				shots[i].x = player.x + player.w/2 - 4;
				shots[i].y = player.y + player.h/2 - 4;
				shots[i].speed_x = 0;
				shots[i].speed_y = -FIX16(SHOOT_SPEED); // horizontal shot
				shots[i].anim = 0; // default animation
				shoot_count++;
				break;
			}
		}
	}
}

void SHOOT_collision(GameObject* player, GameObject* shots, u8 max_shots) {
	// Check for collisions between player shots and enemies
}