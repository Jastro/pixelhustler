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
    #include "../../include/gameplay/Weapons.h"
    #include "../../include/gameplay/Corpses.h"
    #include "../../include/gameplay/Pedestrians.h"
    #include "../../include/gameplay/Explosions.h"
    #include "../../include/gameplay/Police.h"
// *****************************************************************************

int WantedLevel = 0;
int WantedHeat = 0;
int WantedCooldown = 0;
int PoliceRecruitTimer = 0;
int WantedFlashTimer = 0;

int[ MaxCars ] CarCopsOut;
int[ MaxCars ] CarCrew;
float[ MaxCops ] CopX;
float[ MaxCops ] CopY;
float[ MaxCops ] CopAngle;
int[ MaxCops ] CopCar;
int[ MaxCops ] CopState;
int[ MaxCops ] CopHealth;
int[ MaxCops ] CopFireTimer;
int[ MaxCops ] CopFrame;
int[ MaxCops ] CopAnimTimer;
bool[ MaxCops ] CopSeesPlayer;
bool[ MaxCops ] CopActive;
int ArrestTimer = 0;


int CrimeGraceTimer = 0;

int WantedLevelForHeat( int heat )
{
    if( heat >= 50 ) return 6;
    if( heat >= 36 ) return 5;
    if( heat >= 24 ) return 4;
    if( heat >= 14 ) return 3;
    if( heat >= 6 ) return 2;
    if( heat >= 1 ) return 1;
    return 0;
}

int WantedHeatForLevel( int level )
{
    if( level >= 6 ) return 50;
    if( level == 5 ) return 36;
    if( level == 4 ) return 24;
    if( level == 3 ) return 14;
    if( level == 2 ) return 6;
    if( level == 1 ) return 1;
    return 0;
}

void ResetWanted()
{
    WantedLevel = 0;
    WantedHeat = 0;
    WantedCooldown = 0;

    for( int i = 0; i < MaxCars; i++ )
      if( CarIsAI[ i ] && CarMood[ i ] == MoodPolice )
        ResumeTraffic( i );

    for( int c = 0; c < MaxCops; c++ )
      CopState[ c ] = CopStateReturn;
}

void ReportCrime( int points )
{
    if( PlayerDownTimer > 0 || CrimeGraceTimer > 0 )
      return;

    int old_level = WantedLevel;
    WantedHeat += points;
    if( WantedHeat > 60 ) WantedHeat = 60;
    WantedLevel = WantedLevelForHeat( WantedHeat );
    WantedCooldown = 0;

    if( WantedLevel > old_level )
    {
        WantedFlashTimer = 120;
        PlaySfx( SoundWanted );
    }
}

int CountPolice()
{
    int count = 0;

    for( int i = 0; i < MaxCars; i++ )
      if( CarIsAI[ i ] && CarMood[ i ] == MoodPolice )
        count++;

    return count;
}

void RecruitPolice()
{
    int best = -1;
    float best_distance = 0;

    for( int i = 0; i < MaxCars; i++ )
    {
        if( !CarIsAI[ i ] || CarMood[ i ] == MoodPolice )
          continue;

        if( IsMapPointVisible( CarX[ i ], CarY[ i ], 80 ) && CarType[ i ] != CarTypePolice )
          continue;

        if( CarX[ i ] < 0 || CarY[ i ] < 0 || CarX[ i ] > MapTilesX * TILE_SIZE || CarY[ i ] > MapTilesY * TILE_SIZE )
          continue;

        float dx = CarX[ i ] - PlayerX;
        float dy = CarY[ i ] - PlayerY;
        float distance = dx * dx + dy * dy;

        if( CarType[ i ] == CarTypePolice )
          distance *= 0.25;

        if( best < 0 || distance < best_distance )
        {
            best = i;
            best_distance = distance;
        }
    }

    if( best < 0 )
      return;

    CarType[ best ] = CarTypePolice;
    CarCrew[ best ] = POLICE_CREW;
    StartCarMood( best, MoodPolice );
}

