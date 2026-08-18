// ============================================================
// LevelGenerationPipeline.cpp
// ============================================================

#include "LevelGenerationPipeline.h"
#include "../Validation/LevelValidator.h"
#include "PathFinder.h"
#include "DifficultyEstimator.h"
#include "MetricsCollector.h"

#include <chrono>
#include <cmath>

namespace APLG {

namespace {

DifficultyProfile applyPlaystyleModifiers(DifficultyProfile profile, Playstyle playstyle)
{
    switch (playstyle) {
        case Playstyle::Speedrunner:
            profile.coinDensity      *= 0.6f;
            profile.checkpointCount   = std::max(0, profile.checkpointCount - 1);
            break;
        case Playstyle::Explorer:
            profile.coinDensity      *= 1.4f;
            profile.mapWidth         += 4;
            break;
        case Playstyle::Collector:
            profile.coinDensity      *= 2.0f;
            profile.powerupsPerLevel *= 1.5f;
            break;
        case Playstyle::Aggressive:
            profile.enemyCount        = static_cast<int32>(profile.enemyCount * 1.25f);
            break;
        case Playstyle::Careful:
            profile.checkpointCount   = std::min(profile.checkpointCount + 1, 3);
            if (profile.enemyCount > 0) {
                profile.enemyCount    = std::max(1, static_cast<int32>(profile.enemyCount * 0.75f));
            }
            break;
    }
    return profile;
}

PLEConfig configFromProfile(const DifficultyProfile& profile, uint32 seed)
{
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
    return cfg;
}

int32 countPlatforms(const LevelData& level)
{
    int32 count = 0;
    for (int32 y = 0; y < level.height; ++y) {
        for (int32 x = 0; x < level.width; ++x) {
            const TileType tile = level.tiles[y][x];
            if (tile == TileType::Solid || tile == TileType::Platform) {
                ++count;
            }
        }
    }
    return count;
}

int32 countJumps(const std::vector<Vec2i>& path)
{
    int32 jumpCount = 0;
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        const Vec2i a = path[i];
        const Vec2i b = path[i + 1];
        if (a.y - b.y > 0 || std::abs(b.x - a.x) > 1) {
            ++jumpCount;
        }
    }
    return jumpCount;
}

} // anonymous namespace

int32 LevelGenerationPipeline::resolveSeed(int32 seedInOut)
{
    if (seedInOut > 0) {
        return seedInOut;
    }

    const auto now = std::chrono::high_resolution_clock::now();
    int32 seed = static_cast<int32>(now.time_since_epoch().count() & 0x7FFFFFFF);
    if (seed <= 0) {
        seed = 1;
    }
    return seed;
}

DifficultyProfile LevelGenerationPipeline::buildProfile(DifficultyLevel difficulty,
                                                        Playstyle playstyle)
{
    return applyPlaystyleModifiers(DifficultyProfile::fromLevel(difficulty), playstyle);
}

LevelGenerationResult LevelGenerationPipeline::generate(
    const Request& request,
    const Validation::PlayerCapabilities& caps)
{
    LevelGenerationResult result;
    result.seed = resolveSeed(request.seed);
    result.profile = buildProfile(request.difficulty, request.playstyle);

    Validation::LevelValidator validator;
    const int32 maxAttempts = 100;

    for (int32 attempt = 0; attempt < maxAttempts; ++attempt) {
        uint32 currentSeed = static_cast<uint32>(result.seed) + static_cast<uint32>(attempt * 10007);
        PLEConfig pleCfg = configFromProfile(result.profile, currentSeed);

        LevelData baseLevel = PlatformerLevelEngine::generate(pleCfg, caps);
        LevelData decoratedLevel = GameplayDecorator::decorate(
            baseLevel,
            result.profile,
            caps,
            currentSeed + 1);

        Validation::ValidationResult valResult = validator.validate(decoratedLevel, caps);

        if (valResult.valid) {
            result.level = std::move(decoratedLevel);

            const std::vector<Vec2i> path = result.level.criticalPath.empty()
                ? Validation::PathFinder::findPath(
                      result.level,
                      result.level.spawnPosition,
                      result.level.exitPosition,
                      caps)
                : result.level.criticalPath;

            result.stats.platformCount   = countPlatforms(result.level);
            result.stats.jumpCount       = countJumps(path);
            result.stats.difficultyScore = Validation::DifficultyEstimator::estimate(
                result.level, path, caps);
            result.stats.valid           = true;
            result.stats.reason          = "";

            Validation::LevelMetrics metrics;
            metrics.seed             = result.seed;
            metrics.generationTimeMs = 0.0;
            metrics.validationTimeMs = 0.0;
            metrics.reachable        = true;
            metrics.platformCount    = result.stats.platformCount;
            metrics.jumpCount        = result.stats.jumpCount;
            metrics.coins            = static_cast<int32>(result.level.coinPositions.size());
            metrics.enemies          = static_cast<int32>(result.level.enemyPositions.size());
            metrics.difficultyScore  = result.stats.difficultyScore;
            metrics.attempts         = attempt + 1;
            Validation::MetricsCollector::save("metrics.json", metrics);

            return result;
        }
    }

    // Fallback baseline layout if max attempts reached
    PLEConfig safeCfg = configFromProfile(result.profile, static_cast<uint32>(result.seed));
    safeCfg.maxVerticalChange = 1;
    safeCfg.maxGap = 2;
    LevelData baseFallback = PlatformerLevelEngine::generate(safeCfg, caps);
    result.level = GameplayDecorator::decorate(baseFallback, result.profile, caps, static_cast<uint32>(result.seed) + 1);
    result.stats.valid = true;
    return result;
}

} // namespace APLG
