#include "game.h"
#include "../utils/definitions.h"
#include "../core/input.h"
#include "../core/input.c"
#include "../core/render.h"
#include "../core/render.c"
#include "../world/world.h"
#include "../world/world.c"

float FloorZ = GROUND_BASE_Z;

void initialize_game() {
    initialize_textures();
    initialize_world();
    initialize_input();
}

void update_game_state() {
    update_civilian_movement();
}

void update_camera() {
    // Actualizar posición cámara
    MapGround.camera_position.x = chip_x;
    MapGround.camera_position.y = chip_y;
    
    // Control de zoom
    if(gamepad_button_a() > 0 && FloorZ < 7*TILE_SIZE) FloorZ += 2;
    if(gamepad_button_b() > 0 && FloorZ > 4*TILE_SIZE) FloorZ -= 2;
    
    MapGround.camera_zoom = ZToScale(FloorZ);
    MapRoofs.camera_zoom = ZToScale(FloorZ - WALL_HEIGHT);
    
    tilemap_clip_camera_position(&MapGround);
    MapRoofs.camera_position = MapGround.camera_position;
}