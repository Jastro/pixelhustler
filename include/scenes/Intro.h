// *****************************************************************************
    // start include guard
    #ifndef INTRO_HPP
    #define INTRO_HPP

    // include project headers
    #include "../core/Definitions.h"
// *****************************************************************************

#define IntroStateRoom      0
#define IntroStateZoom      1
#define IntroStateChat      2
#define IntroStateFadeOut   3

#define ChatPhaseTyping     0
#define ChatPhaseReveal     1
#define ChatPhaseWait       2
#define ChatPhaseDialog     3

extern int GameScene;

void InitializeIntro();
void StartIntro();
void UpdateIntro();
void RenderIntro();
void FinishIntro();


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
