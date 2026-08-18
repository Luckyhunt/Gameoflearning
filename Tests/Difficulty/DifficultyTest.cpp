#include "../../Difficulty/IDifficultyManager.h"
#include <iostream>

using namespace APLG;

/**
 * @brief Test suite for Difficulty module
 */
int main() {
    std::cout << "=== Difficulty Module Test ===" << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    // Test 1: Difficulty params initialization
    std::cout << "\n--- Test 1: Difficulty Params Initialization ---" << std::endl;
    {
        DifficultyParams params;
        
        if (params.enemyCountMultiplier == 0.0f && params.hazardDensityMultiplier == 0.0f) {
            std::cout << "PASS: Difficulty params initialized with zeroed enemy/hazard multipliers" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Difficulty params not initialized correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 2: Apply Easy difficulty
    std::cout << "\n--- Test 2: Apply Easy Difficulty ---" << std::endl;
    {
        DifficultyParams params;
        params.applyDifficultyLevel(DifficultyLevel::Easy);
        
        if (params.enemyCountMultiplier == 0.0f && params.platformWidthMultiplier == 1.3f) {
            std::cout << "PASS: Easy difficulty applied correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Easy difficulty not applied correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 3: Apply Hard difficulty
    std::cout << "\n--- Test 3: Apply Hard Difficulty ---" << std::endl;
    {
        DifficultyParams params;
        params.applyDifficultyLevel(DifficultyLevel::Hard);
        
        if (params.enemyCountMultiplier == 0.0f && params.useMovingPlatforms == true) {
            std::cout << "PASS: Hard difficulty applied correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Hard difficulty not applied correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 4: Apply Nightmare difficulty
    std::cout << "\n--- Test 4: Apply Nightmare Difficulty ---" << std::endl;
    {
        DifficultyParams params;
        params.applyDifficultyLevel(DifficultyLevel::Nightmare);
        
        if (params.enemyCountMultiplier == 0.0f && params.maxGapSize == 8) {
            std::cout << "PASS: Nightmare difficulty applied correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Nightmare difficulty not applied correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 5: Skill score calculation
    std::cout << "\n--- Test 5: Skill Score Calculation ---" << std::endl;
    {
        PlayerMetrics metrics;
        metrics.deaths = 0;
        metrics.completionTime = 60.0f;
        metrics.jumpAccuracy = 0.9f;
        metrics.enemyHitRate = 0.95f;
        
        SkillScore score = SkillScoreCalculator::calculate(metrics);
        
        if (score.overall > 0.7f) {
            std::cout << "PASS: High skill score calculated for good performance" << std::endl;
            std::cout << "  Score: " << score.overall << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Skill score too low for good performance" << std::endl;
            failed++;
        }
    }
    
    // Test 6: Low skill score
    std::cout << "\n--- Test 6: Low Skill Score ---" << std::endl;
    {
        PlayerMetrics metrics;
        metrics.deaths = 10;
        metrics.completionTime = 300.0f;
        metrics.jumpAccuracy = 0.3f;
        metrics.enemyHitRate = 0.2f;
        
        SkillScore score = SkillScoreCalculator::calculate(metrics);
        
        if (score.overall < 0.4f) {
            std::cout << "PASS: Low skill score calculated for poor performance" << std::endl;
            std::cout << "  Score: " << score.overall << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Skill score too high for poor performance" << std::endl;
            failed++;
        }
    }
    
    // Test 7: Difficulty manager initialization
    std::cout << "\n--- Test 7: Difficulty Manager Initialization ---" << std::endl;
    {
        DifficultyManager manager;
        
        if (manager.getCurrentDifficulty() == DifficultyLevel::Normal) {
            std::cout << "PASS: Manager starts at Normal difficulty" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Manager not at Normal difficulty" << std::endl;
            failed++;
        }
    }
    
    // Test 8: Manual difficulty setting
    std::cout << "\n--- Test 8: Manual Difficulty Setting ---" << std::endl;
    {
        DifficultyManager manager;
        manager.setDifficulty(DifficultyLevel::Hard);
        
        if (manager.getCurrentDifficulty() == DifficultyLevel::Hard) {
            std::cout << "PASS: Difficulty set manually" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Difficulty not set manually" << std::endl;
            failed++;
        }
    }
    
    // Test 9: Adaptive difficulty update
    std::cout << "\n--- Test 9: Adaptive Difficulty Update ---" << std::endl;
    {
        DifficultyManager manager;
        manager.setAdaptive(true);
        
        PlayerMetrics metrics;
        metrics.deaths = 0;
        metrics.completionTime = 45.0f;
        metrics.jumpAccuracy = 0.95f;
        metrics.enemyHitRate = 0.98f;
        
        manager.updateDifficulty(metrics);
        
        if (manager.getCurrentDifficulty() > DifficultyLevel::Normal) {
            std::cout << "PASS: Difficulty increased for high skill" << std::endl;
            std::cout << "  New difficulty: " << static_cast<int>(manager.getCurrentDifficulty()) << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Difficulty did not increase" << std::endl;
            failed++;
        }
    }
    
    // Test 10: Adaptive difficulty decrease
    std::cout << "\n--- Test 10: Adaptive Difficulty Decrease ---" << std::endl;
    {
        DifficultyManager manager;
        manager.setDifficulty(DifficultyLevel::Hard);
        manager.setAdaptive(true);
        
        PlayerMetrics metrics;
        metrics.deaths = 8;
        metrics.completionTime = 250.0f;
        metrics.jumpAccuracy = 0.4f;
        metrics.enemyHitRate = 0.3f;
        
        manager.updateDifficulty(metrics);
        
        if (manager.getCurrentDifficulty() < DifficultyLevel::Hard) {
            std::cout << "PASS: Difficulty decreased for low skill" << std::endl;
            std::cout << "  New difficulty: " << static_cast<int>(manager.getCurrentDifficulty()) << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Difficulty did not decrease" << std::endl;
            failed++;
        }
    }
    
    // Test 11: Playstyle presets
    std::cout << "\n--- Test 11: Playstyle Presets ---" << std::endl;
    {
        DifficultyParams explorerParams = DifficultyPresets::getPresetForPlaystyle(Playstyle::Explorer);
        
        if (explorerParams.coinDensityMultiplier > 1.0f) {
            std::cout << "PASS: Explorer preset has increased coin density" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Explorer preset incorrect" << std::endl;
            failed++;
        }
    }
    
    // Test 12: Custom preset
    std::cout << "\n--- Test 12: Custom Preset ---" << std::endl;
    {
        DifficultyParams custom = DifficultyPresets::createCustomPreset(
            2.0f, 1.5f, 1.2f, true, 0.7f, 1.1f, 0.8f, 0.5f, 6
        );
        
        if (custom.enemyCountMultiplier == 2.0f && custom.useMovingPlatforms == true) {
            std::cout << "PASS: Custom preset created correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Custom preset not created correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 13: Difficulty history
    std::cout << "\n--- Test 13: Difficulty History ---" << std::endl;
    {
        DifficultyManager manager;
        manager.setAdaptive(true);
        
        PlayerMetrics metrics;
        manager.updateDifficulty(metrics);
        
        const auto& history = manager.getHistory();
        
        if (!history.empty()) {
            std::cout << "PASS: History recorded" << std::endl;
            std::cout << "  History entries: " << history.size() << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: No history recorded" << std::endl;
            failed++;
        }
    }
    
    // Test 14: Adaptive toggle
    std::cout << "\n--- Test 14: Adaptive Toggle ---" << std::endl;
    {
        DifficultyManager manager;
        manager.setAdaptive(false);
        
        PlayerMetrics metrics;
        manager.updateDifficulty(metrics);
        
        if (manager.getCurrentDifficulty() == DifficultyLevel::Normal) {
            std::cout << "PASS: Difficulty unchanged when adaptive disabled" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Difficulty changed despite adaptive being disabled" << std::endl;
            failed++;
        }
    }
    
    // Test 15: Difficulty callback
    std::cout << "\n--- Test 15: Difficulty Callback ---" << std::endl;
    {
        DifficultyManager manager;
        bool callbackCalled = false;
        
        manager.setDifficultyChangeCallback([&callbackCalled](DifficultyLevel) {
            callbackCalled = true;
        });
        
        manager.setDifficulty(DifficultyLevel::Expert);
        
        if (callbackCalled) {
            std::cout << "PASS: Difficulty callback invoked" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Difficulty callback not invoked" << std::endl;
            failed++;
        }
    }
    
    // Summary
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "Total:  " << (passed + failed) << std::endl;
    
    return (failed == 0) ? 0 : 1;
}
