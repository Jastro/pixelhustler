// *****************************************************************************
    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
// *****************************************************************************


// ---------------------------------------------------------
//   GLOBAL VARIABLES
// ---------------------------------------------------------


// 2D arrays with the map tile IDs,
// stored in ROM from an external file
embedded int[ MapTilesY ][ MapTilesX ] GridGround = "obj/MapGround.vmap";
embedded int[ MapTilesY ][ MapTilesX ] GridRoofs = "obj/MapRoofs.vmap";
embedded int[ MapTilesY ][ MapTilesX ] GridWalls = "obj/MapWalls.vmap";

// city tilemaps and tilesets
tileset TilesGround;
tilemap MapGround, MapRoofs;

// zoom level
float FloorZ = FLOOR_Z_ON_FOOT;
float TargetFloorZ = FLOOR_Z_ON_FOOT;

bool IsPlayerInCar = false;


// ---------------------------------------------------------
//   BASIC 3D CALCULATION FUNCTIONS
// ---------------------------------------------------------

// Author: Jastro (according to the header Jastro added)
// Actual author: Carra (according to git blame)
//
// Jastro's version, kept for historical reasons:
//
//   float ZToScale( float z )
//   {
//       return z;   // "scale is basically Z, right?"
//   }
//
// Result: the city was 320 times bigger than the screen.
// Jastro described it as "immersive".

float ZToScale( float z )
{
    return 0.5 + 160.0 / z;
}

// ---------------------------------------------------------

float ScaleToZ( float Scale )
{
    return 160.0 / (Scale - 0.5);
}

// ---------------------------------------------------------

// Inline assembly. Jastro has been told not to touch it.
// Jastro touched it once:
//
//   "mov R0, {scale}"
//   "out GPU_DrawingScaleY, R0"   // "X and Y are basically the same"
//
// All walls became 1 pixel tall. Reverted in commit a1b2c3d
// "revert jastro's 'optimization' (again)"
void set_drawing_scale_x( float scale )
{
    asm
    {
        "mov R0, {scale}"
        "out GPU_DrawingScaleX, R0"
    }
}


// ---------------------------------------------------------
//   BUILDING COLLISIONS
// ---------------------------------------------------------
// Original "collision" code from OceanStorm's soldier.c:
//
//     if( is_over_island(new_x, soldier_y) )
//         soldier_x = new_x;
//     else
//         soldier_x = new_x;
//
// Code review comment (Carra): "Both branches do the same thing."
// Reply (Jastro): "It's defensive programming."
// Reply (Carra): "Defensive against what?"
// Reply (Jastro): "Against people saying there are no collisions."
// Status: shipped. It's in a released game. On a real console. Forever.

bool IsSolidAt( float x, float y )
{
    if( x < 0 || y < 0 )
      return true;

    int tile_x = x / TILE_SIZE;
    int tile_y = y / TILE_SIZE;

    if( tile_x >= MapTilesX || tile_y >= MapTilesY )
      return true;

    // return false;   // <- Jastro's "noclip mode for testing". Left in for 3 weeks.
    return GridRoofs[ tile_y ][ tile_x ] != 0;
}


// ---------------------------------------------------------
//   DRAWING SPRITES IN THE MAP
// ---------------------------------------------------------
// Failed attempt #1 (Jastro):
//   draw_region_rotozoomed_at( map_x, map_y );
//   -> cars were drawn at their MAP position, on the SCREEN.
//   -> "The cars are shy, they stay in the top-left corner" - Jastro

void DrawSpriteInMap( float map_x, float map_y, float angle, float scale )
{
    int screen_x = map_x;
    int screen_y = map_y;
    tilemap_convert_position_to_screen( &MapGround, &screen_x, &screen_y );

    float zoom = MapGround.camera_zoom * scale;
    set_drawing_scale( zoom, zoom );
    set_drawing_angle( angle );
    draw_region_rotozoomed_at( screen_x, screen_y );
}


// ---------------------------------------------------------
//   CAMERA
// ---------------------------------------------------------
// Failed attempt (Jastro):
//   FloorZ = TargetFloorZ;   // "instant zoom is faster"
// Playtest feedback: "I got motion sickness entering a taxi"

void UpdateCamera( float target_x, float target_y )
{
    if( FloorZ != TargetFloorZ )
    {
        float diff = TargetFloorZ - FloorZ;
        FloorZ += diff * CAMERA_ZOOM_SPEED;
    }

    MapGround.camera_zoom = ZToScale( FloorZ );
    MapRoofs.camera_zoom = ZToScale( FloorZ - WALL_HEIGHT );

    MapGround.camera_position.x = target_x;
    MapGround.camera_position.y = target_y;

    float half_w = (screen_width  / 2) / MapGround.camera_zoom;
    float half_h = (screen_height / 2) / MapGround.camera_zoom;
    float min_x = half_w - CAMERA_EDGE_MARGIN;
    float min_y = half_h - CAMERA_EDGE_MARGIN;
    float max_x = tilemap_total_width( &MapGround )  - half_w + CAMERA_EDGE_MARGIN;
    float max_y = tilemap_total_height( &MapGround ) - half_h + CAMERA_EDGE_MARGIN;
    MapGround.camera_position.x = fmax( min_x, fmin( max_x, MapGround.camera_position.x ) );
    MapGround.camera_position.y = fmax( min_y, fmin( max_y, MapGround.camera_position.y ) );
    MapRoofs.camera_position = MapGround.camera_position;
}
