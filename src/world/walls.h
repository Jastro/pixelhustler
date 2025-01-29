#ifndef WALLS_H
#define WALLS_H

#include "../utils/definitions.h"

// Bit flags for walls
#define WALL_LEFT   1
#define WALL_RIGHT  2
#define WALL_TOP    4
#define WALL_BOTTOM 8

// Wall drawing functions
void DrawWallLeft(int map_x, float map_y, int wall_tile);
void DrawWallRight(int map_x, float map_y, int wall_tile);
void DrawWallTop(int map_x, float map_y, int wall_tile);
void DrawWallBottom(int map_x, float map_y, int wall_tile);
void DrawWalls();

#endif