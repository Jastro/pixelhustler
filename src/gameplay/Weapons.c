// *****************************************************************************
    // include Vircon32 headers
    #include "math.h"
    #include "misc.h"
    #include "time.h"
    #include "video.h"
    #include "string.h"

    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
    #include "../../include/gameplay/Player.h"
    #include "../../include/gameplay/Car.h"
    #include "../../include/gameplay/Traffic.h"
    #include "../../include/gameplay/Pedestrians.h"
    #include "../../include/gameplay/Police.h"
    #include "../../include/gameplay/Explosions.h"
    #include "../../include/gameplay/Weapons.h"
// *****************************************************************************


// ---------------------------------------------------------
//   WEAPON VARIABLES
// ---------------------------------------------------------
// Porting log (Jastro), full transcript:
//   09:00  start porting weapons from OceanStorm
//   09:01  copy weapon.c
//   09:02  it compiles
//   09:03  "didn't have to touch much"
//   09:04  update LinkedIn: "Led the weapons system port with 100% code reuse"
//   09:05  take the rest of the week off. "It's called being efficient"
//
// Porting log (Carra), same day:
//   09:10  "where is heli_x? why does the pedestrian need an aircraft carrier?"
//   ...
//   18:45  it actually works now
//
// Porting log (Carra), next week:
//   "OceanStorm had infinite ammo. In a city with shops, that's bad for the economy."
//   Jastro: "what economy?"
//   Carra: *adds an economy*

int[ NumWeapons ] WeaponFireMode;
int[ NumWeapons ] WeaponMaxAmmo;
int[ NumWeapons ] WeaponCurrentAmmo;
int[ NumWeapons ] WeaponReserveAmmo;
int[ NumWeapons ] WeaponMaxReserve;
int[ NumWeapons ] WeaponPrice;
float[ NumWeapons ] WeaponFireRate;
float[ NumWeapons ] WeaponReloadTime;
float[ NumWeapons ] WeaponDamage;
float[ NumWeapons ] WeaponRange;
float[ NumWeapons ] WeaponSpread;
int[ NumWeapons ] WeaponBulletCount;
float[ NumWeapons ] WeaponLastShot;
float[ NumWeapons ] WeaponReloadStart;
int[ NumWeapons ] WeaponIsReloading;
float[ NumWeapons ] WeaponSpeed;
int[ NumWeapons ] PlayerHasWeapon;
int CurrentWeapon;
int PlayerMoney;

float[ MaxBullets ] BulletX;
float[ MaxBullets ] BulletY;
float[ MaxBullets ] BulletAngle;
float[ MaxBullets ] BulletSpeed;
float[ MaxBullets ] BulletDamage;
float[ MaxBullets ] BulletDistance;
float[ MaxBullets ] BulletRange;
int[ MaxBullets ] BulletActive;
bool[ MaxBullets ] BulletFromCop;

float[ MaxPickups ] PickupX;
float[ MaxPickups ] PickupY;
int[ MaxPickups ] PickupType;
int[ MaxPickups ] PickupActive;
int[ MaxPickups ] PickupValue;
int[ MaxPickups ] PickupTimer;
bool[ MaxPickups ] PickupRespawns;

int[ 12 ] WeaponHudText;
int[ 40 ] HudMessageText;
int HudMessageTimer = 0;
int HudMessageColor = 0;


// ---------------------------------------------------------
//   INITIALIZATION
// ---------------------------------------------------------
// initialize_weapons() from OceanStorm. Diff vs original:
//   - weapon_type[] removed (it was always equal to the index. Jastro: "redundancy is safety")
//   + nothing else. Numbers are sacred. Jastro balanced them in 2024 by feel.

