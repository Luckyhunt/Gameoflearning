// ============================================================
// PygamePlatformGenerator.h
// ============================================================
// C++ port and DDA adaptation of the Pygame-Platformer generator
// (https://github.com/Imas7264/Pygame-Platformer - branch: sami)
//
// Translates:
//   - container_generator.py (populate_container, store_platform, valid_platform)
//   - level_generator.py (can_reach, jump_reach, generate_level_full)
// Integrates with:
//   - Dynamic Difficulty Adjustment (DDA) scaling
//   - Unidirectional progression & guaranteed Ancient Door reachability
// ============================================================

#pragma once

#include "PlatformerLevelEngine.h"
#include <vector>
#include <cmath>
#include <algorithm>

namespace APLG {

struct GeneratorPlatform {
    int32 x = 0;
    int32 y = 0;
    int32 length = 0;
    int32 connected = 0; // Number of outgoing branches (max 2)
    int32 bound_x_min = 0;
    int32 bound_x_max = 0;
    int32 bound_y_min = 0;
    int32 bound_y_max = 0;
    int32 left_node = -1;
    int32 right_node = -1;
    bool  isStart = false;
    bool  isEnd = false;
    bool  isAlternate = false;
};

class PygamePlatformGenerator {
public:
    /**
     * @brief Port of valid_platform() from container_generator.py
     * Verifies spatial clearance and non-overlap against existing platforms.
     */
    static bool validPlatform(int32 x, int32 y, int32 length,
                              const std::vector<GeneratorPlatform>& platforms,
                              int32 roomWidth, int32 roomHeight);

    /**
     * @brief Port of jump_reach() from level_generator.py
     * Euler integration simulation of maximum reachable horizontal distance for delta Y.
     */
    static float32 jumpReach(int32 dy, float32 tileSize = 32.0f,
                             float32 jumpStrength = -12.0f,
                             float32 gravity = 0.8f,
                             float32 speed = 5.0f);

    /**
     * @brief Port of can_reach() from level_generator.py
     * Verifies kinematic jump reachability between two platforms.
     */
    static bool canReach(const GeneratorPlatform& p1, const GeneratorPlatform& p2,
                         float32 tileSize = 32.0f,
                         float32 jumpStrength = -12.0f,
                         float32 gravity = 0.8f,
                         float32 speed = 5.0f);

    /**
     * @brief Generates platform path using the Pygame container-graph algorithm
     * enhanced with DDA parameter scaling and guaranteed exit reachability.
     */
    static std::vector<PlatformNode> generatePlatforms(const PLEConfig& config);
};

} // namespace APLG
