#include "IGenerator.h"
#include "../Utilities/ILogger.h"
#include <algorithm>
#include <cmath>

namespace APLG {

// Cellular Automata Generator Implementation
CellularAutomataGenerator::CellularAutomataGenerator()
    : m_iterations(5)
    , m_fillProbability(0.45f)
{
}

LevelData CellularAutomataGenerator::generate(const GenerationConfig& config) {
    LOG_INFO("Generating level with Cellular Automata");
    
    LevelData level;
    level.width = config.minWidth + Random::instance().range(0, config.maxWidth - config.minWidth);
    level.height = config.minHeight + Random::instance().range(0, config.maxHeight - config.minHeight);
    level.difficulty = DifficultyLevel::Normal;
    
    // Initialize with random fill
    level.tiles.resize(level.height);
    for (int32 y = 0; y < level.height; ++y) {
        level.tiles[y].resize(level.width);
        for (int32 x = 0; x < level.width; ++x) {
            if (x == 0 || x == level.width - 1 || y == 0 || y == level.height - 1) {
                level.tiles[y][x] = TileType::Solid; // Borders
            } else {
                level.tiles[y][x] = Random::instance().chance(m_fillProbability) ? 
                                   TileType::Solid : TileType::Empty;
            }
        }
    }
    
    // Apply cellular automata rules
    for (int32 i = 0; i < m_iterations; ++i) {
        applyRules(level.tiles, level.width, level.height);
    }
    
    // Place spawn and exit
    level.spawnPosition = Vec2i(2, level.height / 2);
    level.exitPosition = Vec2i(level.width - 3, level.height / 2);
    level.tiles[level.spawnPosition.y][level.spawnPosition.x] = TileType::Spawn;
    level.tiles[level.exitPosition.y][level.exitPosition.x] = TileType::Exit;
    
    // Ensure spawn and exit are empty
    level.tiles[level.spawnPosition.y][level.spawnPosition.x] = TileType::Spawn;
    level.tiles[level.exitPosition.y][level.exitPosition.x] = TileType::Exit;
    
    LOG_INFO("Cellular Automata generation complete");
    return level;
}

void CellularAutomataGenerator::applyRules(std::vector<std::vector<TileType>>& tiles, 
                                           int32 width, int32 height) {
    std::vector<std::vector<TileType>> newTiles = tiles;
    
    for (int32 y = 1; y < height - 1; ++y) {
        for (int32 x = 1; x < width - 1; ++x) {
            int32 neighbors = 0;
            
            // Count solid neighbors
            for (int32 dy = -1; dy <= 1; ++dy) {
                for (int32 dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    if (tiles[y + dy][x + dx] == TileType::Solid) {
                        neighbors++;
                    }
                }
            }
            
            // Apply rule: if neighbors > 4, become solid, else become empty
            if (neighbors > 4) {
                newTiles[y][x] = TileType::Solid;
            } else if (neighbors < 4) {
                newTiles[y][x] = TileType::Empty;
            }
        }
    }
    
    tiles = newTiles;
}

// Random Walk Generator Implementation
RandomWalkGenerator::RandomWalkGenerator()
    : m_steps(1000)
    , m_walkers(3)
{
}

LevelData RandomWalkGenerator::generate(const GenerationConfig& config) {
    LOG_INFO("Generating level with Random Walk");
    
    LevelData level;
    level.width = config.minWidth + Random::instance().range(0, config.maxWidth - config.minWidth);
    level.height = config.minHeight + Random::instance().range(0, config.maxHeight - config.minHeight);
    level.difficulty = DifficultyLevel::Normal;
    
    // Initialize with solid
    level.tiles.resize(level.height);
    for (int32 y = 0; y < level.height; ++y) {
        level.tiles[y].resize(level.width, TileType::Solid);
    }
    
    // Perform random walks from center
    Vec2i center(level.width / 2, level.height / 2);
    for (int32 i = 0; i < m_walkers; ++i) {
        performWalk(level.tiles, level.width, level.height, center);
    }
    
    // Place spawn and exit
    level.spawnPosition = center;
    level.tiles[center.y][center.x] = TileType::Spawn;
    
    // Find furthest point for exit
    Vec2i exitPos = center;
    float32 maxDist = 0.0f;
    for (int32 y = 1; y < level.height - 1; ++y) {
        for (int32 x = 1; x < level.width - 1; ++x) {
            if (level.tiles[y][x] == TileType::Empty) {
                float32 dist = std::sqrt(static_cast<float32>((x - center.x) * (x - center.x) + 
                                                             (y - center.y) * (y - center.y)));
                if (dist > maxDist) {
                    maxDist = dist;
                    exitPos = Vec2i(x, y);
                }
            }
        }
    }
    level.exitPosition = exitPos;
    level.tiles[exitPos.y][exitPos.x] = TileType::Exit;
    
    LOG_INFO("Random Walk generation complete");
    return level;
}

void RandomWalkGenerator::performWalk(std::vector<std::vector<TileType>>& tiles,
                                     int32 width, int32 height, Vec2i start) {
    Vec2i pos = start;
    
    for (int32 i = 0; i < m_steps; ++i) {
        // Stay within bounds
        pos.x = std::max(1, std::min(width - 2, pos.x));
        pos.y = std::max(1, std::min(height - 2, pos.y));
        
        // Carve path
        tiles[pos.y][pos.x] = TileType::Empty;
        
        // Random direction
        int32 dir = Random::instance().range(0, 4);
        switch (dir) {
            case 0: pos.x++; break;
            case 1: pos.x--; break;
            case 2: pos.y++; break;
            case 3: pos.y--; break;
        }
    }
}

// Perlin Noise Generator Implementation
PerlinNoiseGenerator::PerlinNoiseGenerator()
    : m_scale(0.1f)
    , m_threshold(0.3f)
    , m_seed(0)
{
}

LevelData PerlinNoiseGenerator::generate(const GenerationConfig& config) {
    LOG_INFO("Generating level with Perlin Noise");
    
    LevelData level;
    level.width = config.minWidth + Random::instance().range(0, config.maxWidth - config.minWidth);
    level.height = config.minHeight + Random::instance().range(0, config.maxHeight - config.minHeight);
    level.difficulty = DifficultyLevel::Normal;
    
    level.tiles.resize(level.height);
    for (int32 y = 0; y < level.height; ++y) {
        level.tiles[y].resize(level.width);
        for (int32 x = 0; x < level.width; ++x) {
            if (x == 0 || x == level.width - 1 || y == 0 || y == level.height - 1) {
                level.tiles[y][x] = TileType::Solid;
            } else {
                float32 n = noise(x * m_scale, y * m_scale);
                level.tiles[y][x] = n > m_threshold ? TileType::Solid : TileType::Empty;
            }
        }
    }
    
    // Place spawn and exit
    level.spawnPosition = Vec2i(level.width / 4, level.height / 2);
    level.exitPosition = Vec2i(level.width * 3 / 4, level.height / 2);
    level.tiles[level.spawnPosition.y][level.spawnPosition.x] = TileType::Spawn;
    level.tiles[level.exitPosition.y][level.exitPosition.x] = TileType::Exit;
    
    LOG_INFO("Perlin Noise generation complete");
    return level;
}

float32 PerlinNoiseGenerator::noise(float32 x, float32 y) {
    int32 xi = static_cast<int32>(x) & 255;
    int32 yi = static_cast<int32>(y) & 255;
    float32 xf = x - static_cast<int32>(x);
    float32 yf = y - static_cast<int32>(y);
    
    float32 n00 = smoothNoise(xi, yi);
    float32 n01 = smoothNoise(xi, yi + 1);
    float32 n10 = smoothNoise(xi + 1, yi);
    float32 n11 = smoothNoise(xi + 1, yi + 1);
    
    float32 u = xf * xf * (3.0f - 2.0f * xf);
    float32 v = yf * yf * (3.0f - 2.0f * yf);
    
    return linearInterpolate(linearInterpolate(n00, n10, u),
                           linearInterpolate(n01, n11, u), v);
}

float32 PerlinNoiseGenerator::smoothNoise(float32 x, float32 y) {
    // Simple pseudo-random noise
    uint32 n = static_cast<uint32>(x + y * 57 + m_seed);
    n = (n << 13) ^ n;
    n = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
    return static_cast<float32>(n) / 1073741824.0f;
}

float32 PerlinNoiseGenerator::interpolatedNoise(float32 x, float32 y) {
    return 0.0f; // Simplified
}

float32 PerlinNoiseGenerator::linearInterpolate(float32 a, float32 b, float32 x) {
    return a + x * (b - a);
}

// Constraint Generator Implementation
ConstraintGenerator::ConstraintGenerator()
{
}

LevelData ConstraintGenerator::generate(const GenerationConfig& config) {
    LOG_INFO("Generating level with Constraint-Based approach");
    
    LevelData level;
    level.width = config.minWidth + Random::instance().range(0, config.maxWidth - config.minWidth);
    level.height = config.minHeight + Random::instance().range(0, config.maxHeight - config.minHeight);
    level.difficulty = DifficultyLevel::Normal;
    
    // Initialize with empty
    level.tiles.resize(level.height);
    for (int32 y = 0; y < level.height; ++y) {
        level.tiles[y].resize(level.width, TileType::Empty);
    }
    
    // Place borders
    for (int32 x = 0; x < level.width; ++x) {
        level.tiles[0][x] = TileType::Solid;
        level.tiles[level.height - 1][x] = TileType::Solid;
    }
    for (int32 y = 0; y < level.height; ++y) {
        level.tiles[y][0] = TileType::Solid;
        level.tiles[y][level.width - 1] = TileType::Solid;
    }
    
    // Place platforms based on constraints
    placePlatforms(level.tiles, level.width, level.height, config);
    
    // Place enemies
    placeEnemies(level.tiles, level.width, level.height, config, level);
    
    // Place collectables
    placeCollectables(level.tiles, level.width, level.height, config, level);
    
    // Place spawn and exit
    level.spawnPosition = Vec2i(2, level.height - 3);
    level.exitPosition = Vec2i(level.width - 3, level.height - 3);
    level.tiles[level.spawnPosition.y][level.spawnPosition.x] = TileType::Spawn;
    level.tiles[level.exitPosition.y][level.exitPosition.x] = TileType::Exit;
    
    LOG_INFO("Constraint-Based generation complete");
    return level;
}

void ConstraintGenerator::placePlatforms(std::vector<std::vector<TileType>>& tiles,
                                        int32 width, int32 height, 
                                        const GenerationConfig& config) {
    int32 numPlatforms = static_cast<int32>(width * config.platformDensity);
    
    for (int32 i = 0; i < numPlatforms; ++i) {
        int32 platWidth = config.minPlatformWidth + Random::instance().range(0, 5);
        int32 platX = Random::instance().range(2, width - platWidth - 2);
        int32 platY = Random::instance().range(2, height - 2);
        
        if (isValidPlacement(tiles, platX, platY, platWidth, 1)) {
            for (int32 x = 0; x < platWidth; ++x) {
                tiles[platY][platX + x] = TileType::Platform;
            }
        }
    }
}

void ConstraintGenerator::placeEnemies(std::vector<std::vector<TileType>>& tiles,
                                       int32 width, int32 height,
                                       const GenerationConfig& config,
                                       LevelData& level) {
    (void)tiles; (void)width; (void)height; (void)config; (void)level;
    return; // Enemies disabled
}

void ConstraintGenerator::placeCollectables(std::vector<std::vector<TileType>>& tiles,
                                           int32 width, int32 height,
                                           const GenerationConfig& config,
                                           LevelData& level) {
    int32 numCoins = static_cast<int32>(width * config.coinDensity);
    
    for (int32 i = 0; i < numCoins; ++i) {
        int32 cx = Random::instance().range(2, width - 2);
        int32 cy = Random::instance().range(2, height - 2);
        
        if (tiles[cy][cx] == TileType::Empty) {
            level.coinPositions.push_back(Vec2i(cx, cy));
        }
    }
}

bool ConstraintGenerator::isValidPlacement(const std::vector<std::vector<TileType>>& tiles,
                                          int32 x, int32 y, int32 width, int32 height) {
    for (int32 dy = 0; dy < height; ++dy) {
        for (int32 dx = 0; dx < width; ++dx) {
            if (y + dy >= tiles.size() || x + dx >= tiles[0].size()) {
                return false;
            }
            if (tiles[y + dy][x + dx] != TileType::Empty) {
                return false;
            }
        }
    }
    return true;
}

// Grammar Generator Implementation
GrammarGenerator::GrammarGenerator()
    : m_iterations(3)
{
}

LevelData GrammarGenerator::generate(const GenerationConfig& config) {
    LOG_INFO("Generating level with Grammar-Based approach");
    
    LevelData level;
    level.width = config.minWidth + Random::instance().range(0, config.maxWidth - config.minWidth);
    level.height = config.minHeight + Random::instance().range(0, config.maxHeight - config.minHeight);
    level.difficulty = DifficultyLevel::Normal;
    
    // Initialize with empty
    level.tiles.resize(level.height);
    for (int32 y = 0; y < level.height; ++y) {
        level.tiles[y].resize(level.width, TileType::Empty);
    }
    
    // Simple grammar: S -> P-S | P | E
    // P = platform, E = end
    std::string axiom = "S";
    for (int32 i = 0; i < m_iterations; ++i) {
        axiom = applyRules(axiom);
    }
    
    // Interpret the string
    interpretString(axiom, level, level.width, level.height);
    
    // Place spawn and exit
    level.spawnPosition = Vec2i(2, level.height / 2);
    level.exitPosition = Vec2i(level.width - 3, level.height / 2);
    level.tiles[level.spawnPosition.y][level.spawnPosition.x] = TileType::Spawn;
    level.tiles[level.exitPosition.y][level.exitPosition.x] = TileType::Exit;
    
    LOG_INFO("Grammar-Based generation complete");
    return level;
}

std::string GrammarGenerator::applyRules(const std::string& input) {
    std::string output;
    
    for (char c : input) {
        if (c == 'S') {
            // S -> P-S or P or E
            int32 choice = Random::instance().range(0, 3);
            if (choice == 0) output += "P-S";
            else if (choice == 1) output += "P";
            else output += "E";
        } else {
            output += c;
        }
    }
    
    return output;
}

void GrammarGenerator::interpretString(const std::string& s, LevelData& level,
                                       int32 width, int32 height) {
    int32 x = 2;
    int32 y = height / 2;
    
    for (char c : s) {
        if (c == 'P') {
            // Place platform
            int32 platWidth = 3 + Random::instance().range(0, 3);
            for (int32 i = 0; i < platWidth && x + i < width - 2; ++i) {
                if (y >= 0 && y < height) {
                    level.tiles[y][x + i] = TileType::Platform;
                }
            }
            x += platWidth + 2;
        } else if (c == 'E') {
            // End
            break;
        }
        
        if (x >= width - 5) {
            x = 2;
            y = std::max(2, y - 2);
        }
    }
}

// Room Graph Generator Implementation
RoomGraphGenerator::RoomGraphGenerator()
    : m_minRooms(5)
    , m_maxRooms(10)
    , m_minRoomSize(5)
    , m_maxRoomSize(10)
{
}

LevelData RoomGraphGenerator::generate(const GenerationConfig& config) {
    LOG_INFO("Generating level with Room Graph");
    
    LevelData level;
    level.width = config.minWidth + Random::instance().range(0, config.maxWidth - config.minWidth);
    level.height = config.minHeight + Random::instance().range(0, config.maxHeight - config.minHeight);
    level.difficulty = DifficultyLevel::Normal;
    
    // Initialize with solid
    level.tiles.resize(level.height);
    for (int32 y = 0; y < level.height; ++y) {
        level.tiles[y].resize(level.width, TileType::Solid);
    }
    
    // Generate rooms
    std::vector<Room> rooms = generateRooms(level.width, level.height);
    
    // Carve rooms
    for (const auto& room : rooms) {
        carveRoom(level.tiles, room);
    }
    
    // Connect rooms
    connectRooms(level.tiles, rooms);
    
    // Place spawn in first room, exit in last room
    if (!rooms.empty()) {
        level.spawnPosition = rooms[0].position + Vec2i(rooms[0].size.x / 2, rooms[0].size.y / 2);
        level.tiles[level.spawnPosition.y][level.spawnPosition.x] = TileType::Spawn;
        
        if (rooms.size() > 1) {
            level.exitPosition = rooms.back().position + Vec2i(rooms.back().size.x / 2, rooms.back().size.y / 2);
            level.tiles[level.exitPosition.y][level.exitPosition.x] = TileType::Exit;
        }
    }
    
    LOG_INFO("Room Graph generation complete");
    return level;
}

std::vector<RoomGraphGenerator::Room> RoomGraphGenerator::generateRooms(int32 width, int32 height) {
    std::vector<Room> rooms;
    int32 numRooms = m_minRooms + Random::instance().range(0, m_maxRooms - m_minRooms);
    
    for (int32 i = 0; i < numRooms * 3; ++i) { // Try multiple times
        if (rooms.size() >= numRooms) break;
        
        Room room;
        room.size.x = m_minRoomSize + Random::instance().range(0, m_maxRoomSize - m_minRoomSize);
        room.size.y = m_minRoomSize + Random::instance().range(0, m_maxRoomSize - m_minRoomSize);
        room.position.x = Random::instance().range(1, width - room.size.x - 1);
        room.position.y = Random::instance().range(1, height - room.size.y - 1);
        
        // Check for overlap
        bool overlaps = false;
        for (const auto& existing : rooms) {
            if (room.position.x < existing.position.x + existing.size.x + 2 &&
                room.position.x + room.size.x + 2 > existing.position.x &&
                room.position.y < existing.position.y + existing.size.y + 2 &&
                room.position.y + room.size.y + 2 > existing.position.y) {
                overlaps = true;
                break;
            }
        }
        
        if (!overlaps) {
            rooms.push_back(room);
        }
    }
    
    return rooms;
}

void RoomGraphGenerator::connectRooms(std::vector<std::vector<TileType>>& tiles,
                                     const std::vector<Room>& rooms) {
    for (size_t i = 0; i < rooms.size() - 1; ++i) {
        Vec2i centerA = rooms[i].position + rooms[i].size / 2;
        Vec2i centerB = rooms[i + 1].position + rooms[i + 1].size / 2;
        
        carveCorridor(tiles, centerA, centerB);
    }
}

void RoomGraphGenerator::carveRoom(std::vector<std::vector<TileType>>& tiles,
                                  const Room& room) {
    for (int32 y = room.position.y; y < room.position.y + room.size.y; ++y) {
        for (int32 x = room.position.x; x < room.position.x + room.size.x; ++x) {
            if (y >= 0 && y < static_cast<int32>(tiles.size()) &&
                x >= 0 && x < static_cast<int32>(tiles[0].size())) {
                tiles[y][x] = TileType::Empty;
            }
        }
    }
}

void RoomGraphGenerator::carveCorridor(std::vector<std::vector<TileType>>& tiles,
                                       Vec2i start, Vec2i end) {
    Vec2i current = start;
    
    // Horizontal then vertical
    while (current.x != end.x) {
        if (current.y >= 0 && current.y < static_cast<int32>(tiles.size()) &&
            current.x >= 0 && current.x < static_cast<int32>(tiles[0].size())) {
            tiles[current.y][current.x] = TileType::Empty;
        }
        current.x += (current.x < end.x) ? 1 : -1;
    }
    
    while (current.y != end.y) {
        if (current.y >= 0 && current.y < static_cast<int32>(tiles.size()) &&
            current.x >= 0 && current.x < static_cast<int32>(tiles[0].size())) {
            tiles[current.y][current.x] = TileType::Empty;
        }
        current.y += (current.y < end.y) ? 1 : -1;
    }
}

// Generator Factory Implementation
std::unique_ptr<ILevelGenerator> GeneratorFactory::createGenerator(const std::string& type) {
    if (type == "CellularAutomata" || type == "Cellular Automata") {
        return std::make_unique<CellularAutomataGenerator>();
    } else if (type == "RandomWalk" || type == "Random Walk") {
        return std::make_unique<RandomWalkGenerator>();
    } else if (type == "PerlinNoise" || type == "Perlin Noise") {
        return std::make_unique<PerlinNoiseGenerator>();
    } else if (type == "Constraint" || type == "Constraint-Based") {
        return std::make_unique<ConstraintGenerator>();
    } else if (type == "Grammar" || type == "Grammar-Based") {
        return std::make_unique<GrammarGenerator>();
    } else if (type == "RoomGraph" || type == "Room Graph") {
        return std::make_unique<RoomGraphGenerator>();
    }
    
    // Default to cellular automata
    return std::make_unique<CellularAutomataGenerator>();
}

std::vector<std::string> GeneratorFactory::getAvailableGenerators() {
    return {
        "Cellular Automata",
        "Random Walk",
        "Perlin Noise",
        "Constraint-Based",
        "Grammar-Based",
        "Room Graph"
    };
}

} // namespace APLG
