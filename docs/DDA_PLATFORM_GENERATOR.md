# DDA-Based Platform Generator Architecture & Reference Flow Analysis

## 1. Executive Summary

This document describes the design, mathematical formulation, and C++ implementation of the **Dynamic Difficulty Adjustment (DDA) Platform Generator**, incorporating the core mechanics of the reference [Pygame-Platformer (sami branch)](https://github.com/Imas7264/Pygame-Platformer/tree/sami) into the Adaptive Procedural Level Generation (APLG) engine.

---

## 2. Analysis of the Reference Repository (`Pygame-Platformer` - `sami` branch)

The reference implementation in `project/src/level/` consists of two primary generation scripts:
1. `container_generator.py` (`populate_container`)
2. `level_generator.py` (`generate_level_full`, `can_reach`, `jump_reach`, `generate_level_path`)

### 2.1 Reference Generator Logic & Flow
- **Container Enclosure**: Generates a bounded container (`#` border) with dimensions $W \times H$ in tile coordinates.
- **Platform Graph Node Placement**:
  - Starts with an initial platform at `(player_x, player_y + 2)`.
  - Maintains a queue of `active_platforms`.
  - In each iteration, randomly picks a parent platform from the active set and attempts to branch with offsets:
    $$\Delta x \in \{-4, -3, -2, 2, 3, 4\}, \quad \Delta y \in [-2, 2], \quad \text{length} \in [1, 4]$$
  - Bounding box collision check: verifies no overlap with existing platforms using padded margins (`BOUND_X = 1`, `BOUND_Y_UPPER = 2`, `BOUND_Y_LOWER = 1`).
  - Limits platform out-degree to $2$ (`connected == 2`), removing platforms from `active_platforms` once saturated.
  - Constructs traversal graphs with platform edge nodes and landing/drop-down edges.
- **Physics-Based Reachability Simulation**:
  - `jump_reach(dy)` executes a frame-by-frame Euler integration over 300 steps:
    $$v_y \leftarrow v_y + g, \quad x \leftarrow x + v_x, \quad y \leftarrow y + v_y$$
  - `can_reach(p1, p2)` verifies if the horizontal gap is within the simulated `jump_reach(dy)`.
- **Band & Zone Partitioning (`generate_level_full`)**:
  - Divides the room into horizontal bands (`BAND_H = 4`) and vertical zones (`ZONE_W = 8`).
  - Samples random platform candidates inside each cell and accepts those reachable from any already-placed platform.

---

## 3. Critical Flaws in the Reference Flow & Solutions

| # | Flaw in Python Reference Flow | Impact | Solution in APLG C++ DDA Engine |
|---|---|---|---|
| **1** | **No Guaranteed Critical Path / Exit Reachability** | `populate_container` stops at a random count ($8..12$) without placing a dedicated exit or guaranteeing a path to the right boundary. Levels frequently dead-end or cluster on one side. | Strict **Unidirectional Critical Path Spine**: Guaranteed start-to-exit traversability verified by bidirectional pathfinding before branching into alternate ledges. |
| **2** | **Rejection Sampling Bottleneck ($O(N \cdot \text{attempts})$)** | Relies on `while len(platforms) < count and attempts <= 100` with random guessing. High rejection rate leads to under-populated or malformed levels under spatial constraints. | Spatial grid occupancy hashing and guided progression wave that guarantees valid candidate generation in $O(1)$ amortized time. |
| **3** | **Kinematic Discontinuity in `jump_reach`** | Upward jumps use discrete Euler stepping while downward drops switch to closed-form $t = \sqrt{2h/g}$. Lacks collision awareness during arc trajectory (headroom clipping). | Unified kinematic trajectory solver with ceiling headroom raycasting ($\ge 2$ tiles open air guaranteed above each platform). |
| **4** | **Static / Hardcoded Geometry (No DDA)** | Delta offsets ($[-4, 4]$, $[-2, 2]$) and platform lengths ($1..4$) are hardcoded constants. Unresponsive to player performance or difficulty tiers. | **Dynamic Difficulty Adjustment (DDA)**: Platform widths, gap limits, jump heights, hazard density, and branch complexity are continuously scaled by player skill rating ($0.0 \to 1.0$) and difficulty profile. |
| **5** | **Dead-End Queue Exhaustion** | In `populate_container`, if active platforms get trapped near borders, they exhaust their 2 connections or fail boundary checks, terminating generation prematurely. | Resilient fallback queue and global candidate search preventing premature generation termination. |
| **6** | **Non-Deterministic RNG** | Python standard `random` lacks cross-platform determinism and seed replayability. | Pure deterministic seed-based PRNG ensuring identical layout recreation from any given seed. |

---

## 4. C++ DDA Platform Generator Architecture

### 4.1 DDA Parameter Mapping
The generator dynamically maps the player's real-time skill rating ($S \in [0.0, 1.0]$) and target difficulty tier to generator parameters:

$$\text{Platform Width} = \operatorname{round}\left(\operatorname{lerp}(W_{\max}, W_{\min}, S)\right)$$
$$\text{Max Gap} = \operatorname{round}\left(\operatorname{lerp}(G_{\min}, G_{\max}, S)\right)$$
$$\text{Max Step } \Delta y = \begin{cases} 1 & \text{if } S < 0.35 \text{ (Gentle)} \\ 2 & \text{if } 0.35 \le S \le 0.85 \text{ (Standard)} \\ 3 & \text{if } S > 0.85 \text{ (Demanding)} \end{cases}$$
$$\text{Alternate Branch Probability} = \operatorname{lerp}(0.1, 0.7, S)$$

### 4.2 Algorithm Pipeline
1. **Container Initialization**: Allocates $W \times H$ grid enclosed by solid perimeter walls (`Solid` border).
2. **Kinematic Reachability Precomputation**: Generates maximum horizontal gap lookup table $G(\Delta y)$ based on player jump velocity and gravity.
3. **Primary Critical Path Backbone**:
   - Progressive horizontal stepping from spawn $(x_0, y_0)$ to ancient door $(x_k, y_k)$.
   - Organic wave elevation target avoiding diagonal skewness while guaranteeing vertical amplitude $\ge 4$.
4. **Graph-Based Secondary & Alternate Ledge Expansion**:
   - Expands exploration branches and shortcuts from primary nodes using the graph-expansion model.
   - Enforces bounding box clearance and ceiling headroom ($\ge 2$ tiles).
5. **Decoration & Ancient Door Placement**:
   - Stamps platform surfaces (`Platform` type), open air corridors, and places the Ancient Door goal at the terminal node.
