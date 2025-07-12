/**
 * Para compilar & rodar:
 * ---------------------
 *   CTRL + SHIFT + B   (gera out/rom.bin )			<< compilar
 *   [F1], Run Task, Run Gens						<< executar
 *
 * Para compilar & rodar (com extensão Genesis Code):
 * -------------------------------------------------
 *  OBS: instalar extensao Genesis Code e configurar "Gens Path"
 *
 *  Executar: $ cmd									<< pelo terminal do VSCode (extensão não funciona em PowerShell)
 *  F1 -> Genesis Code Compile Project				<< compilar
 *  F1 -> Genesis Code Compiler & Run Project		<< compilar & executar
 * 
 * LINUX
 * ==================================================================
 * Considerando que o caminho seja ~/sgdk200, para fazer build:
 * 
 * $ make GDK=~/sgdk211 -f ~/sgdk211/makefile_wine.gen
 * 
 * TILED -> customizar grade em preferências
 * GENS -> habilitar recursos avançados de debug
 *
 * PROBLEMAS
 * ------------------------------
 * > Mudar imagem de tileset fez com que o Rescomp nao encotrasse mais os tiles (problema no TMX/TSX).
 *   SOLUCAO: criar novo arquivo tsx com a mesma PNG dos tiles e usar a opção "replace tilset" do Tiled.
 * > Mensagens com KLog não aparecem no emulador
 *   SOLUCAO: Option -> Debug -> Active Development Features [x]
 * 
 * IDEIAS
 * ------------------------------
 * Bola que quebra blocos. Pode quebrar na primeia. 
 * Bola que passada através dos blocos.
 * As bolas passam pelas paredes de choque.
 * 
 * TODO
 * ------------------------------ 
 * [  ] BUG - Colisão do tiro falha as vezes
 * [  ] BUG - Arrumar colisão quando player está movendo lateralmente (nave em um y superior ao tile de colisão)
 * [  ] Áudio - PowerUp
 * [  ] Chance de inimigos droparem vida (restaurar vida do player)
 * [  ] Chance de inimigos droparem power-ups
 * [  ] Power-ups
 * [  ] Áudio - Background
 * [  ] Áudio - Tiro
 * [  ] Áudio - Colisão Nave
 * [  ] Tela de Level Clear
 * [  ] Transição de levels
 * [WIP] Inteligencia dos Inimigos
 * [WIP] Implementação do RKT
 * [WIP] Implementação lógica de tiro do UFO
 * [OK] Corrigir sprite da nave
 * [OK] Inimigo do tipo UFO
 * [OK] Spawn de inimigos
 * [OK] Contabilizar dano no player -> Problema com a remoção de tiles do mapa
 * [OK] Tempo de imunidade do player
 * [OK] Tela de Game Over
 * [OK] Colisão com asteroides
 * [OK] Contabilizar bloco destruido
 * [OK] Debug da colisão do level seguir o scroll
 * [OK] Colisão do tiro com blocos
 * [OK] Tiro da nave
 * [OK] Fazer verificação da colisão com o scroll do tilemap do LEVEL
 * [OK] Gerar mapa de colisão do level inteiro
 */
#include <genesis.h>
#include <sprite_eng.h>

#include "engine/globals.h"
#include "resources.h"

#include "engine/utils.h"
#include "engine/gameobject.h"
#include "engine/background.h"
#include "engine/screen.h"
#include "engine/level.h"
#include "engine/hud.h"
#include "engine/objects_pool.h"
#include "engine/mapobjects.h"

#include "entities/player.h"
#include "entities/enemy.h"
#include "entities/shoot.h"

// #define DEBUG

// index for tiles in VRAM (first tile reserved for SGDK)
u16 ind = TILE_USER_INDEX;

// transition timer
u8 transition_timer;

enum NUMBER_OF_ROOMS level_rooms;
enum GAME_STATE game_state;

////////////////////////////////////////////////////////////////////////////
// GAME INIT

// enemies pool
#define MAX_ENEMIES 50
GameObject enemy_array[MAX_ENEMIES];
ObjectsPool enemy_pool;

// power-ups pool
#define MAX_POWER_UPS 10
GameObject p_up_array[MAX_POWER_UPS];
ObjectsPool p_up_pool;

void init_enemies() {
	OBJPOOL_init(&enemy_pool, enemy_array, LEN(enemy_array));
	OBJPOOL_init(&p_up_pool, p_up_array, LEN(p_up_array));
	curr_mapobj = 0;

	// load enemy tiles
	ind += ENEMY_load_tiles(ind);
}

// power-ups init
void init_power_ups(GameObject* enemy){
	GameObject* power_up = OBJPOOL_get_available(&p_up_pool);
	if (!power_up) return;
	ENEMY_on_hit(power_up, enemy);
}

