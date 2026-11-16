// *****************************************************************************
    // include Vircon32 headers
    #include "input.h"
    #include "video.h"
    #include "string.h"
    #include "math.h"
    #include "time.h"

    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
    #include "../../include/ui/Dialog.h"
    #include "../../include/ui/ChatTexts.h"
    #include "../../include/scenes/Intro.h"
    #include "../../include/gameplay/Objective.h"
// *****************************************************************************

int GameScene = SceneTitle;

int IntroState = IntroStateRoom;
int IntroTimer = 0;
float IntroFlash = 0;
int ToastsShown = 0;
int[ NumChatToasts ] ToastFrames = { 100, 140, 168, 188, 202 };

int[ 8 ] ChatScript = { 1, 2, 3, 4, 5, -1, 6, -2 };
int ChatScriptPos = 0;
int ChatShown = 0;
int ChatCurrent = 0;
int ChatPhase = ChatPhaseTyping;
int ChatPhaseTimer = 0;
int ChatChars = 0;
float ChatHighlight = 0;

int[ 256 ] ChatVisibleText;


void InitializeIntro()
{
    select_texture( TextureIntro );
    select_region( RegionIntroRoom );
    define_region( 0, 0, 639, 359, 430, 134 );
    select_region( RegionIntroLight );
    define_region( 0, 360, 639, 719, 430, 494 );

    select_texture( TextureChat );
    select_region( RegionChatBackground );
    define_region_topleft( 0, 0, 639, 359 );

    for( int i = 0; i < NumChatUsers; i++ )
    {
        int x = 650 + (i % 4) * 30;
        int y = 2 + (i / 4) * 30;
        select_region( RegionFirstAvatar + i );
        define_region_topleft( x, y, x + 25, y + 25 );
    }

    select_region( RegionToast );
    define_region_topleft( 650, 70, 989, 115 );
}

// ---------------------------------------------------------

bool AnyButtonPressed()
{
    return gamepad_button_a() == 1 || gamepad_button_b() == 1 ||
           gamepad_button_x() == 1 || gamepad_button_y() == 1;
}

int CountLines( int* text )
{
    int lines = 1;

    while( *text )
    {
        if( *text == '\n' )
          lines++;

        text++;
    }

    return lines;
}

// ---------------------------------------------------------

void BeginChatStep()
{
    int step = ChatScript[ ChatScriptPos ];
    ChatPhaseTimer = 0;

    if( step >= 0 )
    {
        ChatCurrent = step;
        ChatPhase = ChatPhaseTyping;
        ChatChars = 0;
    }
    else if( step == -1 )
    {
        QueueDialog( &DW_ChipReact1 );
        QueueDialog( &DW_ChipReact2 );
        StartDialogSequence();
        ChatPhase = ChatPhaseDialog;
    }
    else
    {
        IntroState = IntroStateFadeOut;
        IntroTimer = 0;
    }
}

void NextChatStep()
{
    ChatScriptPos++;
    BeginChatStep();
}

void StartIntro()
{
    GameScene = SceneIntro;
    IntroState = IntroStateRoom;
    IntroTimer = 0;
    IntroFlash = 0;
    ToastsShown = 0;
    ChatScriptPos = 0;
    ChatShown = 0;
    ChatHighlight = 0;
}

void FinishIntro()
{
    ResetDialog();
    StartInHome();
    StartObjective();
}

// ---------------------------------------------------------