void StartCrimeGrace()
{
    CrimeGraceTimer = CRIME_GRACE_FRAMES;
}

void UpdatePolice()
{
    if( CrimeGraceTimer > 0 )
      CrimeGraceTimer--;

    if( WantedFlashTimer > 0 )
      WantedFlashTimer--;

    if( WantedLevel == 0 )
    {
        if( CountPolice() > 0 )
          ResetWanted();

        return;
    }

    bool seen = false;
    float player_speed = 0;

    if( IsPlayerInCar )
      player_speed = fabs( CarSpeed[ CurrentCar ] );

    for( int i = 0; i < MaxCars; i++ )
    {
        if( !CarIsAI[ i ] || CarMood[ i ] != MoodPolice )
          continue;

        float dx = CarX[ i ] - PlayerX;
        float dy = CarY[ i ] - PlayerY;
        float d2 = dx * dx + dy * dy;

        if( d2 < POLICE_SIGHT * POLICE_SIGHT )
          seen = true;

        if( (WantedLevel < WANTED_RAM_LEVEL || CarHasRammed[ i ]) && CarCopsOut[ i ] == 0 && CarCrew[ i ] > 0 && d2 < COP_DEPLOY_DISTANCE * COP_DEPLOY_DISTANCE &&
            fabs( CarSpeed[ i ] ) < 1.0 && player_speed < 0.6 )
          DeployCops( i );
    }

    if( seen )
      WantedCooldown = 0;
    else
      WantedCooldown++;

    int decay_frames = WANTED_DECAY_FRAMES;
    if( WantedLevel == 1 ) decay_frames = WANTED1_DECAY_FRAMES;

    if( WantedCooldown > decay_frames )
    {
        WantedCooldown = 0;
        WantedLevel--;
        WantedHeat = WantedHeatForLevel( WantedLevel );

        if( WantedLevel == 0 )
        {
            ResetWanted();
            return;
        }
    }

    if( PlayerDownTimer > 0 )
      return;

    PoliceRecruitTimer++;

    if( PoliceRecruitTimer >= 45 )
    {
        PoliceRecruitTimer = 0;
        int wanted_units = WantedLevel;
        if( wanted_units > MaxPolice ) wanted_units = MaxPolice;

        if( CountPolice() < wanted_units )
          RecruitPolice();
    }
}

int FindFreeCop()
{
    for( int c = 0; c < MaxCops; c++ )
      if( !CopActive[ c ] )
        return c;

    return -1;
}

void DeployCops( int car )
{
    CarHasRammed[ car ] = false;

    int count = 1;
    if( WantedLevel >= 2 ) count = 2;
    if( count > CarCrew[ car ] ) count = CarCrew[ car ];

    float sx = cos( CarAngle[ car ] ) * 30;
    float sy = sin( CarAngle[ car ] ) * 30;

    for( int k = 0; k < count; k++ )
    {
        int c = FindFreeCop();
        if( c < 0 ) return;

        float side = 1;
        if( k == 1 ) side = -1;

        float x = CarX[ car ] + sx * side;
        float y = CarY[ car ] + sy * side;

        if( IsSolidAt( x, y ) )
        {
            x = CarX[ car ] - sx * side;
            y = CarY[ car ] - sy * side;
            if( IsSolidAt( x, y ) ) continue;
        }

        CopX[ c ] = x;
        CopY[ c ] = y;
        CopAngle[ c ] = CarAngle[ car ] - pi / 2;
        CopCar[ c ] = car;
        CopState[ c ] = CopStateChase;
        CopHealth[ c ] = COP_HEALTH;
        CopFireTimer[ c ] = 40 + RandomInt( 40 );
        CopFrame[ c ] = 0;
        CopAnimTimer[ c ] = 0;
        CopSeesPlayer[ c ] = false;
        CopActive[ c ] = true;
        CarCopsOut[ car ]++;
    }
}

void ResetCops()
{
    for( int c = 0; c < MaxCops; c++ )
      CopActive[ c ] = false;

    for( int i = 0; i < MaxCars; i++ )
      CarCopsOut[ i ] = 0;

    ArrestTimer = 0;
}

