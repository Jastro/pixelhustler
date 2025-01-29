#ifndef GLOBALS_IMPL_H
#define GLOBALS_IMPL_H

// Map data loaded from files
embedded int[MapTilesY][MapTilesX] GridGround = "obj/MapGround.vmap";
embedded int[MapTilesY][MapTilesX] GridRoofs = "obj/MapRoofs.vmap";
embedded int[MapTilesY][MapTilesX] GridWalls = "obj/MapWalls.vmap";

float ZToScale(float z) {
    return 0.5 + 160.0 / z;
}

float ScaleToZ(float Scale) {
    return 160.0 / (Scale - 0.5);
}

void set_drawing_scale_x( float scale )
{
    asm
    {
        "mov R0, {scale}"
        "out GPU_DrawingScaleX, R0"
    }
}

#endif