# Build Guide — Adaptive Procedural Level Generation

This document provides technical instructions for configuring, building, and testing both the **Standalone C++ Executable** (`StandaloneApp.exe`) and the **Godot 4 GDExtension Shared Library** (`AdaptiveProceduralLevelGeneration_gdextension.dll`).

> 💡 **For a complete step-by-step setup guide on a fresh machine (Windows, Linux, macOS), see [INSTALLATION.md](../INSTALLATION.md).**

---

## Prerequisites

- **Operating System**: Windows 10 / 11 (x64), Linux (x64), or macOS (ARM64/x64)
- **Compiler**: Microsoft Visual Studio 2022 (MSVC v143 or newer), GCC 11+, or Clang 13+ with C++20 support
- **Build System**: CMake v3.20+
- **Python & SCons**: Python 3.7+ with SCons (`pip install scons`)
- **Audio & Graphics**: Standard Windows APIs (Win32 GDI, GDI+, WinMM audio — included with Windows SDK)
- **Godot Engine**: Godot 4.2+ (tested up to Godot 4.4+)

---

## 1. `godot-cpp` Bindings Build Step

Before invoking CMake, build the `godot-cpp` static library (branch 4.2):

```bash
# Clone godot-cpp 4.2 if third_party/godot-cpp is empty
git clone -b 4.2 https://github.com/godotengine/godot-cpp.git third_party/godot-cpp

# Build static library via SCons
# Windows:
scons -C third_party/godot-cpp platform=windows target=template_debug generate_bindings=yes -j8

# Linux:
scons -C third_party/godot-cpp platform=linux target=template_debug generate_bindings=yes -j$(nproc)

# macOS:
scons -C third_party/godot-cpp platform=macos target=template_debug generate_bindings=yes -j$(sysctl -n hw.ncpu)
```

---

## 2. Quick Build Scripts

### Windows Batch Script
Run `build.bat` from the project root directory in Command Prompt or PowerShell:

```cmd
.\build.bat
```

### Linux / macOS Shell Script
```bash
chmod +x build.sh
./build.sh
```

---

## 3. Manual CMake Build Steps

### Step A: Configure CMake
```cmd
mkdir build
cd build

# Windows (Visual Studio 2022 x64):
cmake -S .. -B . -G "Visual Studio 17 2022" -A x64

# Linux / macOS:
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Step B: Build Standalone C++ Game (Windows)
To build the pure C++ standalone executable:
```cmd
cmake --build . --config Release --target StandaloneApp
```
Binary location: `build/Release/StandaloneApp.exe`

### Step C: Build GDExtension Shared Library
To build the GDExtension shared library for Godot:
```cmd
cmake --build . --config Release --target AdaptiveProceduralLevelGeneration_gdextension
```
Library location:
- Windows: `build/Release/AdaptiveProceduralLevelGeneration_gdextension.dll`
- Linux: `build/Release/AdaptiveProceduralLevelGeneration_gdextension.so`
- macOS: `build/Release/AdaptiveProceduralLevelGeneration_gdextension.dylib`

*Post-build step automatically copies the output binary into `godot_project/adaptiveproceduralplatformer/`.*

---

## 4. Running Automated C++ Tests

All core algorithms (procedural generation, reachability validation, difficulty manager, AI player model, physics AABB, save system) are covered by CTest suites.

Run all 10 test suites using CMake:
```cmd
cd build
ctest -C Release --output-on-failure
```

### Expected Test Output:
```text
100% tests passed out of 10
```

---

## 5. Running the Applications

### A. Standalone C++ Platformer Game
Run the executable directly from the project root (so relative Brackeys assets can be found):
```cmd
.\build\Release\StandaloneApp.exe
```

### B. Godot 4 Engine Integration
1. Launch **Godot 4.2+**.
2. Open Project and navigate to: `godot_project/adaptiveproceduralplatformer/project.godot`.
3. Press **F5** to run the main scene (`Scenes/Main.tscn`).

