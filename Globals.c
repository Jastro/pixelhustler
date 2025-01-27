// *****************************************************************************
    // include project headers
    #include "Definitions.h"
    #include "Globals.h"
// *****************************************************************************


// ---------------------------------------------------------
//   GLOBAL VARIABLES
// ---------------------------------------------------------


// our 2D array containing the map tile IDs
// is stored in ROM from an external file
embedded int[ MapTilesY ][ MapTilesX ] GridGround = "obj/MapGround.vmap";
embedded int[ MapTilesY ][ MapTilesX ] GridRoofs = "obj/MapRoofs.vmap";
embedded int[ MapTilesY ][ MapTilesX ] GridWalls = "obj/MapWalls.vmap";

// city tilemaps and tilesets
tileset TilesGround;
tilemap MapGround, MapRoofs;

// zoom level
float FloorZ = GROUND_BASE_Z;


// ---------------------------------------------------------
//   BASIC 3D CALCULATION FUNCTIONS
// ---------------------------------------------------------


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

void set_drawing_scale_x( float scale )
{
    asm
    {
        "mov R0, {scale}"
        "out GPU_DrawingScaleX, R0"
    }
}
