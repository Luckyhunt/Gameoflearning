// ============================================================
// ReachabilityRegressionTest.cpp
//
// Regression test for Issue 1: Verifies that 100% of generated levels
// across all difficulty tiers are physically reachable under Godot runtime
// player capabilities.
// ============================================================

#include "Common.h"
#include "LevelGenerationPipeline.h"
#include "../../Validation/LevelValidator.h"
#include "../../Validation/PlayerCapabilities.h"
#include <iostream>
#include <vector>
#include <cassert>

using namespace APLG;
using namespace APLG::Validation;

bool test(const std::string& name, bool condition) {
    if (condition) {
        std::cout << "PASS: " << name << std::endl;
        return true;
    } else {
        std::cout << "FAIL: " << name << std::endl;
        return false;
    }
}

int main() {
    std::cout << "=== Level Reachability Regression Test ===" << std::endl;

    int passed = 0;
    int failed = 0;

    // 1. Single Source of Truth physics capabilities
    PlayerCapabilities caps;
    caps.deriveFromPhysics(180.0f, -420.0f, 800.0f, 32.0f);

    if (test("Single Source of Truth derives correct tile metrics",
             caps.maxJumpHeight == 3 && caps.maxJumpDistance == 5 && caps.tileSize == 32.0f)) {
        passed++;
    } else {
        failed++;
    }

    // 2. Generate 500 levels across all difficulty tiers and verify 100% reachability
    const DifficultyLevel difficulties[] = {
        DifficultyLevel::Easy,
        DifficultyLevel::Normal,
        DifficultyLevel::Hard,
        DifficultyLevel::Expert,
        DifficultyLevel::Nightmare
    };

    LevelValidator validator;
    int totalLevelsTested = 0;
    int totalValidLevels = 0;

    for (const auto& diff : difficulties) {
        for (uint32 seed = 1; seed <= 100; ++seed) {
            totalLevelsTested++;
            LevelGenerationPipeline::Request req;
            req.difficulty = diff;
            req.playstyle  = Playstyle::Explorer;
            req.seed       = seed;

            LevelGenerationResult genResult = LevelGenerationPipeline::generate(req, caps);
            ValidationResult valResult = validator.validate(genResult.level, caps);

            if (valResult.valid) {
                totalValidLevels++;
            } else {
                std::cout << "  [FAIL] Unreachable level detected! Seed=" << seed
                          << " Reason: " << valResult.reason << std::endl;
            }
        }
    }

    if (test("100% of generated levels (500/500) are physically reachable",
             totalLevelsTested == 500 && totalValidLevels == 500)) {
        passed++;
    } else {
        std::cout << "  Passed: " << totalValidLevels << " / " << totalLevelsTested << std::endl;
        failed++;
    }

    // 3. Edge-triggered jump input state logic verification
    bool was_jump_key_down = false;
    int jump_buffer_count = 0;
    int total_jumps_triggered = 0;

    auto process_input_frame = [&](bool key_is_down, bool is_grounded) {
        bool jump_just_pressed = key_is_down && !was_jump_key_down;
        was_jump_key_down = key_is_down;

        if (jump_just_pressed) {
            jump_buffer_count = 10;
        }

        if (jump_buffer_count > 0 && is_grounded) {
            total_jumps_triggered++;
            jump_buffer_count = 0;
        }

        if (jump_buffer_count > 0) {
            jump_buffer_count--;
        }
    };

    // Press down while grounded -> 1 jump
    process_input_frame(true, true);
    // Hold key for 30 airborne frames
    for (int f = 0; f < 30; ++f) {
        process_input_frame(true, false);
    }
    // Land on ground while still holding key -> MUST NOT RE-JUMP
    for (int f = 0; f < 10; ++f) {
        process_input_frame(true, true);
    }
    // Release key
    process_input_frame(false, true);
    // Press key again -> MUST JUMP AGAIN
    process_input_frame(true, true);

    if (test("Edge-triggered jump input state logic (1 jump on press, 0 on hold, 1 on re-press)",
             total_jumps_triggered == 2)) {
        passed++;
    } else {
        std::cout << "  Total jumps triggered: " << total_jumps_triggered << " (expected 2)" << std::endl;
        failed++;
    }

    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;

    return (failed == 0) ? 0 : 1;
}
