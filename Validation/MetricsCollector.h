#pragma once

#include "../Include/Common.h"
#include <string>

namespace APLG::Validation {

struct LevelMetrics {
    uint32 seed = 0;
    double generationTimeMs = 0.0;
    double validationTimeMs = 0.0;
    bool reachable = false;
    int32 platformCount = 0;
    int32 jumpCount = 0;
    int32 coins = 0;
    int32 enemies = 0;
    float32 difficultyScore = 0.0f;
    int32 attempts = 1;
};

class MetricsCollector {
public:
    /**
     * @brief Save validation and generation metrics to metrics.json
     * @param filename Filename to append metrics to
     * @param metrics Collected metrics data
     * @return true if save succeeded, false otherwise
     */
    static bool save(const std::string& filename, const LevelMetrics& metrics);
};

} // namespace APLG::Validation
