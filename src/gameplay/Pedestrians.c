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
    #include "../../include/gameplay/Corpses.h"
    #include "../../include/gameplay/Weapons.h"
    #include "../../include/gameplay/Police.h"
// *****************************************************************************

// OceanStorm had enemy soldiers that walked in circles around the islands.
// Jastro: "pedestrians are just soldiers without guns"
// Carra: "and without wanting to kill you"
// Jastro: "most of them"

float[ MaxPedestrians ] PedX;
float[ MaxPedestrians ] PedY;
float[ MaxPedestrians ] PedAngle;
int[ MaxPedestrians ] PedFrame;
int[ MaxPedestrians ] PedAnimTimer;
int[ MaxPedestrians ] PedLot;
int[ MaxPedestrians ] PedSide;
bool[ MaxPedestrians ] PedClockwise;
int[ MaxPedestrians ] PedState;
int[ MaxPedestrians ] PedCross;
int[ MaxPedestrians ] PedIgnoreCross;
float[ MaxPedestrians ] PedTargetX;
float[ MaxPedestrians ] PedTargetY;
int[ MaxPedestrians ] PedTargetLot;
int[ MaxPedestrians ] PedTargetSide;
int[ MaxPedestrians ] PedType;
int[ MaxPedestrians ] PedTint;
int[ MaxPedestrians ] PedTimer;
float[ MaxPedestrians ] PedVelX;
float[ MaxPedestrians ] PedVelY;
float[ MaxPedestrians ] PedSpin;
int[ MaxPedestrians ] PedPanic;
bool[ MaxPedestrians ] PedHitByPlayer;


float CornerX( int lot, int corner )
{
    if( corner == 1 || corner == 2 ) return PedLotX1[ lot ];
    return PedLotX0[ lot ];
}

float CornerY( int lot, int corner )
{
    if( corner >= 2 ) return PedLotY1[ lot ];
    return PedLotY0[ lot ];
}

void SetPedTargetCorner( int ped )
{
    int corner = PedSide[ ped ];
    if( PedClockwise[ ped ] ) corner = (PedSide[ ped ] + 1) % 4;

    PedTargetX[ ped ] = CornerX( PedLot[ ped ], corner );
    PedTargetY[ ped ] = CornerY( PedLot[ ped ], corner );
}

void PlacePedestrian( int ped )
{
    int lot = RandomInt( NumPedLots );
    int side = RandomInt( 4 );
    float t = (RandomInt( 100 ) + 1) / 101.0;
    int c0 = side;
    int c1 = (side + 1) % 4;

    PedLot[ ped ] = lot;
    PedSide[ ped ] = side;
    PedClockwise[ ped ] = RandomInt( 2 ) == 0;
    PedX[ ped ] = CornerX( lot, c0 ) + (CornerX( lot, c1 ) - CornerX( lot, c0 )) * t;
    PedY[ ped ] = CornerY( lot, c0 ) + (CornerY( lot, c1 ) - CornerY( lot, c0 )) * t;
    PedState[ ped ] = PedStateWalk;
    PedIgnoreCross[ ped ] = -1;
    PedFrame[ ped ] = 0;
    PedPanic[ ped ] = 0;
    PedHitByPlayer[ ped ] = false;
    PedAnimTimer[ ped ] = RandomInt( PED_ANIM_SPEED );
    SetPedTargetCorner( ped );
}

void InitializePedestrians()
{
    for( int p = 0; p < MaxPedestrians; p++ )
    {
        PlacePedestrian( p );

        int kind = RandomInt( 4 );
        PedType[ p ] = PedCivil1;
        if( kind == 1 ) PedType[ p ] = PedCivil2;
        if( kind == 2 ) PedType[ p ] = PedCivil3;
        if( kind == 3 ) PedType[ p ] = PedCivil4;

        int tint = RandomInt( 4 );
        PedTint[ p ] = color_white;
        if( tint == 1 ) PedTint[ p ] = make_color_rgb( 255, 215, 215 );
        if( tint == 2 ) PedTint[ p ] = make_color_rgb( 215, 230, 255 );
        if( tint == 3 ) PedTint[ p ] = make_color_rgb( 230, 255, 215 );
    }

    PedType[ 0 ] = PedJastro;
    PedTint[ 0 ] = color_white;
    PedType[ 1 ] = PedCarra;
    PedTint[ 1 ] = color_white;
}

// ---------------------------------------------------------

