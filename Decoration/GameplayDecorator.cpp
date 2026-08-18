// ============================================================
// GameplayDecorator.cpp
// ============================================================

#include "GameplayDecorator.h"
#include "PathFinder.h"
#include "ILogger.h"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace APLG {

using namespace Validation;

// ──────────────────────────────────────────────────────────────
// DifficultyProfile::fromLevel
// ──────────────────────────────────────────────────────────────

DifficultyProfile DifficultyProfile::fromLevel(DifficultyLevel level)
{
    DifficultyProfile p{};
    switch (level) {
        case DifficultyLevel::Easy:
            p.mapWidth = 32;          p.mapHeight = 12;
            p.minPlatformWidth = 4;   p.maxPlatformWidth = 7;
            p.maxGap = 2;             p.maxVerticalChange = 1;
            p.movingPlatformRate  = 0.00f;
            p.fallingPlatformRate = 0.00f;
            p.bouncePadRate       = 0.00f;
            p.icePlatformRate     = 0.00f;
            p.enemyCount          = 0;
            p.enemyBaseSpeed      = 40.0f;
            p.hasPatrolEnemies    = false;
            p.hasChasingEnemies   = false;
            p.hasFlyingEnemies    = false;
            p.hazardDensity           = 0.00f;
            p.hazardsAllowedOnPath    = false;
            p.maxHazardClusterSize    = 0;
            p.powerupsPerLevel        = 3.0f;
            p.powerupsAfterHardSection= false;
            p.coinDensity             = 20.0f;
            p.strategicCoinsOnly      = false;
            p.checkpointCount         = 3;
            p.lives                   = 5;
            break;

        case DifficultyLevel::Normal:
            p.mapWidth = 45;          p.mapHeight = 16;
            p.minPlatformWidth = 3;   p.maxPlatformWidth = 6;
            p.maxGap = 3;             p.maxVerticalChange = 2;
            p.movingPlatformRate  = 0.00f;
            p.fallingPlatformRate = 0.00f;
            p.bouncePadRate       = 0.00f;
            p.icePlatformRate     = 0.00f;
            p.enemyCount          = 4;
            p.enemyBaseSpeed      = 60.0f;
            p.hasPatrolEnemies    = true;
            p.hasChasingEnemies   = true;
            p.hasFlyingEnemies    = false;
            p.hazardDensity           = 0.00f;
            p.hazardsAllowedOnPath    = false;
            p.maxHazardClusterSize    = 0;
            p.powerupsPerLevel        = 0.00f;
            p.powerupsAfterHardSection= false;
            p.coinDensity             = 14.0f;
            p.strategicCoinsOnly      = false;
            p.checkpointCount         = 2;
            p.lives                   = 3;
            break;

        case DifficultyLevel::Hard:
            p.mapWidth = 60;          p.mapHeight = 20;
            p.minPlatformWidth = 2;   p.maxPlatformWidth = 5;
            p.maxGap = 3;             p.maxVerticalChange = 2;
            p.movingPlatformRate  = 0.00f;
            p.fallingPlatformRate = 0.00f;
            p.bouncePadRate       = 0.00f;
            p.icePlatformRate     = 0.00f;
            p.enemyCount          = 6;
            p.enemyBaseSpeed      = 80.0f;
            p.hasPatrolEnemies    = true;
            p.hasChasingEnemies   = true;
            p.hasFlyingEnemies    = true;
            p.hazardDensity           = 0.00f;
            p.hazardsAllowedOnPath    = false;
            p.maxHazardClusterSize    = 0;
            p.powerupsPerLevel        = 0.00f;
            p.powerupsAfterHardSection= false;
            p.coinDensity             = 10.0f;
            p.strategicCoinsOnly      = false;
            p.checkpointCount         = 1;
            p.lives                   = 3;
            break;

        case DifficultyLevel::Expert:
            p.mapWidth = 75;          p.mapHeight = 22;
            p.minPlatformWidth = 3;   p.maxPlatformWidth = 5;
            p.maxGap = 2;             p.maxVerticalChange = 2;
            p.movingPlatformRate  = 0.00f;
            p.fallingPlatformRate = 0.00f;
            p.bouncePadRate       = 0.00f;
            p.icePlatformRate     = 0.00f;
            p.enemyCount          = 16;   // Expanded uncapped faculty/alien horde
            p.enemyBaseSpeed      = 100.0f;
            p.hasPatrolEnemies    = true;
            p.hasChasingEnemies   = true;
            p.hasFlyingEnemies    = true;
            p.hazardDensity           = 0.00f;
            p.hazardsAllowedOnPath    = false;
            p.maxHazardClusterSize    = 0;
            p.powerupsPerLevel        = 0.00f;
            p.powerupsAfterHardSection= false;
            p.coinDensity             = 0.0f;
            p.strategicCoinsOnly      = false;
            p.checkpointCount         = 1;
            p.lives                   = 3;
            break;

        case DifficultyLevel::Nightmare:
            p.mapWidth = 95;          p.mapHeight = 26; // Open long playground after Level 10
            p.minPlatformWidth = 3;   p.maxPlatformWidth = 5;
            p.maxGap = 2;             p.maxVerticalChange = 2;
            p.movingPlatformRate  = 0.00f;
            p.fallingPlatformRate = 0.00f;
            p.bouncePadRate       = 0.00f;
            p.icePlatformRate     = 0.00f;
            p.enemyCount          = 25;   // Uncapped extreme enemy horde
            p.enemyBaseSpeed      = 120.0f;
            p.hasPatrolEnemies    = true;
            p.hasChasingEnemies   = true;
            p.hasFlyingEnemies    = true;
            p.hazardDensity           = 0.00f;
            p.hazardsAllowedOnPath    = false;
            p.maxHazardClusterSize    = 0;
            p.powerupsPerLevel        = 0.00f;
            p.powerupsAfterHardSection= false;
            p.coinDensity             = 0.0f;
            p.strategicCoinsOnly      = true;
            p.checkpointCount         = 1;
            p.lives                   = 3;
            break;
    }
    return p;
}

