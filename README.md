# Adaptive Procedural Level Generation Framework for 2D Platform Games

A production-grade, high-performance 2D platformer engine featuring adaptive procedural level generation, reachability validation, and real-time difficulty scaling based on player behavior analytics. Built with Modern C++20, Win32/GDI+, and Godot 4 GDExtension.

---

## Technical Highlights & Delivery Targets

1. **Pure C++ Standalone Platformer (`StandaloneApp.exe`)**:
   - 100% native C++20 executable using Win32 GDI, GDI+, and WinMM audio.
   - Fully integrated with Brackeys 2D platformer assets (animated sprites, tilemaps, WAV sound effects, MP3 background music).
   - Zero external DLL dependencies for runtime deployment.

2. **Godot 4 Engine Integration (`godot_project/`)**:
   - GDExtension shared library (`AdaptiveProceduralLevelGeneration_gdextension.dll`).
   - Clean C++ ↔ Godot communication model (`EngineBridge`).
   - GDScript gameplay orchestration, scene management, and node hierarchy preserved for future engine development.

3. **100% Deterministic & Reachable Level Generation**:
   - 6 procedural generation algorithms: Cellular Automata, Perlin Noise, Random Walk, Constraint-based, Grammar-based, Room-Graph.
   - Guaranteed 100% playability and jump reachability verified via `APLG::Validation::LevelValidator`.

---

## Documentation

Detailed documentation is available in the [`docs/`](./docs) directory:

- [**Build Guide** (`docs/BUILD.md`)](./docs/BUILD.md): Instructions for building C++ targets via CMake, Visual Studio 2022, `build.bat`, and running CTest automated test suites.
- [**Standalone Application** (`docs/STANDALONE.md`)](./docs/STANDALONE.md): C++ standalone controls, Win32 rendering architecture, and real-time dashboard analytics.
- [**Asset Mapping Guide** (`docs/ASSET_MAPPING.md`)](./docs/ASSET_MAPPING.md): Categorization and mapping of all Brackeys platformer assets across Standalone C++ and Godot 4.
- [**Godot Integration** (`docs/GODOT_INTEGRATION.md`)](./docs/GODOT_INTEGRATION.md): Architecture of GDExtension bindings, GDScript layer, and Godot scene structures.

---

## Directory Structure

```
Adaptive Procedural Level Generation/
├── brackeys_platformer_assets/  # Original Brackeys sprites, tiles, audio, fonts
├── docs/                        # Complete project documentation & build guides
│   ├── ASSET_MAPPING.md
│   ├── BUILD.md
│   ├── GODOT_INTEGRATION.md
│   └── STANDALONE.md
├── Engine/                      # Core engine modules and lifecycle
├── LevelEngine/                 # Procedural level generation pipeline
├── Validation/                  # Reachability, jump analysis, and pathfinding
├── Physics/                     # Continuous AABB collision detection
├── Player/                      # Player capabilities & kinematics models
├── Difficulty/                  # Adaptive difficulty manager & skill scoring
├── AI/                          # Player analytics and playstyle classifier
├── Utilities/                   # AssetManager, SaveSystem, Json, Csv, Logger
├── standalone/                  # Standalone C++ application (main.cpp, UIManager.h)
├── Src/                         # Godot GDExtension entry point (GDExtension.cpp)
├── godot_project/               # Godot 4 platformer project (Scenes & GDScript)
├── Tests/                       # 10 CTest automated test suites
├── CMakeLists.txt               # Main CMake build configuration
└── build.bat                    # One-click Windows build script
```

---

## Quick Start

### Building and Running Standalone C++ Game
1. Run the build script:
   ```cmd
   .\build.bat
   ```
2. Launch the standalone application:
   ```cmd
   .\build\Release\StandaloneApp.exe
   ```

### Running CTest Suite (10/10 Tests)
```cmd
cmake --build build --config Release --target RUN_TESTS
```

### Godot 4 Integration
Open `godot_project/adaptiveproceduralplatformer/project.godot` in **Godot 4.4+** and press **F5**.

---

## License & Attribution

- **Engine Core & Source Code**: Distributed under MIT License.
- **Assets**: Brackeys Platformer Assets (`brackeys_platformer_assets/LICENSE & CREDITS.txt`), created by Brackeys (https://brackeys.com).
