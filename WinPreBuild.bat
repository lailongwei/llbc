@echo off

Rem Generate visual studio project files
Rem This file is a part of llbc library

echo Visual Studio solution and project files generate tool.
echo For now supported Visual Studio versions:
echo    vs2005
echo    vs2008
echo    vs2010
echo    vs2012
echo    vs2013
echo    vs2015
echo    vs2017
echo    vs2019
echo    vs2022
set /p choose=Please input:

cd tools\premake && premake5_windows.exe %choose%

if errorlevel 1 (
    echo Failed to generate Visual Studio solution and project files, error: %errorlevel%
    pause
    exit 1
) else (
    echo Succcess to generate Visual Studio solution and project files
    echo Solution file path: build/%choose%/llbc_%choose%.sln
    if "%1"=="" (
        explorer ..\..\build\%choose%
    )
    if "%1"=="1" (
        explorer ..\..\build\%choose%
    )
)
