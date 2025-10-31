// *****************************************************************************
    // start include guard
    #ifndef DEFINITIONS_HPP
    #define DEFINITIONS_HPP
// *****************************************************************************


// ---------------------------------------------------------
//   DEFINITIONS AND CONSTANTS
// ---------------------------------------------------------


// names for textures
enum Textures
{
    TextureGame,
    TextureGround,
    TextureWalls
};

// names for texture regions
#define FirstRegionTileSet 0
#define RegionCharacter 100

// tile sizes
#define TILE_SIZE 80
#define WALL_HEIGHT (2 * TILE_SIZE)

// dimensions of our map, in tiles
#define MapTilesX 14
#define MapTilesY 13

// predefined Z distances
#define GROUND_BASE_Z (4 * TILE_SIZE)
#define ROOFS_BASE_Z (GROUND_BASE_Z - WALL_HEIGHT)

// speed of our character
#define CHAR_SPEED 4

// bit flags for the 4 possible wall sides
// (will be combined to indicate the walls for each tile)
#define WALL_LEFT   1
#define WALL_RIGHT  2
#define WALL_TOP    4
#define WALL_BOTTOM 8


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
