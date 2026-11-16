// *****************************************************************************
    // start include guard
    #ifndef OBJECTIVE_HPP
    #define OBJECTIVE_HPP

    // include project headers
    #include "../core/Definitions.h"
// *****************************************************************************

extern bool ObjectiveActive;
extern int ObjectiveBannerTimer;
extern int ObjectiveStage;

void StartObjective();
void SetObjectiveStage( int stage );
void OnCafeEntered();
void UpdateObjective();
void RenderObjectiveWorld();
void RenderObjectiveHud();


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
