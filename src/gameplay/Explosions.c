// *****************************************************************************
    // include Vircon32 headers
    #include "math.h"
    #include "misc.h"
    #include "time.h"
    #include "video.h"

    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
    #include "../../include/gameplay/Player.h"
    #include "../../include/gameplay/Car.h"
    #include "../../include/gameplay/Traffic.h"
    #include "../../include/gameplay/Pedestrians.h"
    #include "../../include/gameplay/Police.h"
    #include "../../include/gameplay/Explosions.h"
// *****************************************************************************


// ---------------------------------------------------------
//   FX VARIABLES
// ---------------------------------------------------------
// fx.c from OceanStorm. The Splash effect was removed: cars don't fall into the sea.
// Jastro: "they could, if we added a sea"
// Carra: "no"

float[ MaxFX ] FxX;
float[ MaxFX ] FxY;
float[ MaxFX ] FxScale;
int[ MaxFX ] FxFrame;
int[ MaxFX ] FxTimer;
int[ MaxFX ] FxActive;

float[ MaxCars ] CarHealth;
bool[ MaxCars ] CarWrecked;
int[ MaxCars ] CarWreckTimer;
int[ MaxCars ] CarExplodeTimer;
bool[ MaxCars ] CarDamagedByPlayer;


// ---------------------------------------------------------
//   FX
// ---------------------------------------------------------

void ResetFX()
{
    for( int i = 0; i < MaxFX; i++ )
      FxActive[ i ] = 0;
}

void SpawnExplosion( float x, float y, float scale )
{
    for( int i = 0; i < MaxFX; i++ )
    {
        if( FxActive[ i ] )
          continue;

        FxX[ i ] = x;
        FxY[ i ] = y;
        FxScale[ i ] = scale;
        FxFrame[ i ] = 0;
        FxTimer[ i ] = FX_FRAME_TIME;
        FxActive[ i ] = 1;
        break;
    }

    // play_sound( SoundExplosion );   // see Weapons.c for the full sound design document
    PlaySfxAt( SoundExplosion, x, y );   // (the document has been updated)
}

// fx_anim_timer was a float counting seconds (0.05 per frame, 1.0/60.0 per tick).
// Jastro: "floats are more precise"
// Carra: "you're counting frames with a float to get back an integer frame number"
// Now it counts frames. With an int. Revolutionary.

void UpdateFX()
{
    for( int i = 0; i < MaxFX; i++ )
    {
        if( !FxActive[ i ] )
          continue;

        FxTimer[ i ]--;

        if( FxTimer[ i ] <= 0 )
        {
            FxFrame[ i ]++;
            FxTimer[ i ] = FX_FRAME_TIME;

            if( FxFrame[ i ] >= ExplosionFrames )
              FxActive[ i ] = 0;
        }
    }
}

void RenderFX()
{
    select_texture( TextureWeapons );

    for( int i = 0; i < MaxFX; i++ )
    {
        if( !FxActive[ i ] || !IsMapPointVisible( FxX[ i ], FxY[ i ], 80 ) )
          continue;

        select_region( RegionFirstExplosion + FxFrame[ i ] );
        DrawSpriteInMap( FxX[ i ], FxY[ i ], 0, FxScale[ i ] );
    }
}


// ---------------------------------------------------------
//   CAR DAMAGE
// ---------------------------------------------------------

void ResetCarHealth( int car )
{
    CarHealth[ car ] = CAR_MAX_HEALTH;
    CarWrecked[ car ] = false;
    CarWreckTimer[ car ] = 0;
    CarExplodeTimer[ car ] = 0;
    CarDamagedByPlayer[ car ] = false;
    CarStolen[ car ] = false;
    CarSirenOn[ car ] = false;
}

void DamageCar( int car, float amount, bool by_player )
{
    if( CarWrecked[ car ] )
      return;

    if( by_player )
      CarDamagedByPlayer[ car ] = true;

    CarHealth[ car ] -= amount;

    if( CarHealth[ car ] <= 0 && CarExplodeTimer[ car ] <= 0 )
      CarExplodeTimer[ car ] = 1;
}