void InitializeWeapons()
{
    select_texture( TextureWeapons );
    select_region( RegionBullet );
    define_region( 0, 0, 7, 7, 4, 4 );
    select_region( RegionEnemyBullet );
    define_region( 8, 0, 15, 7, 12, 4 );
    select_region( RegionBlood );
    define_region( 0, 16, 79, 95, 40, 56 );
    define_region_matrix( RegionFirstPickup,  96,16,  132,61,  114,32,  4,1,  0 );
    select_region( RegionAmmoBox );
    define_region( 96, 66, 119, 85, 108, 76 );
    select_region( RegionCash );
    define_region( 124, 66, 147, 81, 136, 74 );
    select_region( RegionBadge );
    define_region_topleft( 152, 66, 167, 81 );
    select_region( RegionShopSign );
    define_region( 96, 90, 151, 115, 124, 103 );
    select_region( RegionSmoke );
    define_region( 176, 66, 191, 81, 184, 74 );
    define_region_matrix( RegionFirstExplosion,  0,128,  31,159,  16,144,  ExplosionFrames,1,  0 );

    // pistol
    WeaponSpeed[ 0 ] = 5.0;
    WeaponFireMode[ 0 ] = FireModeSingle;
    WeaponMaxAmmo[ 0 ] = 12;
    WeaponFireRate[ 0 ] = 0.5;
    WeaponReloadTime[ 0 ] = 2.0;
    WeaponDamage[ 0 ] = PistolDamage;
    WeaponRange[ 0 ] = 200;
    WeaponSpread[ 0 ] = 0;
    WeaponBulletCount[ 0 ] = 1;

    // shotgun
    WeaponSpeed[ 1 ] = 4.0;
    WeaponFireMode[ 1 ] = FireModeSpread;
    WeaponMaxAmmo[ 1 ] = 6;
    WeaponFireRate[ 1 ] = 1.0;
    WeaponReloadTime[ 1 ] = 3.0;
    WeaponDamage[ 1 ] = ShotgunDamage;
    WeaponRange[ 1 ] = 150;
    WeaponSpread[ 1 ] = 30;
    WeaponBulletCount[ 1 ] = 5;   // "Reduced to be more balanced" (it was 5 before too)

    // submachine gun
    WeaponSpeed[ 2 ] = 6.0;
    WeaponFireMode[ 2 ] = FireModeAuto;
    WeaponMaxAmmo[ 2 ] = 30;
    WeaponFireRate[ 2 ] = 0.1;
    WeaponReloadTime[ 2 ] = 3.5;
    WeaponDamage[ 2 ] = SubmachineGunDamage;
    WeaponRange[ 2 ] = 150;
    WeaponSpread[ 2 ] = 0;
    WeaponBulletCount[ 2 ] = 1;

    WeaponPrice[ 0 ] = 10;
    WeaponPrice[ 1 ] = 20;
    WeaponPrice[ 2 ] = 30;

    for( int i = 0; i < NumWeapons; i++ )
      WeaponMaxReserve[ i ] = WeaponMaxAmmo[ i ] * 6;

    PlayerMoney = STARTING_MONEY;
    ResetWeapons();
    ResetBullets();
    ResetPickups();

    // OceanStorm: pickups dropped by dead enemies.
    // Pixel Hustler: pickups lying on the sidewalk. Nobody asks questions. It's that kind of city.
    SpawnFixedPickup( 790, 1220, PickupShotgun );
    SpawnFixedPickup( 1840, 460, PickupSubmachine );
    SpawnFixedPickup( 460, 600, PickupAmmo );
    SpawnFixedPickup( 1460, 1080, PickupAmmo );
    SpawnFixedPickup( 2020, 600, PickupAmmo );
}

