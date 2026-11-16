// *****************************************************************************
    // include Vircon32 headers
    #include "input.h"
    #include "video.h"
    #include "math.h"
    #include "string.h"
    #include "time.h"

    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
    #include "../../include/core/InteriorData.h"
    #include "../../include/ui/Dialog.h"
    #include "../../include/ui/CafeTexts.h"
    #include "../../include/gameplay/Player.h"
    #include "../../include/gameplay/Car.h"
    #include "../../include/gameplay/Objective.h"
    #include "../../include/scenes/Intro.h"
    #include "../../include/scenes/Interior.h"
// *****************************************************************************

// OceanStorm had no interiors. The soldier walked around islands and the
// most complex "room" was the aircraft carrier deck.
// Jastro: "an interior is just an island with walls"
// Carra: "that's... actually how I made it"
// Jastro: "see? I'm a game designer"

embedded int[ CafeTilesY ][ CafeTilesX ] GridCafe = "obj/MapCafe.vmap";
embedded int[ HomeUpTilesY ][ HomeUpTilesX ] GridHomeUp = "obj/MapHomeUp.vmap";
embedded int[ HomeDownTilesY ][ HomeDownTilesX ] GridHomeDown = "obj/MapHomeDown.vmap";

int CurrentInterior = InteriorCafe;
int* InteriorGrid;
int InteriorW = CafeTilesX;
int InteriorH = CafeTilesY;

tileset TilesInterior;
tilemap MapInterior;

int TransitionKind = TransitionNone;
int TransitionTimer = 0;

float[ NumCafeNpcs ] NpcX = { 140, 140, 260, 140, 260, 460, 320, 380 };
float[ NumCafeNpcs ] NpcY = { 136, 224, 136, 256, 344, 178, 96, 92 };
float[ NumCafeNpcs ] NpcAngle = { 1.5708, -1.5708, 1.5708, 1.5708, -1.5708, 1.5708, 1.5708, 0 };
int[ NumCafeNpcs ] NpcType = { PedCivil1, PedCivil3, PedCivil2, PedCivil4, PedRigo, PedJoyrider, PedCarra, -1 };
int[ NumCafeNpcs ] NpcSpeaker = { SpeakerNezbo, SpeakerJavi, SpeakerRunig, SpeakerRidote, SpeakerRigo, RegionPortraitJoyrider, RegionPortraitCarra, SpeakerSign };

#define NpcJoyrider  5
#define NpcRigo      4
#define NpcCarra     6
#define NpcSign      7

#define CAFE_SPAWN_X  320
#define CAFE_SPAWN_Y  336
#define CAFE_EXIT_Y   376
#define INTERIOR_SPEED 1.7
#define INTERIOR_TALK_DISTANCE 46

// Chip's house. Jastro's design doc for the stairs: "you step on them and you're downstairs".
// Carra: "that's a teleporter". Jastro: "that's what stairs are, if you think about it".
#define HOME_DESK_X      140
#define HOME_DESK_Y      140
#define HOME_UP_STAIRS_X 460
#define HOME_UP_STAIRS_Y 240
#define HOME_DOWN_STAIRS_X 460
#define HOME_DOWN_STAIRS_Y 140
#define HOME_DOOR_X      280
#define HOME_DOOR_Y      252


void InitializeInterior()
{
    select_texture( TextureInterior );
    define_region_matrix( 0,  0,0,  INTERIOR_TILE-1,INTERIOR_TILE-1,  0,0,  10,5,  0 );

    TilesInterior.width = INTERIOR_TILE;
    TilesInterior.height = INTERIOR_TILE;
    TilesInterior.gap_x = 0;
    TilesInterior.gap_y = 0;
    TilesInterior.texture_id = TextureInterior;
    TilesInterior.tile_zero_region_id = 0;
    TilesInterior.draw_tile_zero = false;

    MapInterior.tiles = &TilesInterior;
    MapInterior.camera_zoom = 1.0;
    SetInterior( InteriorCafe );
}

void SetInterior( int interior )
{
    CurrentInterior = interior;

    if( interior == InteriorHomeUp )
    {
        InteriorGrid = &GridHomeUp[ 0 ][ 0 ];
        InteriorW = HomeUpTilesX;
        InteriorH = HomeUpTilesY;
    }
    else if( interior == InteriorHomeDown )
    {
        InteriorGrid = &GridHomeDown[ 0 ][ 0 ];
        InteriorW = HomeDownTilesX;
        InteriorH = HomeDownTilesY;
    }
    else
    {
        InteriorGrid = &GridCafe[ 0 ][ 0 ];
        InteriorW = CafeTilesX;
        InteriorH = CafeTilesY;
    }

    MapInterior.map = InteriorGrid;
    MapInterior.map_width = InteriorW;
    MapInterior.map_height = InteriorH;
}

