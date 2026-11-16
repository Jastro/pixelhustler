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

// Carra: "the cars need an AI"
// Jastro: "just use the heli AI from OceanStorm"
// Carra: "the enemy helis flew in a straight line towards you and exploded"
// Jastro: "so... GTA traffic"

float TrafficThrottle = 0;
float TrafficSteer = 0;

int[ MaxCars ] CarDir;
int[ MaxCars ] CarRoad;
int[ MaxCars ] CarInterV;
int[ MaxCars ] CarInterH;
bool[ MaxCars ] CarInterPassed;
bool[ MaxCars ] CarExiting;
float[ MaxCars ][ 2 ] CarWpX;
float[ MaxCars ][ 2 ] CarWpY;
int[ MaxCars ] CarWpCount;
int[ MaxCars ] CarWpIndex;
int[ MaxCars ] CarNextDir;
int[ MaxCars ] CarNextRoad;
int[ MaxCars ] CarStuckTimer;
int[ MaxCars ] CarGhostTimer;
float[ MaxCars ] CarCachedGap;
int[ MaxCars ] CarMood;
int[ MaxCars ] CarMoodTimer;
int[ MaxCars ] CarUnstuckTimer;
float[ MaxCars ] CarUnstuckSteer;
int[ MaxCars ] CarBackoffTimer;
bool[ MaxCars ] CarHasRammed;


int RandomInt( int n )
{
    return abs( rand() ) % n;
}

float LaneX( int v, int dir )
{
    if( dir == DirSouth )
      return VRoadX[ v ] * TILE_SIZE;

    return (VRoadX[ v ] + VRoadWidth[ v ]) * TILE_SIZE;
}

float LaneY( int h, int dir )
{
    if( dir == DirWest )
      return HRoadY[ h ] * TILE_SIZE;

    return (HRoadY[ h ] + HRoadHeight[ h ]) * TILE_SIZE;
}

float InterX0( int v ) { return (VRoadX[ v ] - 0.5) * TILE_SIZE; }
float InterX1( int v ) { return (VRoadX[ v ] + VRoadWidth[ v ] + 0.5) * TILE_SIZE; }
float InterY0( int h ) { return (HRoadY[ h ] - 0.5) * TILE_SIZE; }
float InterY1( int h ) { return (HRoadY[ h ] + HRoadHeight[ h ] + 0.5) * TILE_SIZE; }

float DirAngle( int dir )
{
    return dir * pi / 2;
}

// ---------------------------------------------------------

