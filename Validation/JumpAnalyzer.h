#pragma once

#include "../Include/Common.h"
#include "PlayerCapabilities.h"
#include <string>

namespace APLG::Validation {

class JumpAnalyzer {
public:
    /**
     * @brief Analyze jump constraints and headroom clearance.
     * @param level Level data
     * @param caps Player capabilities model
     * @param outReason Set to error reason if validation fails
     * @return true if valid, false otherwise
     */
    static bool analyze(const LevelData& level, const PlayerCapabilities& caps, std::string& outReason);
};

} // namespace APLG::Validation
