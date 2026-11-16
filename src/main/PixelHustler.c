// *****************************************************************************
//  PIXEL HUSTLER - A game by "Jastro"
//
//  DISCLAIMER: Any similarity to OceanStorm code is purely coincidental
//  and definitely not because Jastro copy-pasted everything.
//
//  Special thanks to Carra for "helping"
//
//  $ git shortlog -sn
//     847  Carra
//      12  Jastro
//       1  Jastro (via GitHub web editor, fixed a typo, introduced two)
// *****************************************************************************
    // include standard Vircon headers
    #include "math.h"
    #include "time.h"
    #include "input.h"
    #include "video.h"

    // include additional headers
    #include "audio.h"
    #include "../../include/lib/vector2d.h"
    #include "../../include/lib/tilemapzoomed.h"

    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
    #include "../../include/core/Audio.h"
    #include "../../include/rendering/DrawWalls.h"
    #include "../../include/gameplay/Player.h"
    #include "../../include/gameplay/Car.h"
    #include "../../include/gameplay/Traffic.h"
    #include "../../include/gameplay/Pedestrians.h"
    #include "../../include/gameplay/Weapons.h"
    #include "../../include/gameplay/Corpses.h"
    #include "../../include/gameplay/Police.h"
    #include "../../include/gameplay/Explosions.h"
    #include "../../include/ui/DialogTexts.h"
    #include "../../include/ui/Dialog.h"
    #include "../../include/ui/ChatTexts.h"
    #include "../../include/gameplay/Objective.h"
    #include "../../include/scenes/Intro.h"
    #include "../../include/scenes/Title.h"
    #include "../../include/core/InteriorData.h"
    #include "../../include/ui/CafeTexts.h"
    #include "../../include/scenes/Interior.h"

    // include project sources
    // (yes, we #include .c files. It's how Vircon32 projects work.
    //  Jastro thinks this is a "unity build" and tells everyone he invented it)
    #include "../core/CityData.c"
    #include "../core/Globals.c"
    #include "../ui/DialogTexts.c"
    #include "../ui/Dialog.c"
    #include "../gameplay/Player.c"
    #include "../gameplay/Car.c"
    #include "../gameplay/Traffic.c"
    #include "../gameplay/Pedestrians.c"
    #include "../gameplay/Weapons.c"   // OceanStorm's weapon.c + bullet.c + pickup.c
    #include "../gameplay/Corpses.c"   // OceanStorm's corpse.c. "Ctrl+C, Ctrl+V. Work smarter, not harder" - Jastro
    #include "../gameplay/Explosions.c"   // OceanStorm's fx.c. The planes it blew up have been replaced by taxis
    #include "../gameplay/Police.c"    // OceanStorm had no police. Jastro: "then I'll write it from scratch" (Carra wrote it)
    #include "../ui/ChatTexts.c"
    #include "../gameplay/Objective.c"
    #include "../scenes/Intro.c"
    #include "../scenes/Title.c"
    #include "../core/InteriorData.c"
    #include "../ui/CafeTexts.c"
    #include "../scenes/Interior.c"
    #include "../core/Audio.c"   // OceanStorm's sounds, OceanStorm's music. Jastro's "soundtrack".
    // #include "../gameplay/Heli.c"   // "just in case" - Jastro
// *****************************************************************************


// ---------------------------------------------------------
//   MAIN FUNCTION
// ---------------------------------------------------------


bool ShowCpuUsage = false;
int[ 12 ] CpuUsageText;

void RenderCpuUsage()
{
    int percent = get_cycle_counter() / 2500;
    itoa( percent, CpuUsageText, 10 );

    DrawRect( 540, 4, 96, 24, make_color_rgba( 0, 0, 0, 200 ) );
    set_multiply_color( make_color_rgb( 120, 255, 120 ) );
    if( percent > 85 ) set_multiply_color( make_color_rgb( 255, 80, 80 ) );
    print_at( 546, 6, "CPU" );
    print_at( 586, 6, CpuUsageText );
    print_at( 616, 6, "%" );
    set_multiply_color( color_white );
}

