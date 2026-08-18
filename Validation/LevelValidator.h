#pragma once

#include "../Include/Common.h"
#include "PlayerCapabilities.h"
#include "../Generation/IGenerator.h"
#include <string>
#include <memory>

namespace APLG::Validation {

struct ValidationResult {
    bool valid = true;
    std::string reason = "";
    int32 attempts = 1;
    uint32 seedUsed = 0;
};

class LevelValidator {
public:
    LevelValidator() = default;
    
    /**
     * @brief Validate a generated level structure against player capabilities.
     * @param level Level data to check
     * @param caps Player capabilities model
     * @return ValidationResult structure containing status and details
     */
    ValidationResult validate(const LevelData& level, const PlayerCapabilities& caps);
};

class ValidatedLevelGenerator {
public:
    ValidatedLevelGenerator(std::unique_ptr<ILevelGenerator> generator);
    
    /**
     * @brief Generate a valid level by running the generator and validator repeatedly.
     * Uses seed derivation (seed + attempt) for deterministic regeneration.
     * 
     * @param config Level generation settings
     * @param initialSeed Base seed for reproducibility
     * @param caps Player kinematics parameters
     * @param outResult Stores final validation outcome details
     * @param outGenTimeMs Accumulates total generation duration
     * @param outValTimeMs Accumulates total validation duration
     * @return LevelData representing a verified playable level
     */
    LevelData generateValid(const GenerationConfig& config, uint32 initialSeed, const PlayerCapabilities& caps, ValidationResult& outResult, double& outGenTimeMs, double& outValTimeMs);

private:
    std::unique_ptr<ILevelGenerator> m_generator;
    LevelValidator m_validator;
};

} // namespace APLG::Validation
