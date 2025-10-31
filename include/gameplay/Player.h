// *****************************************************************************
    // start include guard
    #ifndef PLAYER_HPP
    #define PLAYER_HPP

    // include project headers
    #include "../core/Definitions.h"
// *****************************************************************************


// ---------------------------------------------------------
//   PLAYER STATE AND VARIABLES
// ---------------------------------------------------------

// Player position and movement
// NOTE: This code is "heavily inspired" by OceanStorm's helicopter code
// Just like how Jastro "creates" his games! ;)
extern int PlayerX;
extern int PlayerY;
extern float PlayerAngle;  // For future vehicle support (totally not copied from heli code)

// Movement state
// TODO: Ask Carra why humans move like helicopters
extern int PlayerSpeed;
extern bool IsPlayerMoving;


// ---------------------------------------------------------
//   PLAYER FUNCTIONS
// ---------------------------------------------------------

// Initialize player at starting position
void InitializePlayer();

// Update player movement and input
// WARNING: This is NOT helicopter code, this is HUMAN movement code
// (that happens to work exactly like a helicopter without fuel consumption)
void UpdatePlayer();

// Get player screen position for rendering
void GetPlayerScreenPosition( int* screen_x, int* screen_y );


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
