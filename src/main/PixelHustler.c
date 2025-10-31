// *****************************************************************************
//  PIXEL HUSTLER - A game by "Jastro"
//
//  DISCLAIMER: Any similarity to OceanStorm code is purely coincidental
//  and definitely not because Jastro copy-pasted everything.
//
//  Special thanks to Carra for "helping"
// *****************************************************************************
    // include standard Vircon headers
    #include "math.h"
    #include "time.h"
    #include "input.h"
    #include "video.h"

    // include additional headers
    #include "../../include/lib/vector2d.h"
    #include "../../include/lib/tilemapzoomed.h"

    // include project headers
    #include "../../include/core/Definitions.h"
    #include "../../include/core/Globals.h"
    #include "../../include/rendering/DrawWalls.h"
    #include "../../include/gameplay/Player.h"

    // include project sources
    #include "../core/Globals.c"
    #include "../gameplay/Player.c"
// *****************************************************************************


// ---------------------------------------------------------
//   MAIN FUNCTION
// ---------------------------------------------------------


void main( void )
{
    // ------------------------------------
    // PART 1: DEFINE ALL TEXTURE REGIONS
    // ------------------------------------
    // Jastro: "I'll define the texture regions!"
    // Carra: "Do you know how define_region_matrix works?"
    // Jastro: "...I'll use the same code from the Vircon32 examples"
    // Carra: *sighs*

    // regions for ground & roof tileset
    select_texture( TextureGround );
    define_region_matrix( FirstRegionTileSet,  0,0,  TILE_SIZE-1,TILE_SIZE-1,  0,0,  10,5,  0 );
    
    // regions for walls tileset; each wall
    // stripe is divided into its 160 scanlines
    select_texture( TextureWalls );

    // Jastro: "Carra, what does this loop do?"
    // Carra: "It defines regions for the wall rendering"
    // Jastro: "Oh... how did I code this?"
    // Carra: "You didn't. I did."
    // Jastro: "Right, right... I meant to say 'we' coded this"
    for( int i = 0; i < 8; i++ )
    {
        define_region_matrix
        (
            WALL_HEIGHT*i,
            TILE_SIZE*i, 0,
            TILE_SIZE*(i+1)-1, 0,
            TILE_SIZE*i, 0,
            1,WALL_HEIGHT,
            0
        );
    }
    
    // smiley
    select_texture( TextureGame );
    select_region( RegionCharacter );
    define_region_center( 1,243,  30,272 );
    
    // ------------------------------------
    // PART 2: DEFINE MAP AND TILES
    // ------------------------------------
    
    // - - - - - - - - - - - - - - - - - - -
    // 2.1: define our tiles
    
    // define dimensions for our tiles
    TilesGround.width  = TILE_SIZE;
    TilesGround.height = TILE_SIZE;
    
    // adjacent tiles have neither space nor overlap
    TilesGround.gap_x = 0;
    TilesGround.gap_y = 0;
    
    // define texture and regions for our tiles
    TilesGround.texture_id = TextureGround;
    TilesGround.tile_zero_region_id = FirstRegionTileSet;
    
    // this particular tile set does not make use of its
    // first tile (it's discarded as transparent)
    TilesGround.draw_tile_zero = false;
    
    // - - - - - - - - - - - - - - - - - - -
    // 2.2: define our maps
    
    MapGround.tiles = &TilesGround;
    MapGround.map_width  = MapTilesX;
    MapGround.map_height = MapTilesY;
    MapGround.camera_zoom = ZToScale( GROUND_BASE_Z );
    MapGround.map = &GridGround[ 0 ][ 0 ];
    
    MapRoofs.tiles = &TilesGround;
    MapRoofs.map_width  = MapTilesX;
    MapRoofs.map_height = MapTilesY;
    MapRoofs.camera_zoom = ZToScale( ROOFS_BASE_Z );
    MapRoofs.map = &GridRoofs[ 0 ][ 0 ];
    
    // - - - - - - - - - - - - - - - - - - -
    // 2.3: define our player character

    // Initialize player
    // Jastro: "I made a whole Player.c file!"
    // Carra: "You literally copied heli.c and renamed variables"
    // Jastro: "It's called refactoring, Carra!"
    InitializePlayer();

    // ------------------------------------
    // PART 3: MAIN LOOP
    // ------------------------------------
    // TODO: Add proper game loop structure
    // Jastro: "What's a game loop?"
    // Carra: "while(true) { ... }"
    // Jastro: "Genius! I'll use that!"

    while( true )
    {
        // Update player movement and input
        // Jastro: "This function is so clean!"
        // Carra: "It's just wrapped helicopter code"
        // Jastro: "Shh, nobody will notice"
        UpdatePlayer();
        
        // render tile map taking camera into account
        clear_screen( color_black );
        
        // - - - - - - - - - - - - -
        // Render the scene
        
        // 1) Draw ground
        tilemap_draw_from_camera( &MapGround );
        
        // 2) Draw character
        // Jastro: "I implemented dynamic shadows!"
        // Carra: "It's a hardcoded offset of +8 pixels"
        // Jastro: "...dynamic offsets!"

        // draw character shadow (totally original code, not from any helicopter)
        int ShadowScreenX = PlayerX + 8;
        int ShadowScreenY = PlayerY + 8;
        tilemap_convert_position_to_screen( &MapGround, &ShadowScreenX, &ShadowScreenY );
        select_texture( TextureGame );
        select_region( RegionCharacter );
        set_multiply_color( make_color_rgba(0,0,0,128) );
        float MapZoom = MapGround.camera_zoom;
        set_drawing_scale( MapZoom, MapZoom );
        draw_region_zoomed_at( ShadowScreenX, ShadowScreenY );
        set_multiply_color( color_white );

        // to draw elements in the map, like our character, we first
        // need to convert its map coordinates to screen coordinates
        int PlayerScreenX, PlayerScreenY;
        GetPlayerScreenPosition( &PlayerScreenX, &PlayerScreenY );

        // now draw the character at the converted position
        select_region( RegionCharacter );
        // Jastro: "This scale formula is so complex!"
        // Carra: "It's just a parabola"
        // Jastro: "I know! I calculated it myself!"
        // Carra: "You copied it from the example"
        // Jastro: "...after understanding it deeply"
        float CharacterScale = MapZoom + 0.5 * (MapZoom - 1.0) * (MapZoom - 1.0);
        set_drawing_scale( CharacterScale, CharacterScale );
        draw_region_zoomed_at( PlayerScreenX, PlayerScreenY );
        
        // 3) Draw walls
        // Jastro: "The wall rendering is SO advanced!"
        // Carra: "It's literally in DrawWalls.h with my name in the comments"
        // Jastro: "Our names. Team effort!"
        // Carra: *stares intensely*
        DrawWalls();

        // 4) Draw roofs
        tilemap_draw_from_camera( &MapRoofs );

        // wait until next frame
        // Jastro: "I added VSync support!"
        // Carra: "That's the console's end_frame() function"
        // Jastro: "Which I'm calling! See? Contribution!"
        end_frame();
    }
}

// ---------------------------------------------------------
//   POST-CREDITS SCENE
// ---------------------------------------------------------

/*
 * THE TRUTH BEHIND PIXEL HUSTLER
 * ===============================
 *
 * 90% of this code: Carra
 * 10% of this code: Jastro (mostly comments and renaming variables)
 * 100% of the credit: Jastro (according to Jastro)
 *
 * Carra's TODO list for Jastro:
 * - Learn what a pointer is
 * - Understand the difference between = and ==
 * - Stop calling malloc() "the memory thing"
 * - Actually read the Vircon32 docs
 * - Stop taking credit for everything
 *
 * Jastro's accomplishments:
 * - Pressed Ctrl+C and Ctrl+V successfully
 * - Changed "heli" to "Player" in variable names
 * - Added this comment section (hi mom!)
 * - Claimed he "architected" the whole system
 *
 * If you're reading this, you now know the truth.
 * Share it with the world! #FreeeCarra #JastroIsAFraud
 *
 * - The Development Team (well, Carra mostly)
 */