bool IsCrosswalkSafe( int c )
{
    float x0 = CrosswalkX0[ c ] - 6;
    float y0 = CrosswalkY0[ c ] - 6;
    float x1 = CrosswalkX1[ c ] + 6;
    float y1 = CrosswalkY1[ c ] + 6;
    float cx = (x0 + x1) / 2;
    float cy = (y0 + y1) / 2;

    for( int i = 0; i < MaxCars; i++ )
    {
        float px = fmax( x0, fmin( x1, CarX[ i ] ) );
        float py = fmax( y0, fmin( y1, CarY[ i ] ) );
        float dx = CarX[ i ] - px;
        float dy = CarY[ i ] - py;
        float d2 = dx * dx + dy * dy;

        if( d2 < 55 * 55 )
          return false;

        float speed = fabs( CarSpeed[ i ] );

        if( speed > 0.4 && d2 < 200 * 200 )
        {
            float toward = (cx - CarX[ i ]) * CarFwdX[ i ] + (cy - CarY[ i ]) * CarFwdY[ i ];

            if( toward > 0 )
              return false;
        }
    }

    return true;
}

void TryStartCrossing( int ped )
{
    for( int c = 0; c < NumCrosswalks; c++ )
    {
        if( c == PedIgnoreCross[ ped ] )
          continue;

        float ex, ey, ox, oy;
        int other_lot, other_side;

        if( CrosswalkLotA[ c ] == PedLot[ ped ] && CrosswalkSideA[ c ] == PedSide[ ped ] )
        {
            ex = CrosswalkAX[ c ]; ey = CrosswalkAY[ c ];
            ox = CrosswalkBX[ c ]; oy = CrosswalkBY[ c ];
            other_lot = CrosswalkLotB[ c ]; other_side = CrosswalkSideB[ c ];
        }
        else if( CrosswalkLotB[ c ] == PedLot[ ped ] && CrosswalkSideB[ c ] == PedSide[ ped ] )
        {
            ex = CrosswalkBX[ c ]; ey = CrosswalkBY[ c ];
            ox = CrosswalkAX[ c ]; oy = CrosswalkAY[ c ];
            other_lot = CrosswalkLotA[ c ]; other_side = CrosswalkSideA[ c ];
        }
        else
          continue;

        float dx = PedX[ ped ] - ex;
        float dy = PedY[ ped ] - ey;

        if( dx * dx + dy * dy > 1.5 * 1.5 )
          continue;

        PedIgnoreCross[ ped ] = c;

        if( RandomInt( 100 ) >= 45 )
          return;

        PedX[ ped ] = ex;
        PedY[ ped ] = ey;
        PedCross[ ped ] = c;
        PedTargetX[ ped ] = ox;
        PedTargetY[ ped ] = oy;
        PedTargetLot[ ped ] = other_lot;
        PedTargetSide[ ped ] = other_side;
        PedState[ ped ] = PedStateWaitCross;
        PedAngle[ ped ] = atan2( oy - ey, ox - ex );
        return;
    }
}

void CheckPedestrianHit( int ped )
{
    for( int i = 0; i < MaxCars; i++ )
    {
        if( fabs( CarSpeed[ i ] ) < PED_KNOCK_SPEED ) continue;
        if( fabs( PedX[ ped ] - CarX[ i ] ) > 60 || fabs( PedY[ ped ] - CarY[ i ] ) > 60 ) continue;

        float fx = CarFwdX[ i ] * CAR_CIRCLE_SPACING;
        float fy = CarFwdY[ i ] * CAR_CIRCLE_SPACING;

        for( int k = -1; k <= 1; k++ )
        {
            float dx = PedX[ ped ] - (CarX[ i ] + k * fx);
            float dy = PedY[ ped ] - (CarY[ i ] + k * fy);

            if( dx * dx + dy * dy < (CAR_CIRCLE_RADIUS + 9) * (CAR_CIRCLE_RADIUS + 9) )
            {
                PedState[ ped ] = PedStateKnocked;
                PlaySfxAt( SoundThud, PedX[ ped ], PedY[ ped ] );
                PedTimer[ ped ] = PED_KNOCK_FRAMES;
                PedHitByPlayer[ ped ] = IsPlayerInCar && i == CurrentCar;
                PedVelX[ ped ] = CarVelX[ i ] * 1.4 + dy * 0.05;
                PedVelY[ ped ] = CarVelY[ i ] * 1.4 - dx * 0.05;
                PedSpin[ ped ] = 0.35;
                if( RandomInt( 2 ) == 0 ) PedSpin[ ped ] = -0.35;
                return;
            }
        }
    }
}

void PanicPedestrians( float x, float y, float radius )
{
    for( int p = 0; p < MaxPedestrians; p++ )
    {
        if( PedState[ p ] == PedStateKnocked ) continue;

        float dx = PedX[ p ] - x;
        float dy = PedY[ p ] - y;

        if( dx * dx + dy * dy > radius * radius ) continue;

        PedPanic[ p ] = PED_PANIC_FRAMES;

        if( PedState[ p ] == PedStateWalk && RandomInt( 2 ) == 0 )
        {
            PedClockwise[ p ] = !PedClockwise[ p ];
            SetPedTargetCorner( p );
        }
    }
}

void ClearPedestrianCrimeCredit()
{
    for( int p = 0; p < MaxPedestrians; p++ )
      PedHitByPlayer[ p ] = false;
}

