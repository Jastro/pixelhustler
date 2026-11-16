// *****************************************************************************
    // start include guard
    #ifndef EXPLOSIONS_HPP
    #define EXPLOSIONS_HPP

    // include project headers
    #include "../core/Definitions.h"
// *****************************************************************************


// ---------------------------------------------------------
//   EXPLOSIONS AND CAR DAMAGE
// ---------------------------------------------------------
// fx.h from OceanStorm (the explosions) + car health (not from OceanStorm:
// the heli had fuel instead of health. Jastro proposed "cars run out of health
// like the heli ran out of fuel". Carra: "that's just health, Jastro").

extern float[ MaxCars ] CarHealth;
extern bool[ MaxCars ] CarWrecked;

void ResetCarHealth( int car );
void DamageCar( int car, float amount, bool by_player );
void ExplodeCar( int car );
void UpdateCarDamage();
void ClearCarCrimeCredit();
void RenderCarDamage();

void ResetFX();
void SpawnExplosion( float x, float y, float scale );
void UpdateFX();
void RenderFX();


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
