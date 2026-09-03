# C++ Standalone Platformer Application (`StandaloneApp.exe`)

The **Standalone C++ Application** is a 100% native 2D platformer game built directly in C++20 using Windows GDI, GDI+, and WinMM audio. It operates independently of external game engines and showcases the full procedural generation and reachability validation pipeline in real time.

---

## Technical Architecture

```
                                  ┌───────────────────────────────┐
                                  │      WinMain / WndProc        │
                                  └───────────────┬───────────────┘
                                                  │
                                  ┌───────────────▼───────────────┐
                                  │      AssetManager (GDI+)      │
                                  │ (Sprites, Tiles, Audio SFX)   │
                                  └───────────────┬───────────────┘
                                                  │
 ┌───────────────────────────────┐                │                ┌───────────────────────────────┐
 │   LevelGenerationPipeline     ├────────────────┼───────────────►│    GDI Double-Buffer Engine   │
 │   (Generators, Decorator)     │                │                │     (60 FPS Render Loop)      │
 └───────────────┬───────────────┘                │                └───────────────────────────────┘
                 │                                │
 ┌───────────────▼───────────────┐                │
 │ Validation::LevelValidator    │◄───────────────┘
 │ (Reachability & Jump Bounds)  │
 └───────────────────────────────┘
```

---

## Features

1. **Multi-Algorithm Procedural Level Generator**:
   - Generates unique, non-repetitive levels using Cellular Automata, Perlin Noise, Random Walk, Constraint-based, and Room-Graph procedural engines.
   - Scalable width and height based on current level progress and player skill.

2. **100% Reachability Guarantee**:
   - Every generated level is verified by `APLG::Validation::LevelValidator` (analyzing jump gaps, ceiling clearance, and connectivity) before spawning.

3. **Brackeys Asset Pipeline Integration**:
   - Animated player sprite (`knight.png`).
   - Sprite tileset terrain & platforms (`world_tileset.png`, `platforms.png`).
   - Animated enemies (`slime_green.png`, `slime_purple.png`).
   - Animated collectibles (`coin.png`, `fruit.png`).
   - Dynamic WAV audio effects (Jump, Coin, Hurt, Explosion, PowerUp, Tap) and MP3 background music.

4. **Combat & Physics Engine**:
   - Sub-stepped AABB Continuous Collision Detection (CCD) preventing high-velocity tunneling.
   - Smooth horizontal inertia and coyote time jump buffer.
   - Light Attack (Pencil Jab), Heavy Attack (Ruler Sweep), Invincible Dash, and Special AoE Desk Slam.

5. **Live Performance & Skill Analytics Dashboard**:
   - Press `[S]` to open the performance dashboard displaying jump accuracy, combat hit ratio, difficulty scaling parameters, and generation timing.

---

## Controls & Keybindings

| Input Key | Action / Function |
| :--- | :--- |
| **A / D** or **Left / Right** | Move Left / Move Right |
| **Space / W / Up** | Jump / Wall Jump |
| **J** | Light Melee Attack (Pencil Jab) |
| **K** | Heavy Melee Attack (Ruler Sweep) |
| **H** | Invincible Dash |
| **U** | Special AoE Shockwave Attack |
| **I** | Interact / Checkpoint |
| **P / ESC** | Pause / Resume Game |
| **R** | Restart Level |
| **S** | Open Performance Statistics Dashboard |
| **C** | Open Controls & Keybinding Guide |
| **O** | Open Settings & Options Modal |

---

## Runtime Logs

The application outputs real-time diagnostics to `standalone_game.log` in the working directory:
```
1. Entry point reached: WinMain executed.
2. Window successfully created.
Initializing Level Engine & Generator...
AssetManager: Asset root located at: D:/testing/Adaptive Procedural Level Generation/brackeys_platformer_assets
AssetManager: Loaded texture knight
AssetManager: Loaded texture world_tileset
6. Player visible & initialized.
7. Level visible & procedural terrain generated.
3. Message loop running.
4. Game loop executing continuously.
5. Renderer drawing frames.
```
