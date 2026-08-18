#pragma once

#include "../Include/Common.h"
#include "PlayerCapabilities.h"
#include <vector>

namespace APLG::Validation {

class DifficultyEstimator {
public:
    /**
     * @brief Estimate a normalized difficulty score (0.0 to 1.0) for a level.
     * @param level Level data
     * @param solutionPath Solved player path from spawn to exit
     * @param caps Player capabilities model
     * @return Difficulty score between 0.0 (easiest) and 1.0 (hardest)
     */
    static float32 estimate(const LevelData& level, const std::vector<Vec2i>& solutionPath, const PlayerCapabilities& caps);
};

} // namespace APLG::Validation
