// *****************************************************************************
    // start include guard
    #ifndef WEAPONS_HPP
    #define WEAPONS_HPP

    // include project headers
    #include "../core/Definitions.h"
// *****************************************************************************


// ---------------------------------------------------------
//   WEAPONS, BULLETS AND PICKUPS
// ---------------------------------------------------------
// weapon.h + bullet.h + pickup.h from OceanStorm, merged into one file.
// Jastro: "I refactored three modules into one"
// Carra: "you pasted them one after the other"
// Jastro: "a refactor is a refactor"

extern int[ NumWeapons ] WeaponFireMode;
extern int[ NumWeapons ] WeaponMaxAmmo;
extern int[ NumWeapons ] WeaponCurrentAmmo;
extern int[ NumWeapons ] WeaponReserveAmmo;
extern int[ NumWeapons ] WeaponMaxReserve;
extern int[ NumWeapons ] WeaponPrice;
extern float[ NumWeapons ] WeaponFireRate;
extern float[ NumWeapons ] WeaponReloadTime;
extern float[ NumWeapons ] WeaponDamage;
extern float[ NumWeapons ] WeaponRange;
extern float[ NumWeapons ] WeaponSpread;
extern int[ NumWeapons ] WeaponBulletCount;
extern float[ NumWeapons ] WeaponLastShot;
extern float[ NumWeapons ] WeaponReloadStart;
extern int[ NumWeapons ] WeaponIsReloading;
extern float[ NumWeapons ] WeaponSpeed;
extern int[ NumWeapons ] PlayerHasWeapon;
extern int CurrentWeapon;
extern int PlayerMoney;

extern float[ MaxBullets ] BulletX;
extern float[ MaxBullets ] BulletY;
extern float[ MaxBullets ] BulletAngle;
extern float[ MaxBullets ] BulletSpeed;
extern float[ MaxBullets ] BulletDamage;
extern float[ MaxBullets ] BulletDistance;
extern float[ MaxBullets ] BulletRange;
extern int[ MaxBullets ] BulletActive;

extern float[ MaxPickups ] PickupX;
extern float[ MaxPickups ] PickupY;
extern int[ MaxPickups ] PickupType;
extern int[ MaxPickups ] PickupActive;
extern int HudMessageTimer;

void InitializeWeapons();
void ResetWeapons();
void UpdateWeapon();
void FireWeapon( float x, float y, float angle );
void ReloadWeapon();
void AddAmmo( int w, int amount );
void SwitchWeaponNext();
void SwitchWeaponPrevious();
void GiveWeapon( int weapon_type );
int* WeaponName( int weapon );
void ShowHudMessage( int* text, int color );

void CreateBullet( float x, float y, float angle, float spread );
void CreateCopBullet( float x, float y, float angle );
void ResetBullets();
void UpdateBullets();
void RenderBullets();

void ResetPickups();
int SpawnPickup( float x, float y, int type, int value );
void SpawnFixedPickup( float x, float y, int type );
void SpawnCash( float x, float y, int value );
void UpdatePickups();
void RenderPickups();

bool IsPlayerAtShop();
void BuyAmmo();
void RenderShop();
void RenderShopSign();

void RenderWeaponHud();
void RenderMoneyHud( int y );
void RenderHudMessage();


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
