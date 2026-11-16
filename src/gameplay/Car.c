// *****************************************************************************
    // include Vircon32 headers
    #include "input.h"
    #include "math.h"

    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
    #include "../../include/gameplay/Player.h"
    #include "../../include/gameplay/Car.h"
    #include "../../include/gameplay/Traffic.h"
    #include "../../include/gameplay/Police.h"
    #include "../../include/gameplay/Explosions.h"
// *****************************************************************************

// *****************************************************************************
//  CARS
//  "Inspired by" OceanStorm's heli.c. Heavily inspired. Ctrl+C inspired.
//
//  $ diff OceanStorm/src/game/heli.c src/gameplay/Car.c | wc -l
//  Jastro: "see? hundreds of lines changed"
//  Carra:  "those are the comments you added"
//
//  Removed from the heli:
//  - altitude (heli_scale): cars don't take off
//  - fuel: Jastro wanted to keep it "for realism". Rejected.
//  - the 4 cannons: for now (Jastro has opened 6 tickets about it)
//  - landing on the aircraft carrier: there is no aircraft carrier
// *****************************************************************************


// ---------------------------------------------------------
//   CAR VARIABLES
// ---------------------------------------------------------

float[ MaxCars ] CarX;
float[ MaxCars ] CarY;
float[ MaxCars ] CarAngle;
float[ MaxCars ] CarVelX;
float[ MaxCars ] CarVelY;
int[ MaxCars ] CarType;
bool[ MaxCars ] CarIsAI;
float[ MaxCars ] CarFwdX;
float[ MaxCars ] CarFwdY;
float[ MaxCars ] CarSpeed;
int CurrentCar = -1;
bool[ MaxCars ] CarStolen;
bool[ MaxCars ] CarSirenOn;
int PoliceCrashCooldown = 0;
int[ NumParkedCars ] ParkedCarTypes = { CarTypeSport, CarTypeRed, CarTypeVan, CarTypeBlue };

float CarThrottle = 0;
float CarSteer = 0;
bool CarHandbrake = false;


// ---------------------------------------------------------
//   INITIALIZATION
// ---------------------------------------------------------

void PlaceCar( int car, float tile_x, float tile_y, float angle, int type )
{
    CarX[ car ] = tile_x * TILE_SIZE;
    CarY[ car ] = tile_y * TILE_SIZE;
    CarAngle[ car ] = angle;
    CarVelX[ car ] = 0;
    CarVelY[ car ] = 0;
    CarType[ car ] = type;
    CarIsAI[ car ] = false;
    ResetCarHealth( car );
}

// ---------------------------------------------------------

void UpdateCarKinematics()
{
    for( int i = 0; i < MaxCars; i++ )
    {
        CarFwdX[ i ] = sin( CarAngle[ i ] );
        CarFwdY[ i ] = -cos( CarAngle[ i ] );
        CarSpeed[ i ] = CarVelX[ i ] * CarFwdX[ i ] + CarVelY[ i ] * CarFwdY[ i ];
    }
}

// ---------------------------------------------------------

void InitializeCars()
{
    // Hotspot history:
    //   v1 (Jastro): hotspot at (0,0). Cars rotated around their top-left
    //                corner like a door. Jastro: "it's drifting".
    //   v2 (Carra):  hotspot at the center.
    select_texture( TextureSprites );

    for( int i = 0; i < NumCarTypes; i++ )
    {
        select_region( RegionFirstCar + i );
        define_region
        (
            i * CAR_SPRITE_W, 0,
            (i+1) * CAR_SPRITE_W - 1, CAR_SPRITE_H - 1,
            i * CAR_SPRITE_W + CAR_SPRITE_W / 2, CAR_SPRITE_H / 2
        );
    }

    for( int ped = 0; ped < NumPedTypes; ped++ )
    {
        for( int f = 0; f < FramesPerPed; f++ )
        {
            int x = f * PED_SPRITE;
            int y = PED_ATLAS_Y + ped * PED_SPRITE;

            select_region( RegionFirstPed + ped * FramesPerPed + f );
            define_region( x, y, x + PED_SPRITE - 1, y + PED_SPRITE - 1, x + PED_SPRITE / 2, y + PED_SPRITE / 2 );
        }
    }

    for( int ped = 0; ped < NumPedTypes; ped++ )
    {
        int x = (ped % 8) * PED_SPRITE;
        int y = CORPSE_ATLAS_Y + (ped / 8) * PED_SPRITE;

        select_region( RegionFirstCorpse + ped );
        define_region( x, y, x + PED_SPRITE - 1, y + PED_SPRITE - 1, x + PED_SPRITE / 2, y + PED_SPRITE / 2 );
    }

    select_region( RegionEnterSign );
    define_region( 130, 82, 196, 107, 164, 94 );
    select_region( RegionExitSign );
    define_region( 130, 111, 196, 136, 165, 123 );
    select_region( RegionArrow );
    define_region( 200, 82, 223, 105, 211, 93 );
    select_region( RegionRing );
    define_region( 200, 110, 239, 149, 219, 129 );
    select_region( RegionWhite );
    define_region_topleft( 240, 82, 247, 89 );
    select_region( RegionLightBox );
    define_region( 130, 150, 143, 181, 136, 165 );
    select_region( RegionLampGlow );
    define_region( 150, 150, 173, 173, 161, 161 );

    for( int i = 0; i < NumParkedCars; i++ )
      PlaceCar( i, ParkingX[ i ] / 80.0, ParkingY[ i ] / 80.0, 0, ParkedCarTypes[ i ] );

    InitializeTraffic();
    UpdateCarKinematics();

    CurrentCar = -1;
}


