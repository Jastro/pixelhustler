#ifndef GLOBALS_H
#define GLOBALS_H

#include "globals_impl.h"

extern int[MapTilesY][MapTilesX] GridGround;
extern int[MapTilesY][MapTilesX] GridRoofs;
extern int[MapTilesY][MapTilesX] GridWalls;

float ZToScale(float z);
float ScaleToZ(float Scale);
void set_drawing_scale_x( float scale );

#endif