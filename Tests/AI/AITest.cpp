#include "../../AI/IPlayerModel.h"
#include <iostream>

using namespace APLG;

/**
 * @brief Test suite for AI module
 */
int main() {
    std::cout << "=== AI Module Test ===" << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    // Test 1: Player model initialization
    std::cout << "\n--- Test 1: Player Model Initialization ---" << std::endl;
    {
        PlayerModel model;
        
        if (model.getPlaystyle() == Playstyle::Careful) {
            std::cout << "PASS: Player model initialized with default playstyle" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Player model not initialized correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 2: Action recording
    std::cout << "\n--- Test 2: Action Recording ---" << std::endl;
    {
        PlayerModel model;
        model.recordAction(PlayerAction::Jump, Vec2(0, 0), 0.0f);
        model.recordAction(PlayerAction::MoveRight, Vec2(1, 0), 0.1f);
        model.recordAction(PlayerAction::Dash, Vec2(2, 0), 0.2f);
        
        BehaviorStats stats = model.getStatistics();
        
        if (stats.totalActions == 3 && stats.jumpCount == 1 && stats.dashCount == 1) {
            std::cout << "PASS: Actions recorded correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Actions not recorded correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 3: Behavior analysis
    std::cout << "\n--- Test 3: Behavior Analysis ---" << std::endl;
    {
        PlayerModel model;
        
        // Record aggressive actions
        for (int32 i = 0; i < 10; ++i) {
            model.recordAction(PlayerAction::Attack, Vec2(i, 0), static_cast<float32>(i) * 0.1f);
            model.recordAction(PlayerAction::Dash, Vec2(i, 0), static_cast<float32>(i) * 0.1f + 0.05f);
        }
        
        model.analyze();
        
        if (model.getPlaystyle() != Playstyle::Careful) {
            std::cout << "PASS: Playstyle detected from actions" << std::endl;
            std::cout << "  Detected: " << static_cast<int>(model.getPlaystyle()) << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Playstyle not detected" << std::endl;
            failed++;
        }
    }
    
    // Test 4: Explorer playstyle detection
    std::cout << "\n--- Test 4: Explorer Playstyle Detection ---" << std::endl;
    {
        PlayerModel model;
        
        // Record exploration actions
        for (int32 i = 0; i < 20; ++i) {
            Vec2 pos(i % 5, i / 5);
            model.recordAction(PlayerAction::MoveRight, pos, static_cast<float32>(i) * 0.1f);
        }
        
        model.analyze();
        
        float32 confidence = model.getConfidence();
        
        if (confidence > 0.0f) {
            std::cout << "PASS: Explorer playstyle detected with confidence" << std::endl;
            std::cout << "  Confidence: " << confidence << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: No confidence in detection" << std::endl;
            failed++;
        }
    }
    
    // Test 5: Playstyle probabilities
    std::cout << "\n--- Test 5: Playstyle Probabilities ---" << std::endl;
    {
        PlayerModel model;
        model.analyze();
        
        const auto& probs = model.getPlaystyleProbabilities();
        
        if (probs.size() == 5) {
            std::cout << "PASS: Playstyle probabilities calculated" << std::endl;
            for (const auto& [style, prob] : probs) {
                std::cout << "  " << static_cast<int>(style) << ": " << prob << std::endl;
            }
            passed++;
        } else {
            std::cout << "FAIL: Incorrect number of playstyle probabilities" << std::endl;
            failed++;
        }
    }
    
    // Test 6: Clear functionality
    std::cout << "\n--- Test 6: Clear Functionality ---" << std::endl;
    {
        PlayerModel model;
        model.recordAction(PlayerAction::Jump, Vec2(0, 0), 0.0f);
        model.recordAction(PlayerAction::Dash, Vec2(1, 0), 0.1f);
        
        model.clear();
        
        BehaviorStats stats = model.getStatistics();
        
        if (stats.totalActions == 0) {
            std::cout << "PASS: Player model cleared successfully" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Player model not cleared" << std::endl;
            failed++;
        }
    }
    
    // Test 7: Analysis window
    std::cout << "\n--- Test 7: Analysis Window ---" << std::endl;
    {
        PlayerModel model;
        model.setAnalysisWindow(5);
        
        for (int32 i = 0; i < 10; ++i) {
            model.recordAction(PlayerAction::MoveRight, Vec2(i, 0), static_cast<float32>(i) * 0.1f);
        }
        
        BehaviorStats stats = model.getStatistics();
        
        if (stats.totalActions == 5) {
            std::cout << "PASS: Analysis window limits recorded actions" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Analysis window not working" << std::endl;
            failed++;
        }
    }
    
    // Test 8: Adaptive generator config
    std::cout << "\n--- Test 8: Adaptive Generator Config ---" << std::endl;
    {
        GenerationConfig baseConfig;
        GenerationConfig explorerConfig = AdaptiveGenerator::getConfigForPlaystyle(
            Playstyle::Explorer, baseConfig);
        
        if (explorerConfig.coinDensity > baseConfig.coinDensity) {
            std::cout << "PASS: Explorer配置 increases coin density" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Explorer config not applied correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 9: Speedrunner config
    std::cout << "\n--- Test 9: Speedrunner Config ---" << std::endl;
    {
        GenerationConfig baseConfig;
        GenerationConfig speedConfig = AdaptiveGenerator::getConfigForPlaystyle(
            Playstyle::Speedrunner, baseConfig);
        
        if (speedConfig.maxGapSize > baseConfig.maxGapSize) {
            std::cout << "PASS: Speedrunner config increases gap size" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Speedrunner config not applied correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 10: Config adjustment
    std::cout << "\n--- Test 10: Config Adjustment ---" << std::endl;
    {
        PlayerModel model;
        model.recordAction(PlayerAction::Attack, Vec2(0, 0), 0.0f);
        model.recordAction(PlayerAction::Dash, Vec2(1, 0), 0.1f);
        model.analyze();
        
        GenerationConfig baseConfig;
        GenerationConfig adjusted = AdaptiveGenerator::adjustConfig(model, baseConfig);
        
        if (adjusted.platformDensity != baseConfig.platformDensity) {
            std::cout << "PASS: Config adjusted based on player model" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Config not adjusted" << std::endl;
            failed++;
        }
    }
    
    // Test 11: Generator recommendation
    std::cout << "\n--- Test 11: Generator Recommendation ---" << std::endl;
    {
        std::string gen = AdaptiveGenerator::recommendGenerator(Playstyle::Speedrunner);
        
        if (!gen.empty()) {
            std::cout << "PASS: Generator recommended for playstyle" << std::endl;
            std::cout << "  Recommended: " << gen << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: No generator recommended" << std::endl;
            failed++;
        }
    }
    
    // Test 12: Session analytics start
    std::cout << "\n--- Test 12: Session Analytics Start ---" << std::endl;
    {
        SessionAnalytics analytics;
        analytics.startSession();
        
        std::cout << "PASS: Session started" << std::endl;
        passed++;
    }
    
    // Test 13: Level completion recording
    std::cout << "\n--- Test 13: Level Completion Recording ---" << std::endl;
    {
        SessionAnalytics analytics;
        analytics.startSession();
        analytics.recordLevelCompletion(60.0f, 2, 10);
        
        SessionAnalytics::SessionStats stats = analytics.getSessionStats();
        
        if (stats.levelsCompleted == 1 && stats.totalCoins == 10) {
            std::cout << "PASS: Level completion recorded" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Level completion not recorded" << std::endl;
            failed++;
        }
    }
    
    // Test 14: Session statistics
    std::cout << "\n--- Test 14: Session Statistics ---" << std::endl;
    {
        SessionAnalytics analytics;
        analytics.startSession();
        analytics.recordLevelCompletion(60.0f, 2, 10);
        analytics.recordLevelCompletion(45.0f, 1, 15);
        
        SessionAnalytics::SessionStats stats = analytics.getSessionStats();
        
        if (stats.levelsCompleted == 2 && stats.totalCoins == 25) {
            std::cout << "PASS: Session statistics calculated" << std::endl;
            std::cout << "  Levels: " << stats.levelsCompleted << std::endl;
            std::cout << "  Total coins: " << stats.totalCoins << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Session statistics incorrect" << std::endl;
            failed++;
        }
    }
    
    // Test 15: JSON export
    std::cout << "\n--- Test 15: JSON Export ---" << std::endl;
    {
        SessionAnalytics analytics;
        analytics.startSession();
        analytics.recordLevelCompletion(60.0f, 2, 10);
        
        std::string json = analytics.exportToJson();
        
        if (!json.empty() && json.find("levelsCompleted") != std::string::npos) {
            std::cout << "PASS: Session exported to JSON" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: JSON export failed" << std::endl;
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