// ──────────────────────────────────────────────────────────────
// Tile helpers (local to this translation unit)
// ──────────────────────────────────────────────────────────────

bool GameplayDecorator::inBounds(const LevelData& level, int32 x, int32 y)
{
    return x >= 0 && x < level.width && y >= 0 && y < level.height;
}

bool GameplayDecorator::isSolid(const LevelData& level, int32 x, int32 y)
{
    if (!inBounds(level, x, y)) return true; // out of bounds = solid boundary
    TileType t = level.tiles[y][x];
    return t == TileType::Solid
        || t == TileType::Platform
        || t == TileType::MovingPlatform
        || t == TileType::FallingPlatform
        || t == TileType::BouncePad
        || t == TileType::IcePlatform
        || t == TileType::OneWayPlatform;
}

bool GameplayDecorator::isStandable(const LevelData& level, int32 x, int32 y)
{
    if (!inBounds(level, x, y)) return false;
    if (isSolid(level, x, y)) return false;
    return isSolid(level, x, y + 1);
}

bool GameplayDecorator::hasHeadroom(const LevelData& level, int32 x, int32 y)
{
    return !isSolid(level, x, y - 1) && !isSolid(level, x, y - 2);
}

bool GameplayDecorator::isProtected(TileType t)
{
    return t == TileType::Spawn
        || t == TileType::Exit
        || t == TileType::Checkpoint;
}

std::set<std::pair<int32,int32>>
GameplayDecorator::buildPathSet(const std::vector<Vec2i>& path)
{
    std::set<std::pair<int32,int32>> s;
    for (const auto& p : path) s.insert({p.x, p.y});
    return s;
}

int32 GameplayDecorator::manhattanDist(Vec2i a, Vec2i b)
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

bool GameplayDecorator::nearAny(Vec2i pos, const std::vector<Vec2i>& list, int32 radius)
{
    for (const auto& v : list)
        if (manhattanDist(pos, v) <= radius) return true;
    return false;
}

// ──────────────────────────────────────────────────────────────
// Public: decorate
// ──────────────────────────────────────────────────────────────

