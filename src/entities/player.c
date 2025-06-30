#include <genesis.h>
#include <maths.h>

#include "engine/globals.h"
#include "engine/utils.h"
#include "engine/level.h"
#include "engine/hud.h"
#include "player.h"

// #define DEBUG

#define SHOOT_DELAY 10
#define MAX_SHOTS 15
#define SHOOT_SPEED 8
#define IMMUNITY_TIME 15

GameObject player;
u8 shoot_timer = 0;
u8 shoot_count = 0;
GameObject shots[MAX_SHOTS];
// handle player immunity
bool immunity = false;
u8 immunity_time = 0;

////////////////////////////////////////////////////////////////////////////
// INIT

u16 PLAYER_init(u16 ind) {
	ind += GAMEOBJECT_init(&player, &spr_ship, SCREEN_W/2-12, SCREEN_H/2-12, 0, 0, PAL_PLAYER, ind);
	for(u8 i = 0; i < MAX_SHOTS; ++i){
		#ifndef DEBUG
		ind += GAMEOBJECT_init(&shots[i], &spr_bullet, 0, 0, -8, -8, PAL_PLAYER, ind);
		#else
		shots[i].sprite = SPR_addSprite(&spr_bullet, 0, 0, TILE_ATTR_FULL(PAL_PLAYER, FALSE, FALSE, 0, ind));
		#endif
		ind += shots[i].sprite->definition->maxNumTile;
		shots[i].sprite->data = 0;
	}
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
	
	// movement preview
	player.next_x = player.x + player.speed_x;
	player.next_y = player.y + player.speed_y;
	
	// check collision with level tiles
	PLAYER_collision();
	PLAYER_immunity_update();

	// player movement
	player.x = player.next_x;
	player.y = player.next_y;

	// GAMEOBJECT_wrap_screen(&player);
	GAMEOBJECT_clamp_screen(&player);

	// update shots
	SHOTS_update();

	// update VDP/SGDK
	SPR_setPosition(player.sprite, F16_toInt(player.x), F16_toInt(player.y));
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

	if(key_pressed(JOY_1, BUTTON_A)) {
		// validate minimun delay between shots
		if(shoot_timer > SHOOT_DELAY) {
			PLAYER_shoot();
			shoot_timer = 0;
		}
	}

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

void PLAYER_collision() {
	LEVEL_collision(&player);
	// check collision with level tiles
	#ifdef DEBUG
	if(LEVEL_collision_result()){
		kprintf("player x=%d y=%d w=%d h=%d", fix16ToInt(player.x), fix16ToInt(player.y), player.w, player.h);
		kprintf("player_hitbox top=%d bottom=%d left=%d right=%d", player.box.top, player.box.bottom, player.box.left, player.box.right);
		DEBUG_collision_map(player.box.left + player.w/2, player.box.top - player.h/2);
	}
	#endif
	if (LEVEL_collision_result() & COLLISION_TOP) {
		#ifdef DEBUG
		kprintf("top col rmv mt_x=%d mt_y=%d", (player.box.left + player.w/2)/16, (player.box.top - 8)/16);
		#endif
		LEVEL_remove_tile(player.box.left + player.w/2 - 4, player.box.top - 8, 0);
		LEVEL_remove_tile(player.box.left + player.w/2 + 4, player.box.top - 8, 0);
		// add player damage
		PLAYER_damage(1);
	} else	if (LEVEL_collision_result() & COLLISION_BOTTOM) {
		#ifdef DEBUG
		kprintf("bottom col rmv mt_x=%d mt_y=%d", (player.box.left + player.w/2)/16, (player.box.bottom + 8)/16);
		#endif
		LEVEL_remove_tile(player.box.left + player.w/2 - 4, player.box.bottom + 8, 0);
		LEVEL_remove_tile(player.box.left + player.w/2 + 4, player.box.bottom + 8, 0);
		// add player damage
		PLAYER_damage(1);
	} else	if (LEVEL_collision_result() & COLLISION_LEFT) {
		#ifdef DEBUG
		kprintf("left col rmv mt_x=%d mt_y=%d", (player.box.left - 8)/16, (player.box.top + player.h/2)/16);
		#endif
		LEVEL_remove_tile(player.box.left - 8, player.box.top + player.h/2 -8, 0);
		LEVEL_remove_tile(player.box.right + 8, player.box.top + player.h/2, 0);
		LEVEL_remove_tile(player.box.left - 8, player.box.top + player.h/2 +8, 0);
		// add player damage
		PLAYER_damage(1);
	} else	if (LEVEL_collision_result() & COLLISION_RIGHT) {
		#ifdef DEBUG
		kprintf("right col rmv mt_x=%d mt_y=%d", (player.box.right + 8)/16, (player.box.top + player.h/2)/16);
		#endif
		LEVEL_remove_tile(player.box.right + 8, player.box.top + player.h/2 -8, 0);
		LEVEL_remove_tile(player.box.right + 8, player.box.top + player.h/2, 0);
		LEVEL_remove_tile(player.box.right + 8, player.box.top + player.h/2 +8, 0);
		// add player damage
		PLAYER_damage(1);
	}
}

void PLAYER_damage(u8 damage) {
	#ifdef DEBUG
	kprintf("health=%d damage=%d immunity=%d", player.health, damage, immunity);
	#endif
	if(immunity) {
		// player is immune, do not apply damage
		return;
	}

	player.health -= damage;
	HUD_update_health(player.health);
	if (player.health <= 0) {
		player.health = 0;
		game_state = GAME_OVER;
	}
	immunity = true;
	immunity_time = IMMUNITY_TIME;
}

void PLAYER_immunity_update() {
	if(immunity) {
		if(immunity_time == 0) {
			immunity = false;
		}
		--immunity_time;
	}
}

void PLAYER_shoot() {
	// validate shots array size
	if (shoot_count < MAX_SHOTS) {
		// find an empty shot slot
		for (u8 i = 0; i < MAX_SHOTS; i++) {
			// if the shot is not active (data == 0), set it up
			if (shots[i].sprite->data == 0) {
				shots[i].sprite->data = 1;
				shots[i].x = player.x + FIX16(player.w/2 - 4);
				shots[i].y = player.y + FIX16(player.h/2 - 4);
				shots[i].speed_x = 0;
				shots[i].speed_y = -FIX16(SHOOT_SPEED); // horizontal shot
				shots[i].anim = 0; // default animation
				SPR_setVisibility(shots[i].sprite, VISIBLE);
				shoot_count++;
				break;
			}
		}
	}
}

void SHOTS_update() {
	for(int i = 0; i < MAX_SHOTS; i++) {
		// if the shot is active (data == 1), update its position
		if(shots[i].sprite->data == 1){
			shots[i].x += shots[i].speed_x;
			shots[i].y += shots[i].speed_y;

			SPR_setPosition(shots[i].sprite, F16_toInt(shots[i].x), F16_toInt(shots[i].y));
			// check if bullet is outside the screen
			if(!(shots[i].y > 0 && shots[i].y < FIX16(SCREEN_H))){
				SPR_setVisibility(shots[i].sprite, HIDDEN);
				shots[i].sprite->data = 0;
				shoot_count--;
			} else {
				// check collision
				SHOOT_collision(&shots[i]);
				// shots[i].x = shots[i].next_x;
				// shots[i].y = shots[i].next_y;
			}
		}
	}
}

// check collision for a single shot
void SHOOT_collision(GameObject* shot) {
	// check collision with level tiles
	LEVEL_collision(shot);
	if(LEVEL_collision_result()){
		// remove the shot
		SPR_setVisibility(shot->sprite, HIDDEN);
		shot->sprite->data = 0;
		shoot_count--;
		#ifdef DEBUG
		kprintf("remove x=%d y=%d mt_x=%d mt_y=%d", shot->box.left, shot->box.top, shot->box.left/16, shot->box.top/16);
		// char row[SCREEN_METATILES_W + 1] = "";
		// for(int i = 0; i < SCREEN_METATILES_W; i++) {
		// 	row[i] = '0' + LEVEL_tileXY(i, shot->box.top - 16 + screen_y);
		// }
		// row[SCREEN_METATILES_W] = '\0';
		// kprintf("y=%d row 1: %s", (shot->box.top - 16)/16, row);
		// for(int i = 0; i < SCREEN_METATILES_W; i++) {
		// 	row[i] = '0' + LEVEL_tileXY(i*16, shot->box.top + screen_y);
		// }
		// row[SCREEN_METATILES_W] = '\0';
		// kprintf("y=%d row 2: %s", (shot->box.top)/16, row);
		// for(int i = 0; i < SCREEN_METATILES_W; i++) {
		// 	row[i] = '0' + LEVEL_tileXY(i, shot->box.top + 16 + screen_y);
		// }
		// row[SCREEN_METATILES_W] = '\0';
		// kprintf("y=%d row 3: %s", (shot->box.top + 16)/16, row);
		#endif
		// // SEARCH tiles to left and right of the shot
		s16 tile_x = shot->box.left + shot->w/2;
		s16 tile_y = shot->box.top + shot->h/2;
		if(!LEVEL_wallXY(tile_x, tile_y + screen_y)){
			if(LEVEL_wallXY(shot->box.left + shot->w/2 - 4, shot->box.top + screen_y)) tile_x -= 8;
			if(LEVEL_wallXY(shot->box.left + shot->w/2 + 4, shot->box.top + screen_y)) tile_x += 8;
			#ifdef DEBUG
			kprintf("central tile not found removing top side tile_x=%d tile_y=%d", tile_x, tile_y);
			#endif
		}
		LEVEL_remove_tile(tile_x, tile_y, 0);
		// increase score
		HUD_score(ASTEROID);
	};
	// check collision with enemies
}

void DEBUG_collision_map(s16 x, s16 y) {
	// print the collision map for debugging
	// row 1
	char row[SCREEN_METATILES_W + 1] = "";
	for(int i = 0; i < SCREEN_METATILES_W; i++) {
		row[i] = '0' + LEVEL_tileXY(i*16, y - 16 + screen_y);
	}
	row[SCREEN_METATILES_W] = '\0';
	kprintf("y=%d row: %s", (y - 16)/16, row);
	// row 2
	for(int i = 0; i < SCREEN_METATILES_W; i++) {
		row[i] = '0' + LEVEL_tileXY(i*16, y + screen_y);
	}
	row[SCREEN_METATILES_W] = '\0';
	kprintf("y=%d row: %s", y/16, row);
	// row 3
	for(int i = 0; i < SCREEN_METATILES_W; i++) {
		row[i] = '0' + LEVEL_tileXY(i*16, y +16 + screen_y);
	}
	row[SCREEN_METATILES_W] = '\0';
	kprintf("y=%d row: %s", (y + 16)/16, row);
}