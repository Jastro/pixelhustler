// *****************************************************************************
    // start include guard
    #ifndef POLICE_HPP
    #define POLICE_HPP

    // include project headers
    #include "../core/Definitions.h"
// *****************************************************************************

extern int WantedLevel;
extern int WantedHeat;
extern int[ MaxCars ] CarCopsOut;
extern int[ MaxCars ] CarCrew;

void ResetWanted();
void ReportCrime( int points );
void StartCrimeGrace();
void UpdatePolice();
void RenderSirens();
void RenderWantedHud();

void ResetCops();
void DeployCops( int car );
void PullPlayerOut( int c );
void UpdateCops();
void RenderCops();
bool ShootCops( float x, float y, float damage, float angle );
void KillCopsInRadius( float x, float y, float radius, bool by_player );


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
