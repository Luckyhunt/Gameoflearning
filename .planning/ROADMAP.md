# Roadmap: Adaptive Procedural Level Generation (APLG) Framework

## Milestones

- **v1.0 Core Engine & Dual Targets** (Phases 1 - 4) — *Active Milestone*

## Phases

### Phase 1: Procedural Level Generation & Solvability Validation
**Goal:** Complete multi-elevation procedural level generation with 6 algorithms and guaranteed solvability validation.
- [x] Implement 6 generation algorithms (Cellular Automata, Random Walk, Perlin Noise, Constraint, Grammar, Room Graph)
- [x] Build A* pathfinder level validator to verify playability
- [x] Implement procedural level decorator for items, hazards, and enemy spawns
- **Deliverables:** `GenerationModule`, `LevelValidator`, `PlatformerLevelEngine`

### Phase 2: Analytics & Adaptive Difficulty Engine
**Goal:** Track player behavior telemetry and dynamically adapt level difficulty parameters in real-time.
- [x] Build player analytics tracker for movement, combat, exploration, and deaths
- [x] Implement 5 playstyle classifiers (Explorer, Speedrunner, Aggressive, Careful, Collector)
- [x] Implement 5-tier adaptive difficulty manager (Easy -> Nightmare) with skill scoring
- **Deliverables:** `AIModule`, `DifficultyModule`, telemetry JSON/CSV exporters

### Phase 3: Standalone Win32 Game & Godot GDExtension
**Goal:** Deliver 60 FPS Win32 double-buffered game application and Godot 4 native extension DLL.
- [x] Build pure Win32 GDI standalone game loop with combat, UI dashboard, and particle FX
- [x] Implement Godot 4 GDExtension bindings (`Src/GDExtension.cpp`)
- **Deliverables:** `StandaloneApp.exe`, `AdaptiveProceduralLevelGeneration_gdextension.dll`

### Phase 4: Test Suite Refactoring & Pipeline Optimization
**Goal:** Refactor disabled unit tests (`PlayerTest`, `GameplayTest`, `UITest`) and optimize CTest verification pipeline.
- [ ] Fix private member access and API bindings in `PlayerTest`, `GameplayTest`, and `UITest`
- [ ] Ensure 100% CTest pass rate across all unit test targets
- **Deliverables:** Refactored unit tests in `Tests/`

---
*Roadmap created: 2026-09-10*