void PlacePlayerInInterior( int interior, float x, float y, float angle )
{
    GameScene = SceneInterior;
    SetInterior( interior );
    PlayerX = x;
    PlayerY = y;
    PlayerAngle = angle;
    PlayerFrame = 0;
    IsPlayerInCar = false;
    UpdateInteriorCamera();
}

void StartInHome()
{
    PlacePlayerInInterior( InteriorHomeUp, HOME_DESK_X, HOME_DESK_Y, -pi / 2 );
}

// ---------------------------------------------------------
//   TRANSITIONS
// ---------------------------------------------------------

void StartTransition( int kind )
{
    if( TransitionKind != TransitionNone )
      return;

    TransitionKind = kind;
    TransitionTimer = 0;
    PlaySfx( SoundDoor );
}

void UpdateInteriorCamera()
{
    MapInterior.camera_position.x = PlayerX;
    MapInterior.camera_position.y = PlayerY;
    tilemap_clip_camera_position( &MapInterior );

    if( InteriorW * INTERIOR_TILE <= screen_width )
      MapInterior.camera_position.x = InteriorW * INTERIOR_TILE / 2;

    if( InteriorH * INTERIOR_TILE <= screen_height )
      MapInterior.camera_position.y = InteriorH * INTERIOR_TILE / 2;
}

void PerformTransition()
{
    if( TransitionKind == TransitionEnterCafe )
    {
        PlacePlayerInInterior( InteriorCafe, CAFE_SPAWN_X, CAFE_SPAWN_Y, -pi / 2 );
        OnCafeEntered();
    }
    else if( TransitionKind == TransitionEnterHome )
      PlacePlayerInInterior( InteriorHomeDown, HOME_DOOR_X, HOME_DOOR_Y, -pi / 2 );
    else if( TransitionKind == TransitionStairsDown )
      PlacePlayerInInterior( InteriorHomeDown, HOME_DOWN_STAIRS_X, HOME_DOWN_STAIRS_Y, pi );
    else if( TransitionKind == TransitionStairsUp )
      PlacePlayerInInterior( InteriorHomeUp, HOME_UP_STAIRS_X, HOME_UP_STAIRS_Y, pi );
    else
    {
        int entrance = EntranceCafe;
        if( TransitionKind == TransitionExitHome ) entrance = EntranceHome;

        GameScene = SceneCity;
        PlayerX = EntranceX[ entrance ];
        PlayerY = EntranceY[ entrance ] + 28;
        PlayerAngle = pi / 2;
        IsPlayerInCar = false;
        TargetFloorZ = FLOOR_Z_ON_FOOT;
        UpdateCamera( PlayerX, PlayerY );
    }
}

bool UpdateTransition()
{
    if( TransitionKind == TransitionNone )
      return false;

    TransitionTimer++;

    if( TransitionTimer == TRANSITION_HALF )
      PerformTransition();

    if( TransitionTimer >= 2 * TRANSITION_HALF )
      TransitionKind = TransitionNone;

    return true;
}

void RenderTransition()
{
    if( TransitionKind == TransitionNone )
      return;

    int alpha = TransitionTimer * 255 / TRANSITION_HALF;
    if( TransitionTimer > TRANSITION_HALF )
      alpha = (2 * TRANSITION_HALF - TransitionTimer) * 255 / TRANSITION_HALF;

    DrawRect( 0, 0, screen_width, screen_height, make_color_rgba( 6, 4, 12, alpha ) );
}

// ---------------------------------------------------------
//   CITY ENTRANCES
// ---------------------------------------------------------

bool IsNearEntrance( int entrance )
{
    if( IsPlayerInCar )
      return false;

    float dx = PlayerX - EntranceX[ entrance ];
    float dy = PlayerY - EntranceY[ entrance ];
    return dx * dx + dy * dy < ENTRANCE_REACH_DISTANCE * ENTRANCE_REACH_DISTANCE;
}

int NearEnterableEntrance()
{
    if( IsNearEntrance( EntranceCafe ) ) return EntranceCafe;
    if( IsNearEntrance( EntranceHome ) ) return EntranceHome;
    return -1;
}

