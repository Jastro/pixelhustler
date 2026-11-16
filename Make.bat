@echo off
REM create obj and bin folders if non exiting, since
REM the development tools will not create them themselves
if not exist obj mkdir obj
if not exist bin mkdir bin

echo.
echo Import embedded data files
echo --------------------------
tiled2vircon assets\maps\TileMapCity.tmx -o obj\ || goto :failed
tiled2vircon assets\maps\TileMapWalls.tmx  -o obj\ || goto :failed
tiled2vircon assets\maps\TileMapCafe.tmx  -o obj\ || goto :failed
tiled2vircon assets\maps\TileMapHomeUp.tmx  -o obj\ || goto :failed
tiled2vircon assets\maps\TileMapHomeDown.tmx  -o obj\ || goto :failed

echo.
echo Compile the C code
echo --------------------------
compile src\main\PixelHustler.c -o obj\PixelHustler.asm || goto :failed

echo.
echo Assemble the ASM code
echo --------------------------
assemble obj\PixelHustler.asm -o obj\PixelHustler.vbin || goto :failed

echo.
echo Convert the PNG textures
echo --------------------------
png2vircon assets\textures\TextureGame.png   -o obj\TextureGame.vtex   || goto :failed
png2vircon assets\textures\TextureGround.png -o obj\TextureGround.vtex || goto :failed
png2vircon assets\textures\TextureWalls.png  -o obj\TextureWalls.vtex  || goto :failed
png2vircon assets\textures\TextureSprites.png -o obj\TextureSprites.vtex || goto :failed
png2vircon assets\textures\TextureDialog.png -o obj\TextureDialog.vtex || goto :failed
png2vircon assets\textures\TexturePortraits.png -o obj\TexturePortraits.vtex || goto :failed
png2vircon assets\textures\TextureIntro.png -o obj\TextureIntro.vtex || goto :failed
png2vircon assets\textures\TextureChat.png -o obj\TextureChat.vtex || goto :failed
png2vircon assets\textures\TextureTitle.png -o obj\TextureTitle.vtex || goto :failed
png2vircon assets\textures\TextureInterior.png -o obj\TextureInterior.vtex || goto :failed
png2vircon assets\textures\TextureWeapons.png -o obj\TextureWeapons.vtex || goto :failed

echo.
echo Convert the WAV sounds
echo --------------------------
wav2vircon assets\music\menu.wav -o obj\music-menu.vsnd || goto :failed
wav2vircon assets\music\turrets.wav -o obj\music-turrets.vsnd || goto :failed
wav2vircon assets\music\planes.wav -o obj\music-planes.vsnd || goto :failed
wav2vircon assets\music\boss.wav -o obj\music-boss.vsnd || goto :failed
wav2vircon assets\music\ending.wav -o obj\music-ending.vsnd || goto :failed
wav2vircon assets\sounds\shoot.wav -o obj\shoot.vsnd || goto :failed
wav2vircon assets\sounds\enemyshoot.wav -o obj\enemyshoot.vsnd || goto :failed
wav2vircon assets\sounds\explosion.wav -o obj\explosion.vsnd || goto :failed
wav2vircon assets\sounds\hurt.wav -o obj\hurt.vsnd || goto :failed
wav2vircon assets\sounds\death.wav -o obj\death.vsnd || goto :failed
wav2vircon assets\sounds\pickup.wav -o obj\pickup.vsnd || goto :failed
wav2vircon assets\sounds\changeweapon.wav -o obj\changeweapon.vsnd || goto :failed
wav2vircon assets\sounds\actioncancelled.wav -o obj\actioncancelled.vsnd || goto :failed
wav2vircon assets\sounds\enemyhit.wav -o obj\enemyhit.vsnd || goto :failed
wav2vircon assets\sounds\fall.wav -o obj\fall.vsnd || goto :failed
wav2vircon assets\sounds\enterheli.wav -o obj\enterheli.vsnd || goto :failed
wav2vircon assets\sounds\heli.wav -o obj\heli.vsnd || goto :failed
wav2vircon assets\sounds\lowfuel.wav -o obj\lowfuel.vsnd || goto :failed
wav2vircon assets\sounds\menumove.wav -o obj\menumove.vsnd || goto :failed
wav2vircon assets\sounds\menuaccept.wav -o obj\menuaccept.vsnd || goto :failed
wav2vircon assets\sounds\siren.wav -o obj\siren.vsnd || goto :failed
wav2vircon assets\sounds\cash.wav -o obj\cash.vsnd || goto :failed
wav2vircon assets\sounds\reload.wav -o obj\reload.vsnd || goto :failed
wav2vircon assets\sounds\door.wav -o obj\door.vsnd || goto :failed
wav2vircon assets\sounds\busted.wav -o obj\busted.vsnd || goto :failed
wav2vircon assets\sounds\thud.wav -o obj\thud.vsnd || goto :failed
wav2vircon assets\sounds\wanted.wav -o obj\wanted.vsnd || goto :failed

echo.
echo Pack the ROM
echo --------------------------
packrom assets\rom\PixelHustler.xml -o bin\PixelHustler.v32 || goto :failed
goto :succeeded

:failed
echo.
echo BUILD FAILED
exit /b %errorlevel%

:succeeded
echo.
echo BUILD SUCCESSFUL
exit /b

@echo on