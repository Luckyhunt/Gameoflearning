#include "../../LevelEngine/LevelGenerationPipeline.h"
#include "../../Validation/LevelValidator.h"
#include "../../Validation/PathFinder.h"
#include "../../Generation/LevelValidator.h"
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

    // Test 2: Missing spawn detection (legacy rule tests)
    std::cout << "\n--- Test 2: Missing Spawn Detection ---" << std::endl;
    {
        LevelData level;
        level.width = 20;
        level.height = 10;
        level.tiles.resize(level.height, std::vector<TileType>(level.width, TileType::Empty));
        level.spawnPosition = Vec2i(-1, -1);
        level.exitPosition = Vec2i(18, 5);

        APLG::LevelValidator validator;
        validator.setCheckExit(false);
        validator.setCheckPath(false);
        validator.setCheckGaps(false);
        validator.setCheckEnemies(false);
        validator.setCheckCollectables(false);
        APLG::ValidationResult result = validator.validate(level);

        if (testPassed("Missing spawn detected",
                       !result.valid && result.reason.find("Spawn") != std::string::npos)) {
            ++passed;
        } else {
            ++failed;
        }
    }

    // Test 3: Missing exit detection
    std::cout << "\n--- Test 3: Missing Exit Detection ---" << std::endl;
    {
        LevelData level;
        level.width = 20;
        level.height = 10;
        level.tiles.resize(level.height, std::vector<TileType>(level.width, TileType::Empty));
        level.spawnPosition = Vec2i(2, 5);
        level.exitPosition = Vec2i(-1, -1);

        APLG::LevelValidator validator;
        validator.setCheckSpawn(false);
        validator.setCheckPath(false);
        validator.setCheckGaps(false);
        validator.setCheckEnemies(false);
        validator.setCheckCollectables(false);
        APLG::ValidationResult result = validator.validate(level);

        if (testPassed("Missing exit detected",
                       !result.valid && result.reason.find("Exit") != std::string::npos)) {
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

    // Test 8: Custom validator hook (legacy)
    std::cout << "\n--- Test 8: Custom Validator ---" << std::endl;
    {
        const LevelGenerationResult generated = generatePipelineLevel(DifficultyLevel::Easy, 12);

        APLG::LevelValidator validator;
        validator.setCheckPath(false);
        validator.setCheckGaps(false);
        validator.setCheckCollectables(false);
        validator.setCustomValidator([](const LevelData& level) {
            APLG::ValidationResult result;
            if (level.width < 10) {
                result.invalidate("Level too narrow");
            }
            return result;
        });

        APLG::ValidationResult result = validator.validate(generated.level);
        if (testPassed("Custom validator passed on pipeline level", result.valid)) {
            ++passed;
        } else {
            std::cout << "  Reason: " << result.reason << std::endl;
            ++failed;
        }
    }

    // Test 9: Selective validation toggles
    std::cout << "\n--- Test 9: Selective Validation ---" << std::endl;
    {
        LevelData level;
        level.width = 20;
        level.height = 10;
        level.tiles.resize(level.height, std::vector<TileType>(level.width, TileType::Empty));
        level.spawnPosition = Vec2i(-1, -1);
        level.exitPosition = Vec2i(18, 5);

        APLG::LevelValidator validator;
        validator.setCheckSpawn(false);
        validator.setCheckExit(false);
        validator.setCheckPath(false);
        validator.setCheckGaps(false);
        validator.setCheckEnemies(false);
        validator.setCheckCollectables(false);
        APLG::ValidationResult result = validator.validate(level);

        if (testPassed("Selective validation skips disabled checks", result.valid)) {
            ++passed;
        } else {
            ++failed;
        }
    }

    // Test 10: Validation warnings for unfair enemy placement
    std::cout << "\n--- Test 10: Validation Warnings ---" << std::endl;
    {
        const LevelGenerationResult generated = generatePipelineLevel(DifficultyLevel::Normal, 55);
        LevelData level = generated.level;
        level.enemyPositions.push_back(
            Vec2i(level.spawnPosition.x + 1, level.spawnPosition.y));

        APLG::LevelValidator validator;
        validator.setCheckPath(false);
        validator.setCheckGaps(false);
        validator.setCheckCollectables(false);
        APLG::ValidationResult result = validator.validate(level);

        if (testPassed("Warnings generated for enemy near spawn", !result.warnings.empty())) {
            for (const auto& warning : result.warnings) {
                std::cout << "  Warning: " << warning << std::endl;
            }
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
