#pragma once

#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <map>
#include <optional>
#include <variant>
#include <array>
#include <chrono>
#include <random>
#include <iostream>
#include <algorithm>
#include <cmath>

namespace APLG {

// Common type definitions
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using float32 = float;
using float64 = double;

// Vector types for 2D
struct Vec2 {
    float32 x;
    float32 y;
    
    Vec2() : x(0.0f), y(0.0f) {}
    Vec2(float32 x, float32 y) : x(x), y(y) {}
    
    Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(float32 scalar) const { return Vec2(x * scalar, y * scalar); }
    Vec2 operator/(float32 scalar) const { return Vec2(x / scalar, y / scalar); }
    
    Vec2& operator+=(const Vec2& other) { x += other.x; y += other.y; return *this; }
    Vec2& operator-=(const Vec2& other) { x -= other.x; y -= other.y; return *this; }
    Vec2& operator*=(float32 scalar) { x *= scalar; y *= scalar; return *this; }
    Vec2& operator/=(float32 scalar) { x /= scalar; y /= scalar; return *this; }
    
    float32 length() const { return std::sqrt(x * x + y * y); }
    float32 lengthSquared() const { return x * x + y * y; }
    Vec2 normalized() const { float32 len = length(); return len > 0 ? *this / len : Vec2(); }
    float32 dot(const Vec2& other) const { return x * other.x + y * other.y; }
};

struct Vec2i {
    int32 x;
    int32 y;
    
    Vec2i() : x(0), y(0) {}
    Vec2i(int32 x, int32 y) : x(x), y(y) {}
    
    Vec2i operator+(const Vec2i& other) const { return Vec2i(x + other.x, y + other.y); }
    Vec2i operator-(const Vec2i& other) const { return Vec2i(x - other.x, y - other.y); }
    Vec2i operator/(int32 scalar) const { return Vec2i(x / scalar, y / scalar); }
    bool operator==(const Vec2i& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vec2i& other) const { return !(*this == other); }
};

// Rect for collision and bounds
struct Rect {
    Vec2 position;
    Vec2 size;
    
    // Convenience accessors
    float32 x() const { return position.x; }
    float32 y() const { return position.y; }
    float32 width() const { return size.x; }
    float32 height() const { return size.y; }
    
    Rect() : position(0, 0), size(0, 0) {}
    Rect(Vec2 pos, Vec2 sz) : position(pos), size(sz) {}
    Rect(float32 x, float32 y, float32 w, float32 h) : position(x, y), size(w, h) {}
    
    bool contains(const Vec2& point) const {
        return point.x >= position.x && point.x <= position.x + size.x &&
               point.y >= position.y && point.y <= position.y + size.y;
    }
    
    bool intersects(const Rect& other) const {
        return position.x < other.position.x + other.size.x &&
               position.x + size.x > other.position.x &&
               position.y < other.position.y + other.size.y &&
               position.y + size.y > other.position.y;
    }
};

// Enum for difficulty levels
enum class DifficultyLevel {
    Easy,
    Normal,
    Hard,
    Expert,
    Nightmare
};

// Enum for playstyle detection
enum class Playstyle {
    Explorer,
    Speedrunner,
    Aggressive,
    Careful,
    Collector
};

// Enum for tile types
enum class TileType {
    Empty,
    Solid,
    Platform,
    Hazard,
    Spawn,
    Exit,
    Checkpoint,
    Coin,
    Powerup,
    Secret,
    // --- Platform behaviour variants (rendered visually; physics handled by entity nodes) ---
    MovingPlatform,    // 10 — horizontal oscillation
    FallingPlatform,   // 11 — collapses 1s after player contact
    BouncePad,         // 12 — launches player 1.5× jump height
    IcePlatform,       // 13 — low friction
    OneWayPlatform,    // 14 — passable from below, solid from above
};

// Enum for enemy types
enum class EnemyType {
    Patrol,
    Chase,
    Flying,
    Boss,
    Ranged
};

// Platform behaviour variants — used by the Gameplay Decorator to annotate
// floor tiles. Does not affect pathfinding; only determines which Godot entity
// node to spawn at runtime.
enum class PlatformVariant {
    Static,       // default — plain solid tile, no entity node needed
    Moving,       // horizontal oscillation, speed defined per-tile
    Falling,      // collapses after player contact
    BouncePad,    // launches player upward
    Ice,          // low-friction surface
    OneWay,       // passable from below
};

// Enum for powerup types
enum class PowerupType {
    Health,
    DoubleJump,
    Dash,
    Shield,
    Speed
};

// Level data structure
struct LevelData {
    std::vector<std::vector<TileType>> tiles;
    Vec2i spawnPosition;
    Vec2i exitPosition;
    std::vector<Vec2i> enemyPositions;
    std::vector<EnemyType> enemyTypes;
    std::vector<Vec2i> coinPositions;
    std::vector<Vec2i> powerupPositions;
    std::vector<PowerupType> powerupTypes;
    std::vector<Vec2i> checkpointPositions;
    std::vector<Vec2i> hazardPositions;