bool UpdateCityEntrances()
{
    select_gamepad( 0 );

    int entrance = NearEnterableEntrance();

    if( entrance >= 0 && gamepad_button_b() == 1 )
    {
        if( entrance == EntranceHome ) StartTransition( TransitionEnterHome );
        else StartTransition( TransitionEnterCafe );
        return true;
    }

    return false;
}

void RenderCityEntranceSigns()
{
    int entrance = NearEnterableEntrance();

    if( entrance < 0 || get_frame_counter() % 40 <= 8 )
      return;

    int screen_x = EntranceX[ entrance ];
    int screen_y = EntranceY[ entrance ];
    tilemap_convert_position_to_screen( &MapGround, &screen_x, &screen_y );

    select_texture( TextureSprites );
    select_region( RegionEnterSign );
    draw_region_at( screen_x, screen_y - 40 );
}

// ---------------------------------------------------------
//   INTERIOR
// ---------------------------------------------------------

bool InteriorSolidAt( float x, float y )
{
    if( x < 0 || y < 0 )
      return true;

    int tile_x = x / INTERIOR_TILE;
    int tile_y = y / INTERIOR_TILE;

    if( tile_x >= InteriorW )
      return true;

    if( tile_y >= InteriorH )
      return false;

    return InteriorTileSolid[ InteriorGrid[ tile_y * InteriorW + tile_x ] ];
}

int InteriorTileAt( float x, float y )
{
    if( x < 0 || y < 0 )
      return 0;

    int tile_x = x / INTERIOR_TILE;
    int tile_y = y / INTERIOR_TILE;

    if( tile_x >= InteriorW || tile_y >= InteriorH )
      return 0;

    return InteriorGrid[ tile_y * InteriorW + tile_x ];
}

bool InteriorBlockedAt( float x, float y )
{
    if( InteriorSolidAt( x - 9, y - 6 ) ) return true;
    if( InteriorSolidAt( x + 9, y - 6 ) ) return true;
    if( InteriorSolidAt( x - 9, y + 9 ) ) return true;
    if( InteriorSolidAt( x + 9, y + 9 ) ) return true;

    for( int i = 0; i < NumCafeNpcs; i++ )
    {
        if( CurrentInterior != InteriorCafe ) break;
        if( NpcType[ i ] < 0 ) continue;
        float dx = x - NpcX[ i ];
        float dy = y - NpcY[ i ];
        if( dx * dx + dy * dy < 22 * 22 ) return true;
    }

    return false;
}

int NearestNpc()
{
    if( CurrentInterior != InteriorCafe )
      return -1;

    int nearest = -1;
    float best = INTERIOR_TALK_DISTANCE * INTERIOR_TALK_DISTANCE;

    for( int i = 0; i < NumCafeNpcs; i++ )
    {
        float dx = PlayerX - NpcX[ i ];
        float dy = PlayerY - NpcY[ i ];
        float d2 = dx * dx + dy * dy;

        if( d2 < best )
        {
            best = d2;
            nearest = i;
        }
    }

    return nearest;
}

void TalkToNpc( int npc )
{
    if( npc == NpcJoyrider )
    {
        if( ObjectiveStage <= 1 )
        {
            QueueDialog( &DW_Joy1 );
            QueueDialog( &DW_JoyChip1 );
            QueueDialog( &DW_Joy2 );
            QueueDialog( &DW_JoyChip2 );
            QueueDialog( &DW_Joy3 );
            QueueDialog( &DW_JoyChip3 );
            QueueDialog( &DW_Joy4 );
            QueueDialog( &DW_Joy5 );
            QueueDialog( &DW_JoyChip4 );
            QueueDialog( &DW_Joy6 );
            SetObjectiveStage( 2 );
        }
        else
          QueueDialog( &DW_JoyAgain );
    }
    else if( npc == NpcRigo )
    {
        if( ObjectiveStage < 2 )
          QueueDialog( &DW_RigoEarly );
        else if( ObjectiveStage == 2 )
        {
            QueueDialog( &DW_RigoChip1 );
            QueueDialog( &DW_Rigo1 );
            QueueDialog( &DW_RigoChip2 );
            QueueDialog( &DW_Rigo2 );
            QueueDialog( &DW_RigoChip3 );
            QueueDialog( &DW_Rigo3 );
            QueueDialog( &DW_RigoChip4 );
            QueueDialog( &DW_Rigo4 );
            QueueDialog( &DW_Rigo5 );
            SetObjectiveStage( 3 );
        }
        else
          QueueDialog( &DW_RigoAgain );
    }
    else if( npc == NpcCarra )
    {
        QueueDialog( &DW_CarraChip1 );
        QueueDialog( &DW_Carra1 );
        QueueDialog( &DW_CarraChip2 );
        QueueDialog( &DW_Carra2 );
    }
    else if( npc == NpcSign )
      QueueDialog( &DW_SunySign );
    else if( npc == 0 )
      QueueDialog( &DW_Nezbo );
    else if( npc == 1 )
      QueueDialog( &DW_Javi );
    else if( npc == 2 )
      QueueDialog( &DW_Runig );
    else
      QueueDialog( &DW_Ridote );

    StartDialogSequence();
}

