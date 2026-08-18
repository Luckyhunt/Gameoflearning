#pragma once
// ============================================================
// PlatformerLevelEngine.h
//
// Dedicated, always-playable 2-D platformer level generator.
// Generates by CONSTRUCTION — the path from spawn to exit is
// explicitly laid out step-by-step, so:
//
//   No validation retries ever needed
//   Every tile assignment is unique (no overlapping types)
//   Every platform has guaranteed floor under it + headroom above
//   Every jump gap is at most 75% of player capabilities
//   Coins placed only on reachable standable tiles
//   Hazard tiles never appear on the critical path
//   Solid border walls are always intact
//
// Design principles:
//   Spelunky-style chunk construction — divide into column segments
//   and guarantee left-right traversal.
//   Constraint-first: derive jump budget from PlayerCapabilities
//   and keep 25% safety margin so real physics always succeeds.
//   Single-write rule: each cell is assigned exactly once in a
//   priority order: Border > Spawn/Exit > Floor > Air > Decor.
// ============================================================

#include "Common.h"
#include "PlayerCapabilities.h"
#include <random>
#include <vector>
#include <string>

namespace APLG {

// ──────────────────────────────────────────────────────────────
// Configuration
// ──────────────────────────────────────────────────────────────

struct PLEConfig {
    // Level dimensions
    int32 width  = 24;
    int32 height = 12;

    // Gameplay parameters (derived from DifficultyParams in adaptive mode)
    float32 coinDensity        = 0.10f;  // coins per path tile
    float32 hazardDensity      = 0.00f;  // hazard tiles disabled
    int32   minPlatformWidth   = 3;      // minimum safe landing zone
    int32   maxPlatformWidth   = 6;      // maximum platform span
    int32   minGap             = 1;      // minimum horizontal gap between platforms
    int32   maxGap             = 3;      // maximum gap (clamped <= 75% maxJumpDistance)
    int32   maxVerticalChange  = 2;      // max |deltaY| per step (clamped <= 75% maxJumpHeight)

    // Route variety and vertical progression parameters
    float32 branchProbability  = 0.3f;   // probability of creating alternate routes
    int32   maxBranchDepth     = 2;      // maximum branching depth
    float32 verticalityFactor  = 0.5f;   // 0 = flat, 1 = highly vertical
    int32   minVerticalProgression = 4; // minimum required vertical amplitude (maxY - minY)
    bool    progressiveDifficulty  = true;// scale traversal difficulty along X axis
    bool    enableHighRoutes    = true;   // create elevated shortcut routes
    bool    enableLowRoutes     = true;   // create lower risk routes

    // Generation seed
    uint32 seed = 0;
};

// ──────────────────────────────────────────────────────────────
// Internal path node: one "landing zone" on the critical path
// ──────────────────────────────────────────────────────────────

struct PlatformNode {
    int32 x;          // left edge of landing zone (tile col)
    int32 y;          // row the player STANDS on (tile row, 0=top)
    int32 width;      // landing zone width in tiles
    bool  isStart;    // true -> place Spawn tile here
    bool  isEnd;      // true -> place Exit tile here
    bool  isAlternate; // true -> node belongs to an optional branch
};

// ──────────────────────────────────────────────────────────────
// PlatformerLevelEngine
// ──────────────────────────────────────────────────────────────

class PlatformerLevelEngine {
public:
    /**
     * @brief Generate a guaranteed-playable platformer level.
     *
     * 7-phase pipeline:
     *   Phase 1 - Critical path (left-to-right walk respecting jump budget)
     *   Phase 2 - Floor pass    (solid tiles beneath landing zones)
     *   Phase 3 - Clearance     (air carved above zones + across jump arcs)
     *   Phase 4 - Wall fill     (restore solid below gaps between platforms)
     *   Phase 5 - Border        (4 solid border edges, highest priority)
     *   Phase 6 - Decoration    (coins on path, hazards in isolated solid regions)
     *   Phase 7 - Spawn/Exit    (stamped last, cannot be overwritten)
     */
    static LevelData generate(const PLEConfig& cfg,
                              const Validation::PlayerCapabilities& caps);

    /**
     * @brief Build a PLEConfig from DifficultyProfile (single source of truth).
     * Geometry-only: content is applied by GameplayDecorator.
     */
    static PLEConfig configForDifficulty(DifficultyLevel level,
                                         uint32 seed = 0);

    /** Returns true if (x,y) is within level bounds. */
    static bool inBounds(const LevelData& level, int32 x, int32 y);
};

} // namespace APLG