// ---------------------------------------------------------
//   COLLISIONS
// ---------------------------------------------------------

// The heli was clamped to the world: heli_x = clamp(heli_x, 0, WorldWidth).
// Jastro deleted the clamp because "cars don't fly". Cars drove off the edge of the world.
// Carra put an invisible wall back. Jastro: "GTA doesn't have walls, make it round".
// Carra made it wrap around. The city is now a donut. Jastro: "a torus, actually". He googled it.
bool CarBlockedAt( int car, float x, float y )
{
    bool driven_by_player = IsPlayerInCar && car == CurrentCar;
    bool free_roaming = driven_by_player || (CarIsAI[ car ] && CarMood[ car ] == MoodNormal);
    float cx = CarX[ car ];
    float cy = CarY[ car ];

    if( cx < 0 || cy < 0 || cx >= MapTilesX * TILE_SIZE || cy >= MapTilesY * TILE_SIZE )
      free_roaming = true;

    if( free_roaming )
      return IsBuildingAt( x, y );

    if( x < 0 || y < 0 || x >= MapTilesX * TILE_SIZE || y >= MapTilesY * TILE_SIZE )
      return true;

    return IsSolidAt( x, y );
}

bool CarCollidesAt( int car, float x, float y, float angle );

void WrapPlayerCar( int car )
{
    float width = MapTilesX * TILE_SIZE;
    float height = MapTilesY * TILE_SIZE;
    float old_x = CarX[ car ];
    float old_y = CarY[ car ];

    if( CarX[ car ] < 0 ) CarX[ car ] += width;
    else if( CarX[ car ] >= width ) CarX[ car ] -= width;

    if( CarY[ car ] < 0 ) CarY[ car ] += height;
    else if( CarY[ car ] >= height ) CarY[ car ] -= height;

    if( CarX[ car ] == old_x && CarY[ car ] == old_y )
      return;

    if( CarCollidesAt( car, CarX[ car ], CarY[ car ], CarAngle[ car ] ) )
    {
        CarX[ car ] = old_x - CarVelX[ car ];
        CarY[ car ] = old_y - CarVelY[ car ];
        CarVelX[ car ] *= -CAR_BOUNCE;
        CarVelY[ car ] *= -CAR_BOUNCE;
    }
}

bool CarHitsBuilding( int car, float x, float y, float angle )
{
    float forward_x = sin( angle );
    float forward_y = -cos( angle );
    float side_x = cos( angle );
    float side_y = sin( angle );
    float half_length = CAR_SPRITE_H / 2 - 4;
    float half_width = CAR_SPRITE_W / 2 - 4;

    if( CarBlockedAt( car, x, y ) ) return true;
    if( CarBlockedAt( car, x + forward_x * half_length, y + forward_y * half_length ) ) return true;
    if( CarBlockedAt( car, x - forward_x * half_length, y - forward_y * half_length ) ) return true;
    if( CarBlockedAt( car, x + side_x * half_width, y + side_y * half_width ) ) return true;
    if( CarBlockedAt( car, x - side_x * half_width, y - side_y * half_width ) ) return true;
    return false;
}

// ---------------------------------------------------------

