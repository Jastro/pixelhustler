@echo off
REM create obj and bin folders if non exiting, since
REM the development tools will not create them themselves
if not exist obj mkdir obj
if not exist bin mkdir bin

echo.
echo Import embedded data files
echo --------------------------
tiled2vircon maps\TileMapCity.tmx -o obj\ || goto :failed
tiled2vircon maps\TileMapWalls.tmx  -o obj\ || goto :failed

echo.
echo Compile the C code
echo --------------------------
compile PixelHustler.c -o obj\PixelHustler.asm || goto :failed

echo.
echo Assemble the ASM code
echo --------------------------
assemble obj\PixelHustler.asm -o obj\PixelHustler.vbin || goto :failed

echo.
echo Convert the PNG textures
echo --------------------------
png2vircon TextureGame.png   -o obj\TextureGame.vtex   || goto :failed
png2vircon TextureGround.png -o obj\TextureGround.vtex || goto :failed
png2vircon TextureWalls.png  -o obj\TextureWalls.vtex  || goto :failed

echo.
echo Pack the ROM
echo --------------------------
packrom PixelHustler.xml -o bin\PixelHustler.v32 || goto :failed
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