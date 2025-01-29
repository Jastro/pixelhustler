#include "walls.h"
#include "world.h"
#include "../utils/globals.h"

void DrawWallLeft( int map_x, float map_y, int wall_tile )
{
    int ground_x = map_x, roof_x = ground_x;
    int ground_y = map_y, roof_y = ground_y;
    tilemap_convert_position_to_screen( &MapGround, &ground_x, &ground_y );
    tilemap_convert_position_to_screen( &MapRoofs, &roof_x, &roof_y );
    
    if( roof_x <= ground_x )
      return;
    
    set_drawing_angle( pi/2 );
    float screen_y_step = (float)(roof_y - ground_y) / (roof_x - ground_x);
    float scale_step = (MapRoofs.camera_zoom - MapGround.camera_zoom) / (roof_x - ground_x);
    float scale = MapGround.camera_zoom; // + 0.01 ??
    
    int last_line_region = WALL_HEIGHT * (wall_tile+1) - 1;
    
    // limit loop to screen width
    if( roof_x >= screen_width )
      roof_x = screen_width - 1;
    
    // draw wall line by line
    float screen_y = ground_y;
    
    for( int screen_x = ground_x; screen_x <= roof_x; ++screen_x )
    {
        // determine wall line
        float DistanceZ = 160.0 / (scale - 0.5);  // = ScaleToZ( scale );
        int wall_line = FloorZ - DistanceZ;
        
        select_region( last_line_region - wall_line );
        set_drawing_scale_x( scale );
        draw_region_rotozoomed_at( screen_x, screen_y );
        
        // iterate
        scale += scale_step;
        screen_y += screen_y_step;
    }
}

// ---------------------------------------------------------

void DrawWallRight( int map_x, float map_y, int wall_tile )
{
    int ground_x = map_x, roof_x = ground_x;
    int ground_y = map_y, roof_y = ground_y;
    tilemap_convert_position_to_screen( &MapGround, &ground_x, &ground_y );
    tilemap_convert_position_to_screen( &MapRoofs, &roof_x, &roof_y );
    
    if( roof_x >= ground_x )
      return;
    
    set_drawing_angle( -pi/2 );
    float screen_y_step = (float)(roof_y - ground_y) / (roof_x - ground_x);
    float scale_step = (MapRoofs.camera_zoom - MapGround.camera_zoom) / (roof_x - ground_x);
    float scale = MapGround.camera_zoom; // + 0.01 ??
    
    int last_line_region = WALL_HEIGHT * (wall_tile+1) - 1;
    
    // limit loop to screen width
    if( ground_x < 0 )
    {
        ground_y -= screen_y_step * ground_x;
        scale -= scale_step * ground_x;
        ground_x = 0;
    }
    
    // draw wall line by line
    set_multiply_color( make_gray(160) );
    float screen_y = ground_y;
    
    for( int screen_x = ground_x; screen_x >= roof_x; --screen_x )
    {
        // determine wall line
        float DistanceZ = 160.0 / (scale - 0.5);  // = ScaleToZ( scale );
        int wall_line = FloorZ - DistanceZ;
        
        select_region( last_line_region - wall_line );
        set_drawing_scale_x( scale );
        draw_region_rotozoomed_at( screen_x, screen_y );
        
        // iterate
        scale -= scale_step;
        screen_y -= screen_y_step;
    }
    
    set_multiply_color( color_white );
}

// ---------------------------------------------------------

void DrawWallTop( int map_x, float map_y, int wall_tile )
{
    int ground_x = map_x, roof_x = ground_x;
    int ground_y = map_y, roof_y = ground_y;
    tilemap_convert_position_to_screen( &MapGround, &ground_x, &ground_y );
    tilemap_convert_position_to_screen( &MapRoofs, &roof_x, &roof_y );
    
    if( roof_y <= ground_y )
      return;
    
    set_drawing_angle( pi );
    float screen_x_step = (float)(roof_x - ground_x) / (roof_y - ground_y);
    float scale_step = (MapRoofs.camera_zoom - MapGround.camera_zoom) / (roof_y - ground_y);
    float scale = MapGround.camera_zoom; // + 0.01 ??
    
    int last_line_region = WALL_HEIGHT * (wall_tile+1) - 1;
    
    // limit loop to screen height
    if( roof_y >= screen_height )
      roof_y = screen_height - 1;
    
    // draw wall line by line
    float screen_x = ground_x;
    
    for( int screen_y = ground_y; screen_y <= roof_y; ++screen_y )
    {
        // determine wall line
        float DistanceZ = 160.0 / (scale - 0.5);  // = ScaleToZ( scale );
        int wall_line = FloorZ - DistanceZ;
        
        select_region( last_line_region - wall_line );
        set_drawing_scale_x( scale );
        draw_region_rotozoomed_at( screen_x, screen_y );
        
        // iterate
        scale += scale_step;
        screen_x += screen_x_step;
    }
}