void ResetWeapons()
{
    for( int i = 0; i < NumWeapons; i++ )
    {
        WeaponCurrentAmmo[ i ] = 0;
        WeaponReserveAmmo[ i ] = 0;
        WeaponIsReloading[ i ] = 0;
        WeaponLastShot[ i ] = 0;
        PlayerHasWeapon[ i ] = 0;
    }

    // initially he only has the pistol (comment copied verbatim, like everything else)
    PlayerHasWeapon[ WeaponPistol ] = 1;
    WeaponCurrentAmmo[ WeaponPistol ] = WeaponMaxAmmo[ WeaponPistol ];
    WeaponReserveAmmo[ WeaponPistol ] = WeaponMaxAmmo[ WeaponPistol ] * 2;
    CurrentWeapon = WeaponPistol;
}

void ShowHudMessage( int* text, int color )
{
    strcpy( HudMessageText, text );
    HudMessageColor = color;
    HudMessageTimer = 90;
}


// ---------------------------------------------------------
//   FIRING
// ---------------------------------------------------------

void FireWeapon( float x, float y, float angle )
{
    float current_time = get_frame_counter() / 60.0;

    if( WeaponCurrentAmmo[ CurrentWeapon ] <= 0 ||
        WeaponIsReloading[ CurrentWeapon ] ||
        current_time - WeaponLastShot[ CurrentWeapon ] < WeaponFireRate[ CurrentWeapon ] )
      return;

    // play_sound( SoundShoot );
    // ^ Jastro: "sounds work exactly the same as in OceanStorm"
    //   Carra: "we have zero sounds in the ROM"
    //   Jastro: "then they work exactly the same as zero sounds"
    // UPDATE: now there are sounds. They are OceanStorm's. They work exactly the same. Jastro was right (by accident).
    PlaySfx( SoundShoot );

    if( WeaponFireMode[ CurrentWeapon ] == FireModeSpread )
    {
        float spread = pi / 6;
        int count = WeaponBulletCount[ CurrentWeapon ];

        for( int i = 0; i < count; i++ )
        {
            float bullet_angle = angle - spread / 2 + (spread * i) / (count - 1);
            CreateBullet( x, y, bullet_angle, 0 );
        }
    }
    else
      CreateBullet( x, y, angle, WeaponSpread[ CurrentWeapon ] );

    WeaponCurrentAmmo[ CurrentWeapon ]--;
    WeaponLastShot[ CurrentWeapon ] = current_time;
    PanicPedestrians( x, y, PED_PANIC_RADIUS );

    if( WeaponCurrentAmmo[ CurrentWeapon ] <= 0 )
      ReloadWeapon();
}

// OceanStorm reloaded from a magic infinite pocket.
// Jastro's first attempt at limited ammo:
//   WeaponCurrentAmmo[ w ] = WeaponMaxAmmo[ w ];
//   WeaponReserveAmmo[ w ] -= 0;   // "accounting"

void ReloadWeapon()
{
    int w = CurrentWeapon;

    if( !WeaponIsReloading[ w ] &&
        WeaponCurrentAmmo[ w ] < WeaponMaxAmmo[ w ] &&
        WeaponReserveAmmo[ w ] > 0 )
    {
        WeaponIsReloading[ w ] = 1;
        WeaponReloadStart[ w ] = get_frame_counter() / 60.0;
        PlaySfx( SoundReload );
    }
}

void UpdateWeapon()
{
    int w = CurrentWeapon;
    float current_time = get_frame_counter() / 60.0;

    if( WeaponIsReloading[ w ] &&
        current_time - WeaponReloadStart[ w ] >= WeaponReloadTime[ w ] )
    {
        int taken = WeaponMaxAmmo[ w ] - WeaponCurrentAmmo[ w ];
        if( taken > WeaponReserveAmmo[ w ] ) taken = WeaponReserveAmmo[ w ];

        WeaponCurrentAmmo[ w ] += taken;
        WeaponReserveAmmo[ w ] -= taken;
        WeaponIsReloading[ w ] = 0;
    }

    if( !WeaponIsReloading[ w ] && WeaponCurrentAmmo[ w ] <= 0 )
      ReloadWeapon();
}

