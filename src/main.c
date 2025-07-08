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
 * [  ] Chance de inimigos droparem power-ups
 * [  ] Power-ups
 * [  ] Inteligencia dos Inimigos
 * [  ] Spawn de inimigos
 * [  ] Corrigir sprite da nave
 * [TS] BUG - Colisão do tiro falha as vezes
 * [  ] Tela de Level Clear
 * [  ] Transição de levels
 * [  ] Implementação de novo level
 * [WIP] Inimigo do tipo UFO
 * [BUG] Arrumar colisão quando player está movendo lateralmente (nave em um y superior ao tile de colisão)
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

// IF DEBUGGING  CHANGE MAP IN resources.res to DEBUG MAP
// #define DEBUG

// index for tiles in VRAM (first tile reserved for SGDK)
u16 ind = TILE_USER_INDEX;

enum NUMBER_OF_ROOMS level_rooms;
enum GAME_STATE game_state;

////////////////////////////////////////////////////////////////////////////
// GAME INIT

// enemies pool
#define MAX_ENEMIES 50
GameObject enemy_array[MAX_ENEMIES];
// u16 enemy_tiles_ind;
ObjectsPool enemy_pool;

void init_enemies() {
	OBJPOOL_init(&enemy_pool, enemy_array, LEN(enemy_array));

	curr_mapobj = 0;

	// load enemy tiles
	// enemy_tiles_ind = ind;
	ENEMY_load_tiles(ind);
}

void spawn_enemies() {
	// looks if there are enemies in spawn range
	switch (current_level)
	{
	case 1:
		break;
	case 2:
		// entering level 2, start spawning enemies
		MapObject* mapobj = MAPOBJ_lookup_enemies(level2_enemies, LEN(level2_enemies));
		while(mapobj){
			// looks for an available space in the enemy pool
			GameObject* enemy = OBJPOOL_get_available(&enemy_pool);
			if (!enemy) return;
			// Enemy factory function: It gets the needed data from MapObject
			// ENEMY_init(enemy, mapobj, enemy_tiles_ind);
			ENEMY_init(enemy, mapobj, ind);
			mapobj = MAPOBJ_lookup_enemies(level2_enemies, LEN(level2_enemies));
		}
	case 3:
		break;
	case 4:
		break;
	case 5:
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
}


void game_init() {
	init_state();

	#ifdef DEBUG
	level_rooms = DBG;
	#endif
	VDP_setScreenWidth320();
	SPR_init();
	SYS_showFrameLoad(true);

	// init BACKGROUND, LEVEL AND HUD ///////////////////////////////

	#ifdef DEBUG
	VDP_setTextPlane(BG_BACKGROUND);
	#else	
	ind += BACKGROUND_init(ind);
	ind += HUD_init(ind);
	#endif

	ind += LEVEL_init(ind, level_rooms);
	
	#ifdef DEBUG
	LEVEL_draw_map(level_rooms);
	#endif
	
	// init GAME OBJECTS ////////////////////////////////////////////

	PLAYER_init(ind);

	init_enemies();
}

void game_next_level() {
	// show transition screen
	
	SPR_reset();
	ind = TILE_USER_INDEX; // reset index for tiles in VRAM
	//
	current_level++;
	if (current_level > 4) {
		current_level = 1;
		game_state = GAME_CLEAR;
		return;
	}
	kprintf("Next level: %d", current_level);

	ind += BACKGROUND_init(ind);
	ind += HUD_init(ind);
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
	default:
		break;
	}
	// level_rooms = LEVEL2;
	ind += LEVEL_init(ind, level_rooms);
	
	PLAYER_init(ind);

	init_enemies();
	// sleep(500);
}

////////////////////////////////////////////////////////////////////////////
// GAME LOGIC

inline void update_enemies() {
	GameObject* obj = OBJPOOL_loop_init(&enemy_pool);
	while (obj) {
		obj->update(obj);
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

static inline void game_update() {
	update_input();

	PLAYER_update();
	update_enemies();

	spawn_enemies();
	#ifndef DEBUG
	BACKGROUND_update();
	#endif

	LEVEL_update_camera(&player, level_rooms);
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
	
	while (true) {
		switch (game_state) {
		case GAME_INIT:
			game_init();
			game_state = GAME_PLAYING;
			break;
		case GAME_OVER:
				SCREEN_over_update();
				game_state = GAME_INIT;
				break;
		case GAME_PLAYING:
			game_update();
			break;
		case GAME_NEXT_LEVEL:
			game_next_level();
			game_state = GAME_PLAYING;
			break;
		case GAME_CLEAR:
			break;
		}	
		SPR_update();
		SYS_doVBlankProcess();

		// update VDP map manually
		// LEVEL_update_items_in_VDP();
	}

	return 0;
}