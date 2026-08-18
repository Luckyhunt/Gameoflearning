#include "../../Generation/IGenerator.h"
#include "../../Validation/LevelValidator.h"
#include <iostream>
#include <fstream>
#include <chrono>

using namespace APLG;
using namespace APLG::Validation;

/**
 * @brief Stress test for procedural generation
 * 
 * Generates multiple levels and validates them automatically.
 * This test ensures the generation system produces valid, playable levels.
 */
int main() {
    std::cout << "=== Generation Stress Test ===" << std::endl;
    
    const int32 numLevels = 100; // 100 levels for fast verification
    int32 validLevels = 0;
    int32 invalidLevels = 0;
    
    GenerationConfig config;
    config.minWidth = 30;
    config.maxWidth = 50;
    config.minHeight = 15;
    config.maxHeight = 25;
    
    PlayerCapabilities caps;
    
    std::cout << "Generating " << numLevels << " validated levels..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int32 i = 0; i < numLevels; ++i) {
        // Generate level using Validated Room Graph Generator
        ValidatedLevelGenerator generator(std::make_unique<RoomGraphGenerator>());
        ValidationResult result;
        double genMs = 0.0, valMs = 0.0;
        
        LevelData level = generator.generateValid(config, i, caps, result, genMs, valMs);
        
        if (result.valid) {
            validLevels++;
        } else {
            invalidLevels++;
            std::cout << "Invalid level " << i << ": " << result.reason << std::endl;
        }
        
        // Progress indicator
        if ((i + 1) % 100 == 0) {
            std::cout << "Progress: " << (i + 1) << "/" << numLevels << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "\n=== Stress Test Results ===" << std::endl;
    std::cout << "Total levels generated: " << numLevels << std::endl;
    std::cout << "Valid levels: " << validLevels << std::endl;
    std::cout << "Invalid levels: " << invalidLevels << std::endl;
    std::cout << "Success rate: " << (static_cast<float32>(validLevels) / numLevels * 100.0f) << "%" << std::endl;
    std::cout << "Time taken: " << duration.count() << "ms" << std::endl;
    std::cout << "Average time per level: " << (static_cast<float32>(duration.count()) / numLevels) << "ms" << std::endl;
    
    // Write results to file
    std::ofstream results("stress_test_results.txt");
    if (results.is_open()) {
        results << "Generation Stress Test Results\n";
        results << "==============================\n";
        results << "Total levels: " << numLevels << "\n";
        results << "Valid levels: " << validLevels << "\n";
        results << "Invalid levels: " << invalidLevels << "\n";
        results << "Success rate: " << (static_cast<float32>(validLevels) / numLevels * 100.0f) << "%\n";
        results << "Time taken: " << duration.count() << "ms\n";
        results.close();
        std::cout << "Results saved to stress_test_results.txt" << std::endl;
    }
    
    return (invalidLevels == 0) ? 0 : 1;
}
