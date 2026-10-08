# Product Requirements Document (PRD)

## Project Title
**Adaptive Procedural Level Generation (APLG) Framework for 2D Platform Games**

---

## 1. Executive Summary & Vision

The **Adaptive Procedural Level Generation (APLG) Framework** is a production-grade, high-performance C++20 software engine designed to generate dynamic 2D platformer levels adapted in real-time to player behavior analytics and skill metrics.

The framework supports dual execution targets:
1. **Pure Win32 C++ Standalone Application**: A zero-dependency 60 FPS platformer game built directly with Win32 GDI double-buffered graphics, custom particle systems, combat mechanics, and a rich UI dashboard.
2. **Godot 4.x GDExtension**: A shared native extension DLL (`AdaptiveProceduralLevelGeneration_gdextension.dll`) exposing the procedural generation, player analytics, and level validation engines directly to Godot 4 game projects.

---

## 2. Goals & Objectives

- **Adaptive Level Generation**: Real-time mutation of level parameters (platform density, gap widths, hazard frequency, verticality) based on player analytics.
- **Guaranteed Solvability**: 100% pathfinding verification (A* reachability validator) to ensure generated levels are never impossible to beat.
- **Player Playstyle Classification**: Automated categorizing of players into 5 distinct profiles (*Explorer*, *Speedrunner*, *Aggressive*, *Careful*, *Collector*) using timestamped behavioral tracking.
- **Dynamic Difficulty Tuning**: Real-time difficulty adjustment across 5 tiers (*Easy*, *Normal*, *Hard*, *Expert*, *Nightmare*) calculated via continuous skill scoring algorithms.
- **Engine Agnostic & High Performance**: Modular C++20 engine architecture requiring zero external runtime DLLs for standalone execution, with MSVC C++20 optimization.

---

## 3. Core Subsystems & Feature Breakdown

### 3.1. Procedural Level Generation Engine (`GenerationModule` & `LevelEngine`)
- **6 Core Algorithms**:
  1. *Cellular Automata*: Natural cave and organic terrain layout generation.
  2. *Random Walk / Tunneling*: Connected cavernous paths.
  3. *Perlin Noise*: Smooth heightmap terrain elevation generation.
  4. *Constraint-Based*: Rule-based platform and gap placement.
  5. *Grammar-Based*: Structural vocabulary rules for level progression.
  6. *Room Graph*: Node-based layout connecting distinct level rooms.
- **Multi-Elevation Platformer Pipeline**: Generates platforms across variable vertical layers, jump gaps, hazards, and spawn/exit points.
- **Gameplay Decoration**: Procedural placement of coins, gems, keys, health pickups, and enemy spawn locations (`GameplayDecorator`).

### 3.2. Solvability & Reachability Validation (`Validation`)
- **A* Pathfinding Reachability Engine**: Simulates player movement capabilities (jump height, jump distance, double jump, wall jump) against generated level grids.
- **Automatic Solvability Guarantee**: Re-generates or mutates layout parameters if a generated level fails pathfinding validation.
- **Regression Testing Suite**: Automated tests verifying reachability consistency (`ReachabilityRegressionTest`).

### 3.3. Player Analytics & Playstyle Engine (`AIModule`)
- **Action & Movement Tracking**: Logs player velocity, jump frequency, damage taken, death counts, time per section, combat engagement, and item collection.
- **Behavior Classifier**: Classifies players into dominant playstyles with confidence scores:
  - *Explorer*: Focuses on thorough map exploration and secret finding.
  - *Speedrunner*: Minimizes play time and maximizes movement efficiency.
  - *Aggressive*: Prioritizes killing enemies and engaging in combat.
  - *Careful*: Plays cautiously, taking minimal damage and avoiding risky gaps.
  - *Collector*: Focuses on collecting 100% of coins, gems, and items.
- **Data Export**: Serializes session telemetry to CSV (`analytics_test.csv`) and JSON (`metrics.json`).

### 3.4. Dynamic Adaptive Difficulty Pipeline (`DifficultyModule`)
- **Continuous Skill Score Calculation**: Evaluates player performance metrics (K/D ratio, time efficiency, damage taken rate, movement accuracy).
- **5 Difficulty Tiers**: Automatically shifts between *Easy*, *Normal*, *Hard*, *Expert*, and *Nightmare*.
- **Parameter Mutation**: Adjusts level generator parameters in real-time:
  - Gap width & platform spacing.
  - Enemy spawn density & patrol speeds.
  - Hazard frequency (spikes, traps).
  - Health pickup availability.

### 3.5. Standalone Win32 Game (`standalone/main.cpp`)
- **Double-Buffered GDI Renderer**: Smooth 60 FPS rendering at 1280x720 resolution with camera tracking and background parallax.
- **Expanded Player Controls & Combat**:
  - Movement (A/D/Arrows), Variable Jump (Space/W/Up), Light Attack (J), Heavy Attack (K), Dash (H), Special Ability (U), Interact (I).
