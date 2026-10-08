#pragma once

#include "../Include/Common.h"
#include <algorithm>
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
     * @brief Check if HUD is visible
     */
    virtual bool isVisible() const = 0;
    
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
 * @brief Base UI element implementation
 */
class UIElement : public IUIElement {
public:
    UIElement(UIElementType type, const std::string& id)
        : m_type(type)
        , m_id(id)
        , m_position(0, 0)
        , m_size(100, 50)
        , m_visible(true)
        , m_enabled(true)
    {
    }
    
    UIElementType getType() const override { return m_type; }
    std::string getId() const override { return m_id; }
    void setPosition(Vec2 position) override { m_position = position; }
    Vec2 getPosition() const override { return m_position; }
    void setSize(Vec2 size) override { m_size = size; }
    Vec2 getSize() const override { return m_size; }
    void setVisible(bool visible) override { m_visible = visible; }
    bool isVisible() const override { return m_visible; }
    void setEnabled(bool enabled) override { m_enabled = enabled; }
    bool isEnabled() const override { return m_enabled; }
    void update(float32 deltaTime) override {}
    
protected:
    UIElementType m_type;
    std::string m_id;
    Vec2 m_position;
    Vec2 m_size;
    bool m_visible;
    bool m_enabled;
};

/**
 * @brief Label implementation
 */
class UILabel : public IUILabel {
public:
    UILabel(const std::string& id, const std::string& text = "")
        : m_type(UIElementType::Label)
        , m_id(id)
        , m_position(0, 0)
        , m_size(0, 0)
        , m_visible(true)
        , m_enabled(true)
        , m_text(text)
        , m_fontSize(16.0f)
    {
    }
    
    UIElementType getType() const override { return m_type; }
    std::string getId() const override { return m_id; }
    void setPosition(Vec2 position) override { m_position = position; }
    Vec2 getPosition() const override { return m_position; }
    void setSize(Vec2 size) override { m_size = size; }
    Vec2 getSize() const override { return m_size; }
    void setVisible(bool visible) override { m_visible = visible; }
    bool isVisible() const override { return m_visible; }
    void setEnabled(bool enabled) override { m_enabled = enabled; }
    bool isEnabled() const override { return m_enabled; }
    void update(float32 deltaTime) override {}
    
    void setText(const std::string& text) override { m_text = text; }
    std::string getText() const override { return m_text; }
    void setFontSize(float32 size) override { m_fontSize = size; }
    float32 getFontSize() const override { return m_fontSize; }
    
private:
    UIElementType m_type;
    std::string m_id;
    Vec2 m_position;
    Vec2 m_size;
    bool m_visible;
    bool m_enabled;
    std::string m_text;
    float32 m_fontSize;
};

/**
 * @brief Button implementation
 */
class UIButton : public IUIButton {
public:
    UIButton(const std::string& id, const std::string& text = "")
        : m_type(UIElementType::Button)
        , m_id(id)
        , m_position(0, 0)
        , m_size(0, 0)
        , m_visible(true)
        , m_enabled(true)
        , m_text(text)
        , m_hovered(false)
        , m_callback(nullptr)
    {
    }
    
    UIElementType getType() const override { return m_type; }
    std::string getId() const override { return m_id; }
    void setPosition(Vec2 position) override { m_position = position; }
    Vec2 getPosition() const override { return m_position; }
    void setSize(Vec2 size) override { m_size = size; }
    Vec2 getSize() const override { return m_size; }
    void setVisible(bool visible) override { m_visible = visible; }
    bool isVisible() const override { return m_visible; }
    void setEnabled(bool enabled) override { m_enabled = enabled; }
    bool isEnabled() const override { return m_enabled; }
    void update(float32 deltaTime) override {}
    
    void setText(const std::string& text) override { m_text = text; }
    std::string getText() const override { return m_text; }
    void setOnClick(std::function<void()> callback) override { m_callback = callback; }
    void click() override {
        if (m_enabled && m_callback) {
            m_callback();
        }
    }
    bool isHovered() const override { return m_hovered; }
    
private:
    UIElementType m_type;
    std::string m_id;
    Vec2 m_position;
    Vec2 m_size;
    bool m_visible;
    bool m_enabled;
    std::string m_text;
    bool m_hovered;
    std::function<void()> m_callback;
};

/**
 * @brief Progress bar implementation
 */
class UIProgressBar : public IUIProgressBar {
public:
    UIProgressBar(const std::string& id)
        : m_type(UIElementType::ProgressBar)
        , m_id(id)
        , m_position(0, 0)
        , m_size(0, 0)
        , m_visible(true)
        , m_enabled(true)
        , m_progress(0.0f)
        , m_color(0x00FF00)
    {
    }
    
    UIElementType getType() const override { return m_type; }
    std::string getId() const override { return m_id; }
    void setPosition(Vec2 position) override { m_position = position; }
    Vec2 getPosition() const override { return m_position; }
    void setSize(Vec2 size) override { m_size = size; }
    Vec2 getSize() const override { return m_size; }
    void setVisible(bool visible) override { m_visible = visible; }
    bool isVisible() const override { return m_visible; }
    void setEnabled(bool enabled) override { m_enabled = enabled; }
    bool isEnabled() const override { return m_enabled; }
    void update(float32 deltaTime) override {}
    
    void setProgress(float32 progress) override { 
        m_progress = std::clamp(progress, 0.0f, 1.0f); 
    }
    float32 getProgress() const override { return m_progress; }
    void setColor(uint32 color) override { m_color = color; }
    
private:
    UIElementType m_type;
    std::string m_id;
    Vec2 m_position;
    Vec2 m_size;
    bool m_visible;
    bool m_enabled;
    float32 m_progress;
    uint32 m_color;
};

