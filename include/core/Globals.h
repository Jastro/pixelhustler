// *****************************************************************************
    // start include guard
    #ifndef GLOBALS_HPP
    #define GLOBALS_HPP
// *****************************************************************************


// ---------------------------------------------------------
//   GLOBAL VARIABLES
// ---------------------------------------------------------
// Architecture decision record #1 (author: Jastro)
//   Context:  passing parameters is hard
//   Decision: everything is global
//   Status:   accepted (Carra was on holiday)


// 2D arrays with the map tile IDs,
// stored in ROM from an external file
extern int[ MapTilesY ][ MapTilesX ] GridGround;
extern int[ MapTilesY ][ MapTilesX ] GridRoofs;
extern int[ MapTilesY ][ MapTilesX ] GridWalls;

// city tilemaps and tilesets
extern tileset TilesGround;
extern tilemap MapGround, MapRoofs;

// zoom level
extern float FloorZ;
extern float TargetFloorZ;

extern bool IsPlayerInCar;


// ---------------------------------------------------------
//   BASIC 3D CALCULATION FUNCTIONS
// ---------------------------------------------------------


float ZToScale( float z );
float ScaleToZ( float Scale );
void set_drawing_scale_x( float scale );

bool IsSolidAt( float x, float y );

void DrawSpriteInMap( float map_x, float map_y, float angle, float scale );

void UpdateCamera( float target_x, float target_y );


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
