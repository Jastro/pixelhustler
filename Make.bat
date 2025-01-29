@echo off
REM create obj and bin folders if non exiting
if not exist obj mkdir obj
if not exist bin mkdir bin

set DEVTOOLS=.\DevTools
set COMPILER=%DEVTOOLS%\compile.exe
set ASSEMBLER=%DEVTOOLS%\assemble.exe
set TILED2VIRCON=%DEVTOOLS%\tiled2vircon.exe
set PNG2VIRCON=%DEVTOOLS%\png2vircon.exe
set PACKROM=%DEVTOOLS%\packrom.exe

echo.
echo Import embedded data files
echo --------------------------
%TILED2VIRCON% assets\maps\TileMapCity.tmx -o obj\ || goto :failed
%TILED2VIRCON% assets\maps\TileMapWalls.tmx -o obj\ || goto :failed

echo.
echo Compile the C code
echo --------------------------
%COMPILER% src\main.c -o obj\PixelHustler.asm || goto :failed

echo.
echo Assemble the ASM code
echo --------------------------
%ASSEMBLER% obj\PixelHustler.asm -o obj\PixelHustler.vbin || goto :failed

echo.
echo Convert the PNG textures
echo --------------------------
REM UI y texturas base
%PNG2VIRCON% assets\textures\ground.png -o obj\TextureGround.vtex || goto :failed
%PNG2VIRCON% assets\textures\walls.png -o obj\TextureWalls.vtex || goto :failed

REM Personajes y vehículos
%PNG2VIRCON% assets\textures\soldier.png -o obj\soldier.vtex || goto :failed
%PNG2VIRCON% assets\textures\vehicle.png -o obj\vehicle.vtex || goto :failed

REM GUI y efectos
%PNG2VIRCON% assets\textures\gui.png -o obj\gui.vtex || goto :failed

echo.
echo Pack the ROM
echo --------------------------
%PACKROM% PixelHustler.xml -o bin\PixelHustler.v32 || goto :failed
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