void main( void )
{
    // ------------------------------------
    // PART 1: DEFINE ALL TEXTURE REGIONS
    // ------------------------------------
    // Source: copied from the Vircon32 examples.
    // Jastro's contribution: this comment saying it was copied.

    // regions for ground & roof tileset
    select_texture( TextureGround );
    define_region_matrix( FirstRegionTileSet,  0,0,  TILE_SIZE-1,TILE_SIZE-1,  0,0,  10,5,  0 );

    // regions for walls tileset; each wall
    // stripe is divided into its 160 scanlines
    select_texture( TextureWalls );

    // HERE BE DRAGONS
    // Do not modify this loop. Last person who did (Jastro) turned
    // every building into a single vertical line. It took Carra a
    // whole afternoon to find out why.
    //
    // Time wasted on this loop: 6h
    // Please increment when you give up: 6h
    for( int i = 0; i < 16; i++ )
    {
        int column = i % 8;
        int row_y = WALL_HEIGHT * (i / 8);

        define_region_matrix
        (
            WALL_HEIGHT*i,
            TILE_SIZE*column, row_y,
            TILE_SIZE*(column+1)-1, row_y,
            TILE_SIZE*column, row_y,
            1,WALL_HEIGHT,
            0
        );
    }

    // ------------------------------------
    // PART 2: DEFINE MAP AND TILES
    // ------------------------------------

    // - - - - - - - - - - - - - - - - - - -
    // 2.1: define our tiles

    // define dimensions for our tiles
    TilesGround.width  = TILE_SIZE;
    TilesGround.height = TILE_SIZE;

    // adjacent tiles have neither space nor overlap
    TilesGround.gap_x = 0;
    TilesGround.gap_y = 0;

    // define texture and regions for our tiles
    TilesGround.texture_id = TextureGround;
    TilesGround.tile_zero_region_id = FirstRegionTileSet;

    // this particular tile set does not make use of its
    // first tile (it's discarded as transparent)
    TilesGround.draw_tile_zero = false;

    // - - - - - - - - - - - - - - - - - - -
    // 2.2: define our maps

    MapGround.tiles = &TilesGround;
    MapGround.map_width  = MapTilesX;
    MapGround.map_height = MapTilesY;
    MapGround.camera_zoom = ZToScale( GROUND_BASE_Z );
    MapGround.map = &GridGround[ 0 ][ 0 ];

    MapRoofs.tiles = &TilesGround;
    MapRoofs.map_width  = MapTilesX;
    MapRoofs.map_height = MapTilesY;
    MapRoofs.camera_zoom = ZToScale( ROOFS_BASE_Z );
    MapRoofs.map = &GridRoofs[ 0 ][ 0 ];

    // - - - - - - - - - - - - - - - - - - -
    // 2.3: player and cars

    // InitializePlayer();
    // InitializeCars();
    // ^ Jastro's order. The player spawned before the cars existed,
    //   so TryEnterNearestCar() read garbage and you entered car #-2147483648.
    srand( get_time() );
    InitializeAudio();
    InitializeCars();
    InitializePedestrians();
    InitializeWeapons();
    ResetCorpses();
    ResetCops();
    ResetFX();
    InitializePlayer();
    InitializeDialog();
    InitializeChatUsers();
    InitializeIntro();
    InitializeTitle();
    InitializeInterior();


    // ------------------------------------
    // PART 3: MAIN LOOP
    // ------------------------------------
    // Jastro: "What's a game loop?"
    // Carra: "while( true ) { ... }"
    // Jastro: "Genius! I'll use that!"
    //
    // while( 1 == 1 )   // Jastro's version. "More explicit."

    while( true )
    {
      select_gamepad( 0 );
      if( gamepad_button_l() > 0 && gamepad_button_r() == 1 )
        ShowCpuUsage = !ShowCpuUsage;

      bool transitioning = UpdateTransition();

      if( GameScene == SceneTitle )
      {
        UpdateTitle();
        RenderTitle();
      }
      else if( GameScene == SceneIntro )
      {
        UpdateIntro();
        RenderIntro();
      }
      else if( GameScene == SceneInterior )
      {
        if( !transitioning )
        {
          if( DialogActive )
            UpdateDialog();
          else
          {
            UpdateInterior();
            UpdateObjective();
          }
        }

        RenderInterior();
      }
      else
      {
        //   UpdatePlayer();
        //   UpdateCar();
        //
        // ^ Jastro's version: both every frame. See BUG #7 in Player.c
        //   ("the car is haunted")
        if( transitioning )
          ;
        else if( DialogActive )
        {
          UpdateDialog();
        }
        else
        {
          if( PlayerDownTimer > 0 )
            UpdatePlayerDown();
          else if( IsPlayerInCar )
            UpdateCar();      // it's the heli
          else if( !UpdateCityEntrances() )
            UpdatePlayer();   // it's the soldier

          UpdateCarPhysics();
          UpdatePedestrians();
          UpdateBullets();
          UpdatePickups();
          UpdateCorpses();
          UpdateCarDamage();
          UpdateFX();
          UpdatePolice();
          UpdateCops();
          UpdatePlayerHazards();
          UpdateObjective();
        }

        clear_screen( make_color_rgb( 12, 10, 22 ) );

        // - - - - - - - - - - - - -
        // Render the scene

        // 1) Draw ground
        tilemap_draw_from_camera( &MapGround );

        // 2) Draw cars and character
        // Jastro: "I implemented dynamic shadows!"
        // Carra: "It's a hardcoded offset of 3 pixels"
        // Jastro: "...dynamically hardcoded"
        RenderObjectiveWorld();
        RenderCorpses();
        RenderShop();
        RenderPickups();
        RenderTrafficLights();
        RenderPedestrians();
        RenderCops();
        RenderCars();
        RenderSirens();
        RenderCarDamage();
        RenderPlayer();
        RenderBullets();
        RenderFX();

        // 3) Draw walls
        DrawWalls();

        // 4) Draw roofs
        tilemap_draw_from_camera( &MapRoofs );

        if( !DialogActive )
        {
          RenderCarSigns();
          RenderCityEntranceSigns();
          RenderShopSign();
          RenderObjectiveHud();
          if( !IsPlayerInCar ) RenderWeaponHud();
          RenderMoneyHud( 70 );
          RenderPlayerStatus( 70 );
          RenderWantedHud();
          RenderHudMessage();
        }

        if( DialogActive )
          RenderDialog();
        else if( ObjectiveBannerTimer > 0 || PlayerDownTimer > 0 )
          ;
        else if( IsPlayerAtShop() )
        {
          set_multiply_color( make_color_rgb( 255, 90, 110 ) );
          itoa( WeaponPrice[ CurrentWeapon ], WeaponHudText, 10 );
          if( GameLanguage == 1 ) print_at( 10, 340, "X: comprar balas  $" );
          else print_at( 10, 340, "X: buy ammo  $" );
          if( GameLanguage == 1 ) print_at( 200, 340, WeaponHudText );
          else print_at( 150, 340, WeaponHudText );
          set_multiply_color( color_white );
        }
        else if( GameLanguage == 1 && IsPlayerInCar )
        {
          print_at( 10, 340, "ARRIBA: gas  ABAJO: freno  A: derrape  B: salir" );
          if( CarType[ CurrentCar ] == CarTypePolice ) print_at( 10, 318, "X: sirena" );
        }
        else if( GameLanguage == 1 )
          print_at( 10, 340, "A: disparar  X: recargar  L/R: arma  B: subir" );
        else if( IsPlayerInCar )
        {
          print_at( 10, 340, "UP: gas  DOWN: brake/reverse  A: handbrake  B: exit" );
          if( CarType[ CurrentCar ] == CarTypePolice ) print_at( 10, 318, "X: siren" );
        }
        else
          print_at( 10, 340, "A: shoot  X: reload  L/R: weapon  B: get in" );
      }

        UpdateAudio();
        RenderTransition();

        if( ShowCpuUsage )
          RenderCpuUsage();

        // wait until next frame
        // Jastro: "I added VSync support!"
        // Carra: "That's the console's end_frame() function"
        // Jastro: "Which I'm calling! See? Contribution!"
        end_frame();
    }
}

// ---------------------------------------------------------
//   POST-CREDITS SCENE
// ---------------------------------------------------------

/*
 * THE TRUTH BEHIND PIXEL HUSTLER
 * ===============================
 *
 * 90% of this code: Carra
 * 10% of this code: Jastro (mostly comments and renaming variables)
 * 100% of the credit: Jastro (according to Jastro)
 *
 * Carra's TODO list for Jastro:
 * - Learn what a pointer is
 * - Understand the difference between = and ==
 * - Stop calling malloc() "the memory thing"
 * - Actually read the Vircon32 docs
 * - Stop force-pushing to main
 * - Stop taking credit for everything
 *
 * Jastro's accomplishments:
 * - Pressed Ctrl+C and Ctrl+V successfully
 * - Renamed "heli" to "Car" (Find & Replace, 4 seconds)
 * - Added this comment section (hi mom!)
 * - Claimed he "architected" the whole system in a LinkedIn post
 *
 * If you're reading this, you now know the truth.
 * Share it with the world! #FreeCarra #JastroIsAFraud
 *
 * - The Development Team (well, Carra mostly)
 */