void AddAmmo( int w, int amount )
{
    WeaponReserveAmmo[ w ] += amount;

    if( WeaponReserveAmmo[ w ] > WeaponMaxReserve[ w ] )
      WeaponReserveAmmo[ w ] = WeaponMaxReserve[ w ];
}

// switch_weapon_next() / previous() from soldier.c.
// Jastro's version for the record:
//   CurrentWeapon++;   // "what could go wrong"
// Result: weapon #3, which does not exist, fired the dialog portraits.

void SwitchWeaponNext()
{
    int original_weapon = CurrentWeapon;

    do
    {
        CurrentWeapon = (CurrentWeapon + 1) % NumWeapons;
    }
    while( !PlayerHasWeapon[ CurrentWeapon ] && CurrentWeapon != original_weapon );

    // "Indicar con sonidos si realmente hemos cambiado de arma" - OceanStorm, verbatim, finally useful
    if( CurrentWeapon != original_weapon ) PlaySfx( SoundChangeWeapon );
    else PlaySfx( SoundCancel );
}

void SwitchWeaponPrevious()
{
    int original_weapon = CurrentWeapon;

    do
    {
        CurrentWeapon = (CurrentWeapon + NumWeapons - 1) % NumWeapons;
    }
    while( !PlayerHasWeapon[ CurrentWeapon ] && CurrentWeapon != original_weapon );

    if( CurrentWeapon != original_weapon ) PlaySfx( SoundChangeWeapon );
    else PlaySfx( SoundCancel );
}

void GiveWeapon( int weapon_type )
{
    if( PlayerHasWeapon[ weapon_type ] )
    {
        AddAmmo( weapon_type, WeaponMaxAmmo[ weapon_type ] * 2 );
        return;
    }

    PlayerHasWeapon[ weapon_type ] = 1;
    WeaponCurrentAmmo[ weapon_type ] = WeaponMaxAmmo[ weapon_type ];
    WeaponReserveAmmo[ weapon_type ] = WeaponMaxAmmo[ weapon_type ];
    CurrentWeapon = weapon_type;
}


// ---------------------------------------------------------
//   BULLETS
// ---------------------------------------------------------
// bullet.c from OceanStorm, minus turrets, spirals, kamikazes and bosses.
// Lines deleted: 180. Lines "touched": 0.
// Jastro: "see? deleting isn't touching"

void ResetBullets()
{
    for( int i = 0; i < MaxBullets; i++ )
      BulletActive[ i ] = false;
}

void CreateBullet( float x, float y, float angle, float spread )
{
    for( int i = 0; i < MaxBullets; i++ )
    {
        if( BulletActive[ i ] )
          continue;

        BulletX[ i ] = x;
        BulletY[ i ] = y;

        if( spread == 0 )
          BulletAngle[ i ] = angle;
        else
        {
            int spread_int = spread * 1000;
            float random_spread = (rand() % spread_int - spread_int / 2) / 1000.0;
            BulletAngle[ i ] = angle + random_spread;
        }

        // BUG #58 (OceanStorm): "player bullets are twice as fast as they should be"
        //   update_bullets() was called from soldier.c AND from main.c. Every frame. For a year.
        //   Jastro: "that's not a bug, that's the game feel"
        //   Carra: now called once, speed doubled here. Same feel, half the CPU.
        BulletSpeed[ i ] = WeaponSpeed[ CurrentWeapon ] * 2;
        BulletDamage[ i ] = WeaponDamage[ CurrentWeapon ];
        BulletRange[ i ] = WeaponRange[ CurrentWeapon ];
        BulletDistance[ i ] = 0;
        BulletFromCop[ i ] = false;
        BulletActive[ i ] = 1;
        return;
    }
}

// Enemy bullets are back! OceanStorm's turrets shot at you. Now it's the cops.
// Jastro: "see, the turret code was reusable after all"
// Carra: "you deleted the turret code last week"

