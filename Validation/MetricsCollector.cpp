#include "MetricsCollector.h"
#include "../Utilities/Json.h"
#include <fstream>
#include <algorithm>

namespace APLG::Validation {

bool MetricsCollector::save(const std::string& filename, const LevelMetrics& metrics) {
    APLG::JsonValue::Object jsonObj;
    jsonObj["seed"] = static_cast<double>(metrics.seed);
    jsonObj["generation_time_ms"] = metrics.generationTimeMs;
    jsonObj["reachable"] = metrics.reachable;
    jsonObj["platform_count"] = static_cast<double>(metrics.platformCount);
    jsonObj["jump_count"] = static_cast<double>(metrics.jumpCount);
    jsonObj["coins"] = static_cast<double>(metrics.coins);
    jsonObj["enemies"] = static_cast<double>(metrics.enemies);
    jsonObj["difficulty_score"] = static_cast<double>(metrics.difficultyScore);
    jsonObj["validation_time_ms"] = metrics.validationTimeMs;
    jsonObj["attempts"] = static_cast<double>(metrics.attempts);

    std::string jsonStr = APLG::JsonValue(jsonObj).serialize(0);
    // Remove newlines/spaces for single-line JSON Lines format
    jsonStr.erase(std::remove(jsonStr.begin(), jsonStr.end(), '\n'), jsonStr.end());

    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        return false;
    }

    file << jsonStr << "\n";
    return true;
}

} // namespace APLG::Validation
