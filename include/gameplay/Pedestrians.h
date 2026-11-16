// *****************************************************************************
    // start include guard
    #ifndef PEDESTRIANS_HPP
    #define PEDESTRIANS_HPP

    // include project headers
    #include "../core/Definitions.h"
// *****************************************************************************

extern float[ MaxPedestrians ] PedX;
extern float[ MaxPedestrians ] PedY;
extern int[ MaxPedestrians ] PedState;

void InitializePedestrians();
void UpdatePedestrians();
void RenderPedestrians();
void KillPedestrian( int ped, float angle, bool by_player );
void PanicPedestrians( float x, float y, float radius );
void ClearPedestrianCrimeCredit();


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
