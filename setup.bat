@echo off
title SDL2 Setup
echo ===========================================
echo   Installing SDL2 requirements for MinGW
echo ===========================================
echo.

REM Creazione cartella libs se non esiste
if not exist libs mkdir libs

REM Scarico SDL2 (versione MinGW)
echo Downloading SDL2...
powershell -Command "Invoke-WebRequest https://github.com/libsdl-org/SDL/releases/download/release-2.30.8/SDL2-devel-2.30.8-mingw.zip -OutFile libs\SDL2.zip"

REM Estrazione
echo Extracting SDL2...
powershell -Command "Expand-Archive -Path libs\SDL2.zip -DestinationPath libs\SDL2 -Force"

REM Pulizia file zip
del libs\SDL2.zip

REM Mostra cartella estratta
echo SDL2 installed in: %cd%\libs\SDL2

REM Set variabili di ambiente per questa sessione
set INCLUDE=%cd%\libs\SDL2\SDL2-2.30.8\i686-w64-mingw32\include;%INCLUDE%
set LIB=%cd%\libs\SDL2\SDL2-2.30.8\i686-w64-mingw32\lib;%LIB%

echo.
echo SDL2 setup complete!
echo.
pause