/**
 * @brief HUD implementation
 */
class HUD : public IHUD {
public:
    HUD()
        : m_visible(true)
        , m_healthLabel(new UILabel("health", "HP: 100/100"))
        , m_coinLabel(new UILabel("coins", "Coins: 0"))
        , m_timeLabel(new UILabel("time", "Time: 0:00"))
        , m_levelLabel(new UILabel("level", "Level: 1"))
        , m_healthBar(new UIProgressBar("healthBar"))
    {
        m_healthLabel->setPosition(Vec2(10, 10));
        m_coinLabel->setPosition(Vec2(10, 30));
        m_timeLabel->setPosition(Vec2(10, 50));
        m_levelLabel->setPosition(Vec2(10, 70));
        m_healthBar->setPosition(Vec2(10, 90));
        m_healthBar->setSize(Vec2(200, 20));
    }
    
    ~HUD() {
        delete m_healthLabel;
        delete m_coinLabel;
        delete m_timeLabel;
        delete m_levelLabel;
        delete m_healthBar;
    }
    
    void update(float32 deltaTime) override {
        if (m_visible) {
            m_healthLabel->update(deltaTime);
            m_coinLabel->update(deltaTime);
            m_timeLabel->update(deltaTime);
            m_levelLabel->update(deltaTime);
            m_healthBar->update(deltaTime);
        }
    }
    
    void setVisible(bool visible) override { m_visible = visible; }
    bool isVisible() const override { return m_visible; }
    
    void updateHealth(int32 current, int32 max) override {
        m_healthLabel->setText("HP: " + std::to_string(current) + "/" + std::to_string(max));
        m_healthBar->setProgress(max > 0 ? static_cast<float32>(current) / max : 0.0f);
    }
    
    void updateCoins(int32 count) override {
        m_coinLabel->setText("Coins: " + std::to_string(count));
    }
    
    void updateTime(float32 time) override {
        int32 minutes = static_cast<int32>(time) / 60;
        int32 seconds = static_cast<int32>(time) % 60;
        m_timeLabel->setText("Time: " + std::to_string(minutes) + ":" + 
                           (seconds < 10 ? "0" : "") + std::to_string(seconds));
    }
    
    void updateLevel(int32 level) override {
        m_levelLabel->setText("Level: " + std::to_string(level));
    }
    
    void showMessage(const std::string& message, float32 duration) override {
    }
    
private:
    bool m_visible;
    UILabel* m_healthLabel;
    UILabel* m_coinLabel;
    UILabel* m_timeLabel;
    UILabel* m_levelLabel;
    UIProgressBar* m_healthBar;
};

/**
 * @brief Menu implementation
 */
class Menu : public IMenu {
public:
    Menu(const std::string& id)
        : m_id(id)
        , m_visible(false)
    {
    }
    
    void show() override { m_visible = true; }
    void hide() override { m_visible = false; }
    bool isVisible() const override { return m_visible; }
    
    void update(float32 deltaTime) override {
        if (m_visible) {
            for (auto& button : m_buttons) {
                button->update(deltaTime);
            }
        }
    }
    
    void addButton(const std::string& id, const std::string& text,
                   Vec2 position, std::function<void()> callback) override {
        auto button = new UIButton(id, text);
        button->setPosition(position);
        button->setOnClick(callback);
        m_buttons.push_back(button);
    }
    
    IUIButton* getButton(const std::string& id) override {
        for (auto& button : m_buttons) {
            if (button->getId() == id) {
                return button;
            }
        }
        return nullptr;
    }
    
    ~Menu() {
        for (auto& button : m_buttons) {
            delete button;
        }
    }
    
private:
    std::string m_id;
    bool m_visible;
    std::vector<IUIButton*> m_buttons;
};

/**
 * @brief UI Manager implementation
 */
class UIManager : public IUIManager {
public:
    UIManager()
        : m_hud(new HUD())
        , m_mainMenu(new Menu("main"))
        , m_pauseMenu(new Menu("pause"))
        , m_settingsMenu(new Menu("settings"))
    {
        m_mainMenu->addButton("play", "Play", Vec2(100, 100), []() {});
        m_mainMenu->addButton("settings", "Settings", Vec2(100, 160), []() {});
        m_mainMenu->addButton("quit", "Quit", Vec2(100, 220), []() {});
        
        m_pauseMenu->addButton("resume", "Resume", Vec2(100, 100), []() {});
        m_pauseMenu->addButton("restart", "Restart", Vec2(100, 160), []() {});
        m_pauseMenu->addButton("quit", "Quit", Vec2(100, 220), []() {});
    }
    
    ~UIManager() {
        delete m_hud;
        delete m_mainMenu;
        delete m_pauseMenu;
        delete m_settingsMenu;
    }
    
    void update(float32 deltaTime) override {
        m_hud->update(deltaTime);
        m_mainMenu->update(deltaTime);
        m_pauseMenu->update(deltaTime);
        m_settingsMenu->update(deltaTime);
    }
    
    IHUD* getHUD() override { return m_hud; }
    IMenu* getMainMenu() override { return m_mainMenu; }
    IMenu* getPauseMenu() override { return m_pauseMenu; }
    IMenu* getSettingsMenu() override { return m_settingsMenu; }
    
    void showMainMenu(bool show) override {
        if (show) m_mainMenu->show();
        else m_mainMenu->hide();
    }
    
    void showPauseMenu(bool show) override {
        if (show) m_pauseMenu->show();
        else m_pauseMenu->hide();
    }
    
    void showHUD(bool show) override {
        m_hud->setVisible(show);
    }
    
private:
    IHUD* m_hud;
    IMenu* m_mainMenu;
    IMenu* m_pauseMenu;
    IMenu* m_settingsMenu;
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
