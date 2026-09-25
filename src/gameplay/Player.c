// *****************************************************************************
    // include Vircon32 headers
    #include "input.h"
    #include "math.h"

    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
    #include "../../include/gameplay/Player.h"
    #include "../../include/gameplay/Car.h"
// *****************************************************************************


// ---------------------------------------------------------
//   PLAYER VARIABLES
// ---------------------------------------------------------

float PlayerX;
float PlayerY;
float PlayerAngle;
bool IsPlayerMoving = false;
int PlayerFrame = 0;
int PlayerAnimTimer = 0;


bool PlayerHitsCar( float x, float y )
{
    float min_distance2 = (CAR_CIRCLE_RADIUS + PED_RADIUS) * (CAR_CIRCLE_RADIUS + PED_RADIUS);

    for( int i = 0; i < MaxCars; i++ )
    {
        float fx = sin( CarAngle[ i ] ) * CAR_CIRCLE_SPACING;
        float fy = -cos( CarAngle[ i ] ) * CAR_CIRCLE_SPACING;

        for( int c = -1; c <= 1; c++ )
        {
            float dx = x - (CarX[ i ] + c * fx);
            float dy = y - (CarY[ i ] + c * fy);

            if( dx * dx + dy * dy < min_distance2 )
              return true;
        }
    }

    return false;
}

bool PlayerCanMoveTo( float x, float y )
{
    if( IsSolidAt( x, y ) )
      return false;

    if( PlayerHitsCar( PlayerX, PlayerY ) )
      return true;

    return !PlayerHitsCar( x, y );
}


// ---------------------------------------------------------
//   INITIALIZATION
// ---------------------------------------------------------
// reset_soldier() from OceanStorm, "original" edition

void InitializePlayer()
{
    // PlayerX = StartingX;   // Jastro: copied from OceanStorm. StartingX was the aircraft carrier.
    //                        // Player spawned in the ocean. There is no ocean. Player spawned in the void.
    PlayerX = 9 * TILE_SIZE;
    PlayerY = 2.5 * TILE_SIZE;
    PlayerAngle = -pi / 2;
    IsPlayerMoving = false;
    PlayerFrame = 0;
    PlayerAnimTimer = 0;
    IsPlayerInCar = false;
    TargetFloorZ = FLOOR_Z_ON_FOOT;
}


// ---------------------------------------------------------
//   UPDATE
// ---------------------------------------------------------
// update_soldier() from OceanStorm, minus swimming, bullets and bombs.
// Removed features changelog (Jastro):
//   - swimming  ("it's a city")
//   - bombs     ("coming back as DLC")
//   - drowning  ("ok this one was Carra")

void UpdatePlayer()
{
    select_gamepad( 0 );

    // BUG #7 (closed): "pressing B enters and exits the car in the same frame"
    //   Reported by: Jastro
    //   Jastro's analysis: "the car is haunted"
    //   Root cause (Carra): player and car were both updated in the same
    //   frame and both read the same B press. Fixed with the return below.
    //   Jastro's reaction: "so it WASN'T haunted?" (visibly disappointed)
    if( gamepad_button_b() == 1 )
    {
        if( TryEnterNearestCar() )
          return;
    }

    int direction_x, direction_y;
    gamepad_direction( &direction_x, &direction_y );

    // (this time the else branch is different; reviewed by Carra, twice)
    float new_x = PlayerX + direction_x * CHAR_SPEED;
    if( PlayerCanMoveTo( new_x, PlayerY ) )
      PlayerX = new_x;

    float new_y = PlayerY + direction_y * CHAR_SPEED;
    if( PlayerCanMoveTo( PlayerX, new_y ) )
      PlayerY = new_y;

    // NOTE: the player has no weapon. The player aims anyway. With intent.
    IsPlayerMoving = (direction_x != 0 || direction_y != 0);

    if( IsPlayerMoving )
    {
        PlayerAngle = atan2( direction_y, direction_x );

        // Jastro's first attempt reused the heli rotor animation:
        //   PlayerFrame = 1 - PlayerFrame;   // toggles 0/1 every frame
        // Result: the character vibrated at 60 Hz. QA filed it as "possessed".
        PlayerAnimTimer++;
        if( PlayerAnimTimer >= PED_ANIM_SPEED )
        {
            PlayerAnimTimer = 0;
            PlayerFrame = (PlayerFrame + 1) % FramesPerPed;
        }
    }
    else
    {
        PlayerFrame = 0;
        PlayerAnimTimer = 0;
    }

    UpdateCamera( PlayerX, PlayerY );
}


// ---------------------------------------------------------
//   RENDERING
// ---------------------------------------------------------

void RenderPlayer()
{
    if( IsPlayerInCar )
      return;

    select_texture( TextureSprites );
    select_region( RegionFirstPed + PedJastro * FramesPerPed + PlayerFrame );

    //   float draw_angle = PlayerAngle;            // v1: walks sideways like a crab
    //   float draw_angle = PlayerAngle - pi / 2;   // v2: moonwalks. Jastro wanted to keep it.
    float draw_angle = PlayerAngle + pi / 2;     // v3: Carra

    set_multiply_color( make_color_rgba( 0, 0, 0, 100 ) );
    DrawSpriteInMap( PlayerX + 2, PlayerY + 2, draw_angle, 1.0 );

    set_multiply_color( color_white );
    DrawSpriteInMap( PlayerX, PlayerY, draw_angle, 1.0 );
}


// ---------------------------------------------------------
//   DEV TEAM NOTES
// ---------------------------------------------------------

/*
 * $ git log --oneline -- src/gameplay/Player.c
 *
 *   f00dbad  carra   real collisions (the else branch now does something)
 *   c0ffee1  carra   fix sprite angle (+pi/2)
 *   deadbee  carra   fix B entering and exiting car on the same frame
 *   1337abc  jastro  new character system
 *   1337abd  jastro  new character system (fix)
 *   1337abe  jastro  new character system (fix 2)
 *   1337abf  jastro  asdasdasd
 *   1337ac0  jastro  pls work
 *
 * TODO:
 *   - run with A              (assigned: Carra)
 *   - steal cars with people inside, like real GTA  (assigned: Carra)
 *   - pedestrians             (assigned: Carra)
 *   - take credit for all of the above  (assigned: Jastro) [DONE]
 *
 * KNOWN BUGS:
 *   - player aims without a weapon (wontfix: "it's his personality")
 */
