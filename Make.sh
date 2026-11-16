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
tiled2vircon assets/maps/TileMapCafe.tmx  -o obj/
tiled2vircon assets/maps/TileMapHomeUp.tmx  -o obj/
tiled2vircon assets/maps/TileMapHomeDown.tmx  -o obj/

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
png2vircon assets/textures/TextureIntro.png -o obj/TextureIntro.vtex
png2vircon assets/textures/TextureChat.png -o obj/TextureChat.vtex
png2vircon assets/textures/TextureTitle.png -o obj/TextureTitle.vtex
png2vircon assets/textures/TextureInterior.png -o obj/TextureInterior.vtex
png2vircon assets/textures/TextureWeapons.png -o obj/TextureWeapons.vtex

echo; echo "Convert the WAV sounds"; echo "--------------------------"
wav2vircon assets/music/menu.wav -o obj/music-menu.vsnd
wav2vircon assets/music/turrets.wav -o obj/music-turrets.vsnd
wav2vircon assets/music/planes.wav -o obj/music-planes.vsnd
wav2vircon assets/music/boss.wav -o obj/music-boss.vsnd
wav2vircon assets/music/ending.wav -o obj/music-ending.vsnd
wav2vircon assets/sounds/shoot.wav -o obj/shoot.vsnd
wav2vircon assets/sounds/enemyshoot.wav -o obj/enemyshoot.vsnd
wav2vircon assets/sounds/explosion.wav -o obj/explosion.vsnd
wav2vircon assets/sounds/hurt.wav -o obj/hurt.vsnd
wav2vircon assets/sounds/death.wav -o obj/death.vsnd
wav2vircon assets/sounds/pickup.wav -o obj/pickup.vsnd
wav2vircon assets/sounds/changeweapon.wav -o obj/changeweapon.vsnd
wav2vircon assets/sounds/actioncancelled.wav -o obj/actioncancelled.vsnd
wav2vircon assets/sounds/enemyhit.wav -o obj/enemyhit.vsnd
wav2vircon assets/sounds/fall.wav -o obj/fall.vsnd
wav2vircon assets/sounds/enterheli.wav -o obj/enterheli.vsnd
wav2vircon assets/sounds/heli.wav -o obj/heli.vsnd
wav2vircon assets/sounds/lowfuel.wav -o obj/lowfuel.vsnd
wav2vircon assets/sounds/menumove.wav -o obj/menumove.vsnd
wav2vircon assets/sounds/menuaccept.wav -o obj/menuaccept.vsnd
wav2vircon assets/sounds/siren.wav -o obj/siren.vsnd
wav2vircon assets/sounds/cash.wav -o obj/cash.vsnd
wav2vircon assets/sounds/reload.wav -o obj/reload.vsnd
wav2vircon assets/sounds/door.wav -o obj/door.vsnd
wav2vircon assets/sounds/busted.wav -o obj/busted.vsnd
wav2vircon assets/sounds/thud.wav -o obj/thud.vsnd
wav2vircon assets/sounds/wanted.wav -o obj/wanted.vsnd

echo; echo "Pack the ROM"; echo "--------------------------"
packrom assets/rom/PixelHustler.xml -o bin/PixelHustler.v32

echo; echo "BUILD SUCCESSFUL"
