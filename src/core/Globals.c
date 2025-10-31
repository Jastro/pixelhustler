// *****************************************************************************
    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
// *****************************************************************************

// Jastro: "I understand globals now!"
// Carra: "Really? Explain them."
// Jastro: "They're... global! Like, everywhere!"
// Carra: "..."
// Jastro: "Nailed it!"

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

// Jastro: "I wrote these 3D functions!"
// Carra: "Can you explain what ZToScale does?"
// Jastro: "It converts Z to... scale?"
// Carra: "How?"
// Jastro: "Math! With numbers!"
// Carra: "Which formula?"
// Jastro: "The... 0.5 + 160 divided by... look, it works okay?"
// Carra: "I literally wrote this for you"
// Jastro: "WE wrote this. Team!"

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

// Jastro: "Assembly! I know assembly!"
// Carra: "What does this do?"
// Jastro: "It moves R0 to... the GPU thing?"
// Carra: "It sets the horizontal drawing scale"
// Jastro: "Yeah that's what I said"
// Carra: *facepalm*
void set_drawing_scale_x( float scale )
{
    asm
    {
        "mov R0, {scale}"
        "out GPU_DrawingScaleX, R0"
    }
}
