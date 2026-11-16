// *****************************************************************************
    // include Vircon32 headers
    #include "audio.h"
    #include "math.h"
    #include "time.h"

    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
    #include "../../include/core/Audio.h"
    #include "../../include/gameplay/Player.h"
    #include "../../include/gameplay/Car.h"
    #include "../../include/gameplay/Traffic.h"
    #include "../../include/gameplay/Police.h"
    #include "../../include/gameplay/Explosions.h"
    #include "../../include/scenes/Intro.h"
// *****************************************************************************

int CurrentMusic = -1;
int LowFuelTimer = 0;
int LastSfxFrame = -1;
int LastSfxSound = -1;


void InitializeAudio()
{
    for( int i = 0; i < sound_channels; i++ )
    {
        select_channel( i );
        set_channel_volume( 0.35 );
    }

    select_channel( ChannelEngine );
    set_channel_volume( 0.18 );

    for( int m = MusicMenu; m <= MusicInterior; m++ )
    {
        select_sound( m );
        set_sound_loop( true );
    }

    select_sound( SoundSiren );
    set_sound_loop( true );
    select_sound( SoundEngine );
    set_sound_loop( true );
}

void PlaySfx( int sound )
{
    int frame = get_frame_counter();

    if( frame == LastSfxFrame && sound == LastSfxSound )
      return;

    LastSfxFrame = frame;
    LastSfxSound = sound;
    play_sound( sound );
}

void PlaySfxAt( int sound, float x, float y )
{
    if( GameScene == SceneCity && IsMapPointVisible( x, y, 60 ) )
      PlaySfx( sound );
}

void PlayMusic( int track )
{
    if( CurrentMusic == track )
      return;

    CurrentMusic = track;
    play_sound_in_channel( track, ChannelMusic );
}

void UpdateCityLoops()
{
    bool driving = IsPlayerInCar && PlayerDownTimer <= 0 && CurrentCar >= 0;

    if( driving )
    {
        if( get_channel_state( ChannelEngine ) != channel_playing )
          play_sound_in_channel( SoundEngine, ChannelEngine );

        select_channel( ChannelEngine );
        set_channel_speed( 0.6 + fabs( CarSpeed[ CurrentCar ] ) * 0.12 );

        if( CarHealth[ CurrentCar ] < CAR_FIRE_HEALTH )
        {
            LowFuelTimer--;

            if( LowFuelTimer <= 0 )
            {
                PlaySfx( SoundLowFuel );
                LowFuelTimer = 80;
            }
        }
    }
    else
      stop_channel( ChannelEngine );

    float nearest = 99999;

    for( int i = 0; i < MaxCars; i++ )
    {
        bool siren = (CarIsAI[ i ] && CarMood[ i ] == MoodPolice) || CarSirenOn[ i ];
        if( !siren || CarWrecked[ i ] ) continue;

        float dx = CarX[ i ] - PlayerX;
        float dy = CarY[ i ] - PlayerY;
        float d = sqrt( dx * dx + dy * dy );

        if( d < nearest ) nearest = d;
    }

    if( nearest < 650 )
    {
        if( get_channel_state( ChannelSiren ) != channel_playing )
          play_sound_in_channel( SoundSiren, ChannelSiren );

        select_channel( ChannelSiren );
        set_channel_volume( 0.05 + 0.3 * (1 - nearest / 650) );
    }
    else
      stop_channel( ChannelSiren );
}

void UpdateAudio()
{
    if( GameScene == SceneTitle || GameScene == SceneIntro )
      PlayMusic( MusicMenu );
    else if( GameScene == SceneInterior )
      PlayMusic( MusicInterior );
    else if( WantedLevel >= WANTED_RAM_LEVEL )
      PlayMusic( MusicBoss );
    else if( WantedLevel > 0 )
      PlayMusic( MusicChase );
    else
      PlayMusic( MusicCity );

    if( GameScene == SceneCity )
      UpdateCityLoops();
    else
    {
        stop_channel( ChannelEngine );
        stop_channel( ChannelSiren );
    }
}
