#pragma once
// ============================================================
// GameplayDecorator.h
//
// Separates CONTENT from GEOMETRY in level generation.
//
// The PlatformerLevelEngine produces pure geometry (walls,
// floor, spawn, exit).  This decorator takes that geometry
// and populates it with:
//
//   Pass 1  Checkpoint placement  (evenly spaced along path)
//   Pass 2  Enemy placement       (constrained, role-aware)
//   Pass 3  Hazard placement      (scaled patterns per difficulty)
//   Pass 4  Powerup placement     (after hard sections)
//   Pass 5  Coin placement        (density + strategic options)
//   Pass 6  Platform annotation   (Moving/Falling/Bounce/Ice)
//
// All passes respect a unified set of placement constraints:
//   - Never overwrite Spawn / Exit / existing special tiles
//   - Enemies never placed on the critical path
//   - Enemies never within 3 tiles of Spawn
//   - Hazards on Easy never on path; on Hard+ may appear on path edges
//   - Platform variants clamped to the 75% physics budget
// ============================================================

#include "Common.h"
#include "PlayerCapabilities.h"
#include <random>
#include <vector>
#include <set>

namespace APLG {

// ──────────────────────────────────────────────────────────────
// DifficultyProfile
// Full content-parameter set derived from DifficultyLevel.
// Contains the absolute values from the user's design table.
// ──────────────────────────────────────────────────────────────

struct DifficultyProfile {
    // Map geometry (passed to PLEConfig — single source of truth)
    int32 mapWidth;
    int32 mapHeight;
    int32 minPlatformWidth;
    int32 maxPlatformWidth;
    int32 maxGap;
    int32 maxVerticalChange;

    // Platform variants
    float32 movingPlatformRate;   // 0–1: fraction of floor tiles converted
    float32 fallingPlatformRate;
    float32 bouncePadRate;
    float32 icePlatformRate;

    // Enemies
    int32   enemyCount;           // absolute count for the level
    float32 enemyBaseSpeed;       // tiles/sec base patrol speed
    bool    hasPatrolEnemies;
    bool    hasChasingEnemies;
    bool    hasFlyingEnemies;

    // Hazards
    float32 hazardDensity;        // hazard tiles per 100 empty floor tiles
    bool    hazardsAllowedOnPath; // Easy=false, Hard+=true (optional shortcuts)
    int32   maxHazardClusterSize; // max consecutive hazard tiles

    // Powerups
    float32 powerupsPerLevel;     // expected count (fractional OK, used as probability)
    bool    powerupsAfterHardSection;

    // Coins
    float32 coinDensity;          // coins per 100 standable-but-off-exit tiles
    bool    strategicCoinsOnly;   // Nightmare: coins only in dead-end branches

    // Checkpoints
    int32 checkpointCount;

    // Lives per level
    int32 lives;

    // ── Factory ─────────────────────────────────────────────
    static DifficultyProfile fromLevel(DifficultyLevel level);
};

// ──────────────────────────────────────────────────────────────
// GameplayDecorator
// ──────────────────────────────────────────────────────────────

class GameplayDecorator {
public:
    /**
     * @brief Decorate a geometry-only LevelData with full gameplay content.
     *
     * @param level   Geometry produced by PlatformerLevelEngine (modified in-place)
     * @param profile Content parameters from DifficultyProfile::fromLevel()
     * @param caps    Player physics (used for enemy/powerup placement logic)
     * @param seed    RNG seed (same seed → same decoration)
     * @return        Fully decorated LevelData
     */
    static LevelData decorate(const LevelData& level,
                               const DifficultyProfile& profile,
                               const Validation::PlayerCapabilities& caps,
                               uint32 seed);

private:
    // ── Sub-passes (in execution order) ───────────────────────

    static void placeCheckpoints(LevelData& level,
                                  const DifficultyProfile& profile,
                                  const std::vector<Vec2i>& path);

    static void placeEnemies(LevelData& level,
                              const DifficultyProfile& profile,
                              const std::vector<Vec2i>& path,
                              std::mt19937& rng);

    static void placeHazards(LevelData& level,
                               const DifficultyProfile& profile,
                               const std::vector<Vec2i>& path,
                               std::mt19937& rng);

    static void placePowerups(LevelData& level,
                               const DifficultyProfile& profile,
                               const std::vector<Vec2i>& path,
                               std::mt19937& rng);

    static void placeCoins(LevelData& level,
                            const DifficultyProfile& profile,
                            const std::vector<Vec2i>& path,
                            std::mt19937& rng);

    static void annotatePlatformVariants(LevelData& level,
                                          const DifficultyProfile& profile,
                                          const std::vector<Vec2i>& path,
                                          std::mt19937& rng);

    // ── Tile query helpers ─────────────────────────────────────

    static bool inBounds(const LevelData& level, int32 x, int32 y);

    /** True if (x,y) is solid or a platform-variant floor tile. */
    static bool isSolid(const LevelData& level, int32 x, int32 y);

    /** True if player can stand at (x,y): non-solid with solid below. */
    static bool isStandable(const LevelData& level, int32 x, int32 y);

    /** True if (x,y) has two clear rows above (headroom for player). */
    static bool hasHeadroom(const LevelData& level, int32 x, int32 y);

    /** True if tile is a "protected" special tile that cannot be overwritten. */
    static bool isProtected(TileType t);

    // ── Placement constraint helpers ───────────────────────────

    /** Returns set of path tile positions for fast lookup. */
    static std::set<std::pair<int32,int32>> buildPathSet(const std::vector<Vec2i>& path);

    /** Manhattan distance between two positions. */
    static int32 manhattanDist(Vec2i a, Vec2i b);

    /** True if pos is within radius Manhattan distance of any pos in list. */
    static bool nearAny(Vec2i pos, const std::vector<Vec2i>& list, int32 radius);
};

} // namespace APLG