void KillPedestrian( int ped, float angle, bool by_player )
{
    float x = PedX[ ped ];
    float y = PedY[ ped ];

    SpawnCorpse( x, y, angle + pi / 2, PedType[ ped ], PedTint[ ped ] );

    if( by_player )
    {
        ReportCrime( 2 );

        if( RandomInt( 100 ) < 65 )
          SpawnCash( x + 10, y + 6, 5 + RandomInt( 6 ) * 5 );
    }

    PlacePedestrian( ped );
    PanicPedestrians( x, y, PED_PANIC_RADIUS );
}

void MovePedTowardTarget( int ped, float speed )
{
    float dx = PedTargetX[ ped ] - PedX[ ped ];
    float dy = PedTargetY[ ped ] - PedY[ ped ];
    float distance = sqrt( dx * dx + dy * dy );

    if( distance > 0.01 )
    {
        float step = fmin( speed, distance );
        PedX[ ped ] += dx / distance * step;
        PedY[ ped ] += dy / distance * step;
        PedAngle[ ped ] = atan2( dy, dx );
    }

    PedAnimTimer[ ped ]++;

    if( PedAnimTimer[ ped ] >= PED_ANIM_SPEED + 2 )
    {
        PedAnimTimer[ ped ] = 0;
        PedFrame[ ped ] = (PedFrame[ ped ] + 1) % FramesPerPed;
    }
}

void UpdatePedestrians()
{
    for( int p = 0; p < MaxPedestrians; p++ )
    {
        int state = PedState[ p ];

        if( PedPanic[ p ] > 0 )
          PedPanic[ p ]--;

        if( state == PedStateKnocked )
        {
            PedX[ p ] += PedVelX[ p ];
            PedY[ p ] += PedVelY[ p ];
            PedVelX[ p ] *= 0.93;
            PedVelY[ p ] *= 0.93;
            PedAngle[ p ] += PedSpin[ p ];
            PedSpin[ p ] *= 0.96;
            PedTimer[ p ]--;

            if( PedTimer[ p ] <= 0 )
              KillPedestrian( p, PedAngle[ p ], PedHitByPlayer[ p ] );

            continue;
        }

        CheckPedestrianHit( p );
        if( PedState[ p ] == PedStateKnocked )
          continue;

        if( state == PedStateWaitCross )
        {
            PedFrame[ p ] = 0;

            if( PedPanic[ p ] > 0 )
            {
                PedState[ p ] = PedStateCrossing;
                continue;
            }

            if( (get_frame_counter() + p) % 8 == 0 && IsCrosswalkSafe( PedCross[ p ] ) )
              PedState[ p ] = PedStateCrossing;

            continue;
        }

        if( state == PedStateCrossing )
        {
            if( PedPanic[ p ] > 0 ) MovePedTowardTarget( p, PED_CROSS_SPEED * 2.6 );
            else MovePedTowardTarget( p, PED_CROSS_SPEED );

            float dx = PedTargetX[ p ] - PedX[ p ];
            float dy = PedTargetY[ p ] - PedY[ p ];

            if( dx * dx + dy * dy < 1 )
            {
                PedLot[ p ] = PedTargetLot[ p ];
                PedSide[ p ] = PedTargetSide[ p ];
                PedClockwise[ p ] = RandomInt( 2 ) == 0;
                PedIgnoreCross[ p ] = PedCross[ p ];
                PedState[ p ] = PedStateWalk;
                SetPedTargetCorner( p );
            }

            continue;
        }

        if( PedPanic[ p ] > 0 ) MovePedTowardTarget( p, PED_WALK_SPEED * 3.2 );
        else MovePedTowardTarget( p, PED_WALK_SPEED );

        float dx = PedTargetX[ p ] - PedX[ p ];
        float dy = PedTargetY[ p ] - PedY[ p ];

        if( dx * dx + dy * dy < 1 )
        {
            if( PedClockwise[ p ] )
              PedSide[ p ] = (PedSide[ p ] + 1) % 4;
            else
              PedSide[ p ] = (PedSide[ p ] + 3) % 4;

            PedIgnoreCross[ p ] = -1;
            SetPedTargetCorner( p );
        }
        else
          TryStartCrossing( p );
    }
}

// ---------------------------------------------------------

void RenderPedestrians()
{
    select_texture( TextureSprites );

    for( int p = 0; p < MaxPedestrians; p++ )
    {
        if( !IsMapPointVisible( PedX[ p ], PedY[ p ], 30 ) )
          continue;

        select_region( RegionFirstPed + PedType[ p ] * FramesPerPed + PedFrame[ p ] );
        float draw_angle = PedAngle[ p ] + pi / 2;

        set_multiply_color( make_color_rgba( 0, 0, 0, 100 ) );
        DrawSpriteInMap( PedX[ p ] + 2, PedY[ p ] + 2, draw_angle, 1.0 );

        set_multiply_color( PedTint[ p ] );
        DrawSpriteInMap( PedX[ p ], PedY[ p ], draw_angle, 1.0 );
    }

    set_multiply_color( color_white );
}
