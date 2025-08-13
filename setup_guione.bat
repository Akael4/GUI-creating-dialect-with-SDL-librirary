@echo off
setlocal

echo ==========================================
echo   Installazione libreria GUIone per C++
echo ==========================================
echo.

rem Nome del file header della libreria
set HEADER_FILE=guione.h
set CPP_FILE=guione.cpp

rem Percorsi possibili di include (MinGW e Visual Studio)
set MINGW_PATH=C:\mingw64\include
set VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio

rem Controllo MinGW
if exist "%MINGW_PATH%" (
    set TARGET_INCLUDE=%MINGW_PATH%
    goto found_path
)

rem Controllo Visual Studio
for /d %%D in ("%VS_PATH%\*") do (
    if exist "%%D\VC\Tools\MSVC" (
        for /d %%V in ("%%D\VC\Tools\MSVC\*") do (
            if exist "%%V\include" (
                set TARGET_INCLUDE=%%V\include
                goto found_path
            )
        )
    )
)

rem Se non trova nulla, chiede manualmente
echo Non e' stata trovata automaticamente una cartella "include".
set /p TARGET_INCLUDE=Inserisci il percorso della cartella include: 

:found_path
echo.
echo Installazione in: %TARGET_INCLUDE%
echo.

rem Copia i file
if not exist "%TARGET_INCLUDE%" (
    echo Errore: la cartella specificata non esiste.
    pause
    exit /b 1
)

copy "%HEADER_FILE%" "%TARGET_INCLUDE%" >nul
if exist "%CPP_FILE%" copy "%CPP_FILE%" "%TARGET_INCLUDE%" >nul

echo Libreria installata correttamente!
pause
endlocal
