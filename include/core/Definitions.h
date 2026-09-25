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
    TexturePortraits
};

// names for texture regions
#define FirstRegionTileSet 0
#define RegionCharacter 100   // the smiley. Nobody uses it. Nobody dares delete it.

#define RegionFirstCar      0
#define RegionFirstPed      20
#define FramesPerPed        4
#define RegionEnterSign     40
#define RegionExitSign      41

#define RegionPortraitJastro  0
#define RegionPortraitCarra   1

#define DIALOG_FRAME_W          552
#define DIALOG_FRAME_H          124
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

// tile sizes
#define TILE_SIZE 80
#define WALL_HEIGHT (2 * TILE_SIZE)

// map dimensions, in tiles
#define MapTilesX 14
#define MapTilesY 13

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
#define MaxCars 6

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
