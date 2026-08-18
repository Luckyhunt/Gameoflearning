#pragma once

#include "../Include/Common.h"
#include "PlayerCapabilities.h"
#include <string>

namespace APLG::Validation {

class PlatformConnectivity {
public:
    /**
     * @brief Analyze platform connectivity and trap detection (soft-locks).
     * @param level Level data
     * @param caps Player capabilities model
     * @param outReason Set to error reason if a soft-lock or isolation trap is found
     * @return true if valid, false otherwise
     */
    static bool analyze(const LevelData& level, const PlayerCapabilities& caps, std::string& outReason);
};

} // namespace APLG::Validation