int CarHitsOtherCar( int self, float x, float y, float angle )
{
    float min_distance2 = (2 * CAR_CIRCLE_RADIUS) * (2 * CAR_CIRCLE_RADIUS);
    float self_fx = sin( angle ) * CAR_CIRCLE_SPACING;
    float self_fy = -cos( angle ) * CAR_CIRCLE_SPACING;

    for( int other = 0; other < MaxCars; other++ )
    {
        if( other == self )
          continue;

        float dx = CarX[ other ] - x;
        float dy = CarY[ other ] - y;
        if( dx * dx + dy * dy > CAR_SPRITE_H * CAR_SPRITE_H )
          continue;

        float other_fx = sin( CarAngle[ other ] ) * CAR_CIRCLE_SPACING;
        float other_fy = -cos( CarAngle[ other ] ) * CAR_CIRCLE_SPACING;

        for( int a = -1; a <= 1; a++ )
        {
            float ax = x + a * self_fx;
            float ay = y + a * self_fy;

            for( int b = -1; b <= 1; b++ )
            {
                float cx = CarX[ other ] + b * other_fx - ax;
                float cy = CarY[ other ] + b * other_fy - ay;

                if( cx * cx + cy * cy < min_distance2 )
                  return other;
            }
        }
    }

    return -1;
}

// ---------------------------------------------------------

bool CarCollidesAt( int car, float x, float y, float angle )
{
    if( CarHitsBuilding( car, x, y, angle ) )
      return true;

    return CarHitsOtherCar( car, x, y, angle ) >= 0;
}


// ---------------------------------------------------------
//   DRIVER INPUT
// ---------------------------------------------------------
// update_heli() from OceanStorm. Only the input survived.

void UpdateCar()
{
    if( !IsPlayerInCar )
      return;

    select_gamepad( 0 );

    // B: get out (the heli only allowed it over land;
    // here everything is land. Jastro calls this "improving the code")
    if( gamepad_button_b() == 1 )
    {
        ExitCar();
        return;
    }

    // Steering (the OceanStorm comment literally said "Rotar el avión")
    // The heli rotated at a fixed rate even when hovering still.
    // Issue #23 "cars spin in place like a tank" -> fixed by Carra:
    // steering now depends on speed (see UpdateCarPhysics)
    CarSteer = 0;
    if( gamepad_left() > 0 )
      CarSteer = -1;
    if( gamepad_right() > 0 )
      CarSteer = 1;

    // -----------------------------------------------------
    // REVERSE GEAR
    // Helicopters never needed one. Cars, apparently, do.
    // -----------------------------------------------------
    //
    // Attempt #1 (Jastro) - commit 4b1d3a "reverse gear"
    //
    //   if( gamepad_down() > 0 )
    //     CAR_MOVEMENT_SPEED = -CAR_MOVEMENT_SPEED;
    //
    //   compile: error: expression is not assignable
    //   Jastro: "the compiler is broken"
    //
    // Attempt #2 (Jastro) - commit 4b1d3b "reverse gear (working)"
    //
    //   if( gamepad_down() > 0 )
    //     CarAngle[ c ] += pi;
    //
    //   The car does a 180 every frame (60 times per second)
    //   and then drives forward. Jastro: "technically it's reverse"
    //   QA: "the car is a blender"
    //
    // Attempt #3 (Jastro) - commit 4b1d3c "reverse gear (final)"
    //
    //   if( gamepad_down() > 0 )
    //     CarY[ c ] += CAR_MOVEMENT_SPEED;
    //
    //   Always moves DOWN the screen, whatever way the car is facing.
    //   Only works for cars facing up. Jastro: "just drive facing up"
    //
    // Attempt #4 (Carra) - commit 9e8f7a "actually add reverse gear"
    //
    //   Deleted the heli formula:
    //     heli_x += MovementSpeed * sin(heli_angle);
    //     heli_y -= MovementSpeed * cos(heli_angle);
    //   and wrote real car physics. DOWN brakes, then reverses.
    //   Jastro: "that's what I was going to do next"
    // -----------------------------------------------------

    CarThrottle = 0;
    if( gamepad_up() > 0 )
      CarThrottle = 1;
    else if( gamepad_down() > 0 )
      CarThrottle = -1;

    CarHandbrake = (gamepad_button_a() > 0);

    // OceanStorm's heli had no siren. Jastro wanted the siren to play the OceanStorm theme.
    // There are no sounds. It plays it silently. In Jastro's head, it slaps.
    if( gamepad_button_x() == 1 && CarType[ CurrentCar ] == CarTypePolice )
      CarSirenOn[ CurrentCar ] = !CarSirenOn[ CurrentCar ];
}