void UpdateInterior()
{
    select_gamepad( 0 );

    if( gamepad_button_b() == 1 )
    {
        int npc = NearestNpc();

        if( npc >= 0 )
        {
            PlayerFrame = 0;
            TalkToNpc( npc );
            return;
        }
    }

    int direction_x, direction_y;
    gamepad_direction( &direction_x, &direction_y );

    float new_x = PlayerX + direction_x * INTERIOR_SPEED;
    if( !InteriorBlockedAt( new_x, PlayerY ) )
      PlayerX = new_x;

    float new_y = PlayerY + direction_y * INTERIOR_SPEED;
    if( !InteriorBlockedAt( PlayerX, new_y ) )
      PlayerY = new_y;

    IsPlayerMoving = (direction_x != 0 || direction_y != 0);

    if( IsPlayerMoving )
    {
        PlayerAngle = atan2( direction_y, direction_x );
        PlayerAnimTimer++;

        if( PlayerAnimTimer >= PED_ANIM_SPEED )
        {
            PlayerAnimTimer = 0;
            PlayerFrame = (PlayerFrame + 1) % FramesPerPed;
        }
    }
    else
    {
        PlayerFrame = 0;
        PlayerAnimTimer = 0;
    }

    int tile = InteriorTileAt( PlayerX, PlayerY );

    if( CurrentInterior == InteriorCafe && PlayerY > CAFE_EXIT_Y )
      StartTransition( TransitionExitCafe );
    else if( CurrentInterior == InteriorHomeDown && PlayerY > InteriorH * INTERIOR_TILE )
      StartTransition( TransitionExitHome );
    else if( tile == InteriorTileStairsDown )
      StartTransition( TransitionStairsDown );
    else if( tile == InteriorTileStairsUp )
      StartTransition( TransitionStairsUp );

    UpdateInteriorCamera();
}

// ---------------------------------------------------------

void DrawPedInInterior( float x, float y, float angle, int ped_type, int frame )
{
    int screen_x = x;
    int screen_y = y;
    tilemap_convert_position_to_screen( &MapInterior, &screen_x, &screen_y );

    select_texture( TextureSprites );
    select_region( RegionFirstPed + ped_type * FramesPerPed + frame );
    set_drawing_scale( 1, 1 );
    set_drawing_angle( angle + pi / 2 );

    set_multiply_color( make_color_rgba( 0, 0, 0, 100 ) );
    draw_region_rotozoomed_at( screen_x + 2, screen_y + 2 );
    set_multiply_color( color_white );
    draw_region_rotozoomed_at( screen_x, screen_y );
}

void RenderInterior()
{
    clear_screen( make_color_rgb( 6, 4, 12 ) );
    tilemap_draw_from_camera( &MapInterior );

    for( int i = 0; i < NumCafeNpcs; i++ )
      if( CurrentInterior == InteriorCafe && NpcType[ i ] >= 0 )
        DrawPedInInterior( NpcX[ i ], NpcY[ i ], NpcAngle[ i ], NpcType[ i ], 0 );

    DrawPedInInterior( PlayerX, PlayerY, PlayerAngle, PedChip, PlayerFrame );

    if( DialogActive )
    {
        RenderDialog();
        return;
    }

    int npc = NearestNpc();

    if( npc >= 0 )
    {
        int screen_x = NpcX[ npc ];
        int screen_y = NpcY[ npc ];
        tilemap_convert_position_to_screen( &MapInterior, &screen_x, &screen_y );

        int* name = PortraitName( NpcSpeaker[ npc ] );
        int width = strlen( name ) * 10 + 44;
        int x = screen_x - width / 2;

        DrawRect( x, screen_y - 46, width, 24, make_color_rgba( 12, 10, 22, 210 ) );
        set_multiply_color( make_color_rgb( 255, 225, 60 ) );
        print_at( x + 6, screen_y - 44, "[B]" );
        set_multiply_color( PortraitColor( NpcSpeaker[ npc ] ) );
        print_at( x + 40, screen_y - 44, name );
        set_multiply_color( color_white );
    }

    RenderObjectiveHud();
}