void RemoveCop( int c )
{
    CopActive[ c ] = false;
    int car = CopCar[ c ];

    if( car >= 0 && CarCopsOut[ car ] > 0 )
      CarCopsOut[ car ]--;
}

void AbandonPoliceCar( int car )
{
    CarMood[ car ] = MoodNormal;
    CarIsAI[ car ] = false;
    CarCopsOut[ car ] = 0;
    CarHasRammed[ car ] = false;
}

void KillCop( int c, float angle, bool by_player )
{
    SpawnCorpse( CopX[ c ], CopY[ c ], angle + pi / 2, PedCop, color_white );

    if( by_player )
    {
        ReportCrime( 5 );
        SpawnCash( CopX[ c ] + 8, CopY[ c ] + 6, 20 );
    }

    PanicPedestrians( CopX[ c ], CopY[ c ], PED_PANIC_RADIUS );
    RemoveCop( c );

    int car = CopCar[ c ];

    if( car < 0 || CarCrew[ car ] <= 0 )
      return;

    CarCrew[ car ]--;

    if( CarCrew[ car ] == 0 && CarIsAI[ car ] && !CarWrecked[ car ] )
      AbandonPoliceCar( car );
}

bool ShootCops( float x, float y, float damage, float angle )
{
    for( int c = 0; c < MaxCops; c++ )
    {
        if( !CopActive[ c ] ) continue;

        float dx = x - CopX[ c ];
        float dy = y - CopY[ c ];

        if( dx * dx + dy * dy > 12 * 12 ) continue;

        CopHealth[ c ] -= damage;
        ReportCrime( 1 );

        if( CopHealth[ c ] <= 0 )
          KillCop( c, angle, true );

        return true;
    }

    return false;
}

void KillCopsInRadius( float x, float y, float radius, bool by_player )
{
    for( int c = 0; c < MaxCops; c++ )
    {
        if( !CopActive[ c ] ) continue;

        float dx = CopX[ c ] - x;
        float dy = CopY[ c ] - y;

        if( dx * dx + dy * dy < radius * radius )
          KillCop( c, atan2( dy, dx ), by_player );
    }
}

bool HasLineOfSight( float x0, float y0, float x1, float y1 )
{
    float dx = x1 - x0;
    float dy = y1 - y0;
    float distance = sqrt( dx * dx + dy * dy );
    int steps = distance / 20;

    for( int k = 1; k < steps; k++ )
    {
        float t = k / (float)steps;
        if( IsBuildingAt( x0 + dx * t, y0 + dy * t ) )
          return false;
    }

    return true;
}

void MoveCop( int c, float tx, float ty, float speed )
{
    float dx = tx - CopX[ c ];
    float dy = ty - CopY[ c ];
    float distance = sqrt( dx * dx + dy * dy );

    if( distance < 1 )
      return;

    float mx = dx / distance * speed;
    float my = dy / distance * speed;

    if( !IsSolidAt( CopX[ c ] + mx, CopY[ c ] ) ) CopX[ c ] += mx;
    if( !IsSolidAt( CopX[ c ], CopY[ c ] + my ) ) CopY[ c ] += my;

    CopAngle[ c ] = atan2( dy, dx );
    CopAnimTimer[ c ]++;

    if( CopAnimTimer[ c ] >= PED_ANIM_SPEED )
    {
        CopAnimTimer[ c ] = 0;
        CopFrame[ c ] = (CopFrame[ c ] + 1) % FramesPerPed;
    }
}

