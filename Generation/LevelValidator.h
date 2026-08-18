#pragma once

#include "../Include/Common.h"
#include <vector>
#include <functional>
#include <memory>

namespace APLG {

// Forward declaration
class ILevelGenerator;

/**
 * @brief Validation result for a level
 */
struct ValidationResult {
    bool valid;
    std::string reason;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    
    void invalidate(const std::string& reason) {
        valid = false;
        this->reason = reason;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

/**
 * @brief Level validator
 * 
 * Automatically validates generated levels to ensure playability.
 * Checks for spawn, exit, reachable paths, and fair placement.
 */
class LevelValidator {
public:
    /**
     * @brief Validate a level
     * @param level Level data to validate
     * @return Validation result
     */
    ValidationResult validate(const LevelData& level);
    
    /**
     * @brief Set custom validation rules
     */
    void setCustomValidator(std::function<ValidationResult(const LevelData&)> validator) {
        m_customValidator = validator;
    }
    
    /**
     * @brief Enable/disable specific checks
     */
    void setCheckSpawn(bool enabled) { m_checkSpawn = enabled; }
    void setCheckExit(bool enabled) { m_checkExit = enabled; }
    void setCheckPath(bool enabled) { m_checkPath = enabled; }
    void setCheckGaps(bool enabled) { m_checkGaps = enabled; }
    void setCheckEnemies(bool enabled) { m_checkEnemies = enabled; }
    void setCheckCollectables(bool enabled) { m_checkCollectables = enabled; }
    
private:
    bool checkSpawnExists(const LevelData& level, ValidationResult& result);
    bool checkExitExists(const LevelData& level, ValidationResult& result);
    bool checkReachablePath(const LevelData& level, ValidationResult& result);
    bool checkJumpableGaps(const LevelData& level, ValidationResult& result);
    bool checkEnemyFairness(const LevelData& level, ValidationResult& result);
    bool checkCollectableReachability(const LevelData& level, ValidationResult& result);
    
    bool isWalkable(TileType tile);
    bool isSolid(TileType tile);
    bool isPlatform(TileType tile);
    
    std::vector<Vec2i> findPath(const LevelData& level, Vec2i start, Vec2i end);
    bool canJumpBetween(const LevelData& level, Vec2i from, Vec2i to, float32 maxJumpDist);
    
    bool m_checkSpawn = true;
    bool m_checkExit = true;
    bool m_checkPath = true;
    bool m_checkGaps = true;
    bool m_checkEnemies = true;
    bool m_checkCollectables = true;
    
    std::function<ValidationResult(const LevelData&)> m_customValidator;
};

/**
 * @brief Automatic level generator with validation
 * 
 * Generates levels and automatically rejects invalid ones,
 * regenerating until a valid level is produced.
 */
class ValidatedLevelGenerator {
public:
    ValidatedLevelGenerator(std::unique_ptr<ILevelGenerator> generator);
    
    /**
     * @brief Generate a valid level
     * @param config Generation configuration
     * @param maxAttempts Maximum generation attempts
     * @return Valid level data, or empty if failed
     */
    LevelData generateValid(const GenerationConfig& config, int32 maxAttempts = 100);
    
    /**
     * @brief Get the last validation result
     */
    const ValidationResult& getLastValidation() const {
        return m_lastValidation;
    }
    
    /**
     * @brief Get the number of attempts for last generation
     */
    int32 getLastAttempts() const {
        return m_lastAttempts;
    }
    
    /**
     * @brief Set custom validator
     */
    void setValidator(LevelValidator* validator) {
        m_validator = validator;
    }
    
private:
    std::unique_ptr<ILevelGenerator> m_generator;
    LevelValidator* m_validator;
    ValidationResult m_lastValidation;
    int32 m_lastAttempts;
};

/**
 * @brief Pathfinding for validation
 * 
 * Uses BFS to find paths between points in the level.
 */
class PathFinder {
public:
    /**
     * @brief Find path from start to end
     * @param level Level data
     * @param start Start position
     * @param end End position
     * @return Path as vector of positions, empty if no path
     */
    static std::vector<Vec2i> findPath(const LevelData& level, Vec2i start, Vec2i end);
    
    /**
     * @brief Check if position is reachable from start
     * @param level Level data
     * @param start Start position
     * @param target Target position
     * @return true if reachable
     */
    static bool isReachable(const LevelData& level, Vec2i start, Vec2i target);
    
    /**
     * @brief Get all reachable positions from start
     * @param level Level data
     * @param start Start position
     * @return Set of reachable positions
     */
    static std::vector<Vec2i> getReachableArea(const LevelData& level, Vec2i start);
    
private:
    static bool isValidPosition(const LevelData& level, Vec2i pos);
    static bool canStandOn(const LevelData& level, Vec2i pos);
};

} // namespace APLG
