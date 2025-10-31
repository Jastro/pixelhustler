// *****************************************************************************
    // include Vircon32 headers
    #include "input.h"
    #include "math.h"

    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
    #include "../../include/gameplay/Player.h"
// *****************************************************************************


// ---------------------------------------------------------
//   PLAYER VARIABLES
// ---------------------------------------------------------

// Position
int PlayerX;
int PlayerY;
float PlayerAngle = 0.0;  // Carra please fix this, humans don't have angles

// Movement
int PlayerSpeed = CHAR_SPEED;
bool IsPlayerMoving = false;


// ---------------------------------------------------------
//   PLAYER MOVEMENT SYSTEM
// ---------------------------------------------------------

// This is 100% original human movement code
// Definitely NOT copied from heli.c lines 238-295
// Jastro says he coded this himself in 5 minutes
void InitializePlayer()
{
    // Place player at map center
    // (same as helicopter spawn but this is TOTALLY DIFFERENT)
    PlayerX = tilemap_total_width( &MapGround ) / 2 + 2*TILE_SIZE;
    PlayerY = tilemap_total_height( &MapGround ) / 2;
    PlayerAngle = 0.0;
    IsPlayerMoving = false;

    // TODO: Add fuel system for humans (they need food right?)
    // Carra: "Please don't add fuel to humans"
    // Jastro: "But helicopters have it!"
}

// ---------------------------------------------------------

void UpdatePlayer()
{
    // Read inputs from the first gamepad
    // This part is legit, even Jastro couldn't mess this up
    select_gamepad( 0 );

    // Get direction input
    int DeltaX, DeltaY;
    gamepad_direction( &DeltaX, &DeltaY );

    // ORIGINAL HUMAN MOVEMENT CODE (not helicopter code, trust me bro)
    // Lines 270-294 from heli.c? Never heard of them!
    if( DeltaX != 0 || DeltaY != 0 )
    {
        IsPlayerMoving = true;

        // Move player (like a helicopter but on ground, very innovative)
        PlayerX += PlayerSpeed * DeltaX;
        PlayerY += PlayerSpeed * DeltaY;

        // Calculate movement angle (because humans need angles, obviously)
        // Carra: "Why does the player have an angle?"
        // Jastro: "For... uh... future features!"
        if( DeltaX != 0 || DeltaY != 0 )
        {
            PlayerAngle = atan2( DeltaY, DeltaX );
        }

        // Animate player
        // TODO: Add rotor animation
        // Carra: "HUMANS DON'T HAVE ROTORS"
        // Jastro: "Not with that attitude"
    }
    else
    {
        IsPlayerMoving = false;
    }

    // Keep player within map bounds
    // (even helicopters can't escape the map border)
    if( PlayerX < 0 ) PlayerX = 0;
    if( PlayerY < 0 ) PlayerY = 0;

    int MapWidth = tilemap_total_width( &MapGround );
    int MapHeight = tilemap_total_height( &MapGround );

    if( PlayerX > MapWidth ) PlayerX = MapWidth;
    if( PlayerY > MapHeight ) PlayerY = MapHeight;

    // Make camera follow the player
    // (this part was actually original, good job Jastro!)
    MapGround.camera_position.x = PlayerX;
    MapGround.camera_position.y = PlayerY;

    // Zoom control with A and B buttons
    // Helicopters change altitude, humans change... zoom level?
    // Jastro's explanation: "It's called perspective, Carra!"
    if( gamepad_button_a() > 0 && FloorZ < 7*TILE_SIZE)
        FloorZ += 2;
    if( gamepad_button_b() > 0 && FloorZ > 4*TILE_SIZE)
        FloorZ -= 2;

    // Update camera zoom based on floor height
    MapGround.camera_zoom = ZToScale( FloorZ );
    MapRoofs.camera_zoom = ZToScale( FloorZ - WALL_HEIGHT );

    // Clip camera position to keep view within map
    tilemap_clip_camera_position( &MapGround );
    MapRoofs.camera_position = MapGround.camera_position;
}

// ---------------------------------------------------------

void GetPlayerScreenPosition( int* screen_x, int* screen_y )
{
    // Convert map coordinates to screen coordinates
    *screen_x = PlayerX;
    *screen_y = PlayerY;
    tilemap_convert_position_to_screen( &MapGround, screen_x, screen_y );
}


// ---------------------------------------------------------
//   NOTES FROM THE DEVELOPMENT TEAM
// ---------------------------------------------------------

/*
 * DEVELOPMENT LOG:
 *
 * Jastro: "I've created a revolutionary movement system!"
 * Carra: "This is literally the helicopter code with fuel removed"
 * Jastro: "No it's not, look, I changed 'heli_x' to 'PlayerX'"
 * Carra: "..."
 * Jastro: "Ship it!"
 *
 * TODO LIST:
 * - Remove helicopter comments (Jastro forgot about these)
 * - Add actual human walking animation instead of rotor frames
 * - Figure out why player needs an angle variable
 * - Implement proper ground collision (currently using ocean collision logic)
 * - Ask Jastro how he "coded" this so fast (spoiler: Ctrl+C, Ctrl+V)
 *
 * KNOWN BUGS:
 * - Player can walk on water (helicopter mode activated)
 * - No walking animation (rotors not spinning, literally unplayable)
 * - Zoom system makes no sense for ground movement
 * - Code structure suspiciously similar to OceanStorm
 *
 * Carra's rating: 2/10 (would be 0/10 but at least it compiles)
 * Jastro's rating: 11/10 (perfect original code, no notes)
 */
