// *****************************************************************************
    // include Vircon32 headers
    #include "input.h"
    #include "video.h"
    #include "string.h"
    #include "misc.h"
    #include "time.h"

    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/ui/Dialog.h"
// *****************************************************************************

// *****************************************************************************
//  DIALOGS
//  dialog.c from OceanStorm, plus the typewriter effect.
//
//  Typewriter effect, attempt #1 (Jastro):
//    for( int i = 0; i < strlen( text ); i++ ) { print_at( ... ); end_frame(); }
//  The whole game froze while the text was typing. Jastro called it
//  "dramatic tension". Carra moved it to UpdateDialog().
// *****************************************************************************

bool DialogActive = false;
int[ 256 ] DialogTextBuffer;
int DialogRegion;

int CurrentDialog = 0;
DialogWindow*[ MaxDialogs ] DialogSequence;
int NumDialogs = 0;
int DialogCharsShown = 0;

// 0 = English, 1 = Spanish (same as game_language in OceanStorm)
int GameLanguage = 1;

int[ 256 ] DialogVisibleText;


void InitializeDialog()
{
    // OceanStorm: define_region( 0, 0, UIDialogFrameWidth, UIDialogFrameHeight, ... )
    // The frame was 552 px wide and the region 553. Off by one since 2024.
    // Nobody noticed. Jastro: "see? nobody notices anything"
    select_texture( TextureDialog );
    select_region( 0 );
    define_region( 0, 0, DIALOG_FRAME_W - 1, DIALOG_FRAME_H - 1, DIALOG_FRAME_W / 2, DIALOG_FRAME_H / 2 );

    select_texture( TexturePortraits );
    define_region_matrix
    (
        RegionPortraitJastro,
        0, 0,
        DIALOG_PORTRAIT_SIZE - 1, DIALOG_PORTRAIT_SIZE - 1,
        DIALOG_PORTRAIT_SIZE / 2, DIALOG_PORTRAIT_SIZE / 2,
        NumPortraits, 1,
        0
    );
}

void ResetDialog()
{
    NumDialogs = 0;
    CurrentDialog = 0;
    DialogActive = false;
    memset( DialogSequence, NULL, sizeof( DialogSequence ) );
}

void QueueDialog( DialogWindow* dialog )
{
    if( NumDialogs < MaxDialogs )
    {
        DialogSequence[ NumDialogs ] = dialog;
        NumDialogs++;
    }
}

void StartDialogSequence()
{
    if( NumDialogs > 0 )
    {
        CurrentDialog = 0;
        ShowDialog( DialogSequence[ 0 ] );
    }
}

void ShowDialog( DialogWindow* dialog )
{
    // play_sound( SoundMenuMove );   // no sounds yet. Jastro does the beeps with his mouth in playtests.
    DialogActive = true;
    strcpy( DialogTextBuffer, dialog->texts[ GameLanguage ] );
    DialogRegion = dialog->portrait_region;
    DialogCharsShown = 0;
}

void UpdateDialog()
{
    if( !DialogActive ) return;

    int text_length = strlen( DialogTextBuffer );

    if( DialogCharsShown < text_length )
      DialogCharsShown += DIALOG_CHARS_PER_FRAME;

    if( gamepad_button_a() == 1 || gamepad_button_b() == 1 ||
        gamepad_button_x() == 1 || gamepad_button_y() == 1 ||
        gamepad_button_start() == 1 )
    {
        // primero esperar un frame para que no
        // se junten pulsaciones por accidente
        // (original OceanStorm comment. Carra wrote it. In Spanish. At 3 AM.)
        end_frame();

        if( DialogCharsShown < text_length )
        {
            DialogCharsShown = text_length;
            return;
        }

        CurrentDialog++;

        if( CurrentDialog < NumDialogs )
        {
            ShowDialog( DialogSequence[ CurrentDialog ] );
        }
        else
        {
            // play_sound( SoundMenuAccept );
            DialogActive = false;
            NumDialogs = 0;
        }
    }
}

int* PortraitName( int region )
{
    if( region == RegionPortraitJastro )   return "Jastro";
    if( region == RegionPortraitCarra )    return "Carra";
    if( region == RegionPortraitChip )     return "Chip Buster";
    if( region == RegionPortraitJoyrider ) return "Joyrider";
    if( region == SpeakerNezbo )  return "Nezbo";
    if( region == SpeakerJavi )   return "javifugitivo";
    if( region == SpeakerRunig )  return "Runig666";
    if( region == SpeakerRidote ) return "Ridote";
    if( region == SpeakerRigo )   return "R2D2Rigo";
    if( GameLanguage == 1 ) return "Cartel";
    return "Sign";
}

int PortraitColor( int region )
{
    if( region == RegionPortraitJastro )   return make_color_rgb( 60, 220, 120 );
    if( region == RegionPortraitCarra )    return make_color_rgb( 0, 235, 255 );
    if( region == RegionPortraitChip )     return make_color_rgb( 255, 150, 60 );
    if( region == RegionPortraitJoyrider ) return make_color_rgb( 255, 225, 60 );
    if( region == SpeakerRigo )            return make_color_rgb( 170, 255, 120 );
    if( region == SpeakerSign )            return make_color_rgb( 200, 200, 220 );
    return make_color_rgb( 190, 150, 255 );
}

void RenderDialog()
{
    if( !DialogActive ) return;

    int frame_x = 310;
    int frame_y = 279;
    int portrait_x = 103;
    int portrait_y = 268;
    int text_x = 172;
    int text_y = 218;

    set_multiply_color( color_white );
    select_texture( TextureDialog );
    select_region( 0 );
    draw_region_at( frame_x, frame_y );

    if( DialogRegion >= 0 )
    {
        select_texture( TexturePortraits );
        if( DialogRegion >= SpeakerNezbo )
          select_region( RegionPortraitUnknown );
        else
          select_region( DialogRegion );
        draw_region_at( portrait_x, portrait_y );

        int* name = PortraitName( DialogRegion );
        int name_x = portrait_x - strlen( name ) * 5;
        if( name_x < 48 ) name_x = 48;
        set_multiply_color( PortraitColor( DialogRegion ) );
        print_at( name_x, 322, name );
        set_multiply_color( color_white );
    }

    int text_length = strlen( DialogTextBuffer );
    int shown = DialogCharsShown;
    if( shown > text_length ) shown = text_length;

    memcpy( DialogVisibleText, DialogTextBuffer, shown );
    DialogVisibleText[ shown ] = 0;

    print_at( text_x, text_y, DialogVisibleText );

    // OceanStorm drew "Press any button to continue" at y = dialog_y + 124 - 20 = 374.
    // The screen is 360 pixels tall.
    // For a whole game jam, the hint was rendered 14 pixels below the screen.
    // Jastro: "it's there, you just can't see it. Like my contributions."
    if( shown >= text_length && get_frame_counter() % 40 > 12 )
      print_at( 556, 326, ">>" );
}