void UpdateChat()
{
    ChatPhaseTimer++;
    int length = strlen( ChatMessages[ ChatCurrent ].texts[ GameLanguage ] );

    if( ChatPhase == ChatPhaseDialog )
    {
        UpdateDialog();

        if( !DialogActive )
          NextChatStep();
    }
    else if( ChatPhase == ChatPhaseTyping )
    {
        if( ChatPhaseTimer >= 55 || AnyButtonPressed() )
        {
            ChatPhase = ChatPhaseReveal;
            ChatPhaseTimer = 0;
            ChatShown = ChatCurrent + 1;
            ChatChars = 0;
            ChatHighlight = 1;
        }
    }
    else if( ChatPhase == ChatPhaseReveal )
    {
        ChatChars += 2;

        if( AnyButtonPressed() )
          ChatChars = length;

        if( ChatChars >= length )
        {
            ChatChars = length;
            ChatPhase = ChatPhaseWait;
            ChatPhaseTimer = 0;
        }
    }
    else
    {
        if( ChatPhaseTimer >= 120 || AnyButtonPressed() )
          NextChatStep();
    }
}

void UpdateIntro()
{
    select_gamepad( 0 );

    if( gamepad_button_start() == 1 )
    {
        FinishIntro();
        return;
    }

    IntroTimer++;
    IntroFlash *= 0.9;
    ChatHighlight *= 0.93;

    if( IntroState == IntroStateRoom )
    {
        if( ToastsShown < NumChatToasts && IntroTimer >= ToastFrames[ ToastsShown ] )
        {
            ToastsShown++;
            IntroFlash = 1;
        }

        if( IntroTimer >= 290 )
        {
            IntroState = IntroStateZoom;
            IntroTimer = 0;
        }
    }
    else if( IntroState == IntroStateZoom )
    {
        if( IntroTimer >= 90 )
        {
            IntroState = IntroStateChat;
            IntroTimer = 0;
            ChatShown = 1;
            ChatScriptPos = 0;
            BeginChatStep();
        }
    }
    else if( IntroState == IntroStateChat )
    {
        UpdateChat();
    }
    else if( IntroTimer >= 45 )
    {
        FinishIntro();
    }
}

// ---------------------------------------------------------

void DrawRoom( float scale, int x, int y )
{
    set_blending_mode( blending_alpha );
    set_multiply_color( color_white );
    select_texture( TextureIntro );
    select_region( RegionIntroRoom );
    set_drawing_scale( scale, scale );
    draw_region_zoomed_at( x, y );

    int frame = get_frame_counter();
    int light = 95 + 25 * sin( frame * 0.07 ) + 12 * sin( frame * 0.31 ) + IntroFlash * 130;
    if( light > 255 ) light = 255;
    int flash = IntroFlash * 160;

    set_blending_mode( blending_add );
    select_region( RegionIntroLight );
    set_multiply_color( make_color_rgba( 255, 255, 255, light ) );
    draw_region_zoomed_at( x, y );

    if( IntroFlash > 0.02 )
    {
        set_multiply_color( make_color_rgba( 255, 60, 170, flash ) );
        draw_region_zoomed_at( x, y );
    }

    set_blending_mode( blending_alpha );
    set_multiply_color( color_white );
}

void DrawToasts()
{
    int first = ToastsShown - 3;
    if( first < 0 ) first = 0;

    for( int i = ToastsShown - 1; i >= first; i-- )
    {
        int slot = ToastsShown - 1 - i;
        int y = 44 + slot * 50;
        ChatToast* toast = &ChatToasts[ i ];

        select_texture( TextureChat );
        select_region( RegionToast );
        draw_region_at( 262, y );

        select_region( RegionFirstAvatar + toast->user );
        draw_region_at( 268, y + 10 );

        set_multiply_color( ChatUserColors[ toast->user ] );
        print_at( 302, y + 4, ChatUserNames[ toast->user ] );
        set_multiply_color( color_white );
        print_at( 302, y + 23, toast->texts[ GameLanguage ] );
    }
}

