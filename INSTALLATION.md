# Complete Installation & Build Guide

> **Adaptive Procedural Level Generation (APLG)** — A C++20 procedural platformer generation framework with native **Godot 4 GDExtension** integration and a **Win32 C++ Standalone Game**.

This guide provides exact, step-by-step instructions for setting up, building, and running the project from a clean machine across **Windows**, **Linux**, and **macOS**.

---

## Table of Contents

- [System & Dependency Requirements](#system--dependency-requirements)
- [1. Quick Start Guide (TL;DR)](#1-quick-start-guide-tldr)
- [2. Prerequisites Setup](#2-prerequisites-setup)
  - [Windows](#windows)
  - [Linux (Ubuntu / Debian)](#linux-ubuntu--debian)
  - [macOS](#macos)
- [3. Clone Project & Submodules](#3-clone-project--submodules)
- [4. Build `godot-cpp` Bindings](#4-build-godot-cpp-bindings)
- [5. Build Project with CMake](#5-build-project-with-cmake)
  - [Using Quick Build Scripts](#using-quick-build-scripts)
  - [Manual CMake Build (Debug & Release)](#manual-cmake-build-debug--release)
- [6. Running Automated Tests](#6-running-automated-tests)
- [7. Opening & Running in Godot Engine](#7-opening--running-in-godot-engine)
- [8. Running Standalone C++ Game](#8-running-standalone-c-game)
- [9. Clean & Rebuild Instructions](#9-clean--rebuild-instructions)
- [10. Troubleshooting](#10-troubleshooting)

---

## System & Dependency Requirements

| Dependency | Required Version | Purpose |
| :--- | :--- | :--- |
| **Operating System** | Windows 10/11 (x64), Linux (x64), or macOS (ARM64/x64) | Development host |
| **C++ Compiler** | MSVC 2022 (v143+), GCC 11+, or Clang 13+ | C++20 compilation |
| **Build System** | CMake **3.20+** | Project configuration & build management |
| **Python** | Python **3.7+** | Toolchain runner |
| **SCons** | SCons **4.0+** (`pip install scons`) | Compiling `godot-cpp` C++ bindings |
| **Godot Engine** | Godot **4.2+** (tested up to Godot **4.4+**) | Engine runtime for GDExtension |
| **`godot-cpp`** | Branch **`4.2`** | GDExtension C++ bindings library |
| **Windows APIs** | Win32 GDI, GDI+, WinMM, Msimg32 *(Windows only)* | Pure C++ Standalone app graphics/audio |

---

## 1. Quick Start Guide (TL;DR)

On a **Windows 10/11** machine with Visual Studio 2022, Python, CMake, and Godot 4 installed:

```cmd
:: 1. Clone repository
git clone https://github.com/Luckyhunt/Gameoflearning.git AdaptiveProceduralLevelGeneration
cd AdaptiveProceduralLevelGeneration

:: 2. Clone godot-cpp 4.2 branch
git clone -b 4.2 https://github.com/godotengine/godot-cpp.git third_party/godot-cpp

:: 3. Build godot-cpp library
scons -C third_party/godot-cpp platform=windows target=template_debug generate_bindings=yes -j8

:: 4. Build project (Configures CMake, compiles static lib + GDExtension DLL, auto-copies DLL)
.\build.bat

:: 5. Run tests
cd build && ctest -C Release --output-on-failure
```

---

## 2. Prerequisites Setup

### Windows

1. **Visual Studio 2022**:
   - Download [Visual Studio 2022 Community Edition](https://visualstudio.microsoft.com/vs/).
   - In the Visual Studio Installer, select workload: **Desktop development with C++**.
   - Ensure **MSVC v143 - VS 2022 C++ x64/x86 build tools** and **Windows 10/11 SDK** are selected.

2. **CMake**:
   - Download and install [CMake 3.20+](https://cmake.org/download/).
   - Select **Add CMake to the system PATH for all users**.

3. **Python & SCons**:
   - Install [Python 3.10+](https://www.python.org/downloads/). Check **Add python.exe to PATH**.
   - Open Command Prompt / PowerShell and install SCons:
     ```cmd
     pip install scons
     ```

4. **Godot Engine**:
   - Download [Godot Engine 4.2+](https://godotengine.org/download) (Standard 64-bit executable).

---

### Linux (Ubuntu / Debian)

```bash
# 1. Update package list and install build essentials
sudo apt update
sudo apt install -y build-essential cmake python3 python3-pip git

# 2. Install SCons
pip3 install scons

# 3. Download Godot Engine 4.2+
# Download Godot 4 executable from https://godotengine.org/download and place in your PATH
```

---

### macOS

```bash
# 1. Install Xcode Command Line Tools
xcode-select --install

# 2. Install Homebrew (if not already installed), then CMake and Python
brew install cmake python git

# 3. Install SCons
pip3 install scons

# 4. Download Godot Engine 4.2+ (macOS build) from https://godotengine.org/download
```

---

## 3. Clone Project & Submodules

Open terminal / command prompt:

```bash
# Clone the repository
git clone https://github.com/Luckyhunt/Gameoflearning.git AdaptiveProceduralLevelGeneration
cd AdaptiveProceduralLevelGeneration

# Ensure you are on the main branch with the latest code
git checkout main
git pull origin main

# Clone the godot-cpp bindings repository (branch 4.2)
git clone -b 4.2 https://github.com/godotengine/godot-cpp.git third_party/godot-cpp
```

---

## 4. Build `godot-cpp` Bindings

Before building the main C++ project, you must compile the `godot-cpp` static library.

Run SCons from the root directory specifying your target platform:

### Windows (MSVC)
```cmd
scons -C third_party/godot-cpp platform=windows target=template_debug generate_bindings=yes -j8
```
*Output generated:* `third_party/godot-cpp/bin/libgodot-cpp.windows.template_debug.x86_64.lib`

### Linux
```bash
scons -C third_party/godot-cpp platform=linux target=template_debug generate_bindings=yes -j$(nproc)
```
*Output generated:* `third_party/godot-cpp/bin/libgodot-cpp.linux.template_debug.x86_64.a`

### macOS
```bash
scons -C third_party/godot-cpp platform=macos target=template_debug generate_bindings=yes -j$(sysctl -n hw.ncpu)
```
*Output generated:* `third_party/godot-cpp/bin/libgodot-cpp.macos.template_debug.universal.a`

---

## 5. Build Project with CMake

### Using Quick Build Scripts

- **Windows**:
  ```cmd
  .\build.bat
  ```
  *This script creates `build/`, configures CMake for VS 2022 (Release), compiles the C++ library and GDExtension DLL, and auto-copies the DLL to `godot_project/adaptiveproceduralplatformer/`.*

- **Linux / macOS**:
  ```bash
  chmod +x build.sh
  ./build.sh
  ```

---

### Manual CMake Build (Debug & Release)

#### 1. Configure CMake

**Windows (Visual Studio 2022 x64):**
```cmd
mkdir build
cd build
cmake -S .. -B . -G "Visual Studio 17 2022" -A x64
```

**Linux / macOS (Ninja or Makefiles):**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

#### 2. Compile Targets

**Release Build (GDExtension DLL):**
```bash
cmake --build . --config Release --target AdaptiveProceduralLevelGeneration_gdextension
```

**Release Build (Pure C++ Standalone Game — Windows only):**
```bash
cmake --build . --config Release --target StandaloneApp
```

**Debug Build (GDExtension DLL & Standalone App):**
```bash
cmake --build . --config Debug
```

#### Target Summary

- `AdaptiveProceduralLevelGeneration`: Core C++ static library.
- `AdaptiveProceduralLevelGeneration_gdextension`: Shared library (`.dll` / `.so` / `.dylib`) loaded by Godot GDExtension.
- `StandaloneApp`: Pure Win32 GDI/GDI+ desktop game executable.

---

## 6. Running Automated Tests

The framework includes 10 automated test executables powered by CTest.

To execute all test suites:

```bash
# Navigate to build directory
cd build

# Run CTest
ctest -C Release --output-on-failure
```

### Expected Output
```text
100% tests passed out of 10
```

---

## 7. Opening & Running in Godot Engine

1. Launch **Godot Engine 4.2+** (or 4.4+).
2. In the Project Manager, click **Import**.
3. Browse to and select:
   `godot_project/adaptiveproceduralplatformer/project.godot`
4. Click **Import & Edit**.
5. Press **F5** (or click the play icon in the top right) to launch the main scene (`Scenes/Main.tscn`).
6. Check the **Output** dock in Godot to verify GDExtension registration logs (`[APLG GDExtension Initialized]`).

> **Note**: The GDExtension manifest file is located at `godot_project/adaptiveproceduralplatformer/aplg.gdextension`. It automatically points to `res://AdaptiveProceduralLevelGeneration_gdextension.dll`.

---

## 8. Running Standalone C++ Game

The repository contains a standalone Win32 C++ game engine implementation that runs independently of Godot.

To launch the standalone app:

```cmd
:: From the workspace root directory (so Brackeys assets resolve correctly):
.\build\Release\StandaloneApp.exe
```

### Controls (Standalone Game)
- **A / D** or **Left / Right Arrow**: Move left / right
- **Space** / **W** / **Up Arrow**: Jump
- **J**: Pencil Jab Attack
- **K**: Ruler Sweep Attack
- **U**: Desk Slam Attack
- **R**: Restart Level / Game
- **Esc / P**: Pause Game

---

## 9. Clean & Rebuild Instructions

To perform a clean rebuild of the project:

### Windows (Command Prompt / PowerShell)
```cmd
rmdir /s /q build
.\build.bat
```

### Linux / macOS
```bash
rm -rf build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### Cleaning `godot-cpp` Bindings
If you need to clean and rebuild `godot-cpp` bindings:
```bash
scons -C third_party/godot-cpp --clean
```

---

## 10. Troubleshooting

### Issue 1: `FATAL_ERROR: Godot C++ library not found`
- **Symptom**: CMake configure fails with `Godot C++ library not found at .../libgodot-cpp.windows.template_debug.x86_64.lib`.
- **Cause**: `godot-cpp` static library was not compiled before running CMake.
- **Fix**: Run SCons inside `third_party/godot-cpp`:
  ```cmd
  scons -C third_party/godot-cpp platform=windows target=template_debug generate_bindings=yes -j8
  ```

---

### Issue 2: `'scons' is not recognized as an internal or external command`
- **Symptom**: Command prompt returns error when running `scons`.
- **Cause**: Python or SCons scripts folder is not in PATH.
- **Fix**: Re-install SCons with pip (`pip install scons`) and verify Python `Scripts` directory (e.g., `C:\Python310\Scripts`) is in your Environment Variables PATH.

---

### Issue 3: GDExtension DLL fails to load in Godot Editor
- **Symptom**: Godot outputs `GDExtension library not found` or `aplg_library_init symbol missing`.
- **Cause**: DLL was not compiled or not copied to the Godot project directory.
- **Fix**:
  1. Re-run `.\build.bat` or `cmake --build build --config Release --target AdaptiveProceduralLevelGeneration_gdextension`.
  2. Confirm `AdaptiveProceduralLevelGeneration_gdextension.dll` exists inside `godot_project/adaptiveproceduralplatformer/`.
  3. Verify `aplg.gdextension` contains `entry_symbol = "aplg_library_init"`.

---

### Issue 4: `StandaloneApp.exe` crashes or missing textures
- **Symptom**: Standalone app fails to load images or sound effects.
- **Cause**: Running `StandaloneApp.exe` from inside `build/Release/` directory instead of workspace root.
- **Fix**: Always launch `StandaloneApp.exe` relative to the workspace root:
  ```cmd
  .\build\Release\StandaloneApp.exe
  ```
