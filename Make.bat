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