void DrawChat()
{
    select_texture( TextureChat );
    select_region( RegionChatBackground );
    draw_region_at( 0, 0 );

    int bottom = 300;

    for( int i = ChatShown - 1; i >= 0; i-- )
    {
        int* text = ChatMessages[ i ].texts[ GameLanguage ];
        int user = ChatMessages[ i ].user;
        int height = 28 + CountLines( text ) * 20;
        int y = bottom - height;

        if( y < 34 )
          break;

        if( i == ChatCurrent && ChatHighlight > 0.02 )
        {
            int highlight = ChatHighlight * 60;
            DrawRect( 186, y - 4, 450, height, make_color_rgba( 255, 40, 170, highlight ) );
        }

        select_texture( TextureChat );
        select_region( RegionFirstAvatar + user );
        draw_region_at( 194, y );

        set_multiply_color( ChatUserColors[ user ] );
        print_at( 228, y, ChatUserNames[ user ] );

        set_multiply_color( make_color_rgb( 120, 110, 150 ) );
        if( GameLanguage == 1 )
          print_at( 238 + strlen( ChatUserNames[ user ] ) * 10, y, "hoy a las 3:07" );
        else
          print_at( 238 + strlen( ChatUserNames[ user ] ) * 10, y, "today at 3:07 AM" );

        set_multiply_color( color_white );

        if( i == ChatCurrent && ChatPhase == ChatPhaseReveal )
        {
            memcpy( ChatVisibleText, text, ChatChars );
            ChatVisibleText[ ChatChars ] = 0;
            print_at( 228, y + 20, ChatVisibleText );
        }
        else
          print_at( 228, y + 20, text );

        bottom = y;
    }

    if( IntroState == IntroStateChat && ChatPhase == ChatPhaseTyping )
    {
        int user = ChatMessages[ ChatCurrent ].user;
        set_multiply_color( ChatUserColors[ user ] );
        print_at( 194, 302, ChatUserNames[ user ] );
        set_multiply_color( make_color_rgb( 150, 140, 180 ) );

        int x = 204 + strlen( ChatUserNames[ user ] ) * 10;
        if( GameLanguage == 1 )
          print_at( x, 302, "est" "\xE1" " escribiendo" );
        else
          print_at( x, 302, "is typing" );

        int dots = (get_frame_counter() / 12) % 4;
        int dx = x + 90;
        if( GameLanguage == 1 ) dx = x + 160;
        for( int d = 0; d < dots; d++ )
          print_at( dx + d * 10, 302, "." );

        set_multiply_color( color_white );
    }
}

void DrawScanlines()
{
    for( int y = 0; y < screen_height; y += 3 )
      DrawRect( 0, y, screen_width, 1, make_color_rgba( 0, 0, 0, 45 ) );
}

void RenderIntro()
{
    clear_screen( color_black );

    if( IntroState == IntroStateRoom )
    {
        DrawRoom( 1, 430, 134 );
        DrawToasts();
    }
    else if( IntroState == IntroStateZoom )
    {
        float t = IntroTimer / 90.0;
        float e = t * t * (3 - 2 * t);
        DrawRoom( 1 + 2.4 * e, 430 + (320 - 430) * e, 134 + (180 - 134) * e );

        float fade = (t - 0.55) / 0.45;

        if( fade > 0 )
        {
            int alpha = fade * 255;
            select_texture( TextureChat );
            select_region( RegionChatBackground );
            set_multiply_color( make_color_rgba( 255, 255, 255, alpha ) );
            draw_region_at( 0, 0 );
            set_multiply_color( color_white );
        }
    }
    else
    {
        DrawChat();

        if( IntroState == IntroStateChat && ChatPhase == ChatPhaseDialog )
        {
            DrawRect( 0, 0, screen_width, screen_height, make_color_rgba( 0, 0, 0, 160 ) );
            RenderDialog();
        }

        if( IntroState == IntroStateFadeOut )
          DrawRect( 0, 0, screen_width, screen_height, make_color_rgba( 0, 0, 0, IntroTimer * 255 / 45 ) );
    }

    DrawScanlines();

    set_multiply_color( make_color_rgba( 255, 255, 255, 110 ) );
    if( GameLanguage == 1 )
      print_at( 470, 4, "START: saltar" );
    else
      print_at( 480, 4, "START: skip" );
    set_multiply_color( color_white );
}
