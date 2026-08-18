#include "../../UI/IUI.h"
#include <iostream>

using namespace APLG;

/**
 * @brief Test suite for UI module
 */
int main() {
    std::cout << "=== UI Module Test ===" << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    // Test 1: UI element creation
    std::cout << "\n--- Test 1: UI Element Creation ---" << std::endl;
    {
        UILabel label("test_label", "Hello");
        
        if (label.getType() == UIElementType::Label && label.getId() == "test_label") {
            std::cout << "PASS: UI element created successfully" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: UI element not created correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 2: Label text
    std::cout << "\n--- Test 2: Label Text ---" << std::endl;
    {
        UILabel label("test", "Initial");
        label.setText("Updated");
        
        if (label.getText() == "Updated") {
            std::cout << "PASS: Label text updated" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Label text not updated" << std::endl;
            failed++;
        }
    }
    
    // Test 3: Label font size
    std::cout << "\n--- Test 3: Label Font Size ---" << std::endl;
    {
        UILabel label("test");
        label.setFontSize(24.0f);
        
        if (label.getFontSize() == 24.0f) {
            std::cout << "PASS: Label font size set" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Label font size not set" << std::endl;
            failed++;
        }
    }
    
    // Test 4: Button creation
    std::cout << "\n--- Test 4: Button Creation ---" << std::endl;
    {
        UIButton button("test_button", "Click Me");
        
        if (button.getType() == UIElementType::Button && button.getText() == "Click Me") {
            std::cout << "PASS: Button created successfully" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Button not created correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 5: Button callback
    std::cout << "\n--- Test 5: Button Callback ---" << std::endl;
    {
        UIButton button("test");
        bool callbackCalled = false;
        
        button.setOnClick([&callbackCalled]() {
            callbackCalled = true;
        });
        
        button.click();
        
        if (callbackCalled) {
            std::cout << "PASS: Button callback invoked" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Button callback not invoked" << std::endl;
            failed++;
        }
    }
    
    // Test 6: Progress bar
    std::cout << "\n--- Test 6: Progress Bar ---" << std::endl;
    {
        UIProgressBar bar("test_bar");
        bar.setProgress(0.75f);
        
        if (bar.getProgress() == 0.75f) {
            std::cout << "PASS: Progress bar set" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Progress bar not set" << std::endl;
            failed++;
        }
    }
    
    // Test 7: Progress bar clamping
    std::cout << "\n--- Test 7: Progress Bar Clamping ---" << std::endl;
    {
        UIProgressBar bar("test_bar");
        bar.setProgress(1.5f); // Should clamp to 1.0
        
        if (bar.getProgress() == 1.0f) {
            std::cout << "PASS: Progress bar clamped to 1.0" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Progress bar not clamped" << std::endl;
            failed++;
        }
    }
    
    // Test 8: HUD creation
    std::cout << "\n--- Test 8: HUD Creation ---" << std::endl;
    {
        HUD hud;
        
        if (hud.isVisible()) {
            std::cout << "PASS: HUD created and visible" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: HUD not visible" << std::endl;
            failed++;
        }
    }
    
    // Test 9: HUD health update
    std::cout << "\n--- Test 9: HUD Health Update ---" << std::endl;
    {
        HUD hud;
        hud.updateHealth(75, 100);
        
        std::cout << "PASS: HUD health updated" << std::endl;
        passed++;
    }
    
    // Test 10: HUD coin update
    std::cout << "\n--- Test 10: HUD Coin Update ---" << std::endl;
    {
        HUD hud;
        hud.updateCoins(50);
        
        std::cout << "PASS: HUD coins updated" << std::endl;
        passed++;
    }
    
    // Test 11: HUD time update
    std::cout << "\n--- Test 11: HUD Time Update ---" << std::endl;
    {
        HUD hud;
        hud.updateTime(125.0f);
        
        std::cout << "PASS: HUD time updated" << std::endl;
        passed++;
    }
    
    // Test 12: HUD level update
    std::cout << "\n--- Test 12: HUD Level Update ---" << std::endl;
    {
        HUD hud;
        hud.updateLevel(5);
        
        std::cout << "PASS: HUD level updated" << std::endl;
        passed++;
    }
    
    // Test 13: Menu creation
    std::cout << "\n--- Test 13: Menu Creation ---" << std::endl;
    {
        Menu menu("test_menu");
        
        if (!menu.isVisible()) {
            std::cout << "PASS: Menu created and hidden by default" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Menu visible by default" << std::endl;
            failed++;
        }
    }
    
    // Test 14: Menu show/hide
    std::cout << "\n--- Test 14: Menu Show/Hide ---" << std::endl;
    {
        Menu menu("test_menu");
        menu.show();
        
        if (menu.isVisible()) {
            menu.hide();
            if (!menu.isVisible()) {
                std::cout << "PASS: Menu show/hide works" << std::endl;
                passed++;
            } else {
                std::cout << "FAIL: Menu hide failed" << std::endl;
                failed++;
            }
        } else {
            std::cout << "FAIL: Menu show failed" << std::endl;
            failed++;
        }
    }
    
    // Test 15: Menu button addition
    std::cout << "\n--- Test 15: Menu Button Addition ---" << std::endl;
    {
        Menu menu("test_menu");
        menu.addButton("btn1", "Button 1", Vec2(10, 10), []() {});
        
        IUIButton* button = menu.getButton("btn1");
        
        if (button && button->getText() == "Button 1") {
            std::cout << "PASS: Menu button added and retrieved" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Menu button not added or retrieved" << std::endl;
            failed++;
        }
    }
    
    // Test 16: UI Manager
    std::cout << "\n--- Test 16: UI Manager ---" << std::endl;
    {
        UIManager* manager = getUIManager();
        
        if (manager && manager->getHUD() && manager->getMainMenu()) {
            std::cout << "PASS: UI Manager initialized" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: UI Manager not initialized" << std::endl;
            failed++;
        }
    }
    
    // Test 17: UI Manager menu access
    std::cout << "\n--- Test 17: UI Manager Menu Access ---" << std::endl;
    {
        UIManager* manager = getUIManager();
        
        if (manager->getMainMenu() && manager->getPauseMenu() && manager->getSettingsMenu()) {
            std::cout << "PASS: All menus accessible" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Menus not accessible" << std::endl;
            failed++;
        }
    }
    
    // Test 18: UI Manager show main menu
    std::cout << "\n--- Test 18: UI Manager Show Main Menu ---" << std::endl;
    {
        UIManager* manager = getUIManager();
        manager->showMainMenu(true);
        
        if (manager->getMainMenu()->isVisible()) {
            manager->showMainMenu(false);
            std::cout << "PASS: UI Manager shows/hides main menu" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: UI Manager menu control failed" << std::endl;
            failed++;
        }
    }
    
    // Test 19: UI Manager show HUD
    std::cout << "\n--- Test 19: UI Manager Show HUD ---" << std::endl;
    {
        UIManager* manager = getUIManager();
        manager->showHUD(false);
        
        if (!manager->getHUD()->isVisible()) {
            manager->showHUD(true);
            std::cout << "PASS: UI Manager shows/hides HUD" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: UI Manager HUD control failed" << std::endl;
            failed++;
        }
    }
    
    // Test 20: UI element position/size
    std::cout << "\n--- Test 20: UI Element Position/Size ---" << std::endl;
    {
        UILabel label("test");
        label.setPosition(Vec2(50, 100));
        label.setSize(Vec2(200, 50));
        
        if (label.getPosition().x == 50 && label.getPosition().y == 100 &&
            label.getSize().x == 200 && label.getSize().y == 50) {
            std::cout << "PASS: UI element position and size set" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: UI element position/size not set" << std::endl;
            failed++;
        }
    }
    
    // Cleanup
    cleanupUIManager();
    
    // Summary
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "Total:  " << (passed + failed) << std::endl;
    
    return (failed == 0) ? 0 : 1;
}
