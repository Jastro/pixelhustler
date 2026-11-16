// *****************************************************************************
    // include Vircon32 headers
    #include "math.h"
    #include "misc.h"
    #include "video.h"

    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
    #include "../../include/gameplay/Corpses.h"
// *****************************************************************************


// ---------------------------------------------------------
//   CORPSE VARIABLES
// ---------------------------------------------------------
// corpse.c from OceanStorm. Ported by Jastro in record time.
//
//   $ diff OceanStorm/src/game/corpse.c src/gameplay/Corpses.c
//   < select_texture(TextureEnemySoldierDeath);
//   > select_region( RegionFirstCorpse + CorpseType[ i ] );')
//
//   Jastro: "I barely had to touch it. That's not laziness, that's efficiency."
//   Carra: "the death animation was of a soldier with a helmet. The pedestrians are office workers."
//   Jastro: "office workers can wear helmets"

float[ MaxCorpses ] CorpseX;
float[ MaxCorpses ] CorpseY;
float[ MaxCorpses ] CorpseAngle;
float[ MaxCorpses ] CorpseBloodScale;
float[ MaxCorpses ] FinalBloodScale;
float[ MaxCorpses ] CorpseBloodAngle;
int[ MaxCorpses ] CorpseType;
int[ MaxCorpses ] CorpseTint;
int[ MaxCorpses ] CorpseState;
int[ MaxCorpses ] CorpseActive;
int[ MaxCorpses ] CorpseTimer;

// OceanStorm had a "no blood" option in the menu for the sensitive ones.
// Pixel Hustler has the variable. The menu option is "coming soon" (assigned: Carra).
bool BloodEnabled = true;


// ---------------------------------------------------------

void ResetCorpses()
{
    for( int i = 0; i < MaxCorpses; i++ )
      CorpseActive[ i ] = 0;
}

void SpawnCorpse( float x, float y, float angle, int ped_type, int tint )
{
    int free_slot = -1;
    int oldest_offscreen = -1;
    int oldest_onscreen = -1;

    // slot search copied verbatim. "oldest" means "first one found".
    // FIXME(jastro): make it actually pick the oldest
    // WONTFIX(carra): nobody has ever noticed which corpse disappears
    for( int i = 0; i < MaxCorpses; i++ )
    {
        if( !CorpseActive[ i ] )
        {
            free_slot = i;
            break;
        }

        if( CorpseState[ i ] != CorpseStateStatic )
          continue;

        if( !IsMapPointVisible( CorpseX[ i ], CorpseY[ i ], 64 ) )
        {
            if( oldest_offscreen == -1 ) oldest_offscreen = i;
        }
        else if( oldest_onscreen == -1 )
          oldest_onscreen = i;
    }

    int slot = 0;
    if( free_slot != -1 ) slot = free_slot;
    else if( oldest_offscreen != -1 ) slot = oldest_offscreen;
    else if( oldest_onscreen != -1 ) slot = oldest_onscreen;

    CorpseX[ slot ] = x;
    CorpseY[ slot ] = y;
    CorpseAngle[ slot ] = angle;
    CorpseType[ slot ] = ped_type;
    CorpseTint[ slot ] = tint;
    CorpseBloodScale[ slot ] = 0.1;
    FinalBloodScale[ slot ] = 0.4 + (rand() % 400) / 1000.0;
    CorpseBloodAngle[ slot ] = (rand() % 628) / 100.0;
    CorpseTimer[ slot ] = 0;
    CorpseState[ slot ] = CorpseStateAnimating;
    CorpseActive[ slot ] = 1;
}

void UpdateCorpses()
{
    for( int i = 0; i < MaxCorpses; i++ )
    {
        if( !CorpseActive[ i ] )
          continue;

        // GTA2 rule, added by Carra: bodies get picked up, blood never gets cleaned.
        // Jastro: "just like my kitchen"
        if( CorpseTimer[ i ] <= CORPSE_BODY_FRAMES + 60 )
          CorpseTimer[ i ]++;

        if( CorpseState[ i ] != CorpseStateAnimating )
          continue;

        // the blood puddle grows. OceanStorm tied it to the 4-frame death animation.
        // We have no death animation. The puddle grows anyway. Out of habit.
        CorpseBloodScale[ i ] += FinalBloodScale[ i ] / CorpseGrowFrames;

        if( CorpseBloodScale[ i ] >= FinalBloodScale[ i ] )
        {
            CorpseBloodScale[ i ] = FinalBloodScale[ i ];
            CorpseState[ i ] = CorpseStateStatic;
        }
    }
}

// CorpseStateHidden from OceanStorm was removed.
// It stored whether the corpse was on screen, computed every frame,
// so that render could skip it... by checking if it was on screen.
// Carra: "we check it here directly". Jastro: "that's what I said"

void RenderCorpses()
{
    for( int i = 0; i < MaxCorpses; i++ )
    {
        if( !CorpseActive[ i ] || !IsMapPointVisible( CorpseX[ i ], CorpseY[ i ], 40 ) )
          continue;

        if( BloodEnabled )
        {
            select_texture( TextureWeapons );
            select_region( RegionBlood );
            DrawSpriteInMap( CorpseX[ i ], CorpseY[ i ], CorpseBloodAngle[ i ], CorpseBloodScale[ i ] );
        }

        int alpha = 255;
        int age = CorpseTimer[ i ] - CORPSE_BODY_FRAMES;

        if( age >= 60 ) continue;
        if( age > 0 ) alpha = 255 - age * 4;

        select_texture( TextureSprites );
        select_region( RegionFirstCorpse + CorpseType[ i ] );
        int tint = CorpseTint[ i ];
        set_multiply_color( make_color_rgba( (tint & 255) * 4 / 5, ((tint >> 8) & 255) * 4 / 5, ((tint >> 16) & 255) * 4 / 5, alpha ) );
        DrawSpriteInMap( CorpseX[ i ], CorpseY[ i ], CorpseAngle[ i ], 1.0 );
        set_multiply_color( color_white );
    }
}
