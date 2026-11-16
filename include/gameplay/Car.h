// *****************************************************************************
    // start include guard
    #ifndef CAR_HPP
    #define CAR_HPP

    // include project headers
    #include "../core/Definitions.h"
// *****************************************************************************


// ---------------------------------------------------------
//   CAR STATE
// ---------------------------------------------------------
// This is OceanStorm's heli.c.
//
// Design doc (Jastro, 1 line):
//   "We use the heli code, nobody will notice the difference"
//
// Design doc review (Carra, 1 line):
//   "Cars don't fly."
//
// Design doc v2 (Jastro):
//   "Heli code with altitude = 0. Zero-cost abstraction."

extern float[ MaxCars ] CarX;
extern float[ MaxCars ] CarY;
extern float[ MaxCars ] CarAngle;
extern float[ MaxCars ] CarVelX;
extern float[ MaxCars ] CarVelY;
extern int[ MaxCars ] CarType;
extern bool[ MaxCars ] CarIsAI;
extern float[ MaxCars ] CarFwdX;
extern float[ MaxCars ] CarFwdY;
extern float[ MaxCars ] CarSpeed;
extern int CurrentCar;
extern bool[ MaxCars ] CarStolen;
extern bool[ MaxCars ] CarSirenOn;
extern int[ NumParkedCars ] ParkedCarTypes;
// extern float[ MaxCars ] CarAltitude;   // "just in case" - Jastro. Removed - Carra.

extern float CarThrottle;
extern float CarSteer;
extern bool CarHandbrake;


// ---------------------------------------------------------
//   CAR FUNCTIONS
// ---------------------------------------------------------

void InitializeCars();
void UpdateCar();
void UpdateCarPhysics();
void RenderCars();
void RenderCarSigns();
bool TryEnterNearestCar();
void ExitCar();
float GetCarForwardSpeed( int car );
void PlaceCar( int car, float tile_x, float tile_y, float angle, int type );


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
