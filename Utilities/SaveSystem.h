#pragma once

#include "../Include/Common.h"
#include "Json.h"
#include <string>
#include <memory>

namespace APLG {

/**
 * @brief Save data structure
 */
struct SaveData {
    int32 currentLevel;
    int32 maxLevelUnlocked;
    int32 totalCoins;
    int32 highScores[5]; // High scores for each difficulty
    float32 totalPlayTime;
    int32 totalDeaths;
    int32 totalLevelsCompleted;
    std::vector<std::string> unlockedAchievements;
    DifficultyLevel currentDifficulty;
    Vec2 lastCheckpoint;
    
    SaveData()
        : currentLevel(1)
        , maxLevelUnlocked(1)
        , totalCoins(0)
        , totalPlayTime(0.0f)
        , totalDeaths(0)
        , totalLevelsCompleted(0)
        , currentDifficulty(DifficultyLevel::Normal)
        , lastCheckpoint(0, 0)
    {
        for (int32 i = 0; i < 5; ++i) {
            highScores[i] = 0;
        }
    }
};

/**
 * @brief Settings data structure
 */
struct SettingsData {
    float32 masterVolume;
    float32 musicVolume;
    float32 sfxVolume;
    bool fullscreen;
    int32 resolutionWidth;
    int32 resolutionHeight;
    bool vsync;
    bool showFPS;
    DifficultyLevel defaultDifficulty;
    
    SettingsData()
        : masterVolume(1.0f)
        , musicVolume(0.8f)
        , sfxVolume(0.8f)
        , fullscreen(false)
        , resolutionWidth(1920)
        , resolutionHeight(1080)
        , vsync(true)
        , showFPS(false)
        , defaultDifficulty(DifficultyLevel::Normal)
    {
    }
};

/**
 * @brief Save system interface
 */
class ISaveSystem {
public:
    virtual ~ISaveSystem() = default;
    
    /**
     * @brief Save game data
     */
    virtual bool saveGame(const SaveData& data, const std::string& slot = "auto") = 0;
    
    /**
     * @brief Load game data
     */
    virtual bool loadGame(SaveData& data, const std::string& slot = "auto") = 0;
    
    /**
     * @brief Save settings
     */
    virtual bool saveSettings(const SettingsData& settings) = 0;
    
    /**
     * @brief Load settings
     */
    virtual bool loadSettings(SettingsData& settings) = 0;
    
    /**
     * @brief Check if save exists
     */
    virtual bool saveExists(const std::string& slot = "auto") = 0;
    
    /**
     * @brief Delete save
     */
    virtual bool deleteSave(const std::string& slot = "auto") = 0;
    
    /**
     * @brief Get all save slots
     */
    virtual std::vector<std::string> getSaveSlots() = 0;
    
    /**
     * @brief Quick save
     */
    virtual bool quickSave(const SaveData& data) = 0;
    
    /**
     * @brief Quick load
     */
    virtual bool quickLoad(SaveData& data) = 0;
};

/**
 * @brief JSON-based save system
 */
class SaveSystem : public ISaveSystem {
public:
    SaveSystem();
    
    bool saveGame(const SaveData& data, const std::string& slot = "auto") override;
    bool loadGame(SaveData& data, const std::string& slot = "auto") override;
    bool saveSettings(const SettingsData& settings) override;
    bool loadSettings(SettingsData& settings) override;
    bool saveExists(const std::string& slot = "auto") override;
    bool deleteSave(const std::string& slot = "auto") override;
    std::vector<std::string> getSaveSlots() override;
    bool quickSave(const SaveData& data) override;
    bool quickLoad(SaveData& data) override;
    
    /**
     * @brief Set save directory
     */
    void setSaveDirectory(const std::string& directory);
    
    /**
     * @brief Get save directory
     */
    std::string getSaveDirectory() const;
    
private:
    std::string getSaveFilePath(const std::string& slot);
    std::string getSettingsFilePath();
    std::string serializeSaveData(const SaveData& data);
    SaveData deserializeSaveData(const std::string& json);
    std::string serializeSettings(const SettingsData& settings);
    SettingsData deserializeSettings(const std::string& json);
    
    std::string m_saveDirectory;
};

/**
 * @brief Auto-save manager
 * 
 * Automatically saves at checkpoints and level completion.
 */
class AutoSaveManager {
public:
    AutoSaveManager(SaveSystem* saveSystem);
    
    /**
     * @brief Enable/disable auto-save
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Trigger auto-save
     */
    void triggerAutoSave(const SaveData& data);
    
    /**
     * @brief Set auto-save interval (in seconds)
     */
    void setInterval(float32 interval);
    
    /**
     * @brief Update auto-save timer
     */
    void update(float32 deltaTime);
    
private:
    SaveSystem* m_saveSystem;
    bool m_enabled;
    float32 m_interval;
    float32 m_timer;
};

} // namespace APLG
