# Adaptive Procedural Level Generation Framework for 2D Platform Games

## Overview
A comprehensive, production-grade implementation of an intelligent 2D platformer featuring adaptive procedural level generation based on player behavior analytics. Built with Modern C++20 and Godot 4.x GDExtension, designed for research and final-year projects.

## Features

### Procedural Generation
- **6 Generation Algorithms**: Cellular Automata, Random Walk, Perlin Noise, Constraint-Based, Grammar-Based, Room Graph
- **Level Validation**: Automatic verification of playability with pathfinding
- **Validated Generator**: Automatic regeneration until valid level produced
- **Configurable Parameters**: Width, height, density, gap sizes, platform constraints

### Adaptive Difficulty
- **5 Difficulty Levels**: Easy, Normal, Hard, Expert, Nightmare
- **Skill Score Calculation**: Based on deaths, time, accuracy, hit rate
- **Adaptive Adjustment**: Real-time difficulty based on player performance
- **Playstyle Presets**: Explorer, Speedrunner, Aggressive, Careful, Collector
- **Difficulty History**: Track changes over time

### Player Analytics
- **Action Tracking**: Record all player actions with timestamps
- **Behavior Statistics**: Movement speed, exploration, combat engagement, risk-taking
- **Playstyle Detection**: Automatic classification with confidence scoring
- **Session Analytics**: Track level completions, play time, dominant playstyle
- **JSON Export**: Export session data for analysis

### Player Mechanics
- **Movement**: Walk, run, jump with variable height
- **Abilities**: Double jump, wall jump, dash, climbing
- **Advanced**: Coyote time, jump buffering, dash cooldown
- **Status**: Health, lives, invincibility frames, checkpoints
- **Physics Integration**: Full physics body with collision

### Physics System
- **Collision Detection**: AABB-AABB, Circle-Circle, AABB-Circle
- **Physics Bodies**: AABB and Circle shapes with materials
- **Physics World**: Gravity, collision resolution, callbacks
- **Collision Layers**: Layer-based collision filtering
- **Collision Info**: Detailed collision results

### Gameplay Entities
- **Enemy AI**: Patrol, Chase, Flying, Stationary, Boss behaviors
- **Collectables**: Coins, gems, keys, health, ammo with respawn
- **Powerups**: Health, Double Jump, Dash, Shield, Speed
- **Entity Manager**: Centralized entity management with updates

### UI System
- **HUD**: Health, coins, time, level displays with progress bars
- **Menus**: Main menu, pause menu, settings menu
- **UI Elements**: Labels, buttons, progress bars
- **UI Manager**: Centralized UI coordination

### Rendering
- **Godot Integration**: GDExtension hooks for rendering
- **Camera System**: Follow player, bounds, zoom, rotation
- **Sprite Batching**: Optimized sprite rendering with layer sorting
- **Level Rendering**: Complete level display with spawn/exit

### Audio
- **Sound Types**: Music, SFX, Ambient
- **Volume Control**: Master, music, SFX volumes
- **Spatial Audio**: Distance-based volume falloff
- **Sound Library**: Asset management with config loading

### Save/Load System
- **Save Slots**: Multiple save slots with JSON persistence
- **Quick Save/Load**: Instant save functionality
- **Settings**: Audio, display, difficulty settings
- **Auto-Save**: Configurable interval auto-save manager

## Architecture

### Modular Design
Following SOLID principles with clear separation of concerns:

- **Engine**: Core systems (IEngine, IModule, ModuleManager)
- **Player**: Movement mechanics and abilities (IPlayer, Player)
- **Physics**: Collision detection (IPhysicsBody, CollisionDetection, PhysicsWorld)
- **Generation**: Procedural algorithms (ILevelGenerator, 6 implementations)
- **Validation**: Level validation (LevelValidator, PathFinder, ValidatedLevelGenerator)
- **Difficulty**: Adaptive system (IDifficultyManager, DifficultyManager, SkillScoreCalculator)
- **AI**: Player analytics (IPlayerModel, PlayerModel, SessionAnalytics, AdaptiveGenerator)
- **Gameplay**: Entities (IEntity, IEnemy, ICollectable, EntityManager)
- **Rendering**: Godot integration (IRenderer, GodotRenderer, SpriteBatch, LevelRenderer)
- **UI**: Interface (IUIElement, IHUD, IMenu, UIManager)
- **Audio**: Sound system (IAudioSystem, AudioManager, SoundLibrary)
- **Utilities**: JSON, CSV, logging (ILogger, Json, Csv, SaveSystem)

