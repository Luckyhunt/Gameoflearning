# Godot GDExtension Setup Guide

## Prerequisites

1. **Install Godot 4.2+**
   - Download from https://godotengine.org/download
   - Install the .NET version for C# support (optional) or standard version

2. **Download Godot C++ Bindings**
   - Clone or download from: https://github.com/godotengine/godot-cpp
   - For Godot 4.2, use the 4.2 branch
   ```bash
   git clone -b 4.2 https://github.com/godotengine/godot-cpp.git
   ```

3. **Build Godot C++ Bindings**
   ```bash
   cd godot-cpp
   scons platform=windows generate_bindings=yes
   ```
   This will create the necessary headers and libraries.

## Project Structure Setup

1. **Copy Godot C++ headers to project**
   ```
   Adaptive Procedural Level Generation/
   ├── godot/
   │   ├── include/          # Copy from godot-cpp/godot_headers/
   │   └── gdextension/      # Copy from godot-cpp/include/
   ```

2. **Update CMakeLists.txt**
   - Add Godot C++ library linking
   - Add GDExtension entry point

3. **Build the library**
   ```bash
   .\build.bat
   ```
   This should produce `AdaptiveProceduralLevelGeneration.dll`

## Godot Project Setup

1. **Create Godot project**
   - Open Godot Editor
   - Import Project → Select `godot_project/project.godot`
   - Or create new project and copy the configuration

2. **Configure GDExtension**
   - Create `GDExtensionLibrary.gdextension` file:
   ```
   [configuration]
   entry_symbol="example_library_init"
   compatibility_minimum=4.2
   reloadable=true
   ```

3. **Place the DLL**
   - Copy `build/Release/AdaptiveProceduralLevelGeneration.dll` to `godot_project/`
   - Copy `GDExtensionLibrary.gdextension` to `godot_project/`

## Testing the Integration

1. **Create a test scene**
   - In Godot Editor, create a new scene
   - Add a Node2D as root
   - Attach a script that calls GDExtension functions

2. **Run the project**
   - Press F5 in Godot Editor
   - Check console for GDExtension loading messages

## Current Status

- ✅ Core library built successfully
- ✅ GDExtension entry point created
- ⏳ Godot C++ bindings need to be downloaded and built
- ⏳ CMake configuration needs Godot library linking
- ⏳ GDExtension configuration file needs to be created
- ⏳ Test scene needs to be created

## Next Steps

1. Download and build Godot C++ bindings
2. Copy headers to godot/ directory
3. Update CMakeLists.txt with proper Godot linking
4. Build the DLL
5. Set up Godot project
6. Create test scene

## Alternative: Standalone Application

If Godot setup is too complex, you can create a standalone C++ application:
- Use SDL2 for window management
- Use OpenGL for rendering
- Call library functions directly from main.cpp

This would be simpler for testing the procedural generation without the full Godot integration overhead.
