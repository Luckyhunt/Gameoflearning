# Adaptive Procedural Level Generation (APLG) — Godot & C++ Integration Guide

Welcome to the **Adaptive Procedural Level Generation (APLG)** engine documentation! This guide teaches you **everything** about how the core C++ procedural engine integrates seamlessly with Godot 4 using **GDExtension**, and how the Godot project mirrors the full standalone C++ game engine experience.

---

## Table of Contents
1. [Overview & Architecture](#1-overview--architecture)
2. [Standalone C++ Game vs. Godot Engine Parity](#2-standalone-c-game-vs-godot-engine-parity)
3. [Prerequisites & Environment Setup](#3-prerequisites--environment-setup)
4. [Building the C++ GDExtension DLL](#4-building-the-c-gdextension-dll)
5. [Opening and Running the Godot Game](#5-opening-and-running-the-godot-game)
6. [Deep Dive: How the C++ Integration Works](#6-deep-dive-how-the-c-integration-works)
   - [GDExtension Entry Point (`GDExtension.cpp`)](#gdextension-entry-point-gdextensioncpp)
   - [The Bridge Adapter Pattern (`EngineBridge.h` / `EngineBridge.cpp`)](#the-bridge-adapter-pattern-enginebridgeh--enginebridgecpp)
   - [Exposing Methods to GDScript (`_bind_methods`)](#exposing-methods-to-gdscript-_bind_methods)
   - [Data Marshaling (C++ Structs $\leftrightarrow$ Godot Dictionaries & Arrays)](#data-marshaling-c-structs-leftrightarrow-godot-dictionaries--arrays)
7. [GDScript Orchestration & Game Architecture](#7-gdscript-orchestration--game-architecture)
   - [`Main.gd` — Bootstrap & Session Management](#maingd--bootstrap--session-management)
   - [`LevelController.gd` — Level Lifecycle & Physics Checkpoints](#levelcontrollergd--level-lifecycle--physics-checkpoints)
   - [`LevelRenderer.gd` — Procedural Tile Rendering & Visual Tilesets](#levelrenderergd--procedural-tile-rendering--visual-tilesets)
   - [`EntitySpawner.gd` — Dynamic Enemy & Moving Platform Spawning](#entityspawnergd--dynamic-enemy--moving-platform-spawning)
   - [`Player.gd` & `Enemy.gd` — Player Abilities & AI Behaviors](#playergd--enemygd--player-abilities--ai-behaviors)
8. [Dynamic Difficulty Adjustment (DDA) & Player Analytics](#8-dynamic-difficulty-adjustment-dda--player-analytics)
9. [Controls & Keybindings Reference](#9-controls--keybindings-reference)
10. [Troubleshooting & FAQ](#10-troubleshooting--faq)

---

## 1. Overview & Architecture

The APLG system is designed around a **clean separation of concerns**:
- **C++ Core Engine**: High-performance procedural level generation, dynamic difficulty adjustment (DDA), mathematical level validation, graph reachability solvers, player playstyle modeling, and analytics tracking.
- **Godot 4.x Frontend**: Hardware-accelerated 2D rendering, TileMap rendering, physics processing, particle animations, sound effects, and rich UI canvas nodes.
- **Win32 GDI Standalone App**: A pure C++ zero-dependency desktop game built directly on top of the exact same C++ engine components.

```
       ┌─────────────────────────────────────────────────────────────┐
       │                   APLG C++ Core Engine                      │
       │  (LevelEngine, Generation, Validation, Difficulty, AI)     │
       └──────────────┬──────────────────────────────┬───────────────┘
                      │                              │
          Static Link │                              │ Shared Library DLL
                      ▼                              ▼
       ┌──────────────────────────────┐ ┌───────────────────────────┐
       │ Win32 GDI Standalone Game    │ │ Godot 4.x GDExtension     │
       │ (`standalone/main.cpp`)      │ │ (`EngineBridge` Node)     │
       └──────────────────────────────┘ └────────────┬──────────────┘
                                                     │ GDScript
                                                     ▼
                                        ┌───────────────────────────┐
                                        │ Godot 2D Platformer Game  │
                                        │ (Scenes & Scripts)        │
                                        └───────────────────────────┘
```

---

## 2. Standalone C++ Game vs. Godot Engine Parity

Both the **Standalone C++ Game** (`standalone/main.cpp`) and the **Godot Game** (`godot_project/adaptiveproceduralplatformer/`) share 100% parity in core gameplay, mechanics, and AI systems:

| Feature / System | Standalone C++ App (`standalone/main.cpp`) | Godot 4 App (`godot_project/`) |
| :--- | :--- | :--- |
| **Renderer** | Win32 GDI 60 FPS double-buffered graphics | Godot 2D Vulkan / Forward+ / CanvasItem rendering |
| **Level Generator** | `PlatformerLevelEngine` + `GameplayDecorator` | C++ engine via `EngineBridge.generate_adaptive_level()` |
| **Player Abilities** | Move, Jump, Light Attack (J), Heavy Attack (K), Dash (H), Special (U) | `Player.gd` with matching attack ranges, cooldowns & i-frames |
| **Enemy AI Types** | Patrol, Chase, Flying, Ranged (Line-of-Sight Chalk Thrower) | `Enemy.gd` (Patrol, Chase, Flying, Ranged with raycasts) |
| **Level Goal** | Clear all enemies / reach exit portal to advance | Clear all enemies or hit exit tile to advance adaptively |
| **Analytics & DDA** | Real-time skill score calculation, adaptive safety nets | `DifficultyManager` + `PlayerModel` + `Analytics.gd` |
| **Dashboards & UI** | C++ GDI Performance Dashboard, HUD, Pause Menu | Godot UI control nodes (`HUD.tscn`, `PerformanceDashboard.gd`) |

---

## 3. Prerequisites & Environment Setup

To compile the C++ GDExtension library and run the Godot project, ensure you have:

1. **Visual Studio 2022** (with C++ Desktop Development workload)
2. **CMake 3.20+**
3. **Godot Engine 4.2+ or 4.4+** (Standard 64-bit or .NET edition)
4. **Git** (for source control)

*(Note: Pre-configured C++ bindings for Godot 4 are already included in `third_party/godot-cpp` so no extra git submodules are required to build!)*

---

## 4. Building the C++ GDExtension DLL

Building the library is completely automated using CMake and MSVC.

### Windows (PowerShell / Command Prompt)

Run the root `build.bat` script:
```powershell
.\build.bat
```

#### What `build.bat` does under the hood:
1. Creates the `build/` directory if it doesn't exist.
2. Runs CMake to configure the Visual Studio 2022 x64 solution:
   ```bash
   cmake -S . -B build -G "Visual Studio 17 2022" -A x64
   ```
3. Compiles the `AdaptiveProceduralLevelGeneration_gdextension` DLL target in `Release` mode:
   ```bash
   cmake --build build --config Release --target AdaptiveProceduralLevelGeneration_gdextension
   ```
4. Executes the **Post-Build Auto-Copy Step** configured in `CMakeLists.txt`:
   ```
   Copies: build/Release/AdaptiveProceduralLevelGeneration_gdextension.dll
   To:     godot_project/adaptiveproceduralplatformer/AdaptiveProceduralLevelGeneration_gdextension.dll
   ```

---

## 5. Opening and Running the Godot Game

1. Launch **Godot Engine 4.x**.
2. Click **Import** in the Project Manager.
3. Browse to the project path:
   `d:\testing\Adaptive Procedural Level Generation\godot_project\adaptiveproceduralplatformer\`
4. Select `project.godot` and click **Import & Edit**.
5. Once inside the Godot Editor, press **F5** (or click the Play button in the top-right corner) to run `Scenes/Main.tscn`.
6. Look at the Godot **Output Panel** at the bottom — you will see initialization logs confirming the C++ engine has loaded:
   ```text
   ============================================
   [APLG] Adaptive Platform Generation Framework
   ============================================
   [EngineBridge] Initialized C++ APLG Engine v1.0.0
   [LevelController] Level 1 ready — 45x16
   ```

---

## 6. Deep Dive: How the C++ Integration Works

GDExtension is Godot 4's high-performance C++ binding mechanism. It allows C++ code to register custom nodes directly into Godot's type system (`ClassDB`) without modifying or recompiling the Godot Engine source code.

### GDExtension Entry Point (`Src/GDExtension.cpp`)

When Godot opens a project containing an `.gdextension` manifest (`aplg.gdextension`), it loads the DLL and invokes the exported initialization function `aplg_library_init`:

```cpp
extern "C" {

GDE_EXPORT GDExtensionBool aplg_library_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    GDExtensionClassLibraryPtr         p_library,
    GDExtensionInitialization*         r_initialization)
{
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
    init_obj.register_initializer(initialize_aplg_module);
    init_obj.register_terminator(uninitialize_aplg_module);

    return init_obj.init();
}

}
```

During initialization, `initialize_aplg_module` registers our custom node class with Godot:

```cpp
static void initialize_aplg_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) return;
    godot::ClassDB::register_class<EngineBridge>();
}
```

---

### The Bridge Adapter Pattern (`Engine/EngineBridge.h` & `Engine/EngineBridge.cpp`)

The `EngineBridge` class inherits from `godot::Node`. It acts as an **Adapter**, wrapping the native C++ engine classes (`APLG::IEngine`, `APLG::DifficultyManager`, `APLG::PlayerModel`) and translating between C++ data structures and Godot variants.

```cpp
class EngineBridge : public godot::Node {
    GDCLASS(EngineBridge, godot::Node)

public:
    EngineBridge();
    ~EngineBridge() override;

    void _ready() override;

    // Godot Callable Methods
    godot::String hello() const;
    godot::Dictionary generate_adaptive_level() const;
    godot::Dictionary generate_guaranteed_level() const;
    void report_level_metrics(int deaths, float time_sec, int coins, float jump_acc, float enemy_hit);
    int get_difficulty_level() const;
    godot::String get_difficulty_name() const;
    float get_skill_score() const;
    godot::Array get_death_heatmap() const;

protected:
    static void _bind_methods(); // Exposes methods to GDScript

private:
    std::unique_ptr<APLG::IEngine> m_engine;
    APLG::DifficultyManager        m_difficultyManager;
    APLG::PlayerModel              m_playerModel;
};
```

---

### Exposing Methods to GDScript (`_bind_methods`)

Every C++ method that should be accessible from GDScript must be registered inside `_bind_methods()` using `godot::ClassDB::bind_method()`:

```cpp
void EngineBridge::_bind_methods() {
    godot::ClassDB::bind_method(D_METHOD("hello"), &EngineBridge::hello);
    godot::ClassDB::bind_method(D_METHOD("get_engine_version"), &EngineBridge::get_engine_version);
    godot::ClassDB::bind_method(D_METHOD("generate_adaptive_level"), &EngineBridge::generate_adaptive_level);
    godot::ClassDB::bind_method(D_METHOD("generate_guaranteed_level"), &EngineBridge::generate_guaranteed_level);
    godot::ClassDB::bind_method(D_METHOD("report_level_metrics", "deaths", "time_sec", "coins", "jump_accuracy", "enemy_hit_rate"), &EngineBridge::report_level_metrics);
    godot::ClassDB::bind_method(D_METHOD("get_difficulty_level"), &EngineBridge::get_difficulty_level);
    godot::ClassDB::bind_method(D_METHOD("get_difficulty_name"), &EngineBridge::get_difficulty_name);
}
```

Once bound, GDScript code can seamlessly call these C++ functions as if they were native GDScript methods:

```gdscript
var bridge: EngineBridge = $EngineBridge
var level_dict: Dictionary = bridge.generate_adaptive_level()
var diff_name: String = bridge.get_difficulty_name()
```

---

### Data Marshaling (C++ Structs $\leftrightarrow$ Godot Dictionaries & Arrays)

When C++ generates a level (`APLG::LevelData`), `EngineBridge` converts the C++ data structures into a nested `godot::Dictionary` that GDScript can easily parse:

```cpp
godot::Dictionary EngineBridge::buildLevelDictionary(const APLG::LevelData& level, ...) const {
    godot::Dictionary dict;
    dict["width"] = level.width;
    dict["height"] = level.height;
    dict["spawn_x"] = level.spawnPosition.x;
    dict["spawn_y"] = level.spawnPosition.y;
    dict["exit_x"] = level.exitPosition.x;
    dict["exit_y"] = level.exitPosition.y;

    // Convert 2D tile grid to Godot Array of Arrays
    godot::Array tiles_arr;
    for (int y = 0; y < level.height; ++y) {
        godot::Array row;
        for (int x = 0; x < level.width; ++x) {
            row.append(static_cast<int>(level.tiles[y][x]));
        }
        tiles_arr.append(row);
    }
    dict["tiles"] = tiles_arr;

    // Convert Enemy positions and types
    godot::Array enemies_arr;
    for (size_t i = 0; i < level.enemyPositions.size(); ++i) {
        godot::Dictionary ed;
        ed["x"] = level.enemyPositions[i].x;
        ed["y"] = level.enemyPositions[i].y;
        ed["type"] = static_cast<int>(level.enemyTypes[i]);
        ed["speed"] = level.enemySpeeds[i];
        enemies_arr.append(ed);
    }
    dict["enemies"] = enemies_arr;

    return dict;
}
```

---

## 7. GDScript Orchestration & Game Architecture

The Godot frontend is organized into modularGDScript controllers:

### `Main.gd` — Bootstrap & Session Management
- Serves as the root scene controller for `Scenes/Main.tscn`.
- Instantiates the `EngineBridge` node and UI overlays (`HUD.tscn`, `PauseMenu`).
- Handles global keypresses (`ESC` for Pause, `R` for Restart, `N` for Next Level debug skip).

### `LevelController.gd` — Level Lifecycle & Physics Checkpoints
- Calls `EngineBridge.generate_adaptive_level()` to fetch the C++ level dictionary.
- Delegates tile rendering to `LevelRenderer.gd` and entity spawning to `EntitySpawner.gd`.
- Tracks real-time player collisions with Exit Portals, Coins, Checkpoints, and Hazard spikes.
- Connects `enemy_killed` signals: when all level enemies are defeated, automatically triggers level completion and transitions to the next adaptively generated level!

### `LevelRenderer.gd` — Procedural Tile Rendering & Visual Tilesets
- Constructs a dynamic Godot `TileSet` with procedural color-coded canvas textures for Solid Walls, Platforms, Ice Platforms, Bounce Pads, Coins, Spikes, and Exit Gates.
- Populates the Godot `TileMapLayer` node based on the C++ tile matrix.

### `EntitySpawner.gd` — Dynamic Enemy & Moving Platform Spawning
- Instantiates `Enemy.gd` nodes at the positions generated by the C++ engine.
- Assigns patrol parameters, enemy types, and tile lookup tables for line-of-sight and platform edge detection.

### `Player.gd` & `Enemy.gd` — Player Abilities & AI Behaviors
- **Player Controller**: Implements snappy platforming movement, gravity with fast-fall, coyote time (8 frames), jump buffering (10 frames), ice friction, bounce pad launches, and camera bounds constraint.
- **Combat Mechanics**:
  - **Pencil Jab (Light Attack `J`)**: Quick melee attack in facing direction.
  - **Ruler Sweep (Heavy Attack `K`)**: Wide melee swing dealing double damage.
  - **Air/Ground Dash (`H`)**: High-speed dash with 0.4s invulnerability frames.
  - **Desk Slam (Special Attack `U`)**: AoE shockwave clearing all nearby enemies.
- **Enemy AI**:
  - **Patrol (Math Teacher)**: Oscillates horizontally along platforms, turning around at edges.
  - **Chase (Principal / Alien Brute)**: Detects player within 220px, checks Raycast line-of-sight, and pursues player.
  - **Flying (Alien Drone)**: Sinusoidal floating pattern ignoring ground tiles.
  - **Ranged (Science Teacher / Sentry)**: Maintains distance, checks line-of-sight, and fires chalk projectiles at the player.

---

## 8. Dynamic Difficulty Adjustment (DDA) & Player Analytics

The game automatically adapts to player performance after every level completion:

1. At level end, `Player.gd` collects performance metrics:
   - Level completion time ($\text{seconds}$)
   - Total deaths and damage taken
   - Coins collected
   - Jump accuracy ($\frac{\text{Landed Jumps}}{\text{Attempted Jumps}}$)
   - Combat accuracy ($\frac{\text{Landed Attacks}}{\text{Attempted Attacks}}$)
   - Highest combo count
2. `Main.gd` passes these metrics into `EngineBridge.report_level_metrics(...)`.
3. The C++ `DifficultyManager` evaluates performance and adjusts parameters for the next level:
   - **Easy**: Small map, 4 patrol enemies, wide platforms, low gap width.
   - **Normal**: Standard 45x16 map, 6 enemies (Patrol + Chase), moderate hazards.
   - **Hard**: Extended map, 8–10 enemies, moving platforms, narrower jump tolerance.
   - **Nightmare**: High density enemy spawns, ranged sentries, ice platforms, bounce pads.
4. **Struggling Player Safety Net**: If a player dies frequently or takes heavy damage, the C++ engine automatically scales down difficulty to ensure the game remains fun and accessible.

---

## 9. Controls & Keybindings Reference

| Action | Primary Key | Secondary Key | Notes |
| :--- | :--- | :--- | :--- |
| **Move Left** | `A` | `Left Arrow` | Moves player left |
| **Move Right** | `D` | `Right Arrow` | Moves player right |
| **Jump** | `Space` | `W` / `Up Arrow` | Supports coyote time & variable height |
| **Pencil Jab (Light Attack)** | `J` | - | Fast single-target attack |
| **Ruler Sweep (Heavy Attack)** | `K` | - | Wide arc sweep dealing 2x damage |
| **Dash** | `H` | - | Burst of speed + i-frames |
| **Desk Slam (Special)** | `U` | - | AoE shockwave around player |
| **Interact** | `I` | - | Interacts with objects / terminals |
| **Pause / Resume** | `P` | `ESC` | Toggles Pause Overlay Menu |
| **Restart Level** | `R` | - | Instantly restarts current level |
| **Skip Level (Debug)** | `N` | - | Advances directly to next level |

---

## 10. Troubleshooting & FAQ

### Q1: Godot shows `EngineBridge node not found` or crashes on launch.
- **Cause**: The GDExtension DLL is missing or not compiled.
- **Fix**: Run `.\build.bat` in the root folder, verify that `AdaptiveProceduralLevelGeneration_gdextension.dll` is present in `godot_project/adaptiveproceduralplatformer/`, then restart Godot.

### Q2: I modified C++ code in `Engine/` or `Generation/`. How do I update Godot?
- **Fix**: Re-run `.\build.bat`. CMake will recompile the changed files and copy the updated DLL into the Godot project directory automatically. If Godot is running, restart the scene (or restart Godot if hot-reload is disabled).

### Q3: How do I run the Standalone C++ Game instead of Godot?
- **Fix**: Open PowerShell and run:
  ```powershell
  cmake --build build --config Release --target StandaloneApp
  .\build\Release\StandaloneApp.exe
  ```
  This launches the pure Win32 GDI C++ desktop game directly!

---
*Happy Level Generating!*