void CreateCopBullet( float x, float y, float angle )
{
    for( int i = 0; i < MaxBullets; i++ )
    {
        if( BulletActive[ i ] )
          continue;

        BulletX[ i ] = x;
        BulletY[ i ] = y;
        BulletAngle[ i ] = angle + (RandomInt( 25 ) - 12) / 100.0;
        BulletSpeed[ i ] = 7;
        BulletDamage[ i ] = COP_BULLET_DAMAGE;
        BulletRange[ i ] = COP_SHOOT_RANGE + 60;
        BulletDistance[ i ] = 0;
        BulletFromCop[ i ] = true;
        BulletActive[ i ] = 1;
        PlaySfxAt( SoundCopShoot, x, y );
        return;
    }
}

// check_circle_collision() from bullet.c, but inlined.
// Jastro inlined it. By accident. He deleted the function and Carra had to paste it back 3 times.

int BulletHitsCar( float x, float y )
{
    for( int c = 0; c < MaxCars; c++ )
    {
        if( fabs( x - CarX[ c ] ) > 60 || fabs( y - CarY[ c ] ) > 60 )
          continue;

        float fx = CarFwdX[ c ] * CAR_CIRCLE_SPACING;
        float fy = CarFwdY[ c ] * CAR_CIRCLE_SPACING;

        for( int k = -1; k <= 1; k++ )
        {
            float dx = x - (CarX[ c ] + k * fx);
            float dy = y - (CarY[ c ] + k * fy);

            if( dx * dx + dy * dy < CAR_CIRCLE_RADIUS * CAR_CIRCLE_RADIUS )
              return c;
        }
    }

    return -1;
}

void CheckBulletCollisions( int i )
{
    float x = BulletX[ i ];
    float y = BulletY[ i ];

    // OceanStorm bullets flew over the islands. Here there are buildings.
    // First playtest: Jastro shot a pedestrian through 4 blocks. Called it "a skill shot".
    if( IsBuildingAt( x, y ) )
    {
        BulletActive[ i ] = 0;
        return;
    }

    int car = BulletHitsCar( x, y );

    if( car >= 0 )
    {
        BulletActive[ i ] = 0;
        PlaySfxAt( SoundHit, x, y );

        if( !CarWrecked[ car ] )
        {
            DamageCar( car, BulletDamage[ i ], !BulletFromCop[ i ] );
            if( !BulletFromCop[ i ] ) OnCarShot( car );
        }

        return;
    }

    if( BulletFromCop[ i ] )
    {
        if( !IsPlayerInCar && PlayerDownTimer <= 0 )
        {
            float px = x - PlayerX;
            float py = y - PlayerY;

            if( px * px + py * py < 12 * 12 )
            {
                BulletActive[ i ] = 0;
                PlayerTakeDamageFrom( BulletDamage[ i ], DownBusted );
                return;
            }
        }
    }
    else if( ShootCops( x, y, BulletDamage[ i ], BulletAngle[ i ] ) )
    {
        BulletActive[ i ] = 0;
        PlaySfxAt( SoundHit, x, y );
        return;
    }

    // enemies = pedestrians. They never signed up for this.
    float hit_radius = PED_RADIUS + 4;

    for( int p = 0; p < MaxPedestrians; p++ )
    {
        float dx = x - PedX[ p ];
        float dy = y - PedY[ p ];

        if( dx * dx + dy * dy <= hit_radius * hit_radius )
        {
            BulletActive[ i ] = 0;
            // Police bullets used to go through civilians. Jastro: "they're trained professionals".
            // Carra: "they're 8x8 pixels with a sprite from a turret". Now nobody is safe.
            KillPedestrian( p, BulletAngle[ i ], !BulletFromCop[ i ] );
            PlaySfxAt( SoundHit, x, y );
            return;
        }
    }
}