    // Platform-variant entity positions (for Godot entity spawning)
    std::vector<Vec2i> movingPlatformPositions;   // tile becomes TileType::MovingPlatform
    std::vector<Vec2i> fallingPlatformPositions;  // tile becomes TileType::FallingPlatform
    std::vector<Vec2i> bouncePadPositions;        // tile becomes TileType::BouncePad
    std::vector<Vec2i> icePlatformPositions;      // tile becomes TileType::IcePlatform

    // Per-tile enemy speeds (parallel to enemyPositions)
    std::vector<float32> enemySpeeds;

    // Analytics: accumulated death counts per tile (same dims as tiles)
    std::vector<std::vector<int32>> deathHeatmap;

    // Guaranteed critical path from PlatformerLevelEngine (stand positions, spawn→exit)
    std::vector<Vec2i> criticalPath;

    int32 width;
    int32 height;
    DifficultyLevel difficulty;
};

// Player metrics for analytics
struct PlayerMetrics {
    int32 deaths;
    int32 damageTaken;
    float32 completionTime;
    float32 jumpAccuracy;
    float32 enemyHitRate;
    int32 platformMisses;
    float32 idleTime;
    float32 reactionTime;
    int32 livesRemaining;
    int32 checkpointUsage;
    int32 coinsCollected;
    int32 enemiesDefeated;
    int32 highestCombo;
    float32 attackAccuracy;
    int32 totalAttacks;
    int32 attacksLanded;
    int32 totalJumps;
    int32 jumpsLanded;
    float32 explorationPercentage;
    float32 skillConsistency;
    
    PlayerMetrics() : deaths(0), damageTaken(0), completionTime(0.0f), jumpAccuracy(0.0f),
                      enemyHitRate(0.0f), platformMisses(0), idleTime(0.0f), reactionTime(0.0f),
                      livesRemaining(3), checkpointUsage(0), coinsCollected(0), enemiesDefeated(0),
                      highestCombo(0), attackAccuracy(0.0f), totalAttacks(0), attacksLanded(0),
                      totalJumps(0), jumpsLanded(0), explorationPercentage(0.0f), skillConsistency(0.0f) {}
};

// Player skill score
struct SkillScore {
    float32 overall;
    float32 combat;
    float32 platforming;
    float32 exploration;
    float32 speed;
    
    SkillScore() : overall(0.0f), combat(0.0f), platforming(0.0f), exploration(0.0f), speed(0.0f) {}
};

// Consolidated Performance Dashboard Data
struct PerformanceDashboard {
    int32 levelsCompleted = 0;
    int32 enemiesEliminated = 0;
    float32 attackAccuracy = 0.0f;
    float32 jumpSuccessRate = 0.0f;
    int32 damageTaken = 0;
    float32 timePerLevel = 0.0f;
    float32 skillRating = 0.5f;
    float32 adaptiveDifficultyScore = 0.5f;
    DifficultyLevel currentDifficulty = DifficultyLevel::Normal;
    int32 highestCombo = 0;
    float32 overallProgress = 0.0f;
};

// Configuration for procedural generation
struct GenerationConfig {
    int32 minWidth;
    int32 maxWidth;
    int32 minHeight;
    int32 maxHeight;
    float32 platformDensity;
    float32 enemyDensity;
    float32 coinDensity;
    float32 hazardDensity;
    float32 powerupSpawnRate;
    bool useMovingPlatforms;
    int32 maxGapSize;
    int32 minPlatformWidth;
    
    GenerationConfig() : minWidth(50), maxWidth(100), minHeight(20), maxHeight(40),
                        platformDensity(0.3f), enemyDensity(0.0f), coinDensity(0.1f),
                        hazardDensity(0.0f), powerupSpawnRate(0.01f), useMovingPlatforms(false),
                        maxGapSize(5), minPlatformWidth(3) {}
};

// Callback types
using GenerationCallback = std::function<void(const LevelData&)>;
using AnalyticsCallback = std::function<void(const PlayerMetrics&)>;
using DifficultyCallback  = std::function<void(DifficultyLevel)>;

// Random number generator wrapper
class Random {
public:
    static Random& instance() {
        static Random inst;
        return inst;
    }
    
    void seed(uint32 seed) { generator.seed(seed); }
    
    int32 range(int32 min, int32 max) {
        std::uniform_int_distribution<int32> dist(min, max);
        return dist(generator);
    }
    
    float32 range(float32 min, float32 max) {
        std::uniform_real_distribution<float32> dist(min, max);
        return dist(generator);
    }
    
    bool chance(float32 probability) {
        return range(0.0f, 1.0f) < probability;
    }
    
    Vec2i randomPosition(int32 width, int32 height) {
        return Vec2i(range(0, width), range(0, height));
    }
    
private:
    Random() : generator(std::chrono::system_clock::now().time_since_epoch().count()) {}
    std::mt19937 generator;
};

} // namespace APLG
