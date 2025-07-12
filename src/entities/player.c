#include <genesis.h>
#include <maths.h>

#include "engine/globals.h"
#include "engine/utils.h"
#include "engine/level.h"
#include "engine/hud.h"
#include "player.h"

// #define DEBUG

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
	
	// initialize player
	ind += GAMEOBJECT_init(&player, &spr_ship, SCREEN_W/2-12, SCREEN_H/2-12, -8, -8, PAL_PLAYER, ind);

	// initialize shots
	shoot_count = 0;
	for(u8 i = 0; i < MAX_SHOTS; ++i){
		ind += GAMEOBJECT_init(&shots[i], &spr_bullet, 0, 0, -8, -8, PAL_ENEMY, ind);
		shots[i].active = FALSE;
	}
	return ind;
}

////////////////////////////////////////////////////////////////////////////
// UPDATE

void PLAYER_update() {
	shoot_timer += 1 + (1 * player.up_freq);
	if(shoot_timer > MAX_U8 - SHOOT_DELAY){
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
	// SPR_setPosition(player.sprite, F16_toInt(player.x), F16_toInt(player.y));
	GAMEOBJECT_set_hwsprite_position(&player);
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

	if(key_down(JOY_1, BUTTON_A)) {
		// validate minimun delay between shots
		if(shoot_timer > SHOOT_DELAY) {
			PLAYER_shoot();
			shoot_timer = 0;
		}
	}

	if(key_pressed(JOY_1, BUTTON_B)) {
		// level cheat
		game_state = GAME_NEXT_LEVEL;
		return;
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
		// XGM_startPlayPCM(SFX_EXPLOSION, 1,SOUND_PCM_CH3);
		XGM2_playPCM(sfx_explosion, sizeof(sfx_explosion), SOUND_PCM_CH_AUTO);
		game_state = GAME_OVER;
	}
	immunity = true;
	immunity_time = IMMUNITY_TIME + (IMMUNITY_TIME * player.up_immunity);
}

void PLAYER_immunity_update() {
	if(immunity) {
		if(immunity_time % 5 == 0){
			// blink player sprite
			SPR_setVisibility(player.sprite, !SPR_getVisibility(player.sprite));
		}
		if(immunity_time == 0) {
			immunity = false;
		}
		--immunity_time;
	} else if(SPR_getVisibility(player.sprite) == HIDDEN) {
		SPR_setVisibility(player.sprite, VISIBLE);
	}
}

void PLAYER_shoot() {
	// validate shots array size
	if (shoot_count < MAX_SHOTS) {
		// find an empty shot slot
		for (u8 i = 0; i < MAX_SHOTS; i++) {
			// if the shot is not active (data == 0), set it up
			if (!shots[i].active) {
				shots[i].active = TRUE;
				shots[i].x = player.x + FIX16(player.w/2);
				shots[i].y = player.y + FIX16(player.h/2 - player.h_offset);
				shots[i].speed_x = 0;
				shots[i].speed_y = -FIX16(SHOOT_SPEED); // horizontal shot
				shots[i].anim = 0; // default animation
				SPR_setVisibility(shots[i].sprite, VISIBLE);
				// XGM_startPlayPCM(SFX_LASER,1,SOUND_PCM_CH4);
				XGM2_playPCM(sfx_shoot, sizeof(sfx_shoot), SOUND_PCM_CH_AUTO);
				shoot_count++;
				break;
			}
		}
	}
}

void SHOTS_update() {
	for(int i = 0; i < MAX_SHOTS; i++) {
		// if the shot is active (data == 1), update its position
		if(shots[i].active){
			shots[i].x += shots[i].speed_x;
			shots[i].y += shots[i].speed_y;

			GAMEOBJECT_set_hwsprite_position(&shots[i]);
			// SPR_setPosition(shots[i].sprite, F16_toInt(shots[i].x), F16_toInt(shots[i].y));
			// check if bullet is outside the screen
			if(!(shots[i].y > 0 && shots[i].y < FIX16(SCREEN_H))){
				SPR_setVisibility(shots[i].sprite, HIDDEN);
				shots[i].active = FALSE;
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
		shot->active = FALSE;
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

void PLAYER_power_up(GameObject* obj){
	switch (obj->type){
	case PUP_SHOOT:
		if(player.up_shoot < 3){
			player.up_shoot++;
		}
		obj->active = FALSE;
		SPR_setVisibility(obj->sprite, HIDDEN);
		break;
	case PUP_FIRESPD:
		// increase player fire rate
		if(player.up_freq < 1){
			player.up_freq++;
		}
		obj->active = FALSE;
		SPR_setVisibility(obj->sprite, HIDDEN);
		break;
	case PUP_IMN:
		if(player.up_immunity < 1){
			player.up_immunity++;
		}
		obj->active = FALSE;
		SPR_setVisibility(obj->sprite, HIDDEN);
		break;
	default:
		break;
	}
	HUD_update_power_up(&player);
}