# Ralph Loop Prompt - Adaptive Procedural Level Generation (APLG) Framework

**Role:** Senior C++ Game Programmer & Godot Engine Expert
**Objective:** Develop an enterprise-grade, procedural platformer framework using Godot 4.7 as the front-end engine and C++ (via GDExtension) for all core game logic.

## 1. Tech Stack & Architecture
- **Primary Language:** C++ for all core logic, procedural generation, and mechanics.
- **Engine Integration:** Godot 4.7 (used strictly for rendering, engine layer, and editor tools). The game must be playable directly via the Godot editor's "Play" button and exportable as a standalone app.
- **Build System:** CMake must be used for compiling the GDExtension (do not use SCons).

## 2. Antigravity Mechanics (Priority Task)
- **Analysis First:** Analyze the mathematical and physics requirements for an antigravity system (gravity inversion, variable orientation vector, vector projection, player momentum preservation, surface alignment).
- **Implementation:** Integrate the antigravity behavior cleanly into the C++ physics controller so it interacts seamlessly with procedural environments and player mechanics.

## 3. Procedural Platform Generation & Spatial Rules
- **Skewness Fix:** Analyze and resolve any mathematical skewness issues in room generation algorithms.
- **Bounding Box Rules:** Every generated platform (whether a single block or clustered blocks) requires a strict invisible border/margin:
  - **Top Margin:** Equal to the player's maximum jump height ($H_{max} = \frac{v_{jump}^2}{2g}$).
  - **Left, Right, and Bottom Margins:** Exactly 1 block thick.
- **Placement & Overlap Constraints:**
  - Platform border zones are allowed to overlap by a maximum of exactly 1 block.
  - No physical platform block is ever allowed to spawn inside the designated border zone of another platform.
  - Subsequent platform placements must recursively respect these spatial constraints.

## 4. Room Generation & Dynamic Difficulty (DDA)
- **Room Variety:** Rooms must support specific texture/theme assignments. Two rooms with the exact same dimensions must procedurally generate completely different platform layouts, patterns, and levels.
- **Player-Driven Difficulty:** Implement a Dynamic Difficulty Adjustment (DDA) system tracking player performance to dictate platform layout complexity, room sizes, and overall level generation.

## 5. Execution Plan
- Provide C++/CMake boilerplate setup for Godot 4.7 GDExtension.
- Outline mathematical approach for antigravity mechanics and platform border generation logic before writing full scripts.
- Maintain 100% CTest pass rate across all unit tests.

