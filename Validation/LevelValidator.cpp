#include "LevelValidator.h"
#include "ReachabilityAnalyzer.h"
#include "JumpAnalyzer.h"
#include "PlatformConnectivity.h"
#include "../Utilities/ILogger.h"
#include <chrono>

namespace APLG::Validation {

ValidationResult LevelValidator::validate(const LevelData& level, const PlayerCapabilities& caps) {
    ValidationResult result;
    std::string reason;

    // 1. Analyze spawn, exit, and collectible reachability
    if (!ReachabilityAnalyzer::analyze(level, caps, reason)) {
        result.valid = false;
        result.reason = reason;
        return result;
    }

    // 2. Analyze jump gaps and ceiling clearance headroom along path
    if (!JumpAnalyzer::analyze(level, caps, reason)) {
        result.valid = false;
        result.reason = reason;
        return result;
    }

    // 3. Analyze connectivity and check for dead-end or soft-lock regions
    if (!PlatformConnectivity::analyze(level, caps, reason)) {
        result.valid = false;
        result.reason = reason;
        return result;
    }

    return result;
}

ValidatedLevelGenerator::ValidatedLevelGenerator(std::unique_ptr<ILevelGenerator> generator)
    : m_generator(std::move(generator)) {
}

LevelData ValidatedLevelGenerator::generateValid(const GenerationConfig& config, uint32 initialSeed, const PlayerCapabilities& caps, ValidationResult& outResult, double& outGenTimeMs, double& outValTimeMs) {
    const int32 maxAttempts = 100;
    outGenTimeMs = 0.0;
    outValTimeMs = 0.0;

    for (int32 attempt = 0; attempt < maxAttempts; ++attempt) {
        // Seed derivation makes regeneration sequence deterministic from initialSeed
        uint32 currentSeed = initialSeed + static_cast<uint32>(attempt);
        m_generator->setSeed(currentSeed);

        // 1. Generate terrain
        auto genStart = std::chrono::high_resolution_clock::now();
        LevelData level = m_generator->generate(config);
        auto genEnd = std::chrono::high_resolution_clock::now();
        outGenTimeMs += std::chrono::duration<double, std::milli>(genEnd - genStart).count();

        // 2. Validate terrain
        auto valStart = std::chrono::high_resolution_clock::now();
        outResult = m_validator.validate(level, caps);
        auto valEnd = std::chrono::high_resolution_clock::now();
        outValTimeMs += std::chrono::duration<double, std::milli>(valEnd - valStart).count();

        outResult.attempts = attempt + 1;

        if (outResult.valid) {
            LOG_INFO("Valid level generated on attempt: " + std::to_string(attempt + 1) + " (Seed: " + std::to_string(currentSeed) + ")");
            outResult.seedUsed = currentSeed;
            return level;
        }

        LOG_INFO("Attempt " + std::to_string(attempt + 1) + " failed: " + outResult.reason);
    }

    LOG_ERROR("Failed to generate playable level after " + std::to_string(maxAttempts) + " attempts.");
    outResult.valid = false;
    outResult.reason = "Exceeded maximum validation attempts limit.";
    return LevelData();
}

} // namespace APLG::Validation
