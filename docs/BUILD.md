# Build Guide — Adaptive Procedural Level Generation

This document provides complete instructions for configuring, building, and testing both the **Standalone C++ Executable** (`StandaloneApp.exe`) and the **Godot 4 GDExtension Shared Library** (`AdaptiveProceduralLevelGeneration_gdextension.dll`).

---

## Prerequisites

- **Operating System**: Windows 10 / 11 (x64)
- **Compiler**: Microsoft Visual Studio 2022 (MSVC v143 or newer) with C++20 support
- **Build System**: CMake v3.20+
- **Audio & Graphics**: Standard Windows APIs (Win32 GDI, GDI+, WinMM audio — included with Windows SDK)
- **Godot Engine (Optional for Godot testing)**: Godot 4.4+ (x64)

---

## 1. Quick Build (Batch Script)

Run the provided `build.bat` script from the project root directory in PowerShell or Command Prompt:

```cmd
.\build.bat
```

### What `build.bat` does automatically:
1. Creates `build/` directory if missing.
2. Configures CMake for Visual Studio 2022 (C++20 Release mode).
3. Compiles the C++ static library `AdaptiveProceduralLevelGeneration.lib`.
4. Compiles the GDExtension DLL `AdaptiveProceduralLevelGeneration_gdextension.dll`.
5. Automatically copies the compiled GDExtension DLL into `godot_project/adaptiveproceduralplatformer/`.

---

## 2. Manual CMake Build Steps

### Step A: Configure CMake
```cmd
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Step B: Build Standalone C++ Game
To build the pure C++ standalone executable:
```cmd
cmake --build . --config Release --target StandaloneApp
```
The compiled binary will be located at:
`build/Release/StandaloneApp.exe`

### Step C: Build GDExtension DLL
To build the GDExtension shared library for Godot:
```cmd
cmake --build . --config Release --target AdaptiveProceduralLevelGeneration_gdextension
```
The compiled library will be located at:
`build/Release/AdaptiveProceduralLevelGeneration_gdextension.dll`

---

## 3. Running Automated C++ Tests

All core algorithms (procedural generation, reachability validation, difficulty manager, AI player model, physics AABB, save system) are covered by CTest suites.

Run all 10 test suites using CMake:
```cmd
cmake --build build --config Release --target RUN_TESTS
```

Or run `ctest` directly inside the `build/` directory:
```cmd
cd build
ctest -C Release --output-on-failure
```

### Expected Test Output:
```
100% tests passed out of 10
```

---

## 4. Running the Applications

### A. Standalone C++ Platformer Game
Run the executable directly from the project root (so relative Brackeys assets can be found):
```cmd
.\build\Release\StandaloneApp.exe
```

### B. Godot 4 Engine Integration
1. Launch **Godot 4.4+**.
2. Open Project and navigate to: `godot_project/adaptiveproceduralplatformer/project.godot`.
3. Press **F5** to run the main scene (`Scenes/Main.tscn`).
