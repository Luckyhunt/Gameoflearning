#include "DifficultyEstimator.h"
#include <cmath>
#include <algorithm>

namespace APLG::Validation {

float32 DifficultyEstimator::estimate(const LevelData& level, const std::vector<Vec2i>& solutionPath, const PlayerCapabilities& caps) {
    if (solutionPath.empty()) {
        return 0.0f;
    }

    // 1. Count jump transitions along the path
    int32 jumpCount = 0;
    for (size_t i = 0; i < solutionPath.size() - 1; ++i) {
        Vec2i pos = solutionPath[i];
        Vec2i next = solutionPath[i + 1];
        
        int32 dx = std::abs(next.x - pos.x);
        int32 dy = pos.y - next.y; // Positive means climbing higher

        // If climbing higher or crossing a gap of width > 1, count as a jump
        if (dy > 0 || dx > 1) {
            jumpCount++;
        }
    }

    // 2. Count platforms/solids to compute terrain density
    int32 platformCount = 0;
    for (int32 y = 0; y < level.height; ++y) {
        for (int32 x = 0; x < level.width; ++x) {
            TileType t = level.tiles[y][x];
            if (t == TileType::Solid || t == TileType::Platform) {
                platformCount++;
            }
        }
    }

    float32 totalCells = static_cast<float32>(level.width * level.height);
    float32 platformDensity = totalCells > 0.0f ? static_cast<float32>(platformCount) / totalCells : 0.0f;

    // 3. Normalized Difficulty Calculation
    // We weigh:
    //   - Jump transitions count (40% weight, max normalized at 15 jumps)
    //   - Platform sparsity (40% weight, lower density = sparser = harder)
    //   - Path length (20% weight, max normalized at 40 tiles)
    float32 normalizedJumps = std::min(1.0f, static_cast<float32>(jumpCount) / 15.0f);
    float32 normalizedSparsity = std::max(0.0f, 1.0f - (platformDensity / 0.4f)); // 0.4 density is very dense (easy)
    float32 normalizedPathLength = std::min(1.0f, static_cast<float32>(solutionPath.size()) / 40.0f);

    float32 score = (normalizedJumps * 0.4f) + 
                    (normalizedSparsity * 0.4f) + 
                    (normalizedPathLength * 0.2f);

    // Apply base adjustments based on generator base difficulty settings if defined
    float32 baseLevelAdj = 0.0f;
    switch (level.difficulty) {
        case DifficultyLevel::Easy:      baseLevelAdj = -0.1f; break;
        case DifficultyLevel::Normal:    baseLevelAdj = 0.0f; break;
        case DifficultyLevel::Hard:      baseLevelAdj = 0.1f; break;
        case DifficultyLevel::Expert:    baseLevelAdj = 0.2f; break;
        case DifficultyLevel::Nightmare: baseLevelAdj = 0.3f; break;
    }
    
    score += baseLevelAdj;

    return std::max(0.0f, std::min(1.0f, score));
}

} // namespace APLG::Validation