LevelData GameplayDecorator::decorate(const LevelData& src,
                                       const DifficultyProfile& profile,
                                       const PlayerCapabilities& caps,
                                       uint32 seed)
{
    LOG_INFO("GameplayDecorator: decorating " +
             std::to_string(src.width) + "x" + std::to_string(src.height));

    LevelData level = src; // work on a copy

    // Initialise death heatmap
    level.deathHeatmap.assign(level.height,
                              std::vector<int32>(level.width, 0));

    // Use the geometry author's critical path when available; fall back to BFS.
    std::vector<Vec2i> path = level.criticalPath;
    if (path.empty()) {
        path = PathFinder::findPath(
            level, level.spawnPosition, level.exitPosition, caps);
    }

    if (path.empty()) {
        LOG_INFO("GameplayDecorator: no critical path available — skipping decoration");
        return level;
    }

    std::mt19937 rng(seed ? seed : 12345u);

    // Six passes in priority order
    placeCheckpoints(level, profile, path);
    placeEnemies    (level, profile, path, rng);
    placeHazards    (level, profile, path, rng);
    placePowerups   (level, profile, path, rng);
    placeCoins      (level, profile, path, rng);
    annotatePlatformVariants(level, profile, path, rng);

    LOG_INFO("GameplayDecorator: done. Enemies=" +
             std::to_string(level.enemyPositions.size()) +
             " Coins=" + std::to_string(level.coinPositions.size()) +
             " Hazards=" + std::to_string(level.hazardPositions.size()) +
             " Checkpoints=" + std::to_string(level.checkpointPositions.size()));
    return level;
}

// ──────────────────────────────────────────────────────────────
// Pass 1 — Checkpoints
// Place checkpoints at evenly-spaced positions along the path.
// ──────────────────────────────────────────────────────────────

void GameplayDecorator::placeCheckpoints(LevelData& level,
                                          const DifficultyProfile& profile,
                                          const std::vector<Vec2i>& path)
{
    if (profile.checkpointCount <= 0 || path.size() < 2) return;

    int32 n = static_cast<int32>(path.size());
    int32 count = std::min(profile.checkpointCount, std::max(1, n - 1));

    for (int32 i = 1; i <= count; ++i) {
        int32 idx = i * n / (count + 1);
        idx = std::clamp(idx, 1, n - 2);
        Vec2i pos = path[idx];

        Vec2i chosenPos = pos;
        bool found = false;
        if (isStandable(level, pos.x, pos.y) && !isProtected(level.tiles[pos.y][pos.x])) {
            found = true;
        } else {
            for (int32 offset = -3; offset <= 3 && !found; ++offset) {
                int32 searchIdx = std::clamp(idx + offset, 1, n - 2);
                Vec2i candidate = path[searchIdx];
                if (isStandable(level, candidate.x, candidate.y) && !isProtected(level.tiles[candidate.y][candidate.x])) {
                    chosenPos = candidate;
                    found = true;
                }
            }
        }

        if (found) {
            level.tiles[chosenPos.y][chosenPos.x] = TileType::Checkpoint;
            level.checkpointPositions.push_back(chosenPos);
        }
    }

    if (level.checkpointPositions.empty() && profile.checkpointCount > 0) {
        for (int32 y = 1; y < level.height - 1; ++y) {
            for (int32 x = 2; x < level.width - 2; ++x) {
                if (isStandable(level, x, y) && !isProtected(level.tiles[y][x])) {
                    if (manhattanDist(Vec2i(x, y), level.spawnPosition) >= 2 && manhattanDist(Vec2i(x, y), level.exitPosition) >= 2) {
                        level.tiles[y][x] = TileType::Checkpoint;
                        level.checkpointPositions.push_back(Vec2i(x, y));
                        break;
                    }
                }
            }
            if (!level.checkpointPositions.empty()) break;
        }
    }
}

// ──────────────────────────────────────────────────────────────
// Pass 2 — Enemies
// Constraint-aware placement:
//   ✗ Never on critical path
//   ✗ Never within 3 tiles of Spawn
//   ✗ Never within 1 tile of a Checkpoint
//   ✗ Never within 2 tiles of another enemy
//   ✓ Prefer wider platforms for patrol enemies
//   ✓ Assign type based on difficulty profile
// ──────────────────────────────────────────────────────────────