// ---------------------------------------------------------
//   CAR PHYSICS
// ---------------------------------------------------------

float GetCarForwardSpeed( int car )
{
    float forward_x = sin( CarAngle[ car ] );
    float forward_y = -cos( CarAngle[ car ] );
    return CarVelX[ car ] * forward_x + CarVelY[ car ] * forward_y;
}

// ---------------------------------------------------------

void MoveCarAxis( int car, float delta_x, float delta_y )
{
    float new_x = CarX[ car ] + delta_x;
    float new_y = CarY[ car ] + delta_y;

    bool driven_by_player = IsPlayerInCar && car == CurrentCar;

    if( CarHitsBuilding( car, new_x, new_y, CarAngle[ car ] ) )
    {
        float impact = fabs( delta_x ) + fabs( delta_y );

        if( impact > CAR_CRASH_SPEED )
          DamageCar( car, (impact - CAR_CRASH_SPEED) * CAR_CRASH_DAMAGE, driven_by_player );

        if( delta_x != 0 ) CarVelX[ car ] *= -CAR_BOUNCE;
        if( delta_y != 0 ) CarVelY[ car ] *= -CAR_BOUNCE;
        return;
    }

    int other = CarHitsOtherCar( car, new_x, new_y, CarAngle[ car ] );
    bool separating = false;

    if( other >= 0 )
    {
        float old_dx = CarX[ car ] - CarX[ other ];
        float old_dy = CarY[ car ] - CarY[ other ];
        float new_dx = new_x - CarX[ other ];
        float new_dy = new_y - CarY[ other ];
        separating = (new_dx * new_dx + new_dy * new_dy) > (old_dx * old_dx + old_dy * old_dy);
    }

    if( other >= 0 && !separating )
    {
        float impact = fabs( CarVelX[ car ] - CarVelX[ other ] ) + fabs( CarVelY[ car ] - CarVelY[ other ] );

        bool player_involved = driven_by_player || (IsPlayerInCar && other == CurrentCar);

        if( player_involved && impact > 1.0 && PoliceCrashCooldown <= 0 )
        {
            int victim = other;
            if( !driven_by_player ) victim = car;

            if( CarType[ victim ] == CarTypePolice && CarIsAI[ victim ] )
            {
                ReportCrime( 2 );
                StartCarMood( victim, MoodPolice );
                PoliceCrashCooldown = 60;
            }
        }

        if( impact > CAR_CRASH_SPEED )
        {
            float damage = (impact - CAR_CRASH_SPEED) * CAR_CRASH_DAMAGE;
            DamageCar( car, damage, player_involved );
            DamageCar( other, damage, player_involved );
        }

        if( delta_x != 0 )
        {
            CarVelX[ other ] += CarVelX[ car ] * CAR_PUSH_TRANSFER;
            CarVelX[ car ] *= -CAR_BOUNCE;
        }

        if( delta_y != 0 )
        {
            CarVelY[ other ] += CarVelY[ car ] * CAR_PUSH_TRANSFER;
            CarVelY[ car ] *= -CAR_BOUNCE;
        }

        return;
    }

    CarX[ car ] = new_x;
    CarY[ car ] = new_y;
}

// ---------------------------------------------------------