void PlanRoute( int car )
{
    int dir = CarDir[ car ];
    float x = CarX[ car ];
    float y = CarY[ car ];

    CarWpIndex[ car ] = 0;
    CarInterPassed[ car ] = false;
    CarExiting[ car ] = false;

    if( dir == DirNorth || dir == DirSouth )
    {
        int v = CarRoad[ car ];
        float lane_x = LaneX( v, dir );
        int h = -1;

        for( int i = 0; i < NumHRoads; i++ )
        {
            if( dir == DirSouth && InterY0( i ) > y && (h < 0 || InterY0( i ) < InterY0( h )) ) h = i;
            if( dir == DirNorth && InterY1( i ) < y && (h < 0 || InterY1( i ) > InterY1( h )) ) h = i;
        }

        if( h < 0 )
        {
            CarExiting[ car ] = true;
            CarInterV[ car ] = -1;
            CarWpX[ car ][ 0 ] = lane_x;
            CarWpY[ car ][ 0 ] = (MapTilesY + 2) * TILE_SIZE;
            if( dir == DirNorth ) CarWpY[ car ][ 0 ] = -2 * TILE_SIZE;
            CarWpCount[ car ] = 1;
            return;
        }

        CarInterV[ car ] = v;
        CarInterH[ car ] = h;
        int choice = RandomInt( 4 );

        if( choice < 2 )
        {
            CarWpX[ car ][ 0 ] = lane_x;
            CarWpY[ car ][ 0 ] = InterY1( h ) + 40;
            if( dir == DirNorth ) CarWpY[ car ][ 0 ] = InterY0( h ) - 40;
            CarWpCount[ car ] = 1;
            CarNextDir[ car ] = dir;
            CarNextRoad[ car ] = v;
        }
        else
        {
            int new_dir = DirEast;
            if( choice == 3 ) new_dir = DirWest;
            float lane_y = LaneY( h, new_dir );

            CarWpX[ car ][ 0 ] = lane_x;
            CarWpY[ car ][ 0 ] = lane_y;
            CarWpX[ car ][ 1 ] = InterX1( v ) + 40;
            if( new_dir == DirWest ) CarWpX[ car ][ 1 ] = InterX0( v ) - 40;
            CarWpY[ car ][ 1 ] = lane_y;
            CarWpCount[ car ] = 2;
            CarNextDir[ car ] = new_dir;
            CarNextRoad[ car ] = h;
        }
    }
    else
    {
        int h = CarRoad[ car ];
        float lane_y = LaneY( h, dir );
        int v = -1;

        for( int i = 0; i < NumVRoads; i++ )
        {
            if( dir == DirEast && InterX0( i ) > x && (v < 0 || InterX0( i ) < InterX0( v )) ) v = i;
            if( dir == DirWest && InterX1( i ) < x && (v < 0 || InterX1( i ) > InterX1( v )) ) v = i;
        }

        if( v < 0 )
        {
            CarExiting[ car ] = true;
            CarInterV[ car ] = -1;
            CarWpX[ car ][ 0 ] = (MapTilesX + 2) * TILE_SIZE;
            if( dir == DirWest ) CarWpX[ car ][ 0 ] = -2 * TILE_SIZE;
            CarWpY[ car ][ 0 ] = lane_y;
            CarWpCount[ car ] = 1;
            return;
        }

        CarInterV[ car ] = v;
        CarInterH[ car ] = h;
        int choice = RandomInt( 4 );

        if( choice < 2 )
        {
            CarWpX[ car ][ 0 ] = InterX1( v ) + 40;
            if( dir == DirWest ) CarWpX[ car ][ 0 ] = InterX0( v ) - 40;
            CarWpY[ car ][ 0 ] = lane_y;
            CarWpCount[ car ] = 1;
            CarNextDir[ car ] = dir;
            CarNextRoad[ car ] = h;
        }
        else
        {
            int new_dir = DirSouth;
            if( choice == 3 ) new_dir = DirNorth;
            float lane_x = LaneX( v, new_dir );

            CarWpX[ car ][ 0 ] = lane_x;
            CarWpY[ car ][ 0 ] = lane_y;
            CarWpX[ car ][ 1 ] = lane_x;
            CarWpY[ car ][ 1 ] = InterY1( h ) + 40;
            if( new_dir == DirNorth ) CarWpY[ car ][ 1 ] = InterY0( h ) - 40;
            CarWpCount[ car ] = 2;
            CarNextDir[ car ] = new_dir;
            CarNextRoad[ car ] = v;
        }
    }
}

// ---------------------------------------------------------

bool IsAreaClearOfCars( int self, float x, float y, float radius )
{
    for( int i = 0; i < MaxCars; i++ )
    {
        if( i == self ) continue;
        float dx = CarX[ i ] - x;
        float dy = CarY[ i ] - y;
        if( dx * dx + dy * dy < radius * radius ) return false;
    }

    return true;
}

void SetTrafficCar( int car, float x, float y, int dir, int road, float speed )
{
    CarX[ car ] = x;
    CarY[ car ] = y;
    CarAngle[ car ] = DirAngle( dir );
    CarVelX[ car ] = sin( CarAngle[ car ] ) * speed;
    CarVelY[ car ] = -cos( CarAngle[ car ] ) * speed;
    CarDir[ car ] = dir;
    CarRoad[ car ] = road;
    CarIsAI[ car ] = true;
    CarStuckTimer[ car ] = 0;
    CarGhostTimer[ car ] = 0;
    CarCachedGap[ car ] = 9999;
    CarMood[ car ] = MoodNormal;
    CarMoodTimer[ car ] = 0;
    CarUnstuckTimer[ car ] = 0;
    CarBackoffTimer[ car ] = 0;
    CarHasRammed[ car ] = false;
    ResetCarHealth( car );

    int type = RandomInt( 10 );
    if( type < 2 ) CarType[ car ] = CarTypeTaxi;
    else if( type < 4 ) CarType[ car ] = CarTypeRed;
    else if( type < 6 ) CarType[ car ] = CarTypeBlue;
    else if( type < 7 ) CarType[ car ] = CarTypeSport;
    else if( type < 9 ) CarType[ car ] = CarTypeVan;
    else CarType[ car ] = CarTypePolice;

    CarCrew[ car ] = POLICE_CREW;

    PlanRoute( car );
}