void ExplodeCar( int car )
{
    float x = CarX[ car ];
    float y = CarY[ car ];
    bool by_player = CarDamagedByPlayer[ car ];

    CarWrecked[ car ] = true;
    CarWreckTimer[ car ] = 0;
    CarExplodeTimer[ car ] = 0;
    CarHealth[ car ] = 0;
    CarVelX[ car ] *= 0.3;
    CarVelY[ car ] *= 0.3;

    if( CarIsAI[ car ] && CarMood[ car ] != MoodNormal )
      ResumeTraffic( car );

    CarIsAI[ car ] = false;

    SpawnExplosion( x, y, 2.5 );
    SpawnExplosion( x + CarFwdX[ car ] * 22, y + CarFwdY[ car ] * 22, 1.6 );
    SpawnExplosion( x - CarFwdX[ car ] * 22, y - CarFwdY[ car ] * 22, 1.6 );

    if( by_player )
    {
        if( CarType[ car ] == CarTypePolice ) ReportCrime( 6 );
        else ReportCrime( 2 );
    }

    if( IsPlayerInCar && CurrentCar == car )
      StartPlayerDown( DownWasted );

    float r2 = EXPLOSION_RADIUS * EXPLOSION_RADIUS;

    for( int p = 0; p < MaxPedestrians; p++ )
    {
        float dx = PedX[ p ] - x;
        float dy = PedY[ p ] - y;

        if( dx * dx + dy * dy < r2 )
          KillPedestrian( p, atan2( dy, dx ), by_player );
    }

    KillCopsInRadius( x, y, EXPLOSION_RADIUS, by_player );

    if( !IsPlayerInCar )
    {
        float dx = PlayerX - x;
        float dy = PlayerY - y;
        float d2 = dx * dx + dy * dy;

        if( d2 < r2 )
          PlayerTakeDamage( 25 + 60 * (1 - sqrt( d2 ) / EXPLOSION_RADIUS) );
    }

    for( int i = 0; i < MaxCars; i++ )
    {
        if( i == car || CarWrecked[ i ] )
          continue;

        float dx = CarX[ i ] - x;
        float dy = CarY[ i ] - y;
        float d2 = dx * dx + dy * dy;

        if( d2 >= r2 )
          continue;

        float d = sqrt( d2 );
        CarVelX[ i ] += dx / (d + 1) * 3;
        CarVelY[ i ] += dy / (d + 1) * 3;
        DamageCar( i, 30 + 100 * (1 - d / EXPLOSION_RADIUS), by_player );

        if( CarHealth[ i ] <= 0 )
          CarExplodeTimer[ i ] = EXPLOSION_CHAIN_FRAMES + RandomInt( 10 );
    }

    PanicPedestrians( x, y, PED_PANIC_RADIUS * 1.5 );
}

void RestoreWreck( int car )
{
    if( car >= NumParkedCars )
    {
        if( !RespawnTrafficCar( car ) )
          return;
    }
    else
    {
        float px = ParkingX[ car ];
        float py = ParkingY[ car ];

        if( IsMapPointVisible( px, py, 80 ) || !IsAreaClearOfCars( car, px, py, 90 ) )
          return;

        PlaceCar( car, px / 80.0, py / 80.0, 0, ParkedCarTypes[ car ] );
    }

    ResetCarHealth( car );
}

void ClearCarCrimeCredit()
{
    for( int i = 0; i < MaxCars; i++ )
      CarDamagedByPlayer[ i ] = false;
}

void UpdateCarDamage()
{
    for( int i = 0; i < MaxCars; i++ )
    {
        if( CarWrecked[ i ] )
        {
            CarWreckTimer[ i ]++;

            if( CarWreckTimer[ i ] > WRECK_RESPAWN_FRAMES && !IsMapPointVisible( CarX[ i ], CarY[ i ], 100 ) &&
                !(IsPlayerInCar && CurrentCar == i) )
              RestoreWreck( i );

            continue;
        }

        if( CarExplodeTimer[ i ] > 0 )
        {
            CarExplodeTimer[ i ]--;

            if( CarExplodeTimer[ i ] == 0 )
              ExplodeCar( i );

            continue;
        }

        if( CarHealth[ i ] < CAR_FIRE_HEALTH )
        {
            CarHealth[ i ] -= CAR_BURN_RATE;

            if( CarHealth[ i ] <= 0 )
              ExplodeCar( i );
        }
    }
}

// GTA2 did smoke with particles. We do smoke with ONE sprite drawn several times
// at different offsets. Jastro calls it "a particle system".

void RenderCarDamage()
{
    int frame = get_frame_counter();

    select_texture( TextureWeapons );

    for( int i = 0; i < MaxCars; i++ )
    {
        bool burning_wreck = CarWrecked[ i ] && CarWreckTimer[ i ] < 240;

        if( !burning_wreck && (CarWrecked[ i ] || CarHealth[ i ] >= CAR_SMOKE_HEALTH) )
          continue;

        if( !IsMapPointVisible( CarX[ i ], CarY[ i ], 60 ) )
          continue;

        float hx = CarX[ i ] + CarFwdX[ i ] * 22;
        float hy = CarY[ i ] + CarFwdY[ i ] * 22;
        if( CarWrecked[ i ] ) { hx = CarX[ i ]; hy = CarY[ i ]; }

        int grey = 150;
        if( CarHealth[ i ] < CAR_FIRE_HEALTH ) grey = 70;

        select_region( RegionSmoke );

        for( int k = 0; k < 3; k++ )
        {
            int t = (frame + i * 7 + k * 13) % 40;
            float rise = t * 0.7;
            float scale = 0.8 + t * 0.04;
            int alpha = 200 - t * 5;

            set_multiply_color( make_color_rgba( grey, grey, grey, alpha ) );
            DrawSpriteInMap( hx + (k - 1) * 5 + t * 0.2, hy - rise, 0, scale );
        }

        set_multiply_color( color_white );

        if( CarHealth[ i ] < CAR_FIRE_HEALTH || burning_wreck )
        {
            select_region( RegionFirstExplosion + 1 + (frame / 5 + i) % 3 );
            DrawSpriteInMap( hx, hy, 0, 0.9 );
        }
    }
}