void UpdateOneCar( int car, float throttle, float steer, bool handbrake )
{
    float angle = CarAngle[ car ];
    float forward_x = sin( angle );
    float forward_y = -cos( angle );
    float side_x = cos( angle );
    float side_y = sin( angle );

    float forward_speed = CarVelX[ car ] * forward_x + CarVelY[ car ] * forward_y;
    float side_speed    = CarVelX[ car ] * side_x    + CarVelY[ car ] * side_y;

    if( throttle > 0 )
    {
        forward_speed += CAR_ACCELERATION;
    }
    else if( throttle < 0 )
    {
        if( forward_speed > 0.1 )
          forward_speed = fmax( 0, forward_speed - CAR_BRAKE );
        else
          forward_speed -= CAR_REVERSE_ACCELERATION;
    }
    else
    {
        if( forward_speed > 0 )
          forward_speed = fmax( 0, forward_speed - CAR_ROLLING_FRICTION );
        else
          forward_speed = fmin( 0, forward_speed + CAR_ROLLING_FRICTION );
    }

    if( handbrake )
    {
        if( forward_speed > 0 )
          forward_speed = fmax( 0, forward_speed - CAR_HANDBRAKE_DECEL );
        else
          forward_speed = fmin( 0, forward_speed + CAR_HANDBRAKE_DECEL );
    }

    forward_speed *= CAR_DRAG;
    forward_speed = fmax( -CAR_MAX_REVERSE_SPEED, fmin( CAR_MAX_SPEED, forward_speed ) );

    if( handbrake )
      side_speed *= CAR_LATERAL_KEEP_DRIFT;
    else
      side_speed *= CAR_LATERAL_KEEP;

    float steer_factor = fmax( -1, fmin( 1, forward_speed / CAR_FULL_STEER_SPEED ) );
    float turn = steer * CAR_ROTATION_SPEED * steer_factor;

    if( handbrake )
      turn *= CAR_HANDBRAKE_STEER;

    if( turn != 0 && !CarCollidesAt( car, CarX[ car ], CarY[ car ], angle + turn ) )
      CarAngle[ car ] = angle + turn;

    CarVelX[ car ] = forward_x * forward_speed + side_x * side_speed;
    CarVelY[ car ] = forward_y * forward_speed + side_y * side_speed;

    if( fabs( CarVelX[ car ] ) < 0.01 && fabs( CarVelY[ car ] ) < 0.01 )
    {
        CarVelX[ car ] = 0;
        CarVelY[ car ] = 0;
        return;
    }

    MoveCarAxis( car, CarVelX[ car ], 0 );
    MoveCarAxis( car, 0, CarVelY[ car ] );

    if( IsPlayerInCar && car == CurrentCar )
      WrapPlayerCar( car );
}

// ---------------------------------------------------------


void UpdateCarPhysics()
{
    UpdateCarKinematics();

    if( PoliceCrashCooldown > 0 )
      PoliceCrashCooldown--;

    for( int i = 0; i < MaxCars; i++ )
    {
        if( IsPlayerInCar && i == CurrentCar )
          UpdateOneCar( i, CarThrottle, CarSteer, CarHandbrake );
        else if( CarIsAI[ i ] )
        {
            UpdateTrafficCar( i );
            UpdateOneCar( i, TrafficThrottle, TrafficSteer, false );
        }
        else
          UpdateOneCar( i, 0, 0, false );
    }

    if( !IsPlayerInCar )
      return;

    int c = CurrentCar;

    PlayerX = CarX[ c ];
    PlayerY = CarY[ c ];

    TargetFloorZ = FLOOR_Z_IN_CAR + fabs( GetCarForwardSpeed( c ) ) * CAMERA_Z_PER_SPEED;
    UpdateCamera( CarX[ c ], CarY[ c ] );
}


// ---------------------------------------------------------
//   GETTING IN AND OUT
// ---------------------------------------------------------

// OceanStorm had exactly one heli, so the original code was:
//   if( distance_to_heli < 50 ) enter_vehicle();
// Jastro's port with 6 cars:
//   if( distance_to_heli < 50 ) enter_vehicle();   // "heli" is undefined. Compiles anyway? No.
bool TryEnterNearestCar()
{
    if( IsPlayerInCar )
      return false;

    int nearest = -1;
    float nearest_distance = CAR_ENTER_DISTANCE;

    for( int i = 0; i < MaxCars; i++ )
    {
        if( CarWrecked[ i ] )
          continue;

        float dx = PlayerX - CarX[ i ];
        float dy = PlayerY - CarY[ i ];
        float distance = sqrt( dx * dx + dy * dy );

        if( distance < nearest_distance )
        {
            nearest = i;
            nearest_distance = distance;
        }
    }

    if( nearest < 0 )
      return false;

    // enter_vehicle() never checked whose heli it was. There was only one heli. It was yours.
    // Jastro, stealing a police car in the first playtest: "it's mine now, same thing"
    // v2 (Carra): parked cars count too. "It's still not yours, Jastro."
    // v3 (Jastro): reverted v2. "If nobody's inside, it's basically a donation."
    if( !CarStolen[ nearest ] && CarIsAI[ nearest ] )
    {
        if( CarType[ nearest ] == CarTypePolice ) ReportCrime( 6 );
        else ReportCrime( 2 );
    }

    CarStolen[ nearest ] = true;

    CarMood[ nearest ] = MoodNormal;
    CurrentCar = nearest;
    CarIsAI[ nearest ] = false;
    IsPlayerInCar = true;
    PlaySfx( SoundEnterCar );   // the heli boarding sound. For a car. Jastro: "doors are basically rotors"
    CarThrottle = 0;
    CarSteer = 0;
    CarHandbrake = false;
    TargetFloorZ = FLOOR_Z_IN_CAR;

    return true;
}

