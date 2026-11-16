// *****************************************************************************
    // start include guard
    #ifndef AUDIO_HPP
    #define AUDIO_HPP

    // include project headers
    #include "Definitions.h"
// *****************************************************************************

void InitializeAudio();
void PlaySfx( int sound );
void PlaySfxAt( int sound, float x, float y );
void UpdateAudio();


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