bool RespawnTrafficCar( int car )
{
    int entries = 2 * NumVRoads + 2 * NumHRoads;
    int e = RandomInt( entries );
    float x, y;
    int dir, road;

    if( e < NumVRoads )
    {
        road = e; dir = DirSouth;
        x = LaneX( road, dir ); y = -1.0 * TILE_SIZE;
    }
    else if( e < 2 * NumVRoads )
    {
        road = e - NumVRoads; dir = DirNorth;
        x = LaneX( road, dir ); y = (MapTilesY + 1.0) * TILE_SIZE;
    }
    else if( e < 2 * NumVRoads + NumHRoads )
    {
        road = e - 2 * NumVRoads; dir = DirEast;
        x = -1.0 * TILE_SIZE; y = LaneY( road, dir );
    }
    else
    {
        road = e - 2 * NumVRoads - NumHRoads; dir = DirWest;
        x = (MapTilesX + 1.0) * TILE_SIZE; y = LaneY( road, dir );
    }

    if( !IsAreaClearOfCars( car, x, y, 120 ) )
      return false;

    SetTrafficCar( car, x, y, dir, road, AI_CRUISE_SPEED * 0.8 );
    return true;
}

void InitializeTraffic()
{
    for( int car = NumParkedCars; car < MaxCars; car++ )
    {
        for( int attempt = 0; attempt < 20; attempt++ )
        {
            float x, y;
            int dir, road;

            if( RandomInt( 2 ) == 0 )
            {
                road = RandomInt( NumVRoads );
                dir = DirSouth;
                if( RandomInt( 2 ) == 0 ) dir = DirNorth;
                x = LaneX( road, dir );
                y = (1 + RandomInt( MapTilesY - 2 )) * TILE_SIZE + 40;
            }
            else
            {
                road = RandomInt( NumHRoads );
                dir = DirEast;
                if( RandomInt( 2 ) == 0 ) dir = DirWest;
                x = (1 + RandomInt( MapTilesX - 2 )) * TILE_SIZE + 40;
                y = LaneY( road, dir );
            }

            bool in_intersection = false;
            for( int v = 0; v < NumVRoads; v++ )
              for( int h = 0; h < NumHRoads; h++ )
                if( x > InterX0( v ) - 60 && x < InterX1( v ) + 60 && y > InterY0( h ) - 60 && y < InterY1( h ) + 60 )
                  in_intersection = true;

            if( in_intersection || !IsAreaClearOfCars( car, x, y, 140 ) )
              continue;

            SetTrafficCar( car, x, y, dir, road, 0 );
            break;
        }
    }
}

// ---------------------------------------------------------

int GetLightState( int intersection, bool north_south )
{
    int t = (get_frame_counter() + intersection * 217) % LIGHT_CYCLE;

    if( north_south )
    {
        if( t < 380 ) return LightGreen;
        if( t < 460 ) return LightYellow;
        return LightRed;
    }

    if( t >= 500 && t < 880 ) return LightGreen;
    if( t >= 880 && t < 960 ) return LightYellow;
    return LightRed;
}

