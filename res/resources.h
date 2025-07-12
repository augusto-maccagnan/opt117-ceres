#include <genesis.h>

#ifndef _RES_RESOURCES_H_
#define _RES_RESOURCES_H_

extern const u8 sfx_shoot[2304];
extern const u8 sfx_explosion[6400];
extern const u8 background_track[2048];
extern const SpriteDefinition spr_ship;
extern const SpriteDefinition spr_bullet;
extern const SpriteDefinition spr_ufo;
extern const SpriteDefinition spr_kaz;
extern const SpriteDefinition spr_rkt;
extern const Image img_background;
extern const Image img_hud;
extern const Image menu_screen;
extern const TileSet level_tiles;
extern const Palette level_pal;
extern const SpriteDefinition spr_power_shoot;
extern const SpriteDefinition spr_power_firespd;
extern const SpriteDefinition spr_power_imnt;
extern const MapDefinition level1_map;
extern const Image level1_transition;
extern const MapDefinition level2_map;
extern const Image level2_transition;
extern const void* level2_enemies[23];
extern const MapDefinition level3_map;
extern const Image level3_transition;
extern const void* level3_enemies[87];
extern const MapDefinition level4_map;
extern const Image level4_transition;
extern const void* level4_enemies[108];
extern const TileSet level5_map_tileset0;
extern const MapDefinition level5_map;
extern const Image level5_transition;
extern const void* level5_enemies[135];

#endif // _RES_RESOURCES_H_
