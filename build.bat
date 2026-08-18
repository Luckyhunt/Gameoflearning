@echo off
REM ============================================================
REM build.bat — APLG GDExtension Build Script
REM
REM Steps:
REM   1. Run CMake configure (picks up any new source files)
REM   2. Build Release configuration
REM   3. CMake post-build step auto-copies DLL to Godot project
REM ============================================================

echo.
echo [APLG] Creating build directory...
if not exist build mkdir build

echo [APLG] Configuring with CMake (Visual Studio 2022)...
cmake -S . -B build -G "Visual Studio 17 2022" -A x64

if %ERRORLEVEL% NEQ 0 (
    echo [APLG] ERROR: CMake configuration failed!
    exit /b 1
)

echo.
echo [APLG] Building GDExtension DLL (Release)...
cmake --build build --config Release --target AdaptiveProceduralLevelGeneration_gdextension

if %ERRORLEVEL% NEQ 0 (
    echo [APLG] ERROR: Build failed!
    exit /b 1
)

echo.
echo [APLG] Build complete!
echo [APLG] DLL copied to: godot_project\adaptiveproceduralplatformer\
echo [APLG] Open Godot 4.4 and load: godot_project\adaptiveproceduralplatformer\project.godot
echo [APLG] Press F5 to run — check Output panel for APLG test results.
echo.
