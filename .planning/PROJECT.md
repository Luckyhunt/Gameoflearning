# Adaptive Procedural Level Generation (APLG) Framework

## What This Is

A production-grade C++20 framework and 2D platformer engine that generates procedural level layouts adapted in real-time to player behavior analytics and skill metrics. It supports a pure Win32 GDI 60 FPS standalone application as well as a Godot 4.x GDExtension shared library.

## Core Value

Real-time, solvability-guaranteed procedural level generation that dynamically adapts difficulty to player skill scoring without requiring third-party runtime dependencies for standalone play.

## Requirements

### Validated

- ✓ 6 Procedural Generation Algorithms (Cellular Automata, Random Walk, Perlin Noise, Constraint, Grammar, Room Graph) — existing
- ✓ Solvability & Reachability Pathfinder Validation (A*) — existing
- ✓ Player Analytics Engine (5 Playstyle Classifications: Explorer, Speedrunner, Aggressive, Careful, Collector) — existing
- ✓ Adaptive Difficulty Manager (5 Difficulty Tiers: Easy to Nightmare) — existing
- ✓ Standalone Win32 GDI Game Loop at 60 FPS with Particles, UI Dashboard, Enemies, and Combat — existing
- ✓ Godot 4.x GDExtension DLL integration — existing

### Active

- [ ] Complete unit test refactoring for `PlayerTest`, `GameplayTest`, and `UITest`
- [ ] Implement enhanced level decorator themes and additional enemy AI behaviors
- [ ] Add real-time level heatmap visualization for player movement and death telemetry
- [ ] Optimize MSVC build configurations and cmake dependency paths

### Out of Scope

- 3D level generation — framework focused specifically on 2D platformer mechanics.
- External runtime DLL dependencies for standalone executable — standalone mode must rely solely on native Win32 APIs.

## Context

Brownfield codebase containing complete C++20 engine subsystems (`Engine/`, `Player/`, `Physics/`, `Generation/`, `Validation/`, `AI/`, `Difficulty/`, `Gameplay/`, `Rendering/`, `Audio/`, `UI/`, `Utilities/`, `LevelEngine/`, `Decoration/`, `standalone/`, `Src/`, `Tests/`).

## Constraints

- **Language**: Modern C++20 (`std:c++20`).
- **Target OS**: Windows 10/11 x64.
- **Engine**: Pure Win32 GDI (Standalone) & Godot 4.x GDExtension.

## Key Decisions

| Decision | Rationale | Outcome |
|----------|-----------|---------|
| Pure Win32 GDI for Standalone | Guarantees zero external DLL dependencies for immediate execution | ✓ Good |
| A* Pathfinding Validator | Ensures generated levels are 100% solvable before presenting to player | ✓ Good |
| Modular Module System | Keeps physics, AI, difficulty, and rendering cleanly decoupled | ✓ Good |

---
*Last updated: 2026-09-10 after project initialization & PRD creation*

## Evolution

This document evolves at phase transitions and milestone boundaries.