float LightGap( int car )
{
    if( CarExiting[ car ] || CarInterPassed[ car ] || CarInterV[ car ] < 0 )
      return 9999;

    int v = CarInterV[ car ];
    int h = CarInterH[ car ];
    int dir = CarDir[ car ];
    float half = CAR_SPRITE_H / 2;
    float gap = 9999;

    if( dir == DirSouth ) gap = InterY0( h ) - (CarY[ car ] + half);
    if( dir == DirNorth ) gap = CarY[ car ] - half - InterY1( h );
    if( dir == DirEast )  gap = InterX0( v ) - (CarX[ car ] + half);
    if( dir == DirWest )  gap = CarX[ car ] - half - InterX1( v );

    if( gap < -8 )
    {
        CarInterPassed[ car ] = true;
        return 9999;
    }

    int state = GetLightState( v * NumHRoads + h, dir == DirNorth || dir == DirSouth );

    if( state == LightRed ) return gap;
    if( state == LightYellow && gap > 24 ) return gap;
    return 9999;
}

float ObstacleGap( int car )
{
    float fx = CarFwdX[ car ];
    float fy = CarFwdY[ car ];
    float sx = -fy;
    float sy = fx;
    float my_speed = fabs( CarSpeed[ car ] );
    float my_x = CarX[ car ];
    float my_y = CarY[ car ];
    float best = 9999;

    if( CarGhostTimer[ car ] <= 0 )
    {
        for( int i = 0; i < MaxCars; i++ )
        {
            if( i == car ) continue;

            float rx = CarX[ i ] - my_x;
            float ry = CarY[ i ] - my_y;
            if( fabs( rx ) > AI_LOOK_AHEAD || fabs( ry ) > AI_LOOK_AHEAD ) continue;
            float f = rx * fx + ry * fy;
            if( f <= 0 || f > AI_LOOK_AHEAD ) continue;

            float l = rx * sx + ry * sy;
            if( fabs( l ) > AI_LANE_HALF_WIDTH ) continue;

            float heading = fx * CarFwdX[ i ] + fy * CarFwdY[ i ];
            if( heading < -0.5 ) continue;

            if( fabs( heading ) < 0.7 && CarIsAI[ i ] && i > car &&
                my_speed < 0.3 && fabs( CarSpeed[ i ] ) < 0.3 )
              continue;

            float gap = f - CAR_SPRITE_H - 6;
            if( gap < best ) best = gap;
        }
    }

    for( int p = 0; p < MaxPedestrians; p++ )
    {
        if( CarMood[ car ] == MoodFlee ) break;
        if( PedState[ p ] == PedStateKnocked ) continue;

        float rx = PedX[ p ] - my_x;
        float ry = PedY[ p ] - my_y;
        if( fabs( rx ) > AI_LOOK_AHEAD || fabs( ry ) > AI_LOOK_AHEAD ) continue;
        float f = rx * fx + ry * fy;
        if( f <= 0 || f > AI_LOOK_AHEAD ) continue;

        float l = rx * sx + ry * sy;
        if( fabs( l ) > 26 ) continue;

        float gap = f - CAR_SPRITE_H / 2 - 16;
        if( gap < best ) best = gap;
    }

    if( !IsPlayerInCar )
    {
        float rx = PlayerX - CarX[ car ];
        float ry = PlayerY - CarY[ car ];
        float f = rx * fx + ry * fy;
        float l = rx * sx + ry * sy;

        if( f > 0 && f < AI_LOOK_AHEAD && fabs( l ) < 26 )
        {
            float gap = f - CAR_SPRITE_H / 2 - 16;
            if( gap < best ) best = gap;
        }
    }

    return best;
}

// ---------------------------------------------------------

float RoadCenterX( int v ) { return (VRoadX[ v ] + VRoadWidth[ v ] / 2.0) * TILE_SIZE; }
float RoadCenterY( int h ) { return (HRoadY[ h ] + HRoadHeight[ h ] / 2.0) * TILE_SIZE; }

int NearestVRoad( float x )
{
    int best = 0;

    for( int v = 1; v < NumVRoads; v++ )
      if( fabs( x - RoadCenterX( v ) ) < fabs( x - RoadCenterX( best ) ) )
        best = v;

    return best;
}

int NearestHRoad( float y )
{
    int best = 0;

    for( int h = 1; h < NumHRoads; h++ )
      if( fabs( y - RoadCenterY( h ) ) < fabs( y - RoadCenterY( best ) ) )
        best = h;

    return best;
}

