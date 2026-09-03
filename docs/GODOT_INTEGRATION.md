# Godot 4 GDExtension Integration Guide

This document outlines the architecture, setup, and communication model between the **C++ Core Engine** (`AdaptiveProceduralLevelGeneration_gdextension.dll`) and the **Godot 4 Engine GDScript Layer** (`godot_project/adaptiveproceduralplatformer/`).

---

## 1. Architectural Overview & Split Responsibilities

```
 ┌─────────────────────────────────────────────────────────────┐
 │                     Godot Engine 4.4                        │
 │                                                             │
 │   ┌──────────────────────┐      ┌──────────────────────┐   │
 │   │   GDScript Layer     │      │   Godot Scenes       │   │
 │   │  - Main.gd           │      │  - Main.tscn         │   │
 │   │  - LevelRenderer.gd  │      │  - Player.tscn       │   │
 │   │  - Player.gd         │      │  - HUD.tscn          │   │
 │   └──────────┬───────────┘      └──────────────────────┘   │
 └──────────────┼──────────────────────────────────────────────┘
                │ GDExtension Class DB Bindings
 ┌──────────────▼──────────────────────────────────────────────┐
 │                     C++ Core Engine                         │
 │                                                             │
 │   ┌──────────────────────┐      ┌──────────────────────┐   │
 │   │    EngineBridge      │      │ LevelEngine Pipeline │   │
 │   │ (GDExtension Node)   │      │ (Generators, Valids) │   │
 │   └──────────────────────┘      └──────────────────────┘   │
 └─────────────────────────────────────────────────────────────┘
```

### Responsibility Breakdown

#### C++ Core (`EngineBridge.cpp`, `LevelEngine/`, `Validation/`, `Difficulty/`):
- High-performance procedural level generation.
- Real-time reachability validation via `LevelValidator` (A* pathfinding and jump kinematics).
- Seed management and level reproducibility.
- Difficulty scaling and player capability modeling.

#### GDScript / Godot Layer (`Scripts/`, `Scenes/`):
- Godot node management (`Node2D`, `CharacterBody2D`, `TileMapLayer`).
- Tilemap rendering via `LevelRenderer.gd` and `TileService.gd`.
- Player character controller, input handling, and camera tracking.
- Audio streaming and UI control.

---

## 2. GDExtension Manifest (`aplg.gdextension`)

The GDExtension binding configuration is defined in:
`godot_project/adaptiveproceduralplatformer/aplg.gdextension`

```ini
[configuration]
entry_symbol = "aplg_library_init"
compatibility_minimum = 4.1

[libraries]
windows.debug.x86_64 = "res://AdaptiveProceduralLevelGeneration_gdextension.dll"
windows.release.x86_64 = "res://AdaptiveProceduralLevelGeneration_gdextension.dll"
```

The C++ build system (`build.bat`) automatically copies the compiled DLL to `godot_project/adaptiveproceduralplatformer/AdaptiveProceduralLevelGeneration_gdextension.dll` on every successful build.

---

## 3. C++ ↔ Godot Communication Model (`EngineBridge`)

`EngineBridge` is a C++ class inheriting from `godot::Node` and exported via GDExtension.

### Class Definition & Binding
- Entry point: `Src/GDExtension.cpp` registers `EngineBridge` with `ClassDB::register_class<EngineBridge>()`.
- Methods exposed to GDScript:
  - `generate_level(seed, difficulty)` -> Returns dictionary with tile grid, spawn position, exit position, enemy positions, collectible positions.
  - `validate_level(level_data)` -> Returns validation boolean and reachability diagnostics.
  - `update_player_skill(metrics)` -> Updates difficulty manager state based on player telemetry.

---

## 4. Preserved Godot Project Structure

```
godot_project/adaptiveproceduralplatformer/
├── project.godot
├── aplg.gdextension
├── AdaptiveProceduralLevelGeneration_gdextension.dll
├── Scenes/
│   ├── Main.tscn
│   ├── Player.tscn
│   ├── HUD.tscn
│   ├── MainMenu.tscn
│   ├── PauseMenu.tscn
│   ├── GameOverScreen.tscn
│   └── LevelCompleteScreen.tscn
└── Scripts/
    ├── Main.gd
    ├── LevelController.gd
    ├── LevelRenderer.gd
    ├── Player.gd
    ├── GameManager.gd
    └── TileService.gd
```