// shoots pool
#define MAX_SHOOTS 20
GameObject shoot_array[MAX_SHOOTS];
ObjectsPool shoot_pool;

void init_shoots() {
	OBJPOOL_init(&shoot_pool, shoot_array, LEN(shoot_array));

	ind += SHOOT_load_tiles(ind);
}

void shoot(GameObject* obj, u8 type) {
	GameObject* shoot = OBJPOOL_get_available(&shoot_pool);
    if (!shoot) return;
	SHOOT_init(shoot, F16_toInt(obj->x) + obj->w/3, F16_toInt(obj->y) + obj->h/2 + obj->h_offset, type);
	shoot->damage = obj->damage;
}

void spawn_enemies() {
	MapObject* mapobj;
	// looks if there are enemies in spawn range
	switch (current_level)
	{
	case 1:
		break;
	case 2:
		mapobj = MAPOBJ_lookup_enemies(level2_enemies, LEN(level2_enemies));
		while(mapobj){
			// looks for an available space in the enemy pool
			GameObject* enemy = OBJPOOL_get_available(&enemy_pool);
			if (!enemy) return;
			// Enemy factory function: It gets the needed data from MapObject
			// ENEMY_init(enemy, mapobj, enemy_tiles_ind);
			ENEMY_init(enemy, mapobj, ind);
			mapobj = MAPOBJ_lookup_enemies(level2_enemies, LEN(level2_enemies));
		}
		break;
	case 3:
		mapobj = MAPOBJ_lookup_enemies(level3_enemies, LEN(level3_enemies));
		while(mapobj){
			// looks for an available space in the enemy pool
			GameObject* enemy = OBJPOOL_get_available(&enemy_pool);
			if (!enemy) return;
			// Enemy factory function: It gets the needed data from MapObject
			// ENEMY_init(enemy, mapobj, enemy_tiles_ind);
			ENEMY_init(enemy, mapobj, ind);
			mapobj = MAPOBJ_lookup_enemies(level3_enemies, LEN(level3_enemies));
		}
		break;
	case 4:
		mapobj = MAPOBJ_lookup_enemies(level4_enemies, LEN(level4_enemies));
		while(mapobj){
			// looks for an available space in the enemy pool
			GameObject* enemy = OBJPOOL_get_available(&enemy_pool);
			if (!enemy) return;
			// Enemy factory function: It gets the needed data from MapObject
			// ENEMY_init(enemy, mapobj, enemy_tiles_ind);
			ENEMY_init(enemy, mapobj, ind);
			mapobj = MAPOBJ_lookup_enemies(level4_enemies, LEN(level4_enemies));
		}
		break;
	case 5:
		mapobj = MAPOBJ_lookup_enemies(level5_enemies, LEN(level5_enemies));
		while(mapobj){
			// looks for an available space in the enemy pool
			GameObject* enemy = OBJPOOL_get_available(&enemy_pool);
			if (!enemy) return;
			// Enemy factory function: It gets the needed data from MapObject
			// ENEMY_init(enemy, mapobj, enemy_tiles_ind);
			ENEMY_init(enemy, mapobj, ind);
			mapobj = MAPOBJ_lookup_enemies(level5_enemies, LEN(level5_enemies));
		}
		break;
	default:
		break;
	}
}


void init_state() {
	// reset index for tiles in VRAM
	ind = TILE_USER_INDEX; 

	// reset score
	player_score = 0;

	// reset game state
	game_state = GAME_INIT;

	// reset level rooms
	level_rooms = LEVEL1;

	// set current level
	current_level = 1;

	player.health = PLAYER_MAX_HEALTH;

	player.up_freq = 0;
	player.up_immunity = 0;
	player.up_shoot = 0;
}

void game_next_level() {
	transition_timer = 0;
	// reset the sprites
	SPR_reset();
	OBJPOOL_clear(&enemy_pool);
	ind = TILE_USER_INDEX; // reset index for tiles in VRAM

	// increment current level
	current_level++;
	// show transition screen
	if (current_level > 5) {
		current_level = 0;
		game_state = GAME_CLEAR;
		return;
	}
	#ifdef DEBUG
	kprintf("Next level: %d", current_level);
	#endif
	//
	switch (current_level)
	{
	case 1:
		level_rooms = LEVEL1;
		break;
	case 2:
		level_rooms = LEVEL2;
		break;
	case 3:
		level_rooms = LEVEL3;
		break;
	case 4:
		level_rooms = LEVEL4;
		break;
	case 5:
		level_rooms = LEVEL5;
		break;
	default:
		break;
	}
	ind += SCREEN_level_transition(current_level, ind);
	ind += BACKGROUND_init(ind);
	ind += LEVEL_init(ind, level_rooms);
	// holds transition screen for 3 seconds
	while(transition_timer < (60*1)){
		transition_timer++;
		SPR_update();
        SYS_doVBlankProcess();
	}
	ind += HUD_init(ind);
	PLAYER_init(ind);
	init_shoots();
	init_enemies();

	HUD_update_health(player.health);
	HUD_update_power_up(&player);

	game_state = GAME_PLAYING;
}

