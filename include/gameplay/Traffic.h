// *****************************************************************************
    // start include guard
    #ifndef TRAFFIC_HPP
    #define TRAFFIC_HPP

    // include project headers
    #include "../core/Definitions.h"
// *****************************************************************************

#define DirNorth  0
#define DirEast   1
#define DirSouth  2
#define DirWest   3

extern float TrafficThrottle;
extern float TrafficSteer;
extern int[ MaxCars ] CarMood;

int RandomInt( int n );
bool RespawnTrafficCar( int car );
bool IsAreaClearOfCars( int self, float x, float y, float radius );
void ResumeTraffic( int car );
void StartCarMood( int car, int mood );
void OnCarShot( int car );
void StartPoliceBackoff( int car );
extern bool[ MaxCars ] CarHasRammed;

void InitializeTraffic();
void UpdateTrafficCar( int car );
int GetLightState( int intersection, bool north_south );
void RenderTrafficLights();


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
