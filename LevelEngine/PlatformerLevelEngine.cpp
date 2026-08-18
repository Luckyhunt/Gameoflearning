// ============================================================
// PlatformerLevelEngine.cpp
//
// Always-valid 2-D platformer level generation by construction.
//
// Coordinate convention (matches Godot + existing codebase):
//   • tiles[y][x]  —  row y, column x
//   • Y increases DOWNWARD (row 0 = top of screen)
//   • Player stands AT tile (x, y): tiles[y][x] must be non-solid
//   • Floor is the tile BELOW the player: tiles[y+1][x] must be solid
//   • Headroom above player: tiles[y-1][x], tiles[y-2][x] must be empty
//
// Tile priority (single-write rule):
//   BORDER(5) > SPAWN/EXIT(4) > FLOOR(3) > PATH-AIR(2) > DECORATION(1) > FILL(0)
//   A cell can only be overwritten if the new priority > existing priority.
// ============================================================

#include "PlatformerLevelEngine.h"
#include "GameplayDecorator.h"
#include "ILogger.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace APLG {

// ──────────────────────────────────────────────────────────────
// Priority grid helpers
// Each cell also carries a write-priority so higher-importance
// tiles can never be stomped on by decoration passes.
// ──────────────────────────────────────────────────────────────

namespace {

// Write-priority constants (higher = more protected)
constexpr int32 PRI_DEFAULT     = 0;
constexpr int32 PRI_DECORATION  = 1;
constexpr int32 PRI_AIR         = 2;   // explicitly cleared air cells
constexpr int32 PRI_FLOOR       = 3;   // platform surface (solid)
constexpr int32 PRI_SPAWN_EXIT  = 4;
constexpr int32 PRI_BORDER      = 5;

// Per-cell write priority tracker (grid-parallel to LevelData::tiles)
using PriorityGrid = std::vector<std::vector<int32>>;

bool inBounds(const LevelData& level, int32 x, int32 y) {
    return x >= 0 && x < level.width && y >= 0 && y < level.height;
}

void safePut(LevelData& level, PriorityGrid& pri,
             int32 x, int32 y, TileType type, int32 priority)
{
    if (!inBounds(level, x, y)) return;
    if (priority > pri[y][x]) {
        level.tiles[y][x] = type;
        pri[y][x] = priority;
    }
}

} // anonymous namespace

// ──────────────────────────────────────────────────────────────
// Public: configForDifficulty
// ──────────────────────────────────────────────────────────────

PLEConfig PlatformerLevelEngine::configForDifficulty(DifficultyLevel level, uint32 seed)
{
    const DifficultyProfile profile = DifficultyProfile::fromLevel(level);

    PLEConfig cfg;
    cfg.width             = profile.mapWidth;
    cfg.height            = profile.mapHeight;
    cfg.minPlatformWidth  = profile.minPlatformWidth;
    cfg.maxPlatformWidth  = profile.maxPlatformWidth;
    cfg.minGap            = 1;
    cfg.maxGap            = profile.maxGap;
    cfg.maxVerticalChange = profile.maxVerticalChange;
    cfg.coinDensity       = 0.0f;
    cfg.hazardDensity     = 0.0f;
    cfg.seed              = seed;

    // Configure route variety and verticality based on difficulty
    switch (level) {
        case DifficultyLevel::Easy:
            cfg.branchProbability      = 0.1f;
            cfg.maxBranchDepth         = 1;
            cfg.verticalityFactor      = 0.4f;
            cfg.minVerticalProgression = 4;
            cfg.progressiveDifficulty  = true;
            cfg.enableHighRoutes       = false;
            cfg.enableLowRoutes        = false;
            break;
        case DifficultyLevel::Normal:
            cfg.branchProbability      = 0.3f;
            cfg.maxBranchDepth         = 2;
            cfg.verticalityFactor      = 0.6f;
            cfg.minVerticalProgression = 6;
            cfg.progressiveDifficulty  = true;
            cfg.enableHighRoutes       = true;
            cfg.enableLowRoutes        = true;
            break;
        case DifficultyLevel::Hard:
            cfg.branchProbability      = 0.5f;
            cfg.maxBranchDepth         = 3;
            cfg.verticalityFactor      = 0.8f;
            cfg.minVerticalProgression = 8;
            cfg.progressiveDifficulty  = true;
            cfg.enableHighRoutes       = true;
            cfg.enableLowRoutes        = true;
            break;
        case DifficultyLevel::Expert:
            cfg.branchProbability      = 0.7f;
            cfg.maxBranchDepth         = 4;
            cfg.verticalityFactor      = 0.9f;
            cfg.minVerticalProgression = 10;
            cfg.progressiveDifficulty  = true;
            cfg.enableHighRoutes       = true;
            cfg.enableLowRoutes        = true;
            break;
        case DifficultyLevel::Nightmare:
            cfg.branchProbability      = 0.9f;
            cfg.maxBranchDepth         = 5;
            cfg.verticalityFactor      = 1.0f;
            cfg.minVerticalProgression = 12;
            cfg.progressiveDifficulty  = true;
            cfg.enableHighRoutes       = true;
            cfg.enableLowRoutes        = true;
            break;
    }

    return cfg;
}