void game_init() {
	init_state();

	VDP_setScreenWidth320();
	VDP_setPlaneSize(64, 64, TRUE);
	SPR_init();

	SYS_showFrameLoad(true);
	
	// show menu screen
	SCREEN_menu();

	current_level = 0;

	game_next_level();
}

////////////////////////////////////////////////////////////////////////////
// GAME LOGIC

inline void update_enemies() {
	GameObject* obj = OBJPOOL_loop_init(&enemy_pool);
	while (obj) {
		obj->update(obj);
		if(in_shoot){
			shoot(obj, ENEMY_SHOOT);
		}
		GameObject* obj_to_release = NULL;

		if (GAMEOBJECT_check_collision(&player, obj)) {
			PLAYER_damage(obj->damage);
			obj_to_release = obj;
		}

		for(int i = 0; i < shoot_count; i++) {
			if(shots[i].active) {
				if(GAMEOBJECT_check_collision(obj, &shots[i])) {
					// if enemy is hit by player shot, release it
					obj_to_release = obj;
					// remove the shot
					SPR_setVisibility(shots[i].sprite, HIDDEN);
					shots[i].active = FALSE; // remove shot
					shoot_count--;
					// increase score
					switch (obj->type)
					{
					case ENEMY_UFO:
						HUD_score(UFO);
						break;
					case ENEMY_RKT:
						HUD_score(RKT);
						break;
					case ENEMY_KAZ:
						HUD_score(KAZ);
						break;
					default:
						break;
					}
					XGM2_playPCM(sfx_explosion, sizeof(sfx_explosion), SOUND_PCM_CH_AUTO);
					init_power_ups(obj);
				}
			}
		}

		// enemy inactivation, by going out of screen
		if(!obj->active){
			obj_to_release = obj;
		}

		obj = OBJPOOL_loop_next(&enemy_pool);
		if (obj_to_release) {
			OBJPOOL_release(&enemy_pool, obj_to_release);
		}
	}
}

inline void update_shots() {
	GameObject* obj = OBJPOOL_loop_init(&shoot_pool);
	while (obj) {
		obj->update(obj);

		GameObject* obj_to_release = NULL;

		if (GAMEOBJECT_check_collision(&player, obj)) {
			PLAYER_damage(obj->damage);
			obj_to_release = obj;
		}

		// shoot inactivation, by going out of screen
		if(!obj->active){
			obj_to_release = obj;
		}

		obj = OBJPOOL_loop_next(&shoot_pool);
		if (obj_to_release) {
			OBJPOOL_release(&shoot_pool, obj_to_release);
		}
	}
}

inline void update_power_ups() {
	GameObject* obj = OBJPOOL_loop_init(&p_up_pool);
	while (obj) {
		POWERUP_update(obj);
		GameObject* obj_to_release = NULL;
		if (GAMEOBJECT_check_collision(&player, obj)) {
			PLAYER_power_up(obj);
			obj_to_release = obj;
		}

		// power-up inactivation, by going out of screen
		if(!obj->active){
			obj_to_release = obj;
		}
		obj = OBJPOOL_loop_next(&p_up_pool);
		if (obj_to_release) {
			OBJPOOL_release(&p_up_pool, obj_to_release);
		}
	}
}

static inline void game_update() {
	update_input();

	PLAYER_update();
	update_enemies();
	update_shots();

	update_power_ups();

	spawn_enemies();
	BACKGROUND_update();


	LEVEL_update_camera(&player, level_rooms);

	if(!XGM2_isPlaying()){
		XGM2_playTrack(1);
	}
}

////////////////////////////////////////////////////////////////////////////
// MAIN

int main(bool resetType) {
	// Soft reset doesn't clear RAM. Can lead to bugs.
	if (!resetType) {
		SYS_hardReset();
	}
	SYS_showFrameLoad(true);
	
	SYS_doVBlankProcess();

	XGM2_load(background_track);
	XGM2_setLoopNumber(-1);
	XGM2_setFMVolume(30);
	XGM2_playTrack(1);

	while (true) {
		switch (game_state) {
		case GAME_INIT:
			game_init();
			break;
		case GAME_OVER:
			SCREEN_over_update();
			break;
		case GAME_PLAYING:
			game_update();
			break;
		case GAME_NEXT_LEVEL:
			game_next_level();
			break;
		case GAME_CLEAR:
			SCREEN_game_clear_update();
			break;
		}	
		SPR_update();
		SYS_doVBlankProcess();

	}

	return 0;
}