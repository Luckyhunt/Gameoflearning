#include "LevelValidator.h"
#include "IGenerator.h"
#include "../Utilities/ILogger.h"
#include <queue>
#include <algorithm>

namespace APLG {

ValidationResult LevelValidator::validate(const LevelData& level) {
    LOG_INFO("Validating level...");
    
    ValidationResult result;
    
    // Run custom validator if set
    if (m_customValidator) {
        ValidationResult customResult = m_customValidator(level);
        if (!customResult.valid) {
            return customResult;
        }
    }
    
    // Check spawn exists
    if (m_checkSpawn && !checkSpawnExists(level, result)) {
        return result;
    }
    
    // Check exit exists
    if (m_checkExit && !checkExitExists(level, result)) {
        return result;
    }
    
    // Check reachable path
    if (m_checkPath && !checkReachablePath(level, result)) {
        return result;
    }
    
    // Check jumpable gaps
    if (m_checkGaps && !checkJumpableGaps(level, result)) {
        return result;
    }
    
    // Check enemy fairness
    if (m_checkEnemies && !checkEnemyFairness(level, result)) {
        return result;
    }
    
    // Check collectable reachability
    if (m_checkCollectables && !checkCollectableReachability(level, result)) {
        return result;
    }
    
    LOG_INFO("Level validation passed");
    return result;
}

bool LevelValidator::checkSpawnExists(const LevelData& level, ValidationResult& result) {
    if (level.spawnPosition.x < 0 || level.spawnPosition.x >= level.width ||
        level.spawnPosition.y < 0 || level.spawnPosition.y >= level.height) {
        result.invalidate("Spawn position out of bounds");
        return false;
    }
    
    if (level.tiles[level.spawnPosition.y][level.spawnPosition.x] == TileType::Solid) {
        result.invalidate("Spawn position is inside solid tile");
        return false;
    }
    
    return true;
}

bool LevelValidator::checkExitExists(const LevelData& level, ValidationResult& result) {
    if (level.exitPosition.x < 0 || level.exitPosition.x >= level.width ||
        level.exitPosition.y < 0 || level.exitPosition.y >= level.height) {
        result.invalidate("Exit position out of bounds");
        return false;
    }
    
    if (level.tiles[level.exitPosition.y][level.exitPosition.x] == TileType::Solid) {
        result.invalidate("Exit position is inside solid tile");
        return false;
    }
    
    return true;
}

bool LevelValidator::checkReachablePath(const LevelData& level, ValidationResult& result) {
    std::vector<Vec2i> path = PathFinder::findPath(level, level.spawnPosition, level.exitPosition);
    
    if (path.empty()) {
        result.invalidate("No reachable path from spawn to exit");
        return false;
    }
    
    if (path.size() < 5) {
        result.addWarning("Very short path from spawn to exit");
    }
    
    return true;
}

bool LevelValidator::checkJumpableGaps(const LevelData& level, ValidationResult& result) {
    const float32 maxJumpDistance = 5.0f; // Maximum jumpable gap in tiles
    
    int32 impossibleGaps = 0;
    
    // Scan horizontal gaps
    for (int32 y = 2; y < level.height - 2; ++y) {
        int32 gapStart = -1;
        
        for (int32 x = 1; x < level.width - 1; ++x) {
            bool solid = isSolid(level.tiles[y][x]) || isPlatform(level.tiles[y][x]);
            
            if (!solid && gapStart == -1) {
                gapStart = x;
            } else if (solid && gapStart != -1) {
                int32 gapSize = x - gapStart;
                if (gapSize > static_cast<int32>(maxJumpDistance)) {
                    impossibleGaps++;
                    // Check if there's a way around (vertical)
                    bool hasVerticalRoute = false;
                    for (int32 checkY = y - 2; checkY <= y + 2; ++checkY) {
                        if (checkY >= 0 && checkY < level.height) {
                            if (isSolid(level.tiles[checkY][gapStart]) || 
                                isSolid(level.tiles[checkY][x - 1])) {
                                hasVerticalRoute = true;
                                break;
                            }
                        }
                    }
                    
                    if (!hasVerticalRoute) {
                        result.invalidate("Impossible gap detected at row " + std::to_string(y));
                        return false;
                    }
                }
                gapStart = -1;
            }
        }
    }
    
    if (impossibleGaps > 0) {
        result.addWarning("Found " + std::to_string(impossibleGaps) + " large gaps");
    }
    
    return true;
}

bool LevelValidator::checkEnemyFairness(const LevelData& level, ValidationResult& result) {
    // Check if enemies are too close to spawn
    const int32 safeZoneRadius = 5;
    
    for (const auto& enemyPos : level.enemyPositions) {
        int32 dist = std::abs(enemyPos.x - level.spawnPosition.x) + 
                    std::abs(enemyPos.y - level.spawnPosition.y);
        
        if (dist < safeZoneRadius) {
            result.addWarning("Enemy too close to spawn");
        }
    }
    
    // Check if enemies are placed on solid tiles
    int32 invalidEnemyPlacements = 0;
    for (const auto& enemyPos : level.enemyPositions) {
        if (enemyPos.y >= level.height - 1 || 
            !isSolid(level.tiles[enemyPos.y + 1][enemyPos.x])) {
            invalidEnemyPlacements++;
        }
    }
    
    if (invalidEnemyPlacements > 0) {
        result.addWarning(std::to_string(invalidEnemyPlacements) + " enemies not on solid ground");
    }
    
    // Check enemy density
    if (level.enemyPositions.size() > level.width / 5) {
        result.addWarning("High enemy density");
    }
    
    return true;
}

bool LevelValidator::checkCollectableReachability(const LevelData& level, ValidationResult& result) {
    int32 unreachable = 0;
    
    for (const auto& coinPos : level.coinPositions) {
        if (!PathFinder::isReachable(level, level.spawnPosition, coinPos)) {
            unreachable++;
        }
    }
    
    if (unreachable > 0) {
        result.addWarning(std::to_string(unreachable) + " collectables unreachable from spawn");
    }
    
    // Check if collectables are in solid tiles
    int32 invalidPlacements = 0;
    for (const auto& coinPos : level.coinPositions) {
        if (coinPos.x < 0 || coinPos.x >= level.width ||
            coinPos.y < 0 || coinPos.y >= level.height ||
            isSolid(level.tiles[coinPos.y][coinPos.x])) {
            invalidPlacements++;
        }
    }
    
    if (invalidPlacements > 0) {
        result.invalidate(std::to_string(invalidPlacements) + " collectables in invalid positions");
        return false;
    }
    
    return true;
}

bool LevelValidator::isWalkable(TileType tile) {
    return tile == TileType::Empty || tile == TileType::Platform ||
           tile == TileType::Spawn || tile == TileType::Exit ||
           tile == TileType::Checkpoint || tile == TileType::Coin ||
           tile == TileType::Powerup;
}

bool LevelValidator::isSolid(TileType tile) {
    return tile == TileType::Solid;
}

bool LevelValidator::isPlatform(TileType tile) {
    return tile == TileType::Platform;
}

std::vector<Vec2i> LevelValidator::findPath(const LevelData& level, Vec2i start, Vec2i end) {
    return PathFinder::findPath(level, start, end);
}

bool LevelValidator::canJumpBetween(const LevelData& level, Vec2i from, Vec2i to, float32 maxJumpDist) {
    float32 dist = std::sqrt(static_cast<float32>(
        (to.x - from.x) * (to.x - from.x) + (to.y - from.y) * (to.y - from.y)
    ));
    return dist <= maxJumpDist;
}

// ValidatedLevelGenerator Implementation
ValidatedLevelGenerator::ValidatedLevelGenerator(std::unique_ptr<ILevelGenerator> generator)
    : m_generator(std::move(generator))
    , m_validator(nullptr)
    , m_lastAttempts(0)
{
    if (!m_validator) {
        m_validator = new LevelValidator();
    }
}

LevelData ValidatedLevelGenerator::generateValid(const GenerationConfig& config, int32 maxAttempts) {
    LOG_INFO("Generating validated level (max attempts: " + std::to_string(maxAttempts) + ")");
    
    for (int32 attempt = 0; attempt < maxAttempts; ++attempt) {
        m_lastAttempts = attempt + 1;
        
        LevelData level = m_generator->generate(config);
        m_lastValidation = m_validator->validate(level);
        
        if (m_lastValidation.valid) {
            LOG_INFO("Valid level generated on attempt " + std::to_string(attempt + 1));
            return level;
        }
        
        LOG_INFO("Attempt " + std::to_string(attempt + 1) + " failed: " + m_lastValidation.reason);
    }
    
    LOG_ERROR("Failed to generate valid level after " + std::to_string(maxAttempts) + " attempts");
    return LevelData();
}

// PathFinder Implementation
std::vector<Vec2i> PathFinder::findPath(const LevelData& level, Vec2i start, Vec2i end) {
    if (!isValidPosition(level, start) || !isValidPosition(level, end)) {
        return {};
    }
    
    std::queue<Vec2i> queue;
    std::vector<std::vector<Vec2i>> parent(level.height, 
                                          std::vector<Vec2i>(level.width, Vec2i(-1, -1)));
    std::vector<std::vector<bool>> visited(level.height, 
                                           std::vector<bool>(level.width, false));
    
    queue.push(start);
    visited[start.y][start.x] = true;
    
    while (!queue.empty()) {
        Vec2i current = queue.front();
        queue.pop();
        
        if (current == end) {
            // Reconstruct path
            std::vector<Vec2i> path;
            Vec2i node = end;
            while (node != Vec2i(-1, -1)) {
                path.push_back(node);
                node = parent[node.y][node.x];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        // Check neighbors (4-directional)
        Vec2i neighbors[] = {
            Vec2i(current.x + 1, current.y),
            Vec2i(current.x - 1, current.y),
            Vec2i(current.x, current.y + 1),
            Vec2i(current.x, current.y - 1)
        };
        
        for (const auto& neighbor : neighbors) {
            if (isValidPosition(level, neighbor) && !visited[neighbor.y][neighbor.x]) {
                if (canStandOn(level, neighbor)) {
                    visited[neighbor.y][neighbor.x] = true;
                    parent[neighbor.y][neighbor.x] = current;
                    queue.push(neighbor);
                }
            }
        }
    }
    
    return {}; // No path found
}

bool PathFinder::isReachable(const LevelData& level, Vec2i start, Vec2i target) {
    std::vector<Vec2i> path = findPath(level, start, target);
    return !path.empty();
}

std::vector<Vec2i> PathFinder::getReachableArea(const LevelData& level, Vec2i start) {
    std::vector<Vec2i> reachable;
    std::queue<Vec2i> queue;
    std::vector<std::vector<bool>> visited(level.height, 
                                           std::vector<bool>(level.width, false));
    
    if (!isValidPosition(level, start)) {
        return reachable;
    }
    
    queue.push(start);
    visited[start.y][start.x] = true;
    
    while (!queue.empty()) {
        Vec2i current = queue.front();
        queue.pop();
        reachable.push_back(current);
        
        Vec2i neighbors[] = {
            Vec2i(current.x + 1, current.y),
            Vec2i(current.x - 1, current.y),
            Vec2i(current.x, current.y + 1),
            Vec2i(current.x, current.y - 1)
        };
        
        for (const auto& neighbor : neighbors) {
            if (isValidPosition(level, neighbor) && !visited[neighbor.y][neighbor.x]) {
                if (canStandOn(level, neighbor)) {
                    visited[neighbor.y][neighbor.x] = true;
                    queue.push(neighbor);
                }
            }
        }
    }
    
    return reachable;
}

bool PathFinder::isValidPosition(const LevelData& level, Vec2i pos) {
    return pos.x >= 0 && pos.x < level.width && 
           pos.y >= 0 && pos.y < level.height;
}

bool PathFinder::canStandOn(const LevelData& level, Vec2i pos) {
    TileType tile = level.tiles[pos.y][pos.x];
    // Can stand on empty, platform, spawn, exit, checkpoint, coin, powerup
    if (tile == TileType::Solid) {
        return false;
    }
    return true;
}

} // namespace APLG
