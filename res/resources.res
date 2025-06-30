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

# Level 1 Resources
TILESET level1_tiles      "levels/ceres-level/ceres_tiles.png" BEST ALL
PALETTE level1_pal        "levels/ceres-level/level1.pal"
MAP     level1_map        "levels/ceres-level/level1.tmx" map_layer BEST 0
# MAP     level1_map        "levels/ceres-level/debug.tmx" map_layer BEST 0