void UpdateBullets()
{
    for( int i = 0; i < MaxBullets; i++ )
    {
        if( !BulletActive[ i ] )
          continue;

        float dx = cos( BulletAngle[ i ] ) * BulletSpeed[ i ];
        float dy = sin( BulletAngle[ i ] ) * BulletSpeed[ i ];

        BulletX[ i ] += dx;
        BulletY[ i ] += dy;
        BulletDistance[ i ] += BulletSpeed[ i ];

        // bullet_distance += sqrt( dx*dx + dy*dy );
        // ^ OceanStorm. A square root per bullet per frame to get back the speed we already had.
        //   git blame: jastro. Commit message: "math"

        if( BulletDistance[ i ] > BulletRange[ i ] )
          BulletActive[ i ] = 0;
        else
          CheckBulletCollisions( i );
    }
}

void RenderBullets()
{
    select_texture( TextureWeapons );

    for( int i = 0; i < MaxBullets; i++ )
    {
        if( !BulletActive[ i ] || !IsMapPointVisible( BulletX[ i ], BulletY[ i ], 8 ) )
          continue;

        if( BulletFromCop[ i ] ) select_region( RegionEnemyBullet );
        else select_region( RegionBullet );

        DrawSpriteInMap( BulletX[ i ], BulletY[ i ], 0, 1.0 );
    }
}


// ---------------------------------------------------------
//   PICKUPS
// ---------------------------------------------------------
// pickup.c from OceanStorm. Health and armor pickups still exist in the texture.
// They do nothing. Like Jastro in the sprint planning.

void ResetPickups()
{
    for( int i = 0; i < MaxPickups; i++ )
    {
        PickupActive[ i ] = 0;
        PickupRespawns[ i ] = false;
    }
}

int SpawnPickup( float x, float y, int type, int value )
{
    for( int i = 0; i < MaxPickups; i++ )
    {
        if( PickupActive[ i ] || PickupRespawns[ i ] )
          continue;

        PickupX[ i ] = x;
        PickupY[ i ] = y;
        PickupType[ i ] = type;
        PickupValue[ i ] = value;
        PickupTimer[ i ] = 0;
        PickupActive[ i ] = 1;
        return i;
    }

    return -1;
}

void SpawnFixedPickup( float x, float y, int type )
{
    int i = SpawnPickup( x, y, type, 0 );

    if( i >= 0 )
      PickupRespawns[ i ] = true;
}

void SpawnCash( float x, float y, int value )
{
    int i = SpawnPickup( x, y, PickupCash, value );

    if( i >= 0 )
      PickupTimer[ i ] = CASH_LIFETIME_FRAMES;
}

void CollectPickup( int i )
{
    int type = PickupType[ i ];

    if( type == PickupCash )
    {
        PlaySfx( SoundCash );
        PlayerMoney += PickupValue[ i ];
        itoa( PickupValue[ i ], WeaponHudText, 10 );
        strcpy( HudMessageText, "+$" );
        strcat( HudMessageText, WeaponHudText );
        HudMessageColor = make_color_rgb( 120, 255, 140 );
        HudMessageTimer = 60;
    }
    else if( type == PickupAmmo )
    {
        PlaySfx( SoundPickup );
        for( int w = 0; w < NumWeapons; w++ )
          if( PlayerHasWeapon[ w ] )
            AddAmmo( w, WeaponMaxAmmo[ w ] * 2 );

        if( GameLanguage == 1 ) ShowHudMessage( "+MUNICION", make_color_rgb( 255, 225, 60 ) );
        else ShowHudMessage( "+AMMO", make_color_rgb( 255, 225, 60 ) );
    }
    else
    {
        int weapon = WeaponShotgun;
        if( type == PickupSubmachine ) weapon = WeaponSubmachine;

        GiveWeapon( weapon );
        PlaySfx( SoundPickup );
        ShowHudMessage( WeaponName( weapon ), make_color_rgb( 255, 90, 110 ) );
    }

    PickupActive[ i ] = 0;

    if( PickupRespawns[ i ] )
      PickupTimer[ i ] = PICKUP_RESPAWN_FRAMES;
}