void GameplayDecorator::placeEnemies(LevelData& level,
                                      const DifficultyProfile& profile,
                                      const std::vector<Vec2i>& path,
                                      std::mt19937& rng)
{
    if (profile.enemyCount <= 0) return;

    (void)path;
    std::uniform_real_distribution<float32> chance(0.0f, 1.0f);

    // Collect all standable candidate tiles (and air tiles for flying enemies)
    std::vector<Vec2i> groundCandidates;
    std::vector<Vec2i> airCandidates;

    for (int32 y = 1; y < level.height - 1; ++y) {
        for (int32 x = 2; x < level.width - 2; ++x) {
            Vec2i pos(x, y);

            if (manhattanDist(pos, level.spawnPosition) < 7) continue;    // safe spawn radius (minimum 7 tiles away)
            if (nearAny(pos, level.checkpointPositions, 1)) continue;     // near checkpoint
            if (isProtected(level.tiles[y][x])) continue;

            if (isStandable(level, x, y) && hasHeadroom(level, x, y)) {
                groundCandidates.push_back(pos);
            } else if (level.tiles[y][x] == TileType::Empty && level.tiles[y - 1][x] == TileType::Empty) {
                airCandidates.push_back(pos);
            }
        }
    }

    // Distribute ground candidates across elevation tiers (Low, Mid, High)
    std::vector<Vec2i> lowCandidates, midCandidates, highCandidates;
    float32 h = static_cast<float32>(level.height);
    for (const auto& pos : groundCandidates) {
        float32 normY = pos.y / h;
        if (normY > 0.65f) lowCandidates.push_back(pos);
        else if (normY >= 0.35f) midCandidates.push_back(pos);
        else highCandidates.push_back(pos);
    }
    std::shuffle(lowCandidates.begin(), lowCandidates.end(), rng);
    std::shuffle(midCandidates.begin(), midCandidates.end(), rng);
    std::shuffle(highCandidates.begin(), highCandidates.end(), rng);

    std::vector<Vec2i> orderedCandidates;
    size_t maxSize = std::max({lowCandidates.size(), midCandidates.size(), highCandidates.size()});
    for (size_t i = 0; i < maxSize; ++i) {
        if (i < lowCandidates.size()) orderedCandidates.push_back(lowCandidates[i]);
        if (i < midCandidates.size()) orderedCandidates.push_back(midCandidates[i]);
        if (i < highCandidates.size()) orderedCandidates.push_back(highCandidates[i]);
    }

    std::shuffle(airCandidates.begin(), airCandidates.end(), rng);

    int32 placed = 0;
    
    // First place ground enemies (Patrol, Chase, Ranged)
    for (const auto& pos : orderedCandidates) {
        if (placed >= profile.enemyCount) break;
        if (nearAny(pos, level.enemyPositions, 3)) continue;  // separation

        EnemyType type = EnemyType::Patrol;
        float32 roll = chance(rng);
        if (roll < 0.35f) {
            type = EnemyType::Patrol;
        } else if (profile.hasChasingEnemies && roll < 0.70f) {
            type = EnemyType::Chase;
        } else {
            type = EnemyType::Ranged;
        }

        float32 speed = profile.enemyBaseSpeed *
            std::uniform_real_distribution<float32>(0.8f, 1.2f)(rng);

        level.enemyPositions.push_back(pos);
        level.enemyTypes.push_back(type);
        level.enemySpeeds.push_back(speed);
        ++placed;
    }

    // Next place flying enemies if requested and quota not filled
    if (profile.hasFlyingEnemies && placed < profile.enemyCount) {
        for (const auto& pos : airCandidates) {
            if (placed >= profile.enemyCount) break;
            if (nearAny(pos, level.enemyPositions, 3)) continue;

            float32 speed = profile.enemyBaseSpeed *
                std::uniform_real_distribution<float32>(0.8f, 1.2f)(rng);

            level.enemyPositions.push_back(pos);
            level.enemyTypes.push_back(EnemyType::Flying);
            level.enemySpeeds.push_back(speed);
            ++placed;
        }
    }
}

