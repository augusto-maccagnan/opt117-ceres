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
 * $ make GDK=~/sgdk200 -f ~/sgdk200/makefile_wine.gen
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
 * [WIP] Arrumar colisão quando player está movendo lateralmente (nave em um y superior ao tile de colisão)
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

#include "entities/player.h"

// IF DEBUGGING  CHANGE MAP IN resources.res to DEBUG MAP
// #define DEBUG

// index for tiles in VRAM (first tile reserved for SGDK)
u16 ind = TILE_USER_INDEX;

enum NUMBER_OF_ROOMS level_rooms;
enum GAME_STATE game_state;

////////////////////////////////////////////////////////////////////////////
// GAME INIT

void init_state() {
	// reset index for tiles in VRAM
	ind = TILE_USER_INDEX; 

	// reset score
	player_score = 0;

	// reset game state
	game_state = GAME_INIT;

	// reset level rooms
	level_rooms = LEVEL1;
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

	ind += PLAYER_init(ind);
}

////////////////////////////////////////////////////////////////////////////
// GAME LOGIC

// static inline void color_effects() {
// 	--bg_colors_delay;
// 	if (bg_colors_delay == 0) {
// 		// rotate_colors_left(PAL_BACKGROUND*16, PAL_BACKGROUND*16+15);
// 		glow_color(PAL_BACKGROUND*16+8, bg_color_glow, 5);

// 		bg_colors_delay = 15;
// 	}
// }

static inline void game_update() {
	update_input();

	PLAYER_update();

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