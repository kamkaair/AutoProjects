@echo off
setlocal enabledelayedexpansion
:: Delayed expansion allegedly adds !variables!

set PATH=D:\Koulu\OmatProjektit\AutoProjects\src
set OUTPUT=CMakeLists.txt
set FILES=${HDR_FILES})

:: CMake paths
set PROJECT_PATH=D:\Koulu\OmatProjektit\AutoProjects
set PROJECT_BUILD=D:\Koulu\OmatProjektit\AutoProjects\build

:: Add a dot to disable the "ECHO HAS BEEN..." message
echo. > "%OUTPUT%" 

:: Write all the CMake setup...
echo cmake_minimum_required(VERSION 3.5) >> "%OUTPUT%"
echo set(CMAKE_GENERATOR_TOOLSET "v142") >> "%OUTPUT%"
echo set(CMAKE_CXX_STANDARD 17) >> "%OUTPUT%"
echo project(Automation) >> "%OUTPUT%"
echo file(GLOB_RECURSE HDR_FILES "src/*.h") >> "%OUTPUT%"
echo source_group("Header Files", FILES ${HDR_FILES}) >> "%OUTPUT%"
echo file(GLOB_RECURSE SRC_FILES "src/*.cpp") >> "%OUTPUT%"
echo source_group("Source Files", FILES ${SRC_FILES}) >> "%OUTPUT%"

echo. >> "%OUTPUT%"
echo add_executable(Automation ${HDR_FILES} ${SRC_FILES}) >> "%OUTPUT%"
echo. >> "%OUTPUT%"

:: A for loop, which writes all the executables
for %%f in ("%PATH%\*.cpp") do (
:: The syntax is kinda weird, but %%~nxf gives the name and extension
set "full_filename=%%~nxf"
set "filename=%%~nf"
:: Print, for some reason the filename should be referred with !x!
echo add_executable(!filename! src/!full_filename! ${HDR_FILES}^) >> "%OUTPUT%"
)

:: CMake build
:: If it don't exist mkdir (create new dir)
if not exist "%PROJECT_BUILD%" mkdir "%PROJECT_BUILD%"
:: Generate buildsystem and build the project
"C:\Program Files\CMake\bin\cmake.exe" -B "%PROJECT_BUILD%" -S "%PROJECT_PATH%"
"C:\Program Files\CMake\bin\cmake.exe" --build "%PROJECT_BUILD%"

:: Open CMake
::start "" "C:\Program Files\CMake\bin\cmake-gui.exe"