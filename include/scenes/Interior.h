// *****************************************************************************
    // start include guard
    #ifndef INTERIOR_HPP
    #define INTERIOR_HPP

    // include project headers
    #include "../core/Definitions.h"
// *****************************************************************************

#define TransitionNone        0
#define TransitionEnterCafe   1
#define TransitionExitCafe    2
#define TransitionEnterHome   3
#define TransitionExitHome    4
#define TransitionStairsDown  5
#define TransitionStairsUp    6

#define InteriorCafe          0
#define InteriorHomeUp        1
#define InteriorHomeDown      2
#define TRANSITION_HALF       22

#define NumCafeNpcs           8

extern int TransitionKind;
extern int CurrentInterior;

void InitializeInterior();
void StartInHome();
void SetInterior( int interior );
void UpdateInteriorCamera();
void StartTransition( int kind );
bool UpdateTransition();
void RenderTransition();
bool UpdateCityEntrances();
void RenderCityEntranceSigns();
void UpdateInterior();
void RenderInterior();


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