// ──────────────────────────────────────────────────────────────
// Public: generate
// ──────────────────────────────────────────────────────────────

LevelData PlatformerLevelEngine::generate(
    const PLEConfig& cfg,
    const Validation::PlayerCapabilities& caps)
{
    LOG_INFO("PlatformerLevelEngine: generating " +
             std::to_string(cfg.width) + "x" + std::to_string(cfg.height) + " vertical level");

    // ── Clamp config within PlayerCapabilities (75% budget) ────────
    PLEConfig safe = cfg;
    int32 jumpBudget   = static_cast<int32>(caps.maxJumpDistance * 0.75f);
    int32 heightBudget = static_cast<int32>(caps.maxJumpHeight   * 0.75f);
    safe.maxGap             = std::min(safe.maxGap,             jumpBudget);
    safe.maxVerticalChange  = std::min(safe.maxVerticalChange,  heightBudget);
    safe.maxGap             = std::max(safe.maxGap, 1);
    safe.maxVerticalChange  = std::max(safe.maxVerticalChange, 1);

    // Minimum level dimensions for a viable vertical layout
    safe.width  = std::max(safe.width,  16);
    safe.height = std::max(safe.height, 10);

    // ── Initialise level structure ──────────────────────────────────
    LevelData level;
    level.width      = safe.width;
    level.height     = safe.height;
    level.difficulty = DifficultyLevel::Normal;

    // Fill interior with Empty, borders with Solid
    level.tiles.assign(safe.height,
                       std::vector<TileType>(safe.width, TileType::Empty));

    PriorityGrid pri(safe.height, std::vector<int32>(safe.width, PRI_DEFAULT));

    // Fill borders with Solid
    for (int32 x = 0; x < safe.width; ++x) {
        level.tiles[0][x] = TileType::Solid;
        level.tiles[safe.height - 1][x] = TileType::Solid;
    }
    for (int32 y = 0; y < safe.height; ++y) {
        level.tiles[y][0] = TileType::Solid;
        level.tiles[y][safe.width - 1] = TileType::Solid;
    }

    // ── RNG ────────────────────────────────────────────────────────
    std::mt19937 rng(safe.seed ? safe.seed : 42u);
    auto randInt = [&](int32 lo, int32 hi) -> int32 {
        if (lo >= hi) return lo;
        return std::uniform_int_distribution<int32>(lo, hi)(rng);
    };
    auto randFloat = [&](float32 lo, float32 hi) -> float32 {
        return std::uniform_real_distribution<float32>(lo, hi)(rng);
    };

    // ──────────────────────────────────────────────────────────────
    // Phase 1 — Critical path construction (Deliberate Macro-Vertical Layout)
    //
    // Divide map length into 4 structural sections:
    //   1. Entry Base (0% to 20%): Near bottom, wide platforms
    //   2. Vertical Ascent (20% to 50%): Stepped climb up to upper tier
    //   3. High Ridge Section (50% to 80%): Elevated traversal, multi-tier ledges
    //   4. Stepped Descent & Finale (80% to 100%): Controlled drops & exit climb
    // ──────────────────────────────────────────────────────────────

    std::vector<PlatformNode> path;

    // Entry start: near bottom
    int32 startY = safe.height - 4;
    startY = std::clamp(startY, 3, safe.height - 3);

    int32 firstWidth = std::clamp(safe.maxPlatformWidth, safe.minPlatformWidth, safe.width - 4);

    PlatformNode first;
    first.x           = 1;
    first.y           = startY;
    first.width       = firstWidth;
    first.isStart     = true;
    first.isEnd       = false;
    first.isAlternate = false;
    path.push_back(first);

    int32 cursorX = first.x + first.width;
    int32 cursorY = first.y;

    int32 minY = startY;
    int32 maxY = startY;

    const float32 totalSpan = static_cast<float32>(safe.width - 4);

    while (cursorX < safe.width - 3) {
        float32 progress = std::clamp(static_cast<float32>(cursorX) / totalSpan, 0.0f, 1.0f);

        // Calculate progressive difficulty scaling for platform width & gap size
        int32 curMinW = safe.minPlatformWidth;
        int32 curMaxW = safe.maxPlatformWidth;
        if (safe.progressiveDifficulty) {
            float32 wFactor = 1.0f - progress * 0.5f;
            curMaxW = std::max(curMinW, static_cast<int32>(std::round(safe.maxPlatformWidth * wFactor)));
        }
        int32 platW = randInt(curMinW, std::max(curMinW, curMaxW));

        int32 curMaxGap = safe.maxGap;
        if (safe.progressiveDifficulty) {
            float32 gFactor = 0.5f + progress * 0.5f;
            curMaxGap = std::clamp(static_cast<int32>(std::round(safe.maxGap * gFactor)), safe.minGap, safe.maxGap);
        }
        int32 gap = randInt(safe.minGap, curMaxGap);

        // Section-based vertical delta determination
        int32 targetDeltaY = 0;
        if (progress < 0.20f) {
            // Section 1: Gentle entry wobble
            targetDeltaY = randInt(-1, 1);
        } else if (progress < 0.50f) {
            // Section 2: Strong Vertical Ascent Climb
            targetDeltaY = randInt(-safe.maxVerticalChange, -1);
        } else if (progress < 0.80f) {
            // Section 3: High Ridge Traversal (keep Y near upper range, e.g. row 2 to 5)
            targetDeltaY = randInt(-1, 1);
            if (cursorY > 5) targetDeltaY = -randInt(1, safe.maxVerticalChange);
        } else {
            // Section 4: Stepped Descent & Finale
            if (cursorY < safe.height - 5) {
                targetDeltaY = randInt(1, safe.maxVerticalChange);
            } else {
                targetDeltaY = randInt(-1, 1);
            }
        }

        // Clamp vertical changes to max 2 tiles to eliminate deep narrow pits/shafts
        safe.maxVerticalChange = std::min(safe.maxVerticalChange, 2);
        platW = std::max(platW, 4);

        int32 deltaY = std::clamp(targetDeltaY, -safe.maxVerticalChange, safe.maxVerticalChange);
        int32 nextX  = cursorX + gap;
        int32 nextY  = std::clamp(cursorY + deltaY, 2, safe.height - 3);

        // Check horizontal boundary
        if (nextX + safe.minPlatformWidth > safe.width - 2) {
            nextX = std::max(cursorX + 1, safe.width - 2 - safe.minPlatformWidth);
            platW = safe.width - 2 - nextX;
        }

        platW = std::clamp(platW, 1, safe.width - 2 - nextX);
        if (platW < 1) break;

        PlatformNode node;
        node.x           = nextX;
        node.y           = nextY;
        node.width       = platW;
        node.isStart     = false;
        node.isEnd       = (nextX + platW >= safe.width - 3);
        node.isAlternate = false;
        path.push_back(node);

        cursorX = nextX + platW;
        cursorY = nextY;

        minY = std::min(minY, nextY);
        maxY = std::max(maxY, nextY);

        if (node.isEnd) break;
    }

    // Guarantee minimum vertical progression amplitude
    int32 currentAmplitude = maxY - minY;
    if (currentAmplitude < safe.minVerticalProgression && path.size() >= 3) {
        // Boost elevation changes in the middle section of path to guarantee vertical range
        size_t midStart = path.size() / 4;
        size_t midEnd   = (path.size() * 3) / 4;
        int32 targetHighY = std::max(2, safe.height - 4 - safe.minVerticalProgression);

        for (size_t i = midStart; i <= midEnd; ++i) {
            path[i].y = std::clamp(targetHighY + randInt(-1, 1), 2, safe.height - 3);
        }
    }

    if (!path.empty()) {
        path.back().isEnd = true;
    }

    // ──────────────────────────────────────────────────────────────
    // Phase 1.5 — Multi-Level Alternate Routes (High Shortcuts & Low Routes)
    // ──────────────────────────────────────────────────────────────

    std::vector<PlatformNode> alternateNodes;
    if ((safe.enableHighRoutes || safe.enableLowRoutes) && path.size() > 4) {
        for (size_t i = 1; i + 2 < path.size(); ++i) {
            if (path[i].isAlternate || path[i].isStart || path[i].isEnd) continue;

            if (randFloat(0.0f, 1.0f) < safe.branchProbability) {
                bool highBranch = safe.enableHighRoutes && randFloat(0.0f, 1.0f) < 0.6f;
                int32 yOffset   = highBranch ? -3 : 3;
                int32 altY      = std::clamp(path[i].y + yOffset, 2, safe.height - 3);

                if (std::abs(altY - path[i].y) >= 2) {
                    PlatformNode altNode;
                    altNode.x           = path[i].x + 1;
                    altNode.y           = altY;
                    altNode.width       = std::clamp(path[i].width - 1, safe.minPlatformWidth, safe.maxPlatformWidth);
                    altNode.isStart     = false;
                    altNode.isEnd       = false;
                    altNode.isAlternate = true;
                    alternateNodes.push_back(altNode);

                    i += 1; // Step forward to avoid overlapping branch triggers
                }
            }
        }
    }

    // Merge alternate route nodes into path before the finale node
    if (path.size() > 1) {
        path.insert(path.end() - 1, alternateNodes.begin(), alternateNodes.end());
    } else {
        for (const auto& altNode : alternateNodes) {
            path.push_back(altNode);
        }
    }

    // ──────────────────────────────────────────────────────────────
    // Phase 2 — Solid Ground Floor & Clean Ledge Platform Stamping
    // Eliminates narrow vertical chimney shafts completely.
    // ──────────────────────────────────────────────────────────────

    // Continuous Ground Floor across room corridor
    for (int32 px = 1; px < safe.width - 1; ++px) {
        safePut(level, pri, px, safe.height - 2, TileType::Solid, PRI_FLOOR);
    }

    // Stamp Classroom Desk Ledges & Neat Support Legs for each node
    for (const auto& node : path) {
        for (int32 px = node.x; px < node.x + node.width && px < safe.width - 1; ++px) {
            // Desk Ledge surface
            safePut(level, pri, px, node.y, TileType::Platform, PRI_FLOOR);
            // Open air clearance above desk
            for (int32 headrow : {node.y - 1, node.y - 2, node.y - 3}) {
                if (headrow >= 1) {
                    safePut(level, pri, px, headrow, TileType::Empty, PRI_AIR);
                }
            }
        }

        // Support Legs extend 2 tiles down under desk surface (preserving open corridor walking space)
        int32 legX1 = node.x;
        int32 legX2 = std::min(safe.width - 2, node.x + node.width - 1);
        int32 maxLegY = std::min(node.y + 2, safe.height - 5);
        for (int32 py = node.y + 1; py <= maxLegY; ++py) {
            safePut(level, pri, legX1, py, TileType::Solid, PRI_FLOOR);
            if (legX2 > legX1) {
                safePut(level, pri, legX2, py, TileType::Solid, PRI_FLOOR);
            }
        }
    }

    // ──────────────────────────────────────────────────────────────
    // Phase 5 — Border enforcement (highest priority)
    // ──────────────────────────────────────────────────────────────

    for (int32 x = 0; x < safe.width; ++x) {
        safePut(level, pri, x, 0,               TileType::Solid, PRI_BORDER);
        safePut(level, pri, x, safe.height - 1, TileType::Solid, PRI_BORDER);
    }
    for (int32 y = 0; y < safe.height; ++y) {
        safePut(level, pri, 0,              y, TileType::Solid, PRI_BORDER);
        safePut(level, pri, safe.width - 1, y, TileType::Solid, PRI_BORDER);
    }

    // ──────────────────────────────────────────────────────────────
    // Phase 6 — Decoration pass (Coins on path & alternate ledges)
    // ──────────────────────────────────────────────────────────────

    for (size_t i = 1; i + 1 < path.size(); ++i) {
        const PlatformNode& node = path[i];
        for (int32 px = node.x; px < node.x + node.width; ++px) {
            if (randFloat(0.0f, 1.0f) < safe.coinDensity || node.isAlternate) {
                if (inBounds(level, px, node.y) && pri[node.y][px] <= PRI_AIR) {
                    safePut(level, pri, px, node.y, TileType::Coin, PRI_DECORATION);
                    level.coinPositions.push_back(Vec2i(px, node.y));
                }
            }
        }
    }

    // ──────────────────────────────────────────────────────────────
    // Phase 7 — Spawn / Exit stamp
    // ──────────────────────────────────────────────────────────────

    constexpr int32 PRI_TOP = PRI_BORDER + 1;

    // Spawn
    const PlatformNode& spawnNode = path.front();
    int32 spawnX = spawnNode.x + spawnNode.width / 2;
    int32 spawnY = std::clamp(spawnNode.y, 2, safe.height - 3);

    safePut(level, pri, spawnX, spawnY + 1, TileType::Solid,  PRI_TOP);
    safePut(level, pri, spawnX, spawnY,     TileType::Spawn,  PRI_TOP);
    safePut(level, pri, spawnX, spawnY - 1, TileType::Empty,  PRI_TOP);
    safePut(level, pri, spawnX, spawnY - 2, TileType::Empty,  PRI_TOP);

    level.spawnPosition = Vec2i(spawnX, spawnY);

    // Exit
    const PlatformNode& exitNode = path.back();
    int32 exitX = exitNode.x + exitNode.width / 2;
    int32 exitY = std::clamp(exitNode.y, 2, safe.height - 3);

    safePut(level, pri, exitX, exitY + 1, TileType::Solid, PRI_TOP);
    safePut(level, pri, exitX, exitY,     TileType::Exit,  PRI_TOP);
    safePut(level, pri, exitX, exitY - 1, TileType::Empty, PRI_TOP);
    safePut(level, pri, exitX, exitY - 2, TileType::Empty, PRI_TOP);

    level.exitPosition = Vec2i(exitX, exitY);

    // Export construction path
    level.criticalPath.clear();
    level.criticalPath.reserve(path.size());
    for (const PlatformNode& node : path) {
        if (!node.isAlternate) {
            const int32 standX = node.x + node.width / 2;
            level.criticalPath.push_back(Vec2i(standX, node.y));
        }
    }

    LOG_INFO("PlatformerLevelEngine: vertical generation complete. Platforms=" +
             std::to_string(path.size()) +
             " Coins=" + std::to_string(level.coinPositions.size()));

    return level;
}

// ──────────────────────────────────────────────────────────────
// PlatformerLevelEngine::inBounds (static public helper)
// ──────────────────────────────────────────────────────────────

bool PlatformerLevelEngine::inBounds(const LevelData& level, int32 x, int32 y)
{
    return x >= 0 && x < level.width && y >= 0 && y < level.height;
}

} // namespace APLG
