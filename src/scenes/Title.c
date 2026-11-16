// *****************************************************************************
    // include Vircon32 headers
    #include "input.h"
    #include "video.h"
    #include "time.h"

    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
    #include "../../include/ui/Dialog.h"
    #include "../../include/scenes/Intro.h"
    #include "../../include/scenes/Title.h"
// *****************************************************************************

int MenuOption = MenuNewGame;


void InitializeTitle()
{
    select_texture( TextureTitle );
    select_region( 0 );
    define_region_topleft( 0, 0, 639, 359 );
}

void UpdateTitle()
{
    select_gamepad( 0 );

    if( gamepad_up() == 1 )
    {
        MenuOption = (MenuOption + NumMenuOptions - 1) % NumMenuOptions;
        PlaySfx( SoundMenuMove );
    }

    if( gamepad_down() == 1 )
    {
        MenuOption = (MenuOption + 1) % NumMenuOptions;
        PlaySfx( SoundMenuMove );
    }

    bool accept = gamepad_button_a() == 1 || gamepad_button_start() == 1;

    if( MenuOption == MenuLanguage )
    {
        if( accept || gamepad_left() == 1 || gamepad_right() == 1 )
        {
            GameLanguage = 1 - GameLanguage;
            PlaySfx( SoundMenuMove );
        }

        return;
    }

    if( !accept )
      return;

    PlaySfx( SoundMenuAccept );

    if( MenuOption == MenuNewGame )
      StartIntro();
    else
      FinishIntro();
}

int* MenuLabel( int option )
{
    if( GameLanguage == 1 )
    {
        if( option == MenuNewGame )  return "Nueva partida";
        if( option == MenuSkipIntro ) return "Saltar intro";
        return "Idioma: < Espa" "\xF1" "ol >";
    }

    if( option == MenuNewGame )  return "New game";
    if( option == MenuSkipIntro ) return "Skip intro";
    return "Language: < English >";
}

void RenderTitle()
{
    clear_screen( color_black );

    select_texture( TextureTitle );
    select_region( 0 );
    draw_region_at( 0, 0 );

    int panel_x = 190;
    int panel_y = 196;
    int panel_w = 260;
    int panel_h = 112;

    DrawRect( panel_x, panel_y, panel_w, panel_h, make_color_rgba( 10, 8, 20, 215 ) );
    DrawRect( panel_x, panel_y, panel_w, 2, make_color_rgb( 0, 235, 255 ) );
    DrawRect( panel_x, panel_y + panel_h - 2, panel_w, 2, make_color_rgb( 0, 235, 255 ) );

    for( int i = 0; i < NumMenuOptions; i++ )
    {
        int y = panel_y + 14 + i * 32;

        if( i == MenuOption )
        {
            DrawRect( panel_x + 8, y - 4, panel_w - 16, 28, make_color_rgba( 255, 40, 170, 70 ) );
            set_multiply_color( make_color_rgb( 255, 225, 60 ) );

            if( get_frame_counter() % 40 < 28 )
              print_at( panel_x + 16, y, ">" );
        }
        else
          set_multiply_color( make_color_rgb( 190, 180, 220 ) );

        print_at( panel_x + 36, y, MenuLabel( i ) );
    }

    set_multiply_color( make_color_rgba( 255, 255, 255, 140 ) );

    if( GameLanguage == 1 )
      print_at( 150, 322, "A / START: aceptar    Cruceta: elegir" );
    else
      print_at( 170, 322, "A / START: accept    D-pad: choose" );

    set_multiply_color( make_color_rgba( 255, 255, 255, 90 ) );
    print_at( 530, 340, "build 0.99" );
    set_multiply_color( color_white );

    DrawScanlines();
}
