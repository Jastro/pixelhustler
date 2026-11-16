// *****************************************************************************
    // include Vircon32 headers
    #include "video.h"
    #include "string.h"
    #include "math.h"
    #include "time.h"

    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
    #include "../../include/ui/Dialog.h"
    #include "../../include/gameplay/Player.h"
    #include "../../include/gameplay/Car.h"
    #include "../../include/gameplay/Objective.h"
    #include "../../include/scenes/Intro.h"
// *****************************************************************************

bool ObjectiveActive = false;
int ObjectiveBannerTimer = 0;
int ObjectiveStage = 0;


void StartObjective()
{
    ObjectiveActive = true;
    ObjectiveStage = 0;
    ObjectiveBannerTimer = 300;
}

void SetObjectiveStage( int stage )
{
    if( stage <= ObjectiveStage )
      return;

    ObjectiveStage = stage;
    ObjectiveBannerTimer = 240;
}

void OnCafeEntered()
{
    if( ObjectiveStage == 0 )
    {
        ObjectiveStage = 1;
        ObjectiveBannerTimer = 0;
    }
}

void UpdateObjective()
{
    if( ObjectiveActive && ObjectiveBannerTimer > 0 )
      ObjectiveBannerTimer--;
}

int ObjectiveTargetEntrance()
{
    if( ObjectiveStage == 0 ) return EntranceCafe;
    if( ObjectiveStage == 3 ) return EntranceJoyzone;
    return -1;
}

void RenderObjectiveWorld()
{
    int target = ObjectiveTargetEntrance();
    if( !ObjectiveActive || target < 0 )
      return;

    float pulse = 1 + 0.15 * sin( get_frame_counter() * 0.12 );

    select_texture( TextureSprites );
    select_region( RegionRing );
    set_blending_mode( blending_add );
    DrawSpriteInMap( EntranceX[ target ], EntranceY[ target ], 0, pulse );
    set_blending_mode( blending_alpha );
}

int* ObjectiveText()
{
    if( GameLanguage == 1 )
    {
        if( ObjectiveStage == 0 ) return "OBJETIVO: Ve al Discord Caf\xE9 y habla con Joyrider.";
        if( ObjectiveStage == 1 ) return "OBJETIVO: Habla con Joyrider.";
        if( ObjectiveStage == 2 ) return "OBJETIVO: Habla con R2D2Rigo.";
        return "OBJETIVO: Ve a la JoyZone (pr\xF3ximamente).";
    }

    if( ObjectiveStage == 0 ) return "OBJECTIVE: Go to the Discord Caf\xE9 and talk to Joyrider.";
    if( ObjectiveStage == 1 ) return "OBJECTIVE: Talk to Joyrider.";
    if( ObjectiveStage == 2 ) return "OBJECTIVE: Talk to R2D2Rigo.";
    return "OBJECTIVE: Go to the JoyZone (coming soon).";
}

int* ObjectiveShortText()
{
    if( GameLanguage == 1 )
    {
        if( ObjectiveStage == 0 ) return "> Ve al Discord Caf\xE9";
        if( ObjectiveStage == 1 ) return "> Habla con Joyrider";
        if( ObjectiveStage == 2 ) return "> Habla con R2D2Rigo";
        return "> Ve a la JoyZone (pr\xF3ximamente)";
    }

    if( ObjectiveStage == 0 ) return "> Go to the Discord Caf\xE9";
    if( ObjectiveStage == 1 ) return "> Talk to Joyrider";
    if( ObjectiveStage == 2 ) return "> Talk to R2D2Rigo";
    return "> Go to the JoyZone (coming soon)";
}

void RenderObjectiveHud()
{
    if( !ObjectiveActive )
      return;

    if( ObjectiveBannerTimer > 0 )
    {
        int* text = ObjectiveText();
        int width = strlen( text ) * 10 + 40;
        int x = (screen_width - width) / 2;
        int alpha = 230;

        if( ObjectiveBannerTimer < 30 )
          alpha = ObjectiveBannerTimer * 230 / 30;

        DrawRect( x, 150, width, 48, make_color_rgba( 12, 10, 22, alpha ) );
        DrawRect( x, 150, width, 2, make_color_rgba( 255, 225, 60, alpha ) );
        DrawRect( x, 196, width, 2, make_color_rgba( 255, 225, 60, alpha ) );

        set_multiply_color( make_color_rgba( 255, 225, 60, alpha ) );
        print_at( x + 20, 164, text );
        set_multiply_color( color_white );
        return;
    }

    int* short_text = ObjectiveShortText();
    DrawRect( 6, 6, strlen( short_text ) * 10 + 14, 28, make_color_rgba( 12, 10, 22, 190 ) );
    set_multiply_color( make_color_rgb( 255, 225, 60 ) );
    print_at( 13, 10, short_text );
    set_multiply_color( color_white );

    int target = ObjectiveTargetEntrance();
    if( GameScene != SceneCity || target < 0 )
      return;

    int player_x = PlayerX;
    int player_y = PlayerY;
    int target_x = EntranceX[ target ];
    int target_y = EntranceY[ target ];
    tilemap_convert_position_to_screen( &MapGround, &player_x, &player_y );
    tilemap_convert_position_to_screen( &MapGround, &target_x, &target_y );

    float dx = target_x - player_x;
    float dy = target_y - player_y;

    if( dx * dx + dy * dy < 90 * 90 )
      return;

    float angle = atan2( dy, dx );
    float distance = 46 + 4 * sin( get_frame_counter() * 0.15 );

    select_texture( TextureSprites );
    select_region( RegionArrow );
    set_drawing_angle( angle + pi / 2 );
    draw_region_rotated_at( player_x + cos( angle ) * distance, player_y + sin( angle ) * distance );
}