// ---------------------------------------------------------

void DrawWallBottom( int map_x, float map_y, int wall_tile )
{
    int ground_x = map_x, roof_x = ground_x;
    int ground_y = map_y, roof_y = ground_y;
    tilemap_convert_position_to_screen( &MapGround, &ground_x, &ground_y );
    tilemap_convert_position_to_screen( &MapRoofs, &roof_x, &roof_y );
    
    if( roof_y >= ground_y )
      return;
    
    float screen_x_step = (float)(roof_x - ground_x) / (roof_y - ground_y);
    float scale_step = (MapRoofs.camera_zoom - MapGround.camera_zoom) / (roof_y - ground_y);
    float scale = MapGround.camera_zoom; // + 0.01 ??
    
    int last_line_region = WALL_HEIGHT * (wall_tile+1) - 1;
    
    // limit loop to screen height
    if( ground_y < 0 )
    {
        ground_x -= screen_x_step * ground_y;
        scale -= scale_step * ground_y;
        ground_y = 0;
    }
    
    // draw wall line by line
    set_multiply_color( make_gray(160) );
    float screen_x = ground_x;
    
    for( int screen_y = ground_y; screen_y >= roof_y; --screen_y )
    {
        // determine wall line
        float DistanceZ = 160.0 / (scale - 0.5);  // = ScaleToZ( scale );
        int wall_line = FloorZ - DistanceZ;
        
        select_region( last_line_region - wall_line );
        set_drawing_scale_x( scale );
        draw_region_zoomed_at( screen_x, screen_y );
        
        // iterate
        scale -= scale_step;
        screen_x -= screen_x_step;
    }
    
    set_multiply_color( color_white );
}


// ---------------------------------------------------------
//   GENERAL WALL DRAWING FUNCTION
// ---------------------------------------------------------


void DrawWalls()
{
    select_texture( TextureWalls );
    set_drawing_scale( 1, 1 );
    
    // first determine the range of visible tiles in MapGround
    float topleft_x = -MapGround.camera_position.x * MapGround.camera_zoom + screen_width  / 2;
    float topleft_y = -MapGround.camera_position.y * MapGround.camera_zoom + screen_height / 2;
    
    float tile_step_x = tileset_get_step_x( MapGround.tiles ) * MapGround.camera_zoom;
    float tile_step_y = tileset_get_step_y( MapGround.tiles ) * MapGround.camera_zoom;
    
    int min_tile_x = max( -topleft_x / tile_step_x, 0 );
    int min_tile_y = max( -topleft_y / tile_step_y, 0 );
    int max_tile_x = min( (-topleft_x + screen_width ) / tile_step_x, MapGround.map_width -1 );
    int max_tile_y = min( (-topleft_y + screen_height) / tile_step_y, MapGround.map_height-1 );
    
    for( int tile_y = min_tile_y; tile_y <= max_tile_y; ++tile_y )
    {
        for( int tile_x = min_tile_x; tile_x <= max_tile_x; ++tile_x )
        {
            int current_tile = GridWalls[ tile_y ][ tile_x ];
            if( !current_tile || current_tile > 15 ) continue;
            
            if( current_tile & WALL_LEFT )
              DrawWallLeft( tile_x * TILE_SIZE, tile_y * TILE_SIZE, GridWalls[ tile_y ][ tile_x - 1 ] - 16 );
            
            if( current_tile & WALL_RIGHT )
              DrawWallRight( TILE_SIZE * (tile_x+1) - 1, TILE_SIZE * (tile_y+1) - 1, GridWalls[ tile_y ][ tile_x + 1 ] - 16 );
            
            if( current_tile & WALL_TOP )
              DrawWallTop( TILE_SIZE * (tile_x+1) - 1, tile_y * TILE_SIZE, GridWalls[ tile_y - 1 ][ tile_x ] - 16 );
            
            if( current_tile & WALL_BOTTOM )
              DrawWallBottom( tile_x * TILE_SIZE, TILE_SIZE * (tile_y+1) - 1, GridWalls[ tile_y + 1 ][ tile_x ] - 16 );
        }
    }
}