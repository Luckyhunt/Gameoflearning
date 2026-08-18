#include "IUI.h"
#include "../Utilities/ILogger.h"
#include <algorithm>

namespace APLG {

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
    void update(float32 deltaTime) override { /* Base implementation */ }
    
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
    
    // IUIElement methods
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
    void update(float32 deltaTime) override { /* Base implementation */ }
    
    // IUILabel methods
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
    
    // IUIElement methods
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
    void update(float32 deltaTime) override { /* Base implementation */ }
    
    // IUIButton methods
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
    
    // IUIElement methods
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
    void update(float32 deltaTime) override { /* Base implementation */ }
    
    // IUIProgressBar methods
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
        // Position HUD elements
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
        LOG_INFO("HUD Message: " + message + " (duration: " + std::to_string(duration) + "s)");
        // Would show temporary message overlay
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
    
    void show() override { 
        m_visible = true; 
        LOG_INFO("Menu shown: " + m_id);
    }
    
    void hide() override { 
        m_visible = false; 
        LOG_INFO("Menu hidden: " + m_id);
    }
    
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
        // Setup main menu buttons
        m_mainMenu->addButton("play", "Play", Vec2(100, 100), []() {
            LOG_INFO("Play button clicked");
        });
        m_mainMenu->addButton("settings", "Settings", Vec2(100, 160), []() {
            LOG_INFO("Settings button clicked");
        });
        m_mainMenu->addButton("quit", "Quit", Vec2(100, 220), []() {
            LOG_INFO("Quit button clicked");
        });
        
        // Setup pause menu buttons
        m_pauseMenu->addButton("resume", "Resume", Vec2(100, 100), []() {
            LOG_INFO("Resume button clicked");
        });
        m_pauseMenu->addButton("restart", "Restart", Vec2(100, 160), []() {
            LOG_INFO("Restart button clicked");
        });
        m_pauseMenu->addButton("quit", "Quit", Vec2(100, 220), []() {
            LOG_INFO("Quit button clicked");
        });
        
        LOG_INFO("UI Manager initialized");
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
        if (show) {
            m_mainMenu->show();
        } else {
            m_mainMenu->hide();
        }
    }
    
    void showPauseMenu(bool show) override {
        if (show) {
            m_pauseMenu->show();
        } else {
            m_pauseMenu->hide();
        }
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
static UIManager* g_uiManager = nullptr;

IUIManager* getUIManager() {
    if (!g_uiManager) {
        g_uiManager = new UIManager();
    }
    return g_uiManager;
}

/**
 * @brief Cleanup UI manager
 */
void cleanupUIManager() {
    if (g_uiManager) {
        delete g_uiManager;
        g_uiManager = nullptr;
    }
}

} // namespace APLG
