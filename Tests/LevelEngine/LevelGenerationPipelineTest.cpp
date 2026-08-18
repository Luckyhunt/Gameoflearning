#include "../../LevelEngine/LevelGenerationPipeline.h"
#include "../../LevelEngine/PlatformerLevelEngine.h"
#include <iostream>

using namespace APLG;

namespace {

bool test(const char* name, bool condition)
{
    if (condition) {
        std::cout << "PASS: " << name << std::endl;
        return true;
    }
    std::cout << "FAIL: " << name << std::endl;
    return false;
}

} // namespace

int main()
{
    std::cout << "=== Level Generation Pipeline Test ===" << std::endl;

    int passed = 0;
    int failed = 0;

    Validation::PlayerCapabilities caps;

    std::cout << "\n--- Test 1: PLEConfig derives from DifficultyProfile ---" << std::endl;
    {
        const DifficultyProfile easy = DifficultyProfile::fromLevel(DifficultyLevel::Easy);
        const PLEConfig cfg = PlatformerLevelEngine::configForDifficulty(
            DifficultyLevel::Easy, 123u);

        if (test("Easy width matches profile",
                 cfg.width == easy.mapWidth && cfg.height == easy.mapHeight)) passed++; else failed++;
        if (test("Geometry-only content densities",
                 cfg.coinDensity == 0.0f && cfg.hazardDensity == 0.0f)) passed++; else failed++;
    }

    std::cout << "\n--- Test 2: Unified pipeline across all difficulties ---" << std::endl;
    {
        const DifficultyLevel levels[] = {
            DifficultyLevel::Easy,
            DifficultyLevel::Normal,
            DifficultyLevel::Hard,
            DifficultyLevel::Expert,
            DifficultyLevel::Nightmare,
        };

        for (const DifficultyLevel level : levels) {
            LevelGenerationPipeline::Request request;
            request.difficulty = level;
            request.playstyle  = Playstyle::Careful;
            request.seed       = 1000 + static_cast<int32>(level);

            const LevelGenerationResult result =
                LevelGenerationPipeline::generate(request, caps);

            const DifficultyProfile expected = DifficultyProfile::fromLevel(level);
            const bool ok =
                result.stats.valid &&
                result.level.width == expected.mapWidth &&
                result.level.height == expected.mapHeight &&
                result.seed == request.seed;

            if (test("Pipeline valid for difficulty tier",
                     ok)) passed++; else failed++;
        }
    }

    std::cout << "\n--- Test 3: Easy intro content (guaranteed path contract) ---" << std::endl;
    {
        LevelGenerationPipeline::Request request;
        request.difficulty = DifficultyLevel::Easy;
        request.playstyle  = Playstyle::Careful;
        request.seed       = 4242;

        const LevelGenerationResult result =
            LevelGenerationPipeline::generate(request, caps);

        if (test("Easy has no hazards", result.level.hazardPositions.empty())) passed++; else failed++;
        if (test("Easy has no enemies", result.level.enemyPositions.empty())) passed++; else failed++;
        if (test("Easy has checkpoints", !result.level.checkpointPositions.empty())) passed++; else failed++;
    }

    std::cout << "\n--- Test 4: Seed determinism ---" << std::endl;
    {
        LevelGenerationPipeline::Request request;
        request.difficulty = DifficultyLevel::Normal;
        request.playstyle  = Playstyle::Careful;
        request.seed       = 9001;

        const LevelGenerationResult a = LevelGenerationPipeline::generate(request, caps);
        const LevelGenerationResult b = LevelGenerationPipeline::generate(request, caps);

        const bool sameSize =
            a.level.width == b.level.width &&
            a.level.height == b.level.height &&
            a.level.spawnPosition.x == b.level.spawnPosition.x &&
            a.level.spawnPosition.y == b.level.spawnPosition.y &&
            a.level.exitPosition.x == b.level.exitPosition.x &&
            a.level.exitPosition.y == b.level.exitPosition.y;

        if (test("Identical seed produces identical layout", sameSize)) passed++; else failed++;
    }

    std::cout << "\n--- Test 5: Meaningful vertical progression ---" << std::endl;
    {
        LevelGenerationPipeline::Request request;
        request.difficulty = DifficultyLevel::Normal;
        request.playstyle  = Playstyle::Explorer;
        request.seed       = 5555;

        const LevelGenerationResult result = LevelGenerationPipeline::generate(request, caps);

        int minY = result.level.height;
        int maxY = 0;
        for (const auto& pos : result.level.criticalPath) {
            if (pos.y < minY) minY = pos.y;
            if (pos.y > maxY) maxY = pos.y;
        }
        int amplitude = maxY - minY;

        if (test("Normal level achieves vertical amplitude >= 4 tiles", amplitude >= 4)) passed++; else failed++;
    }

    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "Total:  " << (passed + failed) << std::endl;

    return failed == 0 ? 0 : 1;
}
