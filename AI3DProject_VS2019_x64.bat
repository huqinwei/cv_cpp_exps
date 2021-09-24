@echo off

set SOURCE_PATH=%~dp0
cd ..
set CUR_DIR=%cd%\
set TARGET=AI3D
set VS_VER=VS2019
set PLATFORM=x64

set BUILD_PATH=%CUR_DIR%%TARGET%_%VS_VER%_%PLATFORM%

@echo on
echo %BUILD_PATH%
echo %SOURCE_PATH%


if not exist %BUILD_PATH% mkdir %BUILD_PATH%
cd %BUILD_PATH%
cmake -G "Visual Studio 16 2019"  -A %PLATFORM% %SOURCE_PATH%
pause