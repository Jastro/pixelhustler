#ifndef WORLD_H
#define WORLD_H

#include "../utils/definitions.h"
#include "../utils/tilemapzoomed.h"

extern tileset TilesGround;
extern tilemap MapGround, MapRoofs;

void initialize_world();
void render_ground();
void render_roofs();

#endif
