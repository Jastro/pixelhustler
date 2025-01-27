// *****************************************************************************
    // include standard Vircon headers
    #include "math.h"
    #include "time.h"
    #include "input.h"
    #include "video.h"
    
    // include additional headers
    #include "vector2d.h"
    #include "tilemapzoomed.h"
    
    // include project headers
    #include "Definitions.h"
    #include "Globals.h"
    #include "DrawWalls.h"
    
    // include project sources
    #include "Globals.c"
// *****************************************************************************


// ---------------------------------------------------------
//   MAIN FUNCTION
// ---------------------------------------------------------


void main( void )
{
    // ------------------------------------
    // PART 1: DEFINE ALL TEXTURE REGIONS
    // ------------------------------------
    
    
    // regions for ground & roof tileset
    select_texture( TextureGround );
    define_region_matrix( FirstRegionTileSet,  0,0,  TILE_SIZE-1,TILE_SIZE-1,  0,0,  10,5,  0 );
    
    // regions for walls tileset; each wall
    // stripe is divided into its 160 scanlines
    select_texture( TextureWalls );
    
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
    
    // initially place player at map center
    int PlayerX = tilemap_total_width( &MapGround ) / 2 + 2*TILE_SIZE;
    int PlayerY = tilemap_total_height( &MapGround ) / 2;
    
    // ------------------------------------
    // PART 3: MAIN LOOP
    // ------------------------------------
    
    while( true )
    {
        // read inputs from the first gamepad
        select_gamepad( 0 );
        
        // move player character as pressed by player
        int DeltaX, DeltaY;
        gamepad_direction( &DeltaX, &DeltaY );
        
        PlayerX += CHAR_SPEED * DeltaX;
        PlayerY += CHAR_SPEED * DeltaY;
        
        if( PlayerX < 0 ) PlayerX = 0;
        if( PlayerY < 0 ) PlayerY = 0;
        
        // make camera follow the player
        MapGround.camera_position.x = PlayerX;
        MapGround.camera_position.y = PlayerY;
        
        // buttons A and B change zoom level
        if( gamepad_button_a() > 0 && FloorZ < 7*TILE_SIZE) FloorZ += 2;
        if( gamepad_button_b() > 0 && FloorZ > 4*TILE_SIZE) FloorZ -= 2;
        
        MapGround.camera_zoom = ZToScale( FloorZ );
        MapRoofs.camera_zoom = ZToScale( FloorZ - WALL_HEIGHT );
        
        // in this case, we want to restrict camera placement
        // so that, even when the player is near map bounds,
        // the screen view will never reach out of the map
        tilemap_clip_camera_position( &MapGround );
        MapRoofs.camera_position = MapGround.camera_position;
        
        // render tile map taking camera into account
        clear_screen( color_black );
        
        // - - - - - - - - - - - - -
        // Render the scene
        
        // 1) Draw ground
        tilemap_draw_from_camera( &MapGround );
        
        // 2) Draw character
        // draw character shadow
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
        int PlayerScreenX = PlayerX;
        int PlayerScreenY = PlayerY;
        tilemap_convert_position_to_screen( &MapGround, &PlayerScreenX, &PlayerScreenY );
        
        // now draw the character at the converted position
        select_region( RegionCharacter );
        float CharacterScale = MapZoom + 0.5 * (MapZoom - 1.0) * (MapZoom - 1.0);
        set_drawing_scale( CharacterScale, CharacterScale );
        draw_region_zoomed_at( PlayerScreenX, PlayerScreenY );
        
        // 3) Draw walls
        DrawWalls();
        
        // 4) Draw roofs
        tilemap_draw_from_camera( &MapRoofs );
        
        // wait until next frame
        end_frame();
    }
}