void UpdatePickups()
{
    if( HudMessageTimer > 0 )
      HudMessageTimer--;

    for( int i = 0; i < MaxPickups; i++ )
    {
        if( !PickupActive[ i ] )
        {
            if( PickupRespawns[ i ] && PickupTimer[ i ] > 0 )
            {
                PickupTimer[ i ]--;

                if( PickupTimer[ i ] == 0 )
                  PickupActive[ i ] = 1;
            }

            continue;
        }

        if( PickupType[ i ] == PickupCash )
        {
            PickupTimer[ i ]--;

            if( PickupTimer[ i ] <= 0 )
            {
                PickupActive[ i ] = 0;
                continue;
            }
        }

        if( IsPlayerInCar || PlayerDownTimer > 0 )
          continue;

        float dx = PlayerX - PickupX[ i ];
        float dy = PlayerY - PickupY[ i ];

        if( dx * dx + dy * dy < 24 * 24 )
          CollectPickup( i );
    }
}

void RenderPickups()
{
    select_texture( TextureWeapons );
    float bob = 0.05 * sin( get_frame_counter() * 0.1 );

    for( int i = 0; i < MaxPickups; i++ )
    {
        if( !PickupActive[ i ] || !IsMapPointVisible( PickupX[ i ], PickupY[ i ], 30 ) )
          continue;

        int type = PickupType[ i ];

        if( type == PickupCash )
        {
            if( PickupTimer[ i ] < 180 && (get_frame_counter() / 6) % 2 )
              continue;

            select_region( RegionCash );
            DrawSpriteInMap( PickupX[ i ], PickupY[ i ], 0, 1.0 + bob );
        }
        else if( type == PickupAmmo )
        {
            select_region( RegionAmmoBox );
            DrawSpriteInMap( PickupX[ i ], PickupY[ i ], 0, 1.0 + bob );
        }
        else
        {
            select_region( RegionFirstPickup + type );
            DrawSpriteInMap( PickupX[ i ], PickupY[ i ], 0, 0.6 + bob );
        }
    }
}


// ---------------------------------------------------------
//   AMMO SHOP
// ---------------------------------------------------------

bool IsPlayerAtShop()
{
    if( IsPlayerInCar || PlayerDownTimer > 0 )
      return false;

    float dx = PlayerX - SHOP_X;
    float dy = PlayerY - SHOP_Y;
    return dx * dx + dy * dy < SHOP_DISTANCE * SHOP_DISTANCE;
}

void BuyAmmo()
{
    int w = CurrentWeapon;

    if( WeaponReserveAmmo[ w ] >= WeaponMaxReserve[ w ] )
    {
        PlaySfx( SoundCancel );
        if( GameLanguage == 1 ) ShowHudMessage( "YA VAS LLENO", make_color_rgb( 255, 225, 60 ) );
        else ShowHudMessage( "AMMO FULL", make_color_rgb( 255, 225, 60 ) );
        return;
    }

    if( PlayerMoney < WeaponPrice[ w ] )
    {
        PlaySfx( SoundCancel );
        if( GameLanguage == 1 ) ShowHudMessage( "NO TIENES PASTA", make_color_rgb( 255, 80, 80 ) );
        else ShowHudMessage( "NOT ENOUGH CASH", make_color_rgb( 255, 80, 80 ) );
        return;
    }

    PlayerMoney -= WeaponPrice[ w ];
    PlaySfx( SoundCash );
    AddAmmo( w, WeaponMaxAmmo[ w ] );

    if( GameLanguage == 1 ) ShowHudMessage( "+MUNICION", make_color_rgb( 255, 225, 60 ) );
    else ShowHudMessage( "+AMMO", make_color_rgb( 255, 225, 60 ) );
}

