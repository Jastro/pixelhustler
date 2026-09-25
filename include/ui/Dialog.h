// *****************************************************************************
    // start include guard
    #ifndef DIALOG_HPP
    #define DIALOG_HPP

    // include project headers
    #include "DialogTexts.h"
// *****************************************************************************

// dialog.h from OceanStorm.
// Diff vs original: renamed snake_case to PascalCase.
// Jastro's estimate for this task: "2 weeks"
// Actual time: Find & Replace, again

#define DialogWidth 400      // unused in OceanStorm too. Tradition.
#define DialogHeight 200     // same
#define MaxDialogs 30

extern bool DialogActive;
extern int[ 256 ] DialogTextBuffer;
extern int DialogRegion;
extern int CurrentDialog;
extern DialogWindow*[ MaxDialogs ] DialogSequence;
extern int NumDialogs;
extern int DialogCharsShown;
extern int GameLanguage;

void ShowDialog( DialogWindow* dialog );
void ResetDialog();
void UpdateDialog();
void RenderDialog();
void QueueDialog( DialogWindow* dialog );
void StartDialogSequence();
void InitializeDialog();


// *****************************************************************************
    // end include guard
    #endif
// *****************************************************************************