// ──────────────────────────────────────────────────────────────
// Pass 3 — Hazards
// Scale density and cluster size with difficulty.
// Easy: only off-path, isolated single spikes.
// Hard+: may appear on path edges, in clusters.
// ──────────────────────────────────────────────────────────────

void GameplayDecorator::placeHazards(LevelData& level,
                                      const DifficultyProfile& profile,
                                      const std::vector<Vec2i>& path,
                                      std::mt19937& rng)
{
    (void)level; (void)profile; (void)path; (void)rng;
    return; // Hazard placement disabled

    auto pathSet = buildPathSet(path);
    std::uniform_real_distribution<float32> chance(0.0f, 100.0f);

    for (int32 y = 1; y < level.height - 1; ++y) {
        for (int32 x = 2; x < level.width - 2; ++x) {
            if (!isStandable(level, x, y)) continue;
            if (isProtected(level.tiles[y][x])) continue;
            if (level.tiles[y][x] != TileType::Empty) continue;

            bool onPath = pathSet.count({x, y}) > 0;
            if (onPath) continue; // Never place hazards directly on critical path standing tiles

            // Spawn / exit safe-zone (always skip within 3 tiles)
            if (manhattanDist(Vec2i(x,y), level.spawnPosition) < 4) continue;
            if (manhattanDist(Vec2i(x,y), level.exitPosition)  < 3) continue;

            if (chance(rng) < profile.hazardDensity) {
                level.tiles[y][x] = TileType::Hazard;
                level.hazardPositions.push_back(Vec2i(x, y));

                // Cluster: extend rightward
                int32 clusterLen = std::uniform_int_distribution<int32>
                    (1, profile.maxHazardClusterSize)(rng);
                for (int32 cx = 1; cx < clusterLen && x + cx < level.width - 2; ++cx) {
                    Vec2i cp(x + cx, y);
                    if (!isStandable(level, cp.x, cp.y)) break;
                    if (isProtected(level.tiles[cp.y][cp.x])) break;
                    if (level.tiles[cp.y][cp.x] != TileType::Empty) break;
                    level.tiles[cp.y][cp.x] = TileType::Hazard;
                    level.hazardPositions.push_back(cp);
                }
            }
        }
    }
}

// ──────────────────────────────────────────────────────────────
// Pass 4 — Powerups
// Placed after dense hazard clusters (reward for hard sections)
// or at off-path optional branches.
// ──────────────────────────────────────────────────────────────

void GameplayDecorator::placePowerups(LevelData& level,
                                       const DifficultyProfile& profile,
                                       const std::vector<Vec2i>& path,
                                       std::mt19937& rng)
{
    if (profile.powerupsPerLevel <= 0.0f) return;

    auto pathSet = buildPathSet(path);
    int32 targetCount = std::max(1,
        static_cast<int32>(std::round(profile.powerupsPerLevel)));

    // Find standable tiles that are either:
    // (a) Off-path (optional branch), or
    // (b) After a hazard cluster (immediately right of last cluster tile)
    std::vector<Vec2i> candidates;

    for (int32 y = 1; y < level.height - 1; ++y) {
        for (int32 x = 2; x < level.width - 2; ++x) {
            if (!isStandable(level, x, y)) continue;
            if (level.tiles[y][x] != TileType::Empty) continue;
            if (isProtected(level.tiles[y][x])) continue;

            bool onPath = pathSet.count({x, y}) > 0;

            bool afterHazard = false;
            if (x > 0 && level.tiles[y][x-1] == TileType::Hazard) afterHazard = true;

            bool offPath = !onPath;

            if (afterHazard || (offPath && profile.powerupsAfterHardSection)) {
                candidates.push_back(Vec2i(x, y));
            }
        }
    }

    if (candidates.empty()) {
        // Fallback: any empty standable tile not on path
        for (int32 y = 1; y < level.height - 1; ++y)
            for (int32 x = 2; x < level.width - 2; ++x)
                if (isStandable(level, x, y) &&
                    level.tiles[y][x] == TileType::Empty &&
                    !pathSet.count({x, y}))
                    candidates.push_back(Vec2i(x, y));
    }

    std::shuffle(candidates.begin(), candidates.end(), rng);

    PowerupType types[] = {
        PowerupType::DoubleJump, PowerupType::Shield,
        PowerupType::Speed,      PowerupType::Health
    };
    std::uniform_int_distribution<int32> typeRoll(0, 3);

    int32 placed = 0;
    for (const auto& pos : candidates) {
        if (placed >= targetCount) break;
        if (nearAny(pos, level.powerupPositions, 3)) continue;

        level.tiles[pos.y][pos.x] = TileType::Powerup;
        level.powerupPositions.push_back(pos);
        level.powerupTypes.push_back(types[typeRoll(rng)]);
        ++placed;
    }
}