void ResumeTraffic( int car )
{
    CarMood[ car ] = MoodNormal;
    CarMoodTimer[ car ] = 0;
    CarUnstuckTimer[ car ] = 0;
    CarStuckTimer[ car ] = 0;

    float fx = sin( CarAngle[ car ] );
    float fy = -cos( CarAngle[ car ] );
    int v = NearestVRoad( CarX[ car ] );
    int h = NearestHRoad( CarY[ car ] );

    if( fabs( CarX[ car ] - RoadCenterX( v ) ) < fabs( CarY[ car ] - RoadCenterY( h ) ) )
    {
        CarRoad[ car ] = v;
        CarDir[ car ] = DirNorth;
        if( fy > 0 ) CarDir[ car ] = DirSouth;
    }
    else
    {
        CarRoad[ car ] = h;
        CarDir[ car ] = DirWest;
        if( fx > 0 ) CarDir[ car ] = DirEast;
    }

    PlanRoute( car );
}

void StartCarMood( int car, int mood )
{
    if( !CarIsAI[ car ] )
      return;

    if( CarMood[ car ] == MoodPolice && mood != MoodPolice )
      return;

    CarMood[ car ] = mood;
    CarMoodTimer[ car ] = MOOD_FRAMES;
    CarStuckTimer[ car ] = 0;
    CarUnstuckTimer[ car ] = 0;
}

void StartPoliceBackoff( int car )
{
    CarBackoffTimer[ car ] = POLICE_BACKOFF_FRAMES;
    CarHasRammed[ car ] = true;
    CarUnstuckTimer[ car ] = 0;
}

void OnCarShot( int car )
{
    if( !CarIsAI[ car ] )
      return;

    if( CarType[ car ] == CarTypePolice )
    {
        ReportCrime( 4 );
        StartCarMood( car, MoodPolice );
        return;
    }

    ReportCrime( 1 );

    if( CarMood[ car ] != MoodNormal )
      return;

    if( RandomInt( 100 ) < 40 )
      StartCarMood( car, MoodAttack );
    else
      StartCarMood( car, MoodFlee );
}