- **Enemy Archetypes**: Patrol Enemies, Ranged Attackers, Flying Enemies, and Boss Encounters.
- **Particle System**: Spark, jump, dash, attack, and explosion particle effects.
- **In-Game Menus & Dashboard**: Start Menu, Pause Menu, Performance Dashboard, Controls Modal, Settings Modal, Game Over & Level Complete screens.

### 3.6. Godot 4 GDExtension Integration (`Src/GDExtension.cpp`)
- Exposes native C++ level generation, validation, and analytics nodes to Godot Engine 4.x via `third_party/godot-cpp`.
- Auto-copies `AdaptiveProceduralLevelGeneration_gdextension.dll` into the Godot project directory upon build completion.

---

## 4. Technical Requirements & Build Specifications

| Specification | Requirement |
| :--- | :--- |
| **Language Standard** | Modern C++20 (`std:c++20`) |
| **Compiler Support** | MSVC (Visual Studio 2022+), GCC 11+, Clang 13+ |
| **Build System** | CMake 3.20+ |
| **Runtime Target** | Windows 10/11 x64 |
| **Target Frameworks** | Win32 GDI, Godot 4.x GDExtension |
| **Target Frame Rate** | Locked 60 FPS (16.6ms frame budget) |
| **Testing Framework** | CTest / Google Test with fallback native executables |

---

## 5. Installation, Setup & Build Commands

### Prerequisites
- **CMake**: Version 3.20 or higher.
- **C++ Compiler**: Visual Studio 2022 (MSVC v143+) or GCC 11+ with C++20 support.
- **Godot Engine**: (Optional) Godot 4.x for testing GDExtension integration.

### Quick Start Build (Windows MSVC)
Run the automated build script:
```cmd
build.bat
```

### Manual CMake Build Steps
```bash
# 1. Create build directory
mkdir build
cd build

# 2. Configure project with CMake
cmake -G "Visual Studio 17 2022" -A x64 ..

# 3. Build Release targets (StandaloneApp, GDExtension DLL, Static Library, Tests)
cmake --build . --config Release
```

### Running the Standalone Executable
```cmd
build\Release\StandaloneApp.exe
```

### Running Automated Test Suite
```bash
cd build
ctest -C Release --output-on-failure
```

---

## 6. Standalone Game Controls & Usage Guide

| Action | Primary Key | Secondary Key |
| :--- | :--- | :--- |
| **Move Left / Right** | `A` / `D` | Left / Right Arrow |
| **Jump** | `Space` | `W` / Up Arrow |
| **Light Attack** | `J` | Mouse Left Click |
| **Heavy Attack** | `K` | Mouse Right Click |
| **Dash** | `H` | Shift |
| **Special Ability** | `U` | `E` |
| **Interact / Exit** | `I` | Enter |
| **Pause Game** | `P` | `ESC` |
| **Restart Level** | `R` | — |

---

## 7. Godot 4 GDExtension Integration Setup

1. Build the project using `build.bat` or CMake.
2. The build process automatically copies `AdaptiveProceduralLevelGeneration_gdextension.dll` to:
   `godot_project/adaptiveproceduralplatformer/`
3. Open `godot_project/adaptiveproceduralplatformer/project.godot` in Godot Engine 4.x.
4. The C++ GDExtension nodes will be registered natively and available in the Godot inspector.

---

## 8. Non-Functional Requirements

1. **Zero External Dependencies for Standalone**: The standalone executable must launch and run on stock Windows machines without requiring third-party DLLs, redistributables, or game engines.
2. **Modular SOLID Architecture**: All core features must interface through decoupled C++ abstract base classes (`IModule`, `ILevelGenerator`, `IPlayerModel`, `IDifficultyManager`).
3. **Robustness & Error Resilience**: Fallback level generation algorithms ensure that even if an extreme parameter combination fails pathfinding, the engine recovers gracefully.
4. **Performance & Memory Overhead**: Memory allocations must remain stable across hours of continuous level generation; zero memory leaks during runtime.

---

## 9. Success Criteria & Verification

- [x] All 6 procedural generation algorithms generate valid level layouts.
- [x] Solvability validator correctly rejects unplayable levels and verifies valid ones via A*.
- [x] Player analytics model accurately categorizes playstyles based on telemetry.
- [x] Adaptive difficulty manager dynamically mutates level parameters based on calculated skill score.
- [x] Win32 standalone game compiles and runs at 60 FPS with double-buffered GDI rendering.
- [x] GDExtension DLL builds and links with Godot 4.x.
- [x] Automated test suite builds cleanly and passes all subsystem tests (`ctest`).