// ──────────────────────────────────────────────────────────────
// Pass 5 — Coins
// Easy/Normal: scattered on path landing zones.
// Expert/Nightmare (strategicCoinsOnly): only in dead-end areas.
// ──────────────────────────────────────────────────────────────

void GameplayDecorator::placeCoins(LevelData& level,
                                    const DifficultyProfile& profile,
                                    const std::vector<Vec2i>& path,
                                    std::mt19937& rng)
{
    (void)level; (void)profile; (void)path; (void)rng;
    return; // Yellow placeholder coin objects disabled completely
}

// ──────────────────────────────────────────────────────────────
// Pass 6 — Platform variant annotation
// Converts solid floor tiles (tiles at y+1 under standable positions)
// to variant types based on difficulty.  The tile type changes in the
// grid; Main.gd reads these to spawn entity behaviour nodes.
// ──────────────────────────────────────────────────────────────

void GameplayDecorator::annotatePlatformVariants(LevelData& level,
                                                  const DifficultyProfile& profile,
                                                  const std::vector<Vec2i>& path,
                                                  std::mt19937& rng)
{
    if (profile.movingPlatformRate  <= 0.0f &&
        profile.fallingPlatformRate <= 0.0f &&
        profile.bouncePadRate       <= 0.0f &&
        profile.icePlatformRate     <= 0.0f) return;

    auto pathSet = buildPathSet(path);
    std::uniform_real_distribution<float32> roll(0.0f, 1.0f);

    for (int32 y = 1; y < level.height - 1; ++y) {
        for (int32 x = 1; x < level.width - 1; ++x) {
            // We look at FLOOR tiles: solid tiles with a standable position above
            if (!isSolid(level, x, y)) continue;
            if (level.tiles[y][x] != TileType::Solid) continue;  // already annotated

            Vec2i above(x, y - 1);
            if (!isStandable(level, above.x, above.y)) continue; // not a floor tile

            // Never annotate tiles directly under Spawn or Exit
            TileType aboveTile = level.tiles[above.y][above.x];
            if (aboveTile == TileType::Spawn || aboveTile == TileType::Exit) continue;

            // Don't convert if standing position is on critical path
            // (keeps the main route predictable; variants appear on side platforms)
            if (pathSet.count({above.x, above.y}) && roll(rng) > 0.3f) continue;

            float32 r = roll(rng);
            float32 cumul = 0.0f;

            cumul += profile.icePlatformRate;
            if (r < cumul) {
                level.tiles[y][x] = TileType::IcePlatform;
                level.icePlatformPositions.push_back(Vec2i(x, y));
                continue;
            }
            cumul += profile.bouncePadRate;
            if (r < cumul) {
                level.tiles[y][x] = TileType::BouncePad;
                level.bouncePadPositions.push_back(Vec2i(x, y));
                continue;
            }
            cumul += profile.fallingPlatformRate;
            if (r < cumul) {
                level.tiles[y][x] = TileType::FallingPlatform;
                level.fallingPlatformPositions.push_back(Vec2i(x, y));
                continue;
            }
            cumul += profile.movingPlatformRate;
            if (r < cumul) {
                level.tiles[y][x] = TileType::MovingPlatform;
                level.movingPlatformPositions.push_back(Vec2i(x, y));
            }
        }
    }
}

} // namespace APLG
