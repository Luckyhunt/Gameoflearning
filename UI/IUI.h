#pragma once

#include "../Include/Common.h"
#include <functional>
#include <vector>

namespace APLG {

/**
 * @brief UI element types
 */
enum class UIElementType {
    Label,
    Button,
    ProgressBar,
    Slider,
    Checkbox,
    Panel,
    Image,
    Container
};

/**
 * @brief UI element base interface
 */
class IUIElement {
public:
    virtual ~IUIElement() = default;
    
    /**
     * @brief Get element type
     */
    virtual UIElementType getType() const = 0;
    
    /**
     * @brief Get element ID
     */
    virtual std::string getId() const = 0;
    
    /**
     * @brief Set element position
     */
    virtual void setPosition(Vec2 position) = 0;
    
    /**
     * @brief Get element position
     */
    virtual Vec2 getPosition() const = 0;
    
    /**
     * @brief Set element size
     */
    virtual void setSize(Vec2 size) = 0;
    
    /**
     * @brief Get element size
     */
    virtual Vec2 getSize() const = 0;
    
    /**
     * @brief Set element visible
     */
    virtual void setVisible(bool visible) = 0;
    
    /**
     * @brief Check if element is visible
     */
    virtual bool isVisible() const = 0;
    
    /**
     * @brief Set element enabled
     */
    virtual void setEnabled(bool enabled) = 0;
    
    /**
     * @brief Check if element is enabled
     */
    virtual bool isEnabled() const = 0;
    
    /**
     * @brief Update element
     */
    virtual void update(float32 deltaTime) = 0;
};

/**
 * @brief Label element
 */
class IUILabel : public IUIElement {
public:
    virtual ~IUILabel() = default;
    
    /**
     * @brief Set text
     */
    virtual void setText(const std::string& text) = 0;
    
    /**
     * @brief Get text
     */
    virtual std::string getText() const = 0;
    
    /**
     * @brief Set font size
     */
    virtual void setFontSize(float32 size) = 0;
    
    /**
     * @brief Get font size
     */
    virtual float32 getFontSize() const = 0;
};

/**
 * @brief Button element
 */
class IUIButton : public IUIElement {
public:
    virtual ~IUIButton() = default;
    
    /**
     * @brief Set button text
     */
    virtual void setText(const std::string& text) = 0;
    
    /**
     * @brief Get button text
     */
    virtual std::string getText() const = 0;
    
    /**
     * @brief Set click callback
     */
    virtual void setOnClick(std::function<void()> callback) = 0;
    
    /**
     * @brief Trigger click
     */
    virtual void click() = 0;
    
    /**
     * @brief Check if button is hovered
     */
    virtual bool isHovered() const = 0;
};

/**
 * @brief Progress bar element
 */
class IUIProgressBar : public IUIElement {
public:
    virtual ~IUIProgressBar() = default;
    
    /**
     * @brief Set progress (0.0 to 1.0)
     */
    virtual void setProgress(float32 progress) = 0;
    
    /**
     * @brief Get progress
     */
    virtual float32 getProgress() const = 0;
    
    /**
     * @brief Set color
     */
    virtual void setColor(uint32 color) = 0;
};

/**
 * @brief HUD interface
 */
class IHUD {
public:
    virtual ~IHUD() = default;
    
    /**
     * @brief Update HUD
     */
    virtual void update(float32 deltaTime) = 0;
    
    /**
     * @brief Show/hide HUD
     */
    virtual void setVisible(bool visible) = 0;
    
    /**
     * @brief Update health display
     */
    virtual void updateHealth(int32 current, int32 max) = 0;
    
    /**
     * @brief Update coin display
     */
    virtual void updateCoins(int32 count) = 0;
    
    /**
     * @brief Update time display
     */
    virtual void updateTime(float32 time) = 0;
    
    /**
     * @brief Update level display
     */
    virtual void updateLevel(int32 level) = 0;
    
    /**
     * @brief Show message
     */
    virtual void showMessage(const std::string& message, float32 duration) = 0;
};

/**
 * @brief Menu interface
 */
class IMenu {
public:
    virtual ~IMenu() = default;
    
    /**
     * @brief Show menu
     */
    virtual void show() = 0;
    
    /**
     * @brief Hide menu
     */
    virtual void hide() = 0;
    
    /**
     * @brief Check if menu is visible
     */
    virtual bool isVisible() const = 0;
    
    /**
     * @brief Update menu
     */
    virtual void update(float32 deltaTime) = 0;
    
    /**
     * @brief Add button to menu
     */
    virtual void addButton(const std::string& id, const std::string& text, 
                          Vec2 position, std::function<void()> callback) = 0;
    
    /**
     * @brief Get button by ID
     */
    virtual IUIButton* getButton(const std::string& id) = 0;
};

/**
 * @brief UI manager
 * 
 * Manages all UI elements, HUD, and menus.
 */
class IUIManager {
public:
    virtual ~IUIManager() = default;
    
    /**
     * @brief Update UI
     */
    virtual void update(float32 deltaTime) = 0;
    
    /**
     * @brief Get HUD
     */
    virtual IHUD* getHUD() = 0;
    
    /**
     * @brief Get main menu
     */
    virtual IMenu* getMainMenu() = 0;
    
    /**
     * @brief Get pause menu
     */
    virtual IMenu* getPauseMenu() = 0;
    
    /**
     * @brief Get settings menu
     */
    virtual IMenu* getSettingsMenu() = 0;
    
    /**
     * @brief Show/hide main menu
     */
    virtual void showMainMenu(bool show) = 0;
    
    /**
     * @brief Show/hide pause menu
     */
    virtual void showPauseMenu(bool show) = 0;
    
    /**
     * @brief Show/hide HUD
     */
    virtual void showHUD(bool show) = 0;
};

/**
 * @brief Get global UI manager instance
 */
class IUIManager;
IUIManager* getUIManager();

/**
 * @brief Cleanup UI manager
 */
void cleanupUIManager();

} // namespace APLG
