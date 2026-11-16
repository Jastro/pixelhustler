// *****************************************************************************
    // start include guard
    #ifndef DEFINITIONS_HPP
    #define DEFINITIONS_HPP
// *****************************************************************************


// ---------------------------------------------------------
//   DEFINITIONS AND CONSTANTS
// ---------------------------------------------------------
// git blame summary for this file:
//   Carra  ████████████████████████████████████  97%
//   Jastro █                                      3%  (whitespace)


// names for textures
// FIXME(jastro): I added TextureSprites at the top and every texture broke
// FIXED(carra): order matters. Moved it to the bottom. Please read the XML.
enum Textures
{
    TextureGame,
    TextureGround,
    TextureWalls,
    TextureSprites,
    TextureDialog,
    TexturePortraits,
    TextureIntro,
    TextureChat,
    TextureTitle,
    TextureInterior,
    TextureWeapons    // OceanStorm's armory. Moved in without knocking.
};

// names for sounds (order = ROM XML order)
// Jastro: "I composed the soundtrack". Every file is from OceanStorm. Every. Single. One.
// (except the siren. That one is Carra's. The "nee-naw" demo tape was rejected)
enum Sounds
{
    MusicMenu,
    MusicCity,
    MusicChase,
    MusicBoss,
    MusicInterior,
    SoundShoot,
    SoundCopShoot,
    SoundExplosion,
    SoundHurt,
    SoundDeath,
    SoundPickup,
    SoundChangeWeapon,
    SoundCancel,
    SoundHit,
    SoundFall,
    SoundEnterCar,
    SoundEngine,
    SoundLowFuel,
    SoundMenuMove,
    SoundMenuAccept,
    SoundSiren,
    SoundCash,
    SoundReload,
    SoundDoor,
    SoundBusted,
    SoundThud,
    SoundWanted
};

#define ChannelMusic   15
#define ChannelSiren   14
#define ChannelEngine  13

// names for texture regions
#define FirstRegionTileSet 0
#define RegionCharacter 100   // the smiley. Nobody uses it. Nobody dares delete it.

#define RegionFirstCar      0
#define RegionFirstPed      200
#define RegionFirstCorpse   250
#define CORPSE_ATLAS_Y      400
#define FramesPerPed        4
#define RegionEnterSign     40
#define RegionExitSign      41
#define RegionArrow         42
#define RegionRing          43
#define RegionWhite         44
#define RegionLightBox      45
#define RegionLampGlow      46

#define RegionPortraitJastro  0
#define RegionPortraitCarra   1
#define RegionPortraitChip    2
#define RegionPortraitJoyrider 3
#define RegionPortraitUnknown 4
#define NumPortraits          5

#define SpeakerNezbo          100
#define SpeakerJavi           101
#define SpeakerRunig          102
#define SpeakerRidote         103
#define SpeakerRigo           104
#define SpeakerSign           105

#define RegionIntroRoom       0
#define RegionIntroLight      1
#define RegionChatBackground  0
#define RegionFirstAvatar     10
#define RegionToast           20

#define UserChip        0
#define UserJastro      1
#define UserRidote      2
#define UserTotespare   3
#define UserFreak1337   4
#define UserKidandcat   5
#define UserPaletochen  6
#define UserCarra       7
#define NumChatUsers    8

#define SceneIntro  0
#define SceneCity   1
#define SceneTitle  2
#define SceneInterior 3

#define ENTRANCE_REACH_DISTANCE  36.0

#define DIALOG_FRAME_W          552
#define DIALOG_FRAME_H          150
#define DIALOG_PORTRAIT_SIZE    100
#define DIALOG_CHARS_PER_FRAME  1

#define CAR_SPRITE_W  40
#define CAR_SPRITE_H  76
#define PED_SPRITE    32
#define PED_ATLAS_Y   80

