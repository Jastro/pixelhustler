// *****************************************************************************
    // start include guard
    #ifndef GLOBALS_HPP
    #define GLOBALS_HPP
// *****************************************************************************


// ---------------------------------------------------------
//   GLOBAL VARIABLES
// ---------------------------------------------------------


// our 2D array containing the map tile IDs
// is stored in ROM from an external file
extern int[ MapTilesY ][ MapTilesX ] GridGround;
extern int[ MapTilesY ][ MapTilesX ] GridRoofs;
extern int[ MapTilesY ][ MapTilesX ] GridWalls;

// city tilemaps and tilesets
extern tileset TilesGround;
extern tilemap MapGround, MapRoofs;

// zoom level
extern float FloorZ;


// ---------------------------------------------------------
//   BASIC 3D CALCULATION FUNCTIONS
// ---------------------------------------------------------


float ZToScale( float z );
float ScaleToZ( float Scale );
void set_drawing_scale_x( float scale );


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
