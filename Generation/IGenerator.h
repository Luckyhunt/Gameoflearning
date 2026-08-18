#pragma once

#include "../Include/Common.h"

namespace APLG {

/**
 * @brief Base interface for procedural level generators
 * 
 * All generation algorithms inherit from this interface.
 * Follows the Strategy pattern for interchangeable generation methods.
 */
class ILevelGenerator {
public:
    virtual ~ILevelGenerator() = default;
    
    /**
     * @brief Generate a level
     * @param config Generation configuration
     * @return Generated level data
     */
    virtual LevelData generate(const GenerationConfig& config) = 0;
    
    /**
     * @brief Get the generator name
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Set random seed for reproducible generation
     */
    virtual void setSeed(uint32 seed) = 0;
};

/**
 * @brief Cellular Automata generator
 * 
 * Uses cellular automata rules (like Conway's Game of Life)
 * to generate cave-like structures.
 */
class CellularAutomataGenerator : public ILevelGenerator {
public:
    CellularAutomataGenerator();
    
    LevelData generate(const GenerationConfig& config) override;
    std::string getName() const override { return "Cellular Automata"; }
    void setSeed(uint32 seed) override { Random::instance().seed(seed); }
    
    void setIterations(int32 iterations) { m_iterations = iterations; }
    void setFillProbability(float32 probability) { m_fillProbability = probability; }
    
private:
    void applyRules(std::vector<std::vector<TileType>>& tiles, int32 width, int32 height);
    
    int32 m_iterations;
    float32 m_fillProbability;
};

/**
 * @brief Random Walk generator
 * 
 * Uses random walk algorithm to create connected paths.
 * Good for maze-like levels.
 */
class RandomWalkGenerator : public ILevelGenerator {
public:
    RandomWalkGenerator();
    
    LevelData generate(const GenerationConfig& config) override;
    std::string getName() const override { return "Random Walk"; }
    void setSeed(uint32 seed) override { Random::instance().seed(seed); }
    
    void setSteps(int32 steps) { m_steps = steps; }
    void setWalkers(int32 walkers) { m_walkers = walkers; }
    
private:
    void performWalk(std::vector<std::vector<TileType>>& tiles, 
                     int32 width, int32 height, Vec2i start);
    
    int32 m_steps;
    int32 m_walkers;
};

/**
 * @brief Perlin Noise generator
 * 
 * Uses Perlin noise to generate organic, natural-looking terrain.
 * Good for outdoor or cave environments.
 */
class PerlinNoiseGenerator : public ILevelGenerator {
public:
    PerlinNoiseGenerator();
    
    LevelData generate(const GenerationConfig& config) override;
    std::string getName() const override { return "Perlin Noise"; }
    void setSeed(uint32 seed) override { Random::instance().seed(seed); }
    
    void setScale(float32 scale) { m_scale = scale; }
    void setThreshold(float32 threshold) { m_threshold = threshold; }
    
private:
    float32 noise(float32 x, float32 y);
    float32 smoothNoise(float32 x, float32 y);
    float32 interpolatedNoise(float32 x, float32 y);
    float32 linearInterpolate(float32 a, float32 b, float32 x);
    
    float32 m_scale;
    float32 m_threshold;
    uint32 m_seed;
};

/**
 * @brief Constraint-based generator
 * 
 * Generates levels based on placement constraints and rules.
 * Ensures specific gameplay requirements are met.
 */
class ConstraintGenerator : public ILevelGenerator {
public:
    ConstraintGenerator();
    
    LevelData generate(const GenerationConfig& config) override;
    std::string getName() const override { return "Constraint-Based"; }
    void setSeed(uint32 seed) override { Random::instance().seed(seed); }
    
private:
    void placePlatforms(std::vector<std::vector<TileType>>& tiles,
                       int32 width, int32 height, const GenerationConfig& config);
    void placeEnemies(std::vector<std::vector<TileType>>& tiles,
                      int32 width, int32 height, const GenerationConfig& config,
                      LevelData& level);
    void placeCollectables(std::vector<std::vector<TileType>>& tiles,
                          int32 width, int32 height, const GenerationConfig& config,
                          LevelData& level);
    
    bool isValidPlacement(const std::vector<std::vector<TileType>>& tiles,
                         int32 x, int32 y, int32 width, int32 height);
};

/**
 * @brief Grammar-based generator
 * 
 * Uses L-systems or grammars to generate level structures.
 * Good for creating consistent architectural patterns.
 */
class GrammarGenerator : public ILevelGenerator {
public:
    GrammarGenerator();
    
    LevelData generate(const GenerationConfig& config) override;
    std::string getName() const override { return "Grammar-Based"; }
    void setSeed(uint32 seed) override { Random::instance().seed(seed); }
    
    void setIterations(int32 iterations) { m_iterations = iterations; }
    
private:
    std::string applyRules(const std::string& input);
    void interpretString(const std::string& s, LevelData& level, 
                        int32 width, int32 height);
    
    int32 m_iterations;
};

/**
 * @brief Room Graph generator
 * 
 * Creates a graph of connected rooms.
 * Good for dungeon-like levels with distinct areas.
 */
class RoomGraphGenerator : public ILevelGenerator {
public:
    RoomGraphGenerator();
    
    LevelData generate(const GenerationConfig& config) override;
    std::string getName() const override { return "Room Graph"; }
    void setSeed(uint32 seed) override { Random::instance().seed(seed); }
    
    void setMinRooms(int32 minRooms) { m_minRooms = minRooms; }
    void setMaxRooms(int32 maxRooms) { m_maxRooms = maxRooms; }
    void setMinRoomSize(int32 size) { m_minRoomSize = size; }
    void setMaxRoomSize(int32 size) { m_maxRoomSize = size; }
    
private:
    struct Room {
        Vec2i position;
        Vec2i size;
        std::vector<Vec2i> connections;
    };
    
    std::vector<Room> generateRooms(int32 width, int32 height);
    void connectRooms(std::vector<std::vector<TileType>>& tiles,
                     const std::vector<Room>& rooms);
    void carveRoom(std::vector<std::vector<TileType>>& tiles,
                  const Room& room);
    void carveCorridor(std::vector<std::vector<TileType>>& tiles,
                      Vec2i start, Vec2i end);
    
    int32 m_minRooms;
    int32 m_maxRooms;
    int32 m_minRoomSize;
    int32 m_maxRoomSize;
};

/**
 * @brief Generator factory
 * 
 * Creates instances of different generators.
 */
class GeneratorFactory {
public:
    static std::unique_ptr<ILevelGenerator> createGenerator(const std::string& type);
    static std::vector<std::string> getAvailableGenerators();
};

} // namespace APLG
