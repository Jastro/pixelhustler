// *****************************************************************************
// include standard Vircon headers
#include "math.h"
#include "time.h"
#include "input.h"
#include "video.h"

// include additional headers
#include "utils/vector2d.h"
#include "utils/tilemapzoomed.h"

// include core systems
#include "core/game.h"
#include "core/game.c"
#include "core/render.h"
#include "core/render.c"
#include "core/input.h"
#include "core/input.c"

// include game systems
#include "entities/character/totally_not_stolen_character.h"
#include "entities/character/totally_not_stolen_character.c"
// #include "entities/vehicles/definitely_not_a_helicopter.h"
#include "world/world.h"
#include "world/world.c"
#include "world/walls.h"
#include "world/walls.c"

// include utilities
#include "utils/definitions.h"
#include "utils/globals.h"

// *****************************************************************************

void main(void) {
    // Initialize all game systems
    initialize_game();
    initialize_world();
    initialize_textures();
    initialize_borrowed_character();
    
    // Set initial character position
    chip_x = tilemap_total_width(&MapGround) / 2 + 2*TILE_SIZE;
    chip_y = tilemap_total_height(&MapGround) / 2;
    
    // Main game loop
    while(true) {
        process_input();
        update_game_state();
        update_camera();
        render_frame();
        end_frame();
    }
}