#define CarTypeRed     0
#define CarTypeBlue    1
#define CarTypeTaxi    2
#define CarTypePolice  3
#define CarTypeSport   4
#define CarTypeVan     5
#define NumCarTypes    6
// #define CarTypeHelicopter 6   // rejected in code review. "It's a car game" - Carra

#define PedJastro  0
#define PedCarra   1
#define PedCivil1  2
#define PedCivil2  3
#define PedChip    4
#define PedCivil3  5
#define PedCivil4  6
#define PedJoyrider 7
#define PedRigo    8
#define PedCop     9
#define NumPedTypes 10

// tile sizes
#define TILE_SIZE 80
#define WALL_HEIGHT (2 * TILE_SIZE)

// map dimensions, in tiles
#include "CityData.h"

// predefined Z distances
#define GROUND_BASE_Z (4 * TILE_SIZE)
#define ROOFS_BASE_Z (GROUND_BASE_Z - WALL_HEIGHT)

// #define FLOOR_Z_IN_CAR (60 * TILE_SIZE)   // Jastro: "cinematic mode". Entire city = 4 pixels.
#define FLOOR_Z_ON_FOOT  (4 * TILE_SIZE)
#define FLOOR_Z_IN_CAR   (6 * TILE_SIZE)
#define CAMERA_ZOOM_SPEED 0.05

#define CHAR_SPEED 2.0
#define PED_ANIM_SPEED 8
#define PED_RADIUS 8.0

// cars: values from OceanStorm's heli, "fine-tuned"
// PR #12 "Car physics tuning" by jastro
//   - #define CAR_ROTATION_SPEED 0.05
//   + #define CAR_ROTATION_SPEED 0.05
//   Carra: "This diff is empty."
//   Jastro: "I tuned it and it was already perfect."
#define CAR_ROTATION_SPEED 0.05
#define CAR_MOVEMENT_SPEED 3.0       // heli MovementSpeed (no longer used, kept for the museum)
#define CAR_ENTER_DISTANCE 50.0
#define CAR_EXIT_DISTANCE  30.0
#define MaxCars 16
#define NumParkedCars 4

#define AI_CRUISE_SPEED       2.6
#define AI_TURN_SPEED         1.5
#define AI_LOOK_AHEAD         170.0
#define AI_LANE_HALF_WIDTH    30.0
#define AI_WAYPOINT_RADIUS    32.0
#define AI_STUCK_FRAMES       540
#define AI_GHOST_FRAMES       90

#define LIGHT_CYCLE           1000
#define LightGreen            0
#define LightYellow           1
#define LightRed              2

#define MaxPedestrians        14
#define PED_WALK_SPEED        0.7
#define PED_CROSS_SPEED       1.0
#define PED_KNOCK_SPEED       1.4
#define PED_KNOCK_FRAMES      45
#define PedStateWalk          0
#define PedStateWaitCross     1
#define PedStateCrossing      2
#define PedStateKnocked       3

#define CAR_ACCELERATION          0.08
#define CAR_REVERSE_ACCELERATION  0.05
#define CAR_BRAKE                 0.18
#define CAR_MAX_SPEED             5.0
#define CAR_MAX_REVERSE_SPEED     2.0
#define CAR_DRAG                  0.99
#define CAR_ROLLING_FRICTION      0.03
#define CAR_LATERAL_KEEP          0.15
#define CAR_LATERAL_KEEP_DRIFT    0.94
#define CAR_HANDBRAKE_DECEL       0.06
#define CAR_FULL_STEER_SPEED      2.0
#define CAR_HANDBRAKE_STEER       1.6
#define CAR_BOUNCE                0.35
#define CAR_PUSH_TRANSFER         0.5
#define CAR_CIRCLE_RADIUS         17.0
#define CAR_CIRCLE_SPACING        22.0
#define CAMERA_Z_PER_SPEED        30.0
#define CAMERA_EDGE_MARGIN        (1.5 * TILE_SIZE)
#define CAR_EXIT_SIGN_SPEED       0.3

#define RegionBullet        0
#define RegionEnemyBullet   1
#define RegionBlood         2
#define RegionFirstPickup   3

// weapons: OceanStorm's weapon.h, values untouched
// PR #31 "weapons" by jastro  (+3 lines, -0 lines)
//   Carra: "you only changed the #include path"
//   Jastro: "didn't need to touch much. It's called being efficient."
//   Carra: "it's called me writing it two years ago"
#define WeaponPistol      0
#define WeaponShotgun     1
#define WeaponSubmachine  2
#define NumWeapons        3
#define FireModeSingle    0
#define FireModeAuto      1
#define FireModeSpread    2

#define MaxBullets        50
#define PistolDamage      25
#define ShotgunDamage     15
#define SubmachineGunDamage 10

#define MaxCorpses        64
#define CorpseGrowFrames  48
#define CorpseStateAnimating 0
#define CorpseStateStatic    1

#define RegionAmmoBox       7
#define RegionCash          8
#define RegionBadge         9
#define RegionShopSign      10
#define RegionSmoke         11
#define RegionFirstExplosion 12
#define ExplosionFrames     7

#define MaxFX               16
#define FX_FRAME_TIME       4

#define CAR_MAX_HEALTH      150
#define CAR_SMOKE_HEALTH    80
#define CAR_FIRE_HEALTH     35
#define CAR_BURN_RATE       0.12
#define CAR_CRASH_SPEED     2.5
#define CAR_CRASH_DAMAGE    6
#define EXPLOSION_RADIUS    110.0
#define EXPLOSION_CHAIN_FRAMES 12
#define WRECK_RESPAWN_FRAMES 1200

#define MaxCops             6
#define COP_SPEED           1.3
#define COP_HEALTH          50
#define COP_SHOOT_RANGE     220.0
#define COP_DEPLOY_DISTANCE 220.0
#define COP_BULLET_DAMAGE   8
#define COP_ARREST_DISTANCE 24.0
#define COP_ARREST_FRAMES   45
#define COP_PULL_DISTANCE   46.0
#define POLICE_CREW         2
#define PLAYER_STUN_FRAMES  40
#define PLAYER_FLAT_FRAMES  60
#define POLICE_BACKOFF_FRAMES 90
#define POLICE_BACKOFF_BRAKE  35
#define CopStateChase       0
#define CopStateReturn      1

#define MaxPickups        16
#define PickupShotgun     0
#define PickupSubmachine  1
#define PickupAmmo        2
#define PickupCash        3
#define PICKUP_RESPAWN_FRAMES 2700
#define CASH_LIFETIME_FRAMES  1200

#define SHOP_X            640
#define SHOP_Y            940
#define SHOP_DISTANCE     40.0
#define STARTING_MONEY    40

#define PLAYER_MAX_HEALTH     100
#define PLAYER_IMMUNITY_FRAMES 60
#define PLAYER_DOWN_FRAMES    200
#define DownWasted            0
#define DownBusted            1

#define CORPSE_BODY_FRAMES    1800

#define PED_PANIC_FRAMES      300
#define PED_PANIC_RADIUS      280.0

#define MoodNormal        0
#define MoodFlee          1
#define MoodAttack        2
#define MoodPolice        3
#define AI_FLEE_SPEED     4.6
#define AI_CHASE_SPEED    4.4
#define MOOD_FRAMES       600

#define MaxWanted         6
#define MaxPolice         5
#define WANTED_RAM_LEVEL  5
#define CRIME_GRACE_FRAMES 180
#define WANTED_DECAY_FRAMES   720
#define WANTED1_DECAY_FRAMES  300
#define POLICE_SIGHT          380.0

// bit flags for the 4 possible wall sides
// (combined to indicate the walls for each tile)
#define WALL_LEFT   1
#define WALL_RIGHT  2
#define WALL_TOP    4
#define WALL_BOTTOM 8


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