void UpdateChaseCar( int car )
{
    float x = CarX[ car ];
    float y = CarY[ car ];
    float to_player_x = PlayerX - x;
    float to_player_y = PlayerY - y;
    float player_distance = sqrt( to_player_x * to_player_x + to_player_y * to_player_y );

    if( CarMood[ car ] == MoodAttack )
    {
        CarMoodTimer[ car ]--;

        if( CarMoodTimer[ car ] <= 0 || player_distance > 700 || PlayerDownTimer > 0 )
        {
            ResumeTraffic( car );
            return;
        }
    }

    if( CarUnstuckTimer[ car ] > 0 )
    {
        CarUnstuckTimer[ car ]--;
        TrafficThrottle = -1;
        TrafficSteer = CarUnstuckSteer[ car ];
        return;
    }

    if( CarBackoffTimer[ car ] > 0 )
    {
        CarBackoffTimer[ car ]--;

        if( CarBackoffTimer[ car ] > POLICE_BACKOFF_BRAKE )
        {
            float away = atan2( -to_player_x, to_player_y );
            float away_diff = away - CarAngle[ car ];
            while( away_diff > pi ) away_diff -= 2 * pi;
            while( away_diff < -pi ) away_diff += 2 * pi;

            TrafficSteer = fmax( -1, fmin( 1, away_diff * 2 ) );
            if( CarSpeed[ car ] < 2.2 ) TrafficThrottle = 1;
        }
        else if( CarSpeed[ car ] > 0.1 )
          TrafficThrottle = -1;

        return;
    }

    if( CarMood[ car ] == MoodPolice && CarCopsOut[ car ] > 0 )
    {
        if( CarSpeed[ car ] > 0.1 ) TrafficThrottle = -1;
        return;
    }

    float tx = PlayerX;
    float ty = PlayerY;
    int v = NearestVRoad( x );
    int h = NearestHRoad( y );
    int vp = NearestVRoad( PlayerX );
    int hp = NearestHRoad( PlayerY );
    bool on_v = fabs( x - RoadCenterX( v ) ) < VRoadWidth[ v ] * TILE_SIZE / 2 + 20;
    bool on_h = fabs( y - RoadCenterY( h ) ) < HRoadHeight[ h ] * TILE_SIZE / 2 + 20;

    if( player_distance < 260 || (on_v && v == vp) || (on_h && h == hp) )
      ;
    else if( on_v )
    {
        tx = RoadCenterX( v );
        ty = RoadCenterY( hp );
    }
    else if( on_h )
    {
        tx = RoadCenterX( vp );
        ty = RoadCenterY( h );
    }
    else if( fabs( x - RoadCenterX( v ) ) < fabs( y - RoadCenterY( h ) ) )
    {
        tx = RoadCenterX( v );
        ty = y;
    }
    else
    {
        tx = x;
        ty = RoadCenterY( h );
    }

    float dx = tx - x;
    float dy = ty - y;
    float desired = atan2( dx, -dy );
    float diff = desired - CarAngle[ car ];

    while( diff > pi ) diff -= 2 * pi;
    while( diff < -pi ) diff += 2 * pi;

    TrafficSteer = fmax( -1, fmin( 1, diff * 3 ) );

    float target = AI_CHASE_SPEED;
    if( fabs( diff ) > 0.5 ) target = 2.4;
    if( fabs( diff ) > 1.5 ) target = 1.4;

    bool player_stopped = !IsPlayerInCar || fabs( CarSpeed[ CurrentCar ] ) < 0.6;

    if( CarMood[ car ] == MoodPolice && WantedLevel < WANTED_RAM_LEVEL && player_stopped && player_distance < 140 )
      target = fmin( target, (player_distance - 40) / 40 );

    float speed = CarSpeed[ car ];

    if( speed < target - 0.15 )
      TrafficThrottle = 1;
    else if( speed > target + 0.25 && speed > 0.2 )
      TrafficThrottle = -1;

    if( fabs( speed ) < 0.3 && target > 0.5 )
      CarStuckTimer[ car ]++;
    else
      CarStuckTimer[ car ] = 0;

    if( CarStuckTimer[ car ] > 45 )
    {
        CarStuckTimer[ car ] = 0;
        CarUnstuckTimer[ car ] = 40;
        CarUnstuckSteer[ car ] = -1;
        if( diff < 0 ) CarUnstuckSteer[ car ] = 1;
    }
}

