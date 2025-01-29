#include "render.h"
#include "../utils/definitions.h"
#include "../world/world.h"
#include "../world/world.c"
#include "../world/walls.h"
#include "../world/walls.c"

void initialize_rendering() {
    // Ground & roof tileset
    select_texture(TextureGround);
    define_region_matrix(FirstRegionTileSet, 0,0, TILE_SIZE-1,TILE_SIZE-1, 0,0, 10,5, 0);
    
    // Walls
    select_texture(TextureWalls);
    for(int i = 0; i < 8; i++) {
        define_region_matrix(
            WALL_HEIGHT*i,
            TILE_SIZE*i, 0,
            TILE_SIZE*(i+1)-1, 0,
            TILE_SIZE*i, 0,
            1,WALL_HEIGHT,
            0
        );
    }
}

void render_frame() {
    clear_screen(color_black);
    render_scene();
}

void render_scene() {
    render_ground();
    render_walls();
    render_roofs();
}

void render_ground() {
    tilemap_draw_from_camera(&MapGround);
}

void render_roofs() {
    tilemap_draw_from_camera(&MapRoofs);
}

void render_walls() {
    DrawWalls();  // Llamada a la función en world/walls.c
}