# SPRITE  spr_ball            "sprites/ball.png"      2 2 FAST 3

# Player Resources
SPRITE  spr_ship            "sprites/ship_new.png"      3 3 FAST 3
# SPRITE  spr_ship            "sprites/ship_bound_new.png"      3 3 FAST 3
SPRITE  spr_bullet          "sprites/bullet.png"    1 1 FAST 3

# Enemies Resources
SPRITE  spr_ufo            "sprites/ufo.png"      3 3 FAST 3
SPRITE  spr_kaz            "sprites/kaz_ship.png"      3 3 FAST 3
SPRITE  spr_rkt            "sprites/rkt_ship.png"      3 3 FAST 3

# Hud and Background Resources
IMAGE   img_background    "backgrounds/background_1.png" BEST
IMAGE   img_hud           "hud/hud.png" FAST

TILESET level_tiles      "levels/level1/ceres_tiles.png" BEST ALL
PALETTE level_pal        "levels/level1/level1.pal"
# Level 1 Resources
MAP     level1_map        "levels/level1/level1.tmx" map_layer BEST 0
# Level 2 Resources
MAP     level2_map        "levels/level2/lvl2.tmx" map_layer BEST 0
OBJECTS level2_enemies    "levels/level2/lvl2_enemy.tmx" enemy_layer "x:f32;y:f32;speed:f16;enemy_type:u8;damage:u8" "sortby:y"