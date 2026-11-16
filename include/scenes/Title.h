// *****************************************************************************
    // start include guard
    #ifndef TITLE_HPP
    #define TITLE_HPP

    // include project headers
    #include "../core/Definitions.h"
// *****************************************************************************

#define MenuNewGame     0
#define MenuSkipIntro   1
#define MenuLanguage    2
#define NumMenuOptions  3

extern int MenuOption;

void InitializeTitle();
void UpdateTitle();
void RenderTitle();


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
