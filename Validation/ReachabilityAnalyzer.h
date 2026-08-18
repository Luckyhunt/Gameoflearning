#pragma once

#include "../Include/Common.h"
#include "PlayerCapabilities.h"
#include <string>

namespace APLG::Validation {

class ReachabilityAnalyzer {
public:
    /**
     * @brief Analyze reachability constraints of a generated level.
     * @param level Level data
     * @param caps Player capabilities model
     * @param outReason Set to error reason if reachability check fails
     * @return true if reachability is valid, false otherwise
     */
    static bool analyze(const LevelData& level, const PlayerCapabilities& caps, std::string& outReason);
};

} // namespace APLG::Validation