bool CopHitByCar( int c )
{
    for( int i = 0; i < MaxCars; i++ )
    {
        if( fabs( CarSpeed[ i ] ) < PED_KNOCK_SPEED ) continue;
        if( fabs( CopX[ c ] - CarX[ i ] ) > 60 || fabs( CopY[ c ] - CarY[ i ] ) > 60 ) continue;

        for( int k = -1; k <= 1; k++ )
        {
            float dx = CopX[ c ] - (CarX[ i ] + k * CarFwdX[ i ] * CAR_CIRCLE_SPACING);
            float dy = CopY[ c ] - (CarY[ i ] + k * CarFwdY[ i ] * CAR_CIRCLE_SPACING);

            if( dx * dx + dy * dy < (CAR_CIRCLE_RADIUS + 9) * (CAR_CIRCLE_RADIUS + 9) )
            {
                PlaySfxAt( SoundThud, CopX[ c ], CopY[ c ] );
                KillCop( c, CarAngle[ i ], IsPlayerInCar && i == CurrentCar );
                return true;
            }
        }
    }

    return false;
}

void PullPlayerOut( int c )
{
    ExitCar();

    if( IsPlayerInCar )
    {
        IsPlayerInCar = false;
        CurrentCar = -1;
        PlayerX = CopX[ c ];
        PlayerY = CopY[ c ];
        TargetFloorZ = FLOOR_Z_ON_FOOT;
    }

    PlayerAngle = atan2( PlayerY - CopY[ c ], PlayerX - CopX[ c ] );
    PlayerStunTimer = PLAYER_STUN_FRAMES;
    ArrestTimer = 0;
    PlaySfx( SoundThud );
}

void UpdateCops()
{
    bool arresting = false;
    float player_speed = 0;

    if( IsPlayerInCar )
      player_speed = fabs( CarSpeed[ CurrentCar ] );

    for( int c = 0; c < MaxCops; c++ )
    {
        if( !CopActive[ c ] )
          continue;

        if( CopHitByCar( c ) )
          continue;

        int car = CopCar[ c ];
        bool car_ok = car >= 0 && CarIsAI[ car ] && CarMood[ car ] == MoodPolice && !CarWrecked[ car ];
        float dx = PlayerX - CopX[ c ];
        float dy = PlayerY - CopY[ c ];
        float distance = sqrt( dx * dx + dy * dy );

        if( CopState[ c ] == CopStateChase )
        {
            if( WantedLevel == 0 || PlayerDownTimer > 0 || distance > 450 || (IsPlayerInCar && player_speed > 1.0 && distance > 200) )
              CopState[ c ] = CopStateReturn;
        }

        if( CopState[ c ] == CopStateReturn )
        {
            if( !car_ok )
            {
                if( !IsMapPointVisible( CopX[ c ], CopY[ c ], 40 ) ) RemoveCop( c );
                else MoveCop( c, CopX[ c ] + cos( CopAngle[ c ] ) * 10, CopY[ c ] + sin( CopAngle[ c ] ) * 10, COP_SPEED );
                continue;
            }

            float cx = CarX[ car ] - CopX[ c ];
            float cy = CarY[ car ] - CopY[ c ];

            if( cx * cx + cy * cy < 30 * 30 || !IsMapPointVisible( CopX[ c ], CopY[ c ], 60 ) )
              RemoveCop( c );
            else
              MoveCop( c, CarX[ car ], CarY[ car ], COP_SPEED * 1.4 );

            if( CopState[ c ] == CopStateReturn && WantedLevel > 0 && PlayerDownTimer <= 0 && !IsPlayerInCar && distance < 200 )
              CopState[ c ] = CopStateChase;

            continue;
        }

        if( (get_frame_counter() + c) % 8 == 0 )
          CopSeesPlayer[ c ] = HasLineOfSight( CopX[ c ], CopY[ c ], PlayerX, PlayerY );

        bool shooter = WantedLevel >= 2 && !IsPlayerInCar;

        if( IsPlayerInCar )
        {
            MoveCop( c, PlayerX, PlayerY, COP_SPEED * 1.2 );

            if( distance < COP_PULL_DISTANCE && player_speed < 1.0 )
              PullPlayerOut( c );

            continue;
        }

        if( shooter && CopSeesPlayer[ c ] && distance < 110 && distance > 40 )
        {
            CopAngle[ c ] = atan2( dy, dx );
            CopFrame[ c ] = 0;
        }
        else if( distance > COP_ARREST_DISTANCE - 6 )
          MoveCop( c, PlayerX, PlayerY, COP_SPEED );

        if( !IsPlayerInCar && PlayerStunTimer <= 0 && distance < COP_ARREST_DISTANCE )
          arresting = true;

        CopFireTimer[ c ]--;

        if( shooter && CopFireTimer[ c ] <= 0 && CopSeesPlayer[ c ] && distance < COP_SHOOT_RANGE && distance > 30 )
        {
            float angle = atan2( dy, dx );
            CreateCopBullet( CopX[ c ] + cos( angle ) * 10, CopY[ c ] + sin( angle ) * 10, angle );
            CopFireTimer[ c ] = 50 + RandomInt( 40 );
        }
    }

    if( arresting && PlayerDownTimer <= 0 )
      ArrestTimer++;
    else
      ArrestTimer = 0;

    if( ArrestTimer > COP_ARREST_FRAMES )
    {
        ArrestTimer = 0;
        StartPlayerDown( DownBusted );
    }
}

