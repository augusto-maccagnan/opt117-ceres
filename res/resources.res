# Player Resources
SPRITE  spr_ship            "sprites/ship_new.png"      3 3 FAST 3
# Bullet Resources
SPRITE  spr_bullet          "sprites/bullet.png"    1 1 FAST 3
# Enemies Resources
SPRITE  spr_ufo             "sprites/ufo.png"      3 3 FAST
SPRITE  spr_kaz             "sprites/kaz_ship.png"      3 3 FAST
SPRITE  spr_rkt             "sprites/rkt_ship.png"      3 3 FAST
# Hud, Background, Menu and Game Over Resources
IMAGE   img_background      "backgrounds/background_1.png" BEST
IMAGE   img_hud             "hud/hud.png" FAST
IMAGE   menu_screen         "screens/menu_screen.png" BEST
# Music and Sound Effects Resources
XGM2     background_track    "sounds/background_track.vgm"
WAV     sfx_shoot           "sounds/laser.wav" XGM2
WAV     sfx_explosion       "sounds/explosion.wav" XGM2

# Geral Level Resources
TILESET level_tiles         "levels/level1/ceres_tiles.png" BEST ALL
PALETTE level_pal           "levels/level1/level1.pal"
# Level 1 Resources
MAP     level1_map          "levels/level1/level1.tmx" map_layer BEST 0
IMAGE   level1_transition   "screens/level1_screen.png" BEST
# Level 2 Resources
MAP     level2_map          "levels/level2/lvl2.tmx" map_layer BEST 0
IMAGE   level2_transition   "screens/level2_screen.png" BEST
OBJECTS level2_enemies      "levels/level2/lvl2_enemy.tmx" enemy_layer "x:f32;y:f32;speed:f16;enemy_type:u8;damage:u8" "sortby:y"
# Level 3 Resources
MAP     level3_map          "levels/level3/lvl3.tmx" map_layer BEST 0
IMAGE   level3_transition   "screens/level3_screen.png" BEST
OBJECTS level3_enemies      "levels/level3/lvl3_enemy.tmx" enemy_layer "x:f32;y:f32;speed:f16;enemy_type:u8;damage:u8" "sortby:y"

#Level 4 Resources
MAP     level4_map          "levels/level4/lvl4.tmx" map_layer BEST 0
IMAGE   level4_transition   "screens/level4_screen.png" BEST
OBJECTS level4_enemies      "levels/level4/lvl4_enemy.tmx" enemy_layer "x:f32;y:f32;speed:f16;enemy_type:u8;damage:u8" "sortby:y"

# Level 5 Resources
MAP     level5_map          "levels/level5/lvl5.tmx" map_layer
IMAGE   level5_transition   "screens/level5_screen.png" BEST
OBJECTS level5_enemies      "levels/level5/lvl5_enemy.tmx" enemy_layer "x:f32;y:f32;speed:f16;enemy_type:u8;damage:u8" "sortby:y"