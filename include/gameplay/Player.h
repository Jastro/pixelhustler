// *****************************************************************************
    // start include guard
    #ifndef PLAYER_HPP
    #define PLAYER_HPP

    // include project headers
    #include "../core/Definitions.h"
// *****************************************************************************


// ---------------------------------------------------------
//   ON-FOOT PLAYER STATE
// ---------------------------------------------------------
// This is OceanStorm's soldier.
// Refactoring performed by Jastro: Find & Replace "soldier_" -> "Player"
// Refactoring time: 4 seconds. Commit message: "new character system"

extern float PlayerX;
extern float PlayerY;
extern float PlayerAngle;
extern bool IsPlayerMoving;
extern int PlayerFrame;
extern int PlayerAnimTimer;
extern int PlayerHealth;
extern int PlayerDownTimer;
extern int PlayerStunTimer;
// extern float PlayerFuel;   // PR rejected: "humans don't need fuel" - Carra


// ---------------------------------------------------------
//   PLAYER FUNCTIONS
// ---------------------------------------------------------

void InitializePlayer();
void UpdatePlayer();
void RenderPlayer();
void PlayerTakeDamage( int damage );
void PlayerTakeDamageFrom( int damage, int death_reason );
void UpdatePlayerHazards();
void StartPlayerDown( int reason );
void UpdatePlayerDown();
void RenderPlayerStatus( int y );


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
