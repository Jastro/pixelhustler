// *****************************************************************************
    // start include guard
    #ifndef CORPSES_HPP
    #define CORPSES_HPP

    // include project headers
    #include "../core/Definitions.h"
// *****************************************************************************


// ---------------------------------------------------------
//   CORPSES AND BLOOD
// ---------------------------------------------------------
// corpse.h from OceanStorm.
// Jastro's changes: none. Jastro's commit message: "added gore system"

extern float[ MaxCorpses ] CorpseX;
extern float[ MaxCorpses ] CorpseY;
extern float[ MaxCorpses ] CorpseAngle;
extern float[ MaxCorpses ] CorpseBloodScale;
extern float[ MaxCorpses ] FinalBloodScale;
extern float[ MaxCorpses ] CorpseBloodAngle;
extern int[ MaxCorpses ] CorpseType;
extern int[ MaxCorpses ] CorpseTint;
extern int[ MaxCorpses ] CorpseState;
extern int[ MaxCorpses ] CorpseActive;
extern bool BloodEnabled;

void ResetCorpses();
void SpawnCorpse( float x, float y, float angle, int ped_type, int tint );
void UpdateCorpses();
void RenderCorpses();


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