void UpdateTrafficCar( int car )
{
    TrafficThrottle = 0;
    TrafficSteer = 0;

    if( CarMood[ car ] == MoodAttack || CarMood[ car ] == MoodPolice )
    {
        UpdateChaseCar( car );
        return;
    }

    if( CarUnstuckTimer[ car ] > 0 )
    {
        CarUnstuckTimer[ car ]--;
        TrafficThrottle = -1;
        TrafficSteer = CarUnstuckSteer[ car ];
        return;
    }

    bool fleeing = CarMood[ car ] == MoodFlee;

    if( fleeing )
    {
        CarMoodTimer[ car ]--;

        if( CarMoodTimer[ car ] <= 0 )
        {
            CarMood[ car ] = MoodNormal;
            fleeing = false;
        }
    }

    float x = CarX[ car ];
    float y = CarY[ car ];

    if( CarExiting[ car ] )
    {
        if( x < -1.5 * TILE_SIZE || y < -1.5 * TILE_SIZE ||
            x > (MapTilesX + 1.5) * TILE_SIZE || y > (MapTilesY + 1.5) * TILE_SIZE )
        {
            if( !RespawnTrafficCar( car ) )
            {
                CarVelX[ car ] = 0;
                CarVelY[ car ] = 0;
            }
            return;
        }
    }

    int index = CarWpIndex[ car ];
    float wx = CarWpX[ car ][ index ];
    float wy = CarWpY[ car ][ index ];
    float dx = wx - x;
    float dy = wy - y;

    if( dx * dx + dy * dy < AI_WAYPOINT_RADIUS * AI_WAYPOINT_RADIUS && !CarExiting[ car ] )
    {
        CarWpIndex[ car ]++;

        if( CarWpIndex[ car ] >= CarWpCount[ car ] )
        {
            CarDir[ car ] = CarNextDir[ car ];
            CarRoad[ car ] = CarNextRoad[ car ];
            PlanRoute( car );
        }

        index = CarWpIndex[ car ];
        wx = CarWpX[ car ][ index ];
        wy = CarWpY[ car ][ index ];
        dx = wx - x;
        dy = wy - y;
    }

    float desired = atan2( dx, -dy );
    float diff = desired - CarAngle[ car ];

    while( diff > pi ) diff -= 2 * pi;
    while( diff < -pi ) diff += 2 * pi;

    TrafficSteer = fmax( -1, fmin( 1, diff * 4 ) );

    float target = AI_CRUISE_SPEED;
    if( fleeing ) target = AI_FLEE_SPEED;
    if( fabs( diff ) > 0.35 ) target = AI_TURN_SPEED;

    if( (get_frame_counter() + car) % 2 == 0 )
      CarCachedGap[ car ] = ObstacleGap( car );

    float light_gap = 9999;
    if( !fleeing ) light_gap = LightGap( car );
    float gap = fmin( CarCachedGap[ car ], light_gap );

    if( gap < 8 )
      target = 0;
    else if( gap < 80 )
      target = fmin( target, (gap - 8) / 30 );

    float speed = CarSpeed[ car ];

    if( speed < target - 0.15 )
      TrafficThrottle = 1;
    else if( speed > target + 0.25 && speed > 0.2 )
      TrafficThrottle = -1;

    if( fabs( speed ) < 0.2 && target > 0.5 && !CarExiting[ car ] )
    {
        CarStuckTimer[ car ]++;

        if( CarStuckTimer[ car ] > 60 )
        {
            CarStuckTimer[ car ] = 0;
            CarUnstuckTimer[ car ] = 35;
            CarUnstuckSteer[ car ] = -TrafficSteer;
        }
    }
    else if( fabs( speed ) < 0.1 && target == 0 && light_gap > 900 )
      CarStuckTimer[ car ]++;
    else
      CarStuckTimer[ car ] = 0;

    if( CarStuckTimer[ car ] > AI_STUCK_FRAMES )
    {
        CarGhostTimer[ car ] = AI_GHOST_FRAMES;
        CarStuckTimer[ car ] = 0;
    }

    if( CarGhostTimer[ car ] > 0 )
      CarGhostTimer[ car ]--;
}

// ---------------------------------------------------------

void DrawTrafficLight( float x, float y, int state )
{
    if( !IsMapPointVisible( x, y, 40 ) )
      return;

    select_texture( TextureSprites );
    select_region( RegionLightBox );
    DrawSpriteInMap( x, y, 0, 1.0 );

    float lamp_y = y - 10;
    int color = make_color_rgb( 255, 40, 60 );

    if( state == LightYellow )
    {
        lamp_y = y;
        color = make_color_rgb( 255, 200, 40 );
    }
    else if( state == LightGreen )
    {
        lamp_y = y + 10;
        color = make_color_rgb( 40, 255, 120 );
    }

    set_blending_mode( blending_add );
    select_region( RegionLampGlow );
    set_multiply_color( color );
    DrawSpriteInMap( x - 1, lamp_y, 0, 1.0 );
    DrawSpriteInMap( x - 1, lamp_y, 0, 0.6 );
    set_multiply_color( color_white );
    set_blending_mode( blending_alpha );
}

void RenderTrafficLights()
{
    float offset = 0.22 * TILE_SIZE;

    for( int v = 0; v < NumVRoads; v++ )
    {
        for( int h = 0; h < NumHRoads; h++ )
        {
            int id = v * NumHRoads + h;
            int ns = GetLightState( id, true );
            int ew = GetLightState( id, false );

            DrawTrafficLight( InterX0( v ) - offset, InterY0( h ) - offset, ns );
            DrawTrafficLight( InterX1( v ) + offset, InterY1( h ) + offset, ns );
            DrawTrafficLight( InterX0( v ) - offset, InterY1( h ) + offset, ew );
            DrawTrafficLight( InterX1( v ) + offset, InterY0( h ) - offset, ew );
        }
    }
}
