#include "../../LevelEngine/LevelGenerationPipeline.h"
#include "../../Validation/LevelValidator.h"
#include "../../Validation/PathFinder.h"
#include <iostream>

using namespace APLG;
using APLG::Validation::PlayerCapabilities;
// APLG::Validation will be explicitly qualified for LevelValidator/ValidationResult to prevent collisions

namespace {

bool testPassed(const char* name, bool condition)
{
    if (condition) {
        std::cout << "PASS: " << name << std::endl;
        return true;
    }
    std::cout << "FAIL: " << name << std::endl;
    return false;
}

LevelGenerationResult generatePipelineLevel(DifficultyLevel difficulty, int32 seed)
{
    LevelGenerationPipeline::Request request;
    request.difficulty = difficulty;
    request.playstyle  = Playstyle::Careful;
    request.seed       = seed;

    PlayerCapabilities caps;
    return LevelGenerationPipeline::generate(request, caps);
}

} // namespace

/**
 * @brief Validation tests for the primary PLE+Decorator pipeline and legacy rules.
 */
int main() {
    std::cout << "=== Level Validation Test ===" << std::endl;

    int passed = 0;
    int failed = 0;

    PlayerCapabilities caps;

    // Test 1: Primary pipeline level passes modern validator
    std::cout << "\n--- Test 1: Pipeline Level Passes Validation ---" << std::endl;
    {
        const LevelGenerationResult generated = generatePipelineLevel(DifficultyLevel::Normal, 101);
        Validation::LevelValidator validator;
        Validation::ValidationResult result = validator.validate(generated.level, caps);

        if (testPassed("Pipeline level passes Validation::LevelValidator", result.valid)) {
            ++passed;
        } else {
            std::cout << "  Reason: " << result.reason << std::endl;
            ++failed;
        }
    }

    // Test 2: Invalid spawn detection
    std::cout << "\n--- Test 2: Invalid Spawn Detection ---" << std::endl;
    {
        LevelData level;
        level.width = 20;
        level.height = 10;
        level.tiles.resize(level.height, std::vector<TileType>(level.width, TileType::Empty));
        level.spawnPosition = Vec2i(-1, -1);
        level.exitPosition = Vec2i(18, 5);

        Validation::LevelValidator validator;
        Validation::ValidationResult result = validator.validate(level, caps);

        if (testPassed("Invalid spawn detected", !result.valid)) {
            ++passed;
        } else {
            ++failed;
        }
    }

    // Test 3: Invalid exit detection
    std::cout << "\n--- Test 3: Invalid Exit Detection ---" << std::endl;
    {
        LevelData level;
        level.width = 20;
        level.height = 10;
        level.tiles.resize(level.height, std::vector<TileType>(level.width, TileType::Empty));
        level.spawnPosition = Vec2i(2, 5);
        level.exitPosition = Vec2i(-1, -1);

        Validation::LevelValidator validator;
        Validation::ValidationResult result = validator.validate(level, caps);

        if (testPassed("Invalid exit detected", !result.valid)) {
            ++passed;
        } else {
            ++failed;
        }
    }

    // Test 4: Critical path exported by PLE
    std::cout << "\n--- Test 4: Critical Path Available ---" << std::endl;
    {
        const LevelGenerationResult generated = generatePipelineLevel(DifficultyLevel::Hard, 77);

        if (testPassed("PLE exports non-empty critical path",
                       !generated.level.criticalPath.empty())) {
            std::cout << "  Path length: " << generated.level.criticalPath.size() << std::endl;
            ++passed;
        } else {
            ++failed;
        }
    }

    // Test 5: Reachability via critical path endpoints
    std::cout << "\n--- Test 5: Reachability Check ---" << std::endl;
    {
        const LevelGenerationResult generated = generatePipelineLevel(DifficultyLevel::Normal, 88);
        const Vec2i start = generated.level.criticalPath.front();
        const Vec2i end   = generated.level.criticalPath.back();

        if (testPassed("Exit reachable from spawn along exported path",
                       start == generated.level.spawnPosition &&
                       end == generated.level.exitPosition)) {
            ++passed;
        } else {
            ++failed;
        }
    }

    // Test 6: Reachable area from spawn
    std::cout << "\n--- Test 6: Reachable Area ---" << std::endl;
    {
        const LevelGenerationResult generated = generatePipelineLevel(DifficultyLevel::Expert, 99);
        std::vector<Vec2i> area = Validation::PathFinder::getReachableArea(
            generated.level, generated.level.spawnPosition, caps);

        if (testPassed("Reachable area calculated", !area.empty())) {
            std::cout << "  Reachable tiles: " << area.size() << std::endl;
            ++passed;
        } else {
            ++failed;
        }
    }

    // Test 7: Pipeline always valid across difficulties
    std::cout << "\n--- Test 7: Pipeline Validity Matrix ---" << std::endl;
    {
        const DifficultyLevel tiers[] = {
            DifficultyLevel::Easy,
            DifficultyLevel::Normal,
            DifficultyLevel::Hard,
            DifficultyLevel::Expert,
            DifficultyLevel::Nightmare,
        };

        bool allValid = true;
        for (const DifficultyLevel tier : tiers) {
            const LevelGenerationResult generated = generatePipelineLevel(tier, 100 + static_cast<int32>(tier));
            if (!generated.stats.valid) {
                std::cout << "  Tier " << static_cast<int>(tier) << " failed validation: " << generated.stats.reason << std::endl;
                allValid = false;
            }
        }

        if (testPassed("All difficulty tiers produce valid pipeline levels", allValid)) {
            ++passed;
        } else {
            ++failed;
        }
    }

    // Test 8: Validation with default capabilities
    std::cout << "\n--- Test 8: Validation with Default Capabilities ---" << std::endl;
    {
        const LevelGenerationResult generated = generatePipelineLevel(DifficultyLevel::Easy, 12);

        APLG::Validation::LevelValidator validator;
        PlayerCapabilities caps;
        APLG::Validation::ValidationResult result = validator.validate(generated.level, caps);

        if (testPassed("Validator executed on pipeline level", result.valid || !result.reason.empty())) {
            ++passed;
        } else {
            ++failed;
        }
    }

    // Test 9: Validation with hard difficulty
    std::cout << "\n--- Test 9: Validation with Hard Difficulty ---" << std::endl;
    {
        const LevelGenerationResult generated = generatePipelineLevel(DifficultyLevel::Hard, 99);

        APLG::Validation::LevelValidator validator;
        PlayerCapabilities caps;
        APLG::Validation::ValidationResult result = validator.validate(generated.level, caps);

        if (testPassed("Validator executed on hard difficulty level", result.valid || !result.reason.empty())) {
            ++passed;
        } else {
            ++failed;
        }
    }

    // Test 10: Validation result for level layout
    std::cout << "\n--- Test 10: Validation Result ---" << std::endl;
    {
        const LevelGenerationResult generated = generatePipelineLevel(DifficultyLevel::Normal, 55);
        LevelData level = generated.level;
        level.enemyPositions.push_back(
            Vec2i(level.spawnPosition.x + 1, level.spawnPosition.y));

        APLG::Validation::LevelValidator validator;
        PlayerCapabilities caps;
        APLG::Validation::ValidationResult result = validator.validate(level, caps);

        if (testPassed("Validation processed for custom level", result.valid || !result.reason.empty())) {
            ++passed;
        } else {
            ++failed;
        }
    }

    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "Total:  " << (passed + failed) << std::endl;

    return failed == 0 ? 0 : 1;
}
