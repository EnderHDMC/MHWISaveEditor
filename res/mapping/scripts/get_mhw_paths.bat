@echo off

:: Query the registry for the steam path, game path and current Steam user
reg query "HKEY_CURRENT_USER\SOFTWARE\Valve\Steam" /v "SteamPath"
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App 582010" /v "InstallLocation"
reg query "HKEY_CURRENT_USER\Software\Valve\Steam\ActiveProcess" /v "ActiveUser"

:: Do the same thing but put them in variables this time & echo the results
for /F "skip=2 tokens=2,*" %%A in ('reg query "HKEY_CURRENT_USER\SOFTWARE\Valve\Steam" /v "SteamPath"') do set "STEAM_INSTALL=%%B"
echo STEAM_INSTALL = %STEAM_INSTALL%

for /F "skip=2 tokens=2,*" %%A in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Steam App 582010" /v "InstallLocation"') do set "MHW_INSTALL=%%B"
echo MHW_INSTALL = %MHW_INSTALL%

for /F "skip=2 tokens=2,*" %%A in ('reg query "HKEY_CURRENT_USER\Software\Valve\Steam\ActiveProcess" /v "ActiveUser"') do set "CURRENT_STEAM_USER=%%B"
set /a CURRENT_STEAM_USER = %CURRENT_STEAM_USER%
echo CURRENT_STEAM_USER = %CURRENT_STEAM_USER%

echo.

:: Construct the path for if the save file is relative to Steam install path
set SAVE_DIR_STEAM = %STEAM_INSTALL%/userdata/%CURRENT_STEAM_USER%/582010/remote/
set SAVE_DIR_MHW = %MHW_INSTALL%

:: Construct the path for if the save file is relative to MHW install path
set SAVE_DIR_MHW = %SAVE_DIR_MHW%\..\..\..\userdata\%CURRENT_STEAM_USER%\582010\remote\

:: Clean the paths
set SAVE_DIR_STEAM = "%SAVE_DIR_STEAM:/=\%"
for %%i in ("%SAVE_DIR_MHW%") do set "SAVE_DIR_MHW_CLEAN=%%~fi"
set SAVE_DIR_MHW = "%SAVE_DIR_MHW_CLEAN%"

echo MHW save path: %SAVE_DIR_STEAM%
echo Steam save path: %SAVE_DIR_MHW%

echo.

:: This took so long because batch is cursed.
:: Checking if a directory exists in a variable isn't parsed correctly, so it executes both the if and else block.

:: batch is frustrating, bash is better!
:: Check if the save exists relative to steam
:: if exist "%SAVE_DIR_STEAM%" (
::  echo Save location (Steam) exists: %SAVE_DIR_STEAM%
:: ) else (
::  echo Save location (Steam): %SAVE_DIR_STEAM%
:: )

cd "%SAVE_DIR_STEAM%" >nul 2>&1 && echo Steam save exists: %SAVE_DIR_STEAM%
cd "%SAVE_DIR_MHW%" >nul 2>&1 && echo MHW save exists: %SAVE_DIR_MHW%

pause