void RenderShop()
{
    if( !IsMapPointVisible( SHOP_X, SHOP_Y, 60 ) )
      return;

    float pulse = 0.8 + 0.1 * sin( get_frame_counter() * 0.12 );

    select_texture( TextureSprites );
    select_region( RegionRing );
    set_blending_mode( blending_add );
    set_multiply_color( make_color_rgb( 255, 60, 90 ) );
    DrawSpriteInMap( SHOP_X, SHOP_Y, 0, pulse );
    set_multiply_color( color_white );
    set_blending_mode( blending_alpha );
}

void RenderShopSign()
{
    if( !IsMapPointVisible( SHOP_X, SHOP_Y, 60 ) )
      return;

    select_texture( TextureWeapons );
    select_region( RegionShopSign );

    if( get_frame_counter() % 90 < 4 )
      set_multiply_color( make_color_rgb( 120, 60, 70 ) );

    DrawSpriteInMap( SHOP_X, SHOP_Y - 30, 0, 1.0 );
    set_multiply_color( color_white );
}


// ---------------------------------------------------------
//   HUD
// ---------------------------------------------------------
// render_soldier_gui() used gui.png with fancy frames.
// Here it's print_at(). Jastro calls it "minimalist UI".

int* WeaponName( int weapon )
{
    if( GameLanguage == 1 )
    {
        if( weapon == WeaponShotgun ) return "ESCOPETA";
        if( weapon == WeaponSubmachine ) return "METRALLETA";
        return "PISTOLA";
    }

    if( weapon == WeaponShotgun ) return "SHOTGUN";
    if( weapon == WeaponSubmachine ) return "SMG";
    return "PISTOL";
}

void RenderWeaponHud()
{
    int y = 38;
    int w = CurrentWeapon;

    DrawRect( 6, y, 240, 28, make_color_rgba( 12, 10, 22, 190 ) );
    set_multiply_color( make_color_rgb( 255, 90, 110 ) );
    print_at( 13, y + 5, WeaponName( w ) );

    if( WeaponIsReloading[ w ] )
    {
        if( (get_frame_counter() / 8) % 2 )
        {
            if( GameLanguage == 1 ) print_at( 125, y + 5, "RECARGANDO" );
            else print_at( 125, y + 5, "RELOADING" );
        }
    }
    else if( WeaponCurrentAmmo[ w ] <= 0 && WeaponReserveAmmo[ w ] <= 0 )
    {
        set_multiply_color( make_color_rgb( 255, 60, 60 ) );
        if( GameLanguage == 1 ) print_at( 125, y + 5, "SIN BALAS" );
        else print_at( 125, y + 5, "NO AMMO" );
    }
    else
    {
        set_multiply_color( color_white );
        itoa( WeaponCurrentAmmo[ w ], WeaponHudText, 10 );
        print_at( 125, y + 5, WeaponHudText );
        set_multiply_color( make_color_rgb( 150, 150, 170 ) );
        print_at( 150, y + 5, "/" );
        itoa( WeaponReserveAmmo[ w ], WeaponHudText, 10 );
        print_at( 162, y + 5, WeaponHudText );
    }

    set_multiply_color( color_white );
}

void RenderMoneyHud( int y )
{
    DrawRect( 6, y, 110, 28, make_color_rgba( 12, 10, 22, 190 ) );
    set_multiply_color( make_color_rgb( 120, 255, 140 ) );
    print_at( 13, y + 5, "$" );
    itoa( PlayerMoney, WeaponHudText, 10 );
    print_at( 27, y + 5, WeaponHudText );
    set_multiply_color( color_white );
}

void RenderHudMessage()
{
    if( HudMessageTimer <= 0 )
      return;

    int width = strlen( HudMessageText ) * 10 + 24;
    int x = (screen_width - width) / 2;

    DrawRect( x, 250, width, 28, make_color_rgba( 12, 10, 22, 190 ) );
    set_multiply_color( HudMessageColor );
    print_at( x + 12, 255, HudMessageText );
    set_multiply_color( color_white );
}
