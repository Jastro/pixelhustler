#!/usr/bin/env bash
# Linux equivalent of Make.bat
set -euo pipefail
cd "$(dirname "$0")"

# use local Vircon32 devtools if present
TOOLS="${VIRCON32_DEVTOOLS:-../tools/DevTools}"
[ -d "$TOOLS" ] && export PATH="$(cd "$TOOLS" && pwd):$PATH"

mkdir -p obj bin

echo; echo "Import embedded data files"; echo "--------------------------"
tiled2vircon assets/maps/TileMapCity.tmx  -o obj/
tiled2vircon assets/maps/TileMapWalls.tmx -o obj/

echo; echo "Compile the C code"; echo "--------------------------"
compile src/main/PixelHustler.c -o obj/PixelHustler.asm

echo; echo "Assemble the ASM code"; echo "--------------------------"
assemble obj/PixelHustler.asm -o obj/PixelHustler.vbin

echo; echo "Convert the PNG textures"; echo "--------------------------"
png2vircon assets/textures/TextureGame.png   -o obj/TextureGame.vtex
png2vircon assets/textures/TextureGround.png -o obj/TextureGround.vtex
png2vircon assets/textures/TextureWalls.png  -o obj/TextureWalls.vtex
png2vircon assets/textures/TextureSprites.png -o obj/TextureSprites.vtex
png2vircon assets/textures/TextureDialog.png -o obj/TextureDialog.vtex
png2vircon assets/textures/TexturePortraits.png -o obj/TexturePortraits.vtex

echo; echo "Pack the ROM"; echo "--------------------------"
packrom assets/rom/PixelHustler.xml -o bin/PixelHustler.v32

echo; echo "BUILD SUCCESSFUL"
