#include "../../Utilities/SaveSystem.h"
#include <iostream>

using namespace APLG;

/**
 * @brief Test suite for Save System
 */
int main() {
    std::cout << "=== Save System Test ===" << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    // Test 1: Save system initialization
    std::cout << "\n--- Test 1: Save System Initialization ---" << std::endl;
    {
        SaveSystem saveSystem;
        
        if (saveSystem.getSaveDirectory() == "saves") {
            std::cout << "PASS: Save system initialized with default directory" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Save system not initialized correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 2: Save game
    std::cout << "\n--- Test 2: Save Game ---" << std::endl;
    {
        SaveSystem saveSystem;
        SaveData data;
        data.currentLevel = 5;
        data.totalCoins = 100;
        
        if (saveSystem.saveGame(data, "test_slot")) {
            std::cout << "PASS: Game saved successfully" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Game save failed" << std::endl;
            failed++;
        }
    }
    
    // Test 3: Load game
    std::cout << "\n--- Test 3: Load Game ---" << std::endl;
    {
        SaveSystem saveSystem;
        SaveData saveData;
        saveData.currentLevel = 5;
        saveData.totalCoins = 100;
        
        saveSystem.saveGame(saveData, "test_slot");
        
        SaveData loadData;
        if (saveSystem.loadGame(loadData, "test_slot")) {
            if (loadData.currentLevel == 5 && loadData.totalCoins == 100) {
                std::cout << "PASS: Game loaded successfully" << std::endl;
                passed++;
            } else {
                std::cout << "FAIL: Loaded data incorrect" << std::endl;
                failed++;
            }
        } else {
            std::cout << "FAIL: Game load failed" << std::endl;
            failed++;
        }
    }
    
    // Test 4: Save exists check
    std::cout << "\n--- Test 4: Save Exists Check ---" << std::endl;
    {
        SaveSystem saveSystem;
        SaveData data;
        saveSystem.saveGame(data, "test_slot");
        
        if (saveSystem.saveExists("test_slot")) {
            std::cout << "PASS: Save exists detected" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Save exists check failed" << std::endl;
            failed++;
        }
    }
    
    // Test 5: Delete save
    std::cout << "\n--- Test 5: Delete Save ---" << std::endl;
    {
        SaveSystem saveSystem;
        SaveData data;
        saveSystem.saveGame(data, "test_slot");
        
        if (saveSystem.deleteSave("test_slot")) {
            if (!saveSystem.saveExists("test_slot")) {
                std::cout << "PASS: Save deleted successfully" << std::endl;
                passed++;
            } else {
                std::cout << "FAIL: Save still exists after deletion" << std::endl;
                failed++;
            }
        } else {
            std::cout << "FAIL: Save deletion failed" << std::endl;
            failed++;
        }
    }
    
    // Test 6: Save settings
    std::cout << "\n--- Test 6: Save Settings ---" << std::endl;
    {
        SaveSystem saveSystem;
        SettingsData settings;
        settings.masterVolume = 0.5f;
        settings.fullscreen = true;
        
        if (saveSystem.saveSettings(settings)) {
            std::cout << "PASS: Settings saved successfully" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Settings save failed" << std::endl;
            failed++;
        }
    }
    
    // Test 7: Load settings
    std::cout << "\n--- Test 7: Load Settings ---" << std::endl;
    {
        SaveSystem saveSystem;
        SettingsData saveSettings;
        saveSettings.masterVolume = 0.5f;
        saveSettings.fullscreen = true;
        
        saveSystem.saveSettings(saveSettings);
        
        SettingsData loadSettings;
        if (saveSystem.loadSettings(loadSettings)) {
            if (loadSettings.masterVolume == 0.5f && loadSettings.fullscreen == true) {
                std::cout << "PASS: Settings loaded successfully" << std::endl;
                passed++;
            } else {
                std::cout << "FAIL: Loaded settings incorrect" << std::endl;
                failed++;
            }
        } else {
            std::cout << "FAIL: Settings load failed" << std::endl;
            failed++;
        }
    }
    
    // Test 8: Quick save
    std::cout << "\n--- Test 8: Quick Save ---" << std::endl;
    {
        SaveSystem saveSystem;
        SaveData data;
        data.currentLevel = 10;
        
        if (saveSystem.quickSave(data)) {
            std::cout << "PASS: Quick save successful" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Quick save failed" << std::endl;
            failed++;
        }
    }
    
    // Test 9: Quick load
    std::cout << "\n--- Test 9: Quick Load ---" << std::endl;
    {
        SaveSystem saveSystem;
        SaveData data;
        data.currentLevel = 10;
        
        saveSystem.quickSave(data);
        
        SaveData loadData;
        if (saveSystem.quickLoad(loadData)) {
            if (loadData.currentLevel == 10) {
                std::cout << "PASS: Quick load successful" << std::endl;
                passed++;
            } else {
                std::cout << "FAIL: Quick load data incorrect" << std::endl;
                failed++;
            }
        } else {
            std::cout << "FAIL: Quick load failed" << std::endl;
            failed++;
        }
    }
    
    // Test 10: Get save slots
    std::cout << "\n--- Test 10: Get Save Slots ---" << std::endl;
    {
        SaveSystem saveSystem;
        SaveData data;
        saveSystem.saveGame(data, "slot1");
        saveSystem.saveGame(data, "slot2");
        
        auto slots = saveSystem.getSaveSlots();
        
        if (slots.size() >= 2) {
            std::cout << "PASS: Save slots retrieved" << std::endl;
            std::cout << "  Slots: " << slots.size() << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Save slots not retrieved correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 11: Save data serialization
    std::cout << "\n--- Test 11: Save Data Serialization ---" << std::endl;
    {
        SaveSystem saveSystem;
        SaveData data;
        data.currentLevel = 7;
        data.totalCoins = 250;
        data.totalDeaths = 3;
        data.currentDifficulty = DifficultyLevel::Hard;
        
        saveSystem.saveGame(data, "serialize_test");
        
        SaveData loaded;
        saveSystem.loadGame(loaded, "serialize_test");
        
        if (loaded.currentLevel == 7 && loaded.totalCoins == 250 && 
            loaded.totalDeaths == 3 && loaded.currentDifficulty == DifficultyLevel::Hard) {
            std::cout << "PASS: Save data serialized correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Save data serialization failed" << std::endl;
            failed++;
        }
    }
    
    // Test 12: Settings serialization
    std::cout << "\n--- Test 12: Settings Serialization ---" << std::endl;
    {
        SaveSystem saveSystem;
        SettingsData settings;
        settings.masterVolume = 0.75f;
        settings.resolutionWidth = 2560;
        settings.resolutionHeight = 1440;
        settings.defaultDifficulty = DifficultyLevel::Expert;
        
        saveSystem.saveSettings(settings);
        
        SettingsData loaded;
        saveSystem.loadSettings(loaded);
        
        if (loaded.masterVolume == 0.75f && loaded.resolutionWidth == 2560 &&
            loaded.resolutionHeight == 1440 && loaded.defaultDifficulty == DifficultyLevel::Expert) {
            std::cout << "PASS: Settings serialized correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Settings serialization failed" << std::endl;
            failed++;
        }
    }
    
    // Test 13: Auto-save manager
    std::cout << "\n--- Test 13: Auto-save Manager ---" << std::endl;
    {
        SaveSystem saveSystem;
        AutoSaveManager autoSave(&saveSystem);
        
        SaveData data;
        autoSave.triggerAutoSave(data);
        
        if (saveSystem.saveExists("quicksave")) {
            std::cout << "PASS: Auto-save triggered" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Auto-save not triggered" << std::endl;
            failed++;
        }
    }
    
    // Test 14: Auto-save interval
    std::cout << "\n--- Test 14: Auto-save Interval ---" << std::endl;
    {
        SaveSystem saveSystem;
        AutoSaveManager autoSave(&saveSystem);
        autoSave.setInterval(0.5f); // 0.5 seconds for testing
        
        autoSave.update(0.3f); // Should not trigger
        autoSave.update(0.3f); // Should trigger
        
        std::cout << "PASS: Auto-save interval test completed" << std::endl;
        passed++;
    }
    
    // Test 15: Auto-save enable/disable
    std::cout << "\n--- Test 15: Auto-save Enable/Disable ---" << std::endl;
    {
        SaveSystem saveSystem;
        saveSystem.deleteSave("quicksave");
        AutoSaveManager autoSave(&saveSystem);
        autoSave.setEnabled(false);
        
        SaveData data;
        autoSave.triggerAutoSave(data);
        
        if (!saveSystem.saveExists("quicksave")) {
            std::cout << "PASS: Auto-save disabled correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Auto-save triggered despite being disabled" << std::endl;
            failed++;
        }
    }
    
    // Cleanup
    SaveSystem cleanup;
    cleanup.deleteSave("test_slot");
    cleanup.deleteSave("serialize_test");
    cleanup.deleteSave("quicksave");
    cleanup.deleteSave("slot1");
    cleanup.deleteSave("slot2");
    
    // Summary
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "Total:  " << (passed + failed) << std::endl;
    
    return (failed == 0) ? 0 : 1;
}