void RenderCops()
{
    select_texture( TextureSprites );

    for( int c = 0; c < MaxCops; c++ )
    {
        if( !CopActive[ c ] || !IsMapPointVisible( CopX[ c ], CopY[ c ], 30 ) )
          continue;

        select_region( RegionFirstPed + PedCop * FramesPerPed + CopFrame[ c ] );
        float draw_angle = CopAngle[ c ] + pi / 2;

        set_multiply_color( make_color_rgba( 0, 0, 0, 100 ) );
        DrawSpriteInMap( CopX[ c ] + 2, CopY[ c ] + 2, draw_angle, 1.0 );
        set_multiply_color( color_white );
        DrawSpriteInMap( CopX[ c ], CopY[ c ], draw_angle, 1.0 );
    }
}

void RenderSirens()
{
    bool phase = (get_frame_counter() / 8) % 2 == 0;

    select_texture( TextureSprites );
    select_region( RegionLampGlow );
    set_blending_mode( blending_add );

    for( int i = 0; i < MaxCars; i++ )
    {
        bool siren = (CarIsAI[ i ] && CarMood[ i ] == MoodPolice) || CarSirenOn[ i ];

        if( !siren || !IsMapPointVisible( CarX[ i ], CarY[ i ], 60 ) )
          continue;

        float sx = cos( CarAngle[ i ] ) * 9;
        float sy = sin( CarAngle[ i ] ) * 9;

        set_multiply_color( make_color_rgb( 255, 30, 50 ) );
        if( !phase ) set_multiply_color( make_color_rgb( 60, 60, 120 ) );
        DrawSpriteInMap( CarX[ i ] - sx, CarY[ i ] - sy, 0, 1.3 );

        set_multiply_color( make_color_rgb( 40, 90, 255 ) );
        if( phase ) set_multiply_color( make_color_rgb( 120, 60, 60 ) );
        DrawSpriteInMap( CarX[ i ] + sx, CarY[ i ] + sy, 0, 1.3 );
    }

    set_multiply_color( color_white );
    set_blending_mode( blending_alpha );
}

void RenderWantedHud()
{
    select_texture( TextureWeapons );
    select_region( RegionBadge );

    bool flash = WantedFlashTimer > 0 && (WantedFlashTimer / 6) % 2 == 0;

    for( int i = 0; i < MaxWanted; i++ )
    {
        int x = screen_width - 26 - i * 22;

        if( i < WantedLevel )
        {
            if( flash || (WantedCooldown > 0 && (get_frame_counter() / 20) % 2 == 0) )
              set_multiply_color( make_color_rgb( 255, 255, 255 ) );
            else if( (get_frame_counter() / 10 + i) % 2 == 0 )
              set_multiply_color( make_color_rgb( 255, 50, 70 ) );
            else
              set_multiply_color( make_color_rgb( 60, 110, 255 ) );
        }
        else
          set_multiply_color( make_color_rgba( 80, 80, 100, 120 ) );

        draw_region_at( x, 8 );
    }

    set_multiply_color( color_white );
}
