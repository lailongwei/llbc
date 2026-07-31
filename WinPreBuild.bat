@echo off

Rem Generate visual studio project files.
Rem This file is a part of llbc library.

Rem Get CMake binary path.
set cmake_bin_path=%~dp0tools\cmake\bin\cmake-4.4.0-windows-
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    set cmake_bin_path=%cmake_bin_path%x86_64\bin\cmake.exe
) else if "%PROCESSOR_ARCHITECTURE%"=="ARM64" (
    set cmake_bin_path=%cmake_bin_path%arm64\bin\cmake.exe
) else (
    echo Unsupported processor architecture: %PROCESSOR_ARCHITECTURE%
    exit /b 1
)
echo CMake binary path: %cmake_bin_path%

Rem Get Visual Studio version from user input.
echo Visual Studio solution and project files generate tool.
echo For now supported Visual Studio versions:
echo    vs2017
echo    vs2019
echo    vs2022
echo    vs2026
set /p choose=Please input:

set build_dir=%~dp0build_%choose%
mkdir %build_dir% 2>nul
echo Build directory: %build_dir%

pushd "%build_dir%"
if "%choose%"=="vs2017" (
    "%cmake_bin_path%" -G "Visual Studio 15 2017" ..
) else if "%choose%"=="vs2019" (
    "%cmake_bin_path%" -G "Visual Studio 16 2019" ..
) else if "%choose%"=="vs2022" (
    "%cmake_bin_path%" -G "Visual Studio 17 2022" ..
) else if "%choose%"=="vs2026" (
    "%cmake_bin_path%" -G "Visual Studio 18 2026" ..
) else (
    echo Unsupported Visual Studio version: %choose%
    popd
    pause
    exit /b 1
)

popd
if errorlevel 1 (
    echo Failed to generate Visual Studio solution and project files, error: %errorlevel%
    pause
    exit /b 1
) else (
    echo Succcess to generate Visual Studio solution and project files
    echo Solution file path: build_%choose%/llbc.sln
    if "%1"=="" (
        explorer .\build_%choose%
    )
)