## Project Structure
```
AdaptiveProceduralLevelGeneration/
├── Engine/               # Core engine systems
│   ├── IEngine.h
│   ├── IModule.h
│   └── Engine.cpp
├── Player/               # Player mechanics
│   ├── IPlayer.h
│   └── PlayerModule.cpp
├── Physics/              # Physics and collision
│   ├── IPhysicsBody.h
│   ├── CollisionDetection.h
│   └── PhysicsModule.cpp
├── Generation/           # Procedural generation
│   ├── IGenerator.h
│   ├── Generator.cpp
│   ├── LevelValidator.h
│   ├── LevelValidator.cpp
│   └── GenerationModule.cpp
├── AI/                   # Player behavior analytics
│   ├── IPlayerModel.h
│   ├── PlayerModel.cpp
│   └── AIModule.cpp
├── Difficulty/           # Adaptive difficulty
│   ├── IDifficultyManager.h
│   ├── DifficultyManager.cpp
│   └── DifficultyModule.cpp
├── Gameplay/             # Game entities
│   ├── IEntity.h
│   ├── Entity.cpp
│   └── GameplayModule.cpp
├── Rendering/            # Godot rendering
│   ├── IRenderer.h
│   ├── Renderer.cpp
│   └── RenderingModule.cpp
├── Audio/                # Sound system
│   ├── IAudio.h
│   ├── Audio.cpp
│   └── AudioModule.cpp
├── UI/                   # User interface
│   ├── IUI.h
│   ├── UI.cpp
│   └── UIModule.cpp
├── Utilities/            # Helper utilities
│   ├── ILogger.h
│   ├── Json.h
│   ├── Csv.h
│   ├── SaveSystem.h
│   ├── SaveSystem.cpp
│   └── UtilitiesModule.cpp
├── Include/              # Public headers
│   └── Common.h
├── Src/                  # Main implementation
│   └── main.cpp
├── Tests/                # Unit tests
│   ├── Utilities/
│   ├── Physics/
│   ├── Player/
│   ├── Generation/
│   ├── Difficulty/
│   ├── AI/
│   ├── Gameplay/
│   └── UI/
├── godot/                # Godot integration
│   └── gdextension_config.json
├── CMakeLists.txt
├── build.bat
└── build.sh
```

## Requirements
- C++20 compatible compiler (MSVC 2019+, GCC 10+, Clang 12+)
- CMake 3.20+
- Godot 4.x (for GDExtension integration)
- Git

## Build Instructions

### Windows
```bash
# Clone repository
git clone <repository-url>
cd AdaptiveProceduralLevelGeneration

# Build using batch script
build.bat

# Or manually:
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Linux/macOS
```bash
# Clone repository
git clone <repository-url>
cd AdaptiveProceduralLevelGeneration

# Build using shell script
chmod +x build.sh
./build.sh

# Or manually:
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Testing

### Run All Tests
```bash
cd build
ctest
```

### Run Individual Tests
```bash
# Utilities test
./UtilitiesTest

# Physics test
./PhysicsTest

# Player test
./PlayerTest

# Generation test
./GenerationTest

# Level validation test
./LevelValidationTest

# Difficulty test
./DifficultyTest

# AI test
./AITest

# Gameplay test
./GameplayTest

# Save system test
./SaveSystemTest

# UI test
./UITest
```

### Stress Testing
```bash
# Generate 1000 levels and validate
./GenerationStressTest

# Results saved to stress_test_results.txt
```

## Usage Examples

### Generating a Level
```cpp
#include "Generation/IGenerator.h"

using namespace APLG;

GenerationConfig config;
config.minWidth = 50;
config.maxWidth = 80;
config.minHeight = 20;
config.maxHeight = 30;

RoomGraphGenerator generator;
generator.setSeed(12345);
LevelData level = generator.generate(config);
```

### Validating a Level
```cpp
#include "Generation/LevelValidator.h"

LevelValidator validator;
ValidationResult result = validator.validate(level);

if (result.valid) {
    // Level is playable
} else {
    std::cout << "Invalid: " << result.reason << std::endl;
}
```

### Adaptive Difficulty
```cpp
#include "Difficulty/IDifficultyManager.h"

DifficultyManager manager;
manager.setAdaptive(true);

// Update based on player performance
PlayerMetrics metrics;
metrics.deaths = 2;
metrics.completionTime = 60.0f;
metrics.jumpAccuracy = 0.9f;
metrics.enemyHitRate = 0.95f;

manager.updateDifficulty(metrics);
DifficultyLevel current = manager.getCurrentDifficulty();
```

### Player Analytics
```cpp
#include "AI/IPlayerModel.h"

PlayerModel model;
model.recordAction(PlayerAction::Jump, Vec2(0, 0), 0.0f);
model.recordAction(PlayerAction::Dash, Vec2(1, 0), 0.1f);

model.analyze();
Playstyle playstyle = model.getPlaystyle();
float32 confidence = model.getConfidence();
```

## Algorithm Explanations

### Cellular Automata
Uses Game of Life-like rules to generate cave-like structures. Cells become solid if they have >4 solid neighbors, empty if <4. Iterated 5 times for smooth results.

### Random Walk
Multiple walkers carve paths through solid terrain. Each walker moves randomly for a set number of steps, creating maze-like interconnected paths.

### Perlin Noise
Generates organic terrain using smooth noise interpolation. Threshold-based solid/empty assignment creates natural-looking caves and terrain.

### Constraint-Based
Places platforms, enemies, and collectables based on placement rules. Ensures specific gameplay requirements are met with validation checks.

### Grammar-Based
Uses L-systems with production rules to generate architectural patterns. S -> P-S | P | E, where P places platforms and E ends generation.

### Room Graph
Generates non-overlapping rooms and connects them with corridors. Uses BSP-like room placement with minimum/maximum size constraints.

## Complexity Analysis

### Generation Algorithms
- **Cellular Automata**: O(W × H × I) where W=width, H=height, I=iterations
- **Random Walk**: O(S × W) where S=steps, W=walkers
- **Perlin Noise**: O(W × H) with interpolation
- **Constraint-Based**: O(W × H) with validation
- **Grammar-Based**: O(L × R) where L=length, R=rules
- **Room Graph**: O(R²) for room placement, O(R × W) for corridors

### Pathfinding
- **BFS**: O(W × H) for level validation
- **Reachability**: O(W × H) per query

## License
MIT License - See LICENSE file for details

## Citation
If you use this project in research, please cite:
```
[Author]. Adaptive Procedural Level Generation Framework for 2D Platform Games using Player Behavior Analytics. [Year].
```

## Acknowledgments
Built with Modern C++20 and Godot 4.x GDExtension for research and educational purposes.