// ---------------------------------------------------------

void ExitCar()
{
    if( !IsPlayerInCar )
      return;

    int c = CurrentCar;

    float exit_x = CarX[ c ] + cos( CarAngle[ c ] ) * CAR_EXIT_DISTANCE;
    float exit_y = CarY[ c ] + sin( CarAngle[ c ] ) * CAR_EXIT_DISTANCE;

    // FIXME(jastro): let the player exit into buildings, "for secret rooms"
    // WONTFIX(carra): there are no rooms. Buildings are solid roofs.
    if( IsSolidAt( exit_x, exit_y ) )
    {
        exit_x = CarX[ c ] - cos( CarAngle[ c ] ) * CAR_EXIT_DISTANCE;
        exit_y = CarY[ c ] - sin( CarAngle[ c ] ) * CAR_EXIT_DISTANCE;

        if( IsSolidAt( exit_x, exit_y ) )
          return;
    }

    PlaySfx( SoundEnterCar );
    PlayerX = exit_x;
    PlayerY = exit_y;
    PlayerAngle = CarAngle[ c ];
    IsPlayerInCar = false;
    CurrentCar = -1;
    TargetFloorZ = FLOOR_Z_ON_FOOT;
}


// ---------------------------------------------------------
//   RENDERING
// ---------------------------------------------------------

void RenderCars()
{
    select_texture( TextureSprites );

    for( int i = 0; i < MaxCars; i++ )
    {
        if( !IsMapPointVisible( CarX[ i ], CarY[ i ], 60 ) )
          continue;

        select_region( RegionFirstCar + CarType[ i ] );

        // 1. shadow. The heli offset it by altitude; cars have
        //    altitude 0, so 3 px and done.
        //    float shadow_offset = 2 + height_factor * HeliShadowOffset;   // height_factor: undefined
        set_multiply_color( make_color_rgba( 0, 0, 0, 110 ) );
        DrawSpriteInMap( CarX[ i ] + 3, CarY[ i ] + 3, CarAngle[ i ], 1.0 );

        // OceanStorm's destroyed enemies just vanished. Here the wreck stays, charred.
        // Jastro wanted them to vanish too: "less memory". The array is the same size either way.
        set_multiply_color( color_white );
        if( CarWrecked[ i ] ) set_multiply_color( make_color_rgb( 60, 48, 48 ) );
        DrawSpriteInMap( CarX[ i ], CarY[ i ], CarAngle[ i ], 1.0 );
    }
}


// ---------------------------------------------------------

// OceanStorm, soldier.c + heli.c, "Dibujar con parpadeo para que se vea mejor"
// The ENTER / EXIT signs are the original OceanStorm ones. Same pixels.
// Same blink timing (frame % 40 > 8). Same 30 px offset... roughly.
// Carra: "the heli sign said B ENTER because B entered the heli"
// Jastro: "and here B enters the car. Consistency. You're welcome."
void RenderCarSigns()
{
    if( get_frame_counter() % 40 <= 8 )
      return;

    int sign_car = -1;
    int sign_region = RegionEnterSign;

    if( IsPlayerInCar )
    {
        if( fabs( GetCarForwardSpeed( CurrentCar ) ) < CAR_EXIT_SIGN_SPEED )
        {
            sign_car = CurrentCar;
            sign_region = RegionExitSign;
        }
    }
    else
    {
        float nearest_distance2 = CAR_ENTER_DISTANCE * CAR_ENTER_DISTANCE;

        for( int i = 0; i < MaxCars; i++ )
        {
            if( CarWrecked[ i ] )
              continue;

            float dx = PlayerX - CarX[ i ];
            float dy = PlayerY - CarY[ i ];
            float distance2 = dx * dx + dy * dy;

            if( distance2 < nearest_distance2 )
            {
                sign_car = i;
                nearest_distance2 = distance2;
            }
        }
    }

    if( sign_car < 0 )
      return;

    int screen_x = CarX[ sign_car ];
    int screen_y = CarY[ sign_car ];
    tilemap_convert_position_to_screen( &MapGround, &screen_x, &screen_y );

    select_texture( TextureSprites );
    select_region( sign_region );
    draw_region_at( screen_x, screen_y - CAR_SPRITE_H * MapGround.camera_zoom / 2 - 18 );
}
