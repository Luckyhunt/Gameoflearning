#include "SaveSystem.h"
#include "ILogger.h"
#include <filesystem>
#include <fstream>

namespace APLG {

// SaveSystem Implementation
SaveSystem::SaveSystem()
    : m_saveDirectory("saves")
{
    // Create save directory if it doesn't exist
    std::filesystem::create_directories(m_saveDirectory);
    
    LOG_INFO("Save system initialized with directory: " + m_saveDirectory);
}

bool SaveSystem::saveGame(const SaveData& data, const std::string& slot) {
    std::string filepath = getSaveFilePath(slot);
    std::string json = serializeSaveData(data);
    
    if (JsonWriter::writeToFile(filepath, json)) {
        LOG_INFO("Game saved to slot: " + slot);
        return true;
    }
    
    LOG_ERROR("Failed to save game to slot: " + slot);
    return false;
}

bool SaveSystem::loadGame(SaveData& data, const std::string& slot) {
    std::string filepath = getSaveFilePath(slot);
    
    if (!std::filesystem::exists(filepath)) {
        LOG_WARNING("Save file does not exist: " + filepath);
        return false;
    }
    
    JsonValue json = JsonParser::parseFile(filepath);
    if (json.isNull()) {
        LOG_ERROR("Failed to parse save file: " + filepath);
        return false;
    }
    
    data = deserializeSaveData(json.serialize());
    LOG_INFO("Game loaded from slot: " + slot);
    return true;
}

bool SaveSystem::saveSettings(const SettingsData& settings) {
    std::string filepath = getSettingsFilePath();
    std::string json = serializeSettings(settings);
    
    if (JsonWriter::writeToFile(filepath, json)) {
        LOG_INFO("Settings saved");
        return true;
    }
    
    LOG_ERROR("Failed to save settings");
    return false;
}

bool SaveSystem::loadSettings(SettingsData& settings) {
    std::string filepath = getSettingsFilePath();
    
    if (!std::filesystem::exists(filepath)) {
        LOG_WARNING("Settings file does not exist, using defaults");
        return false;
    }
    
    JsonValue json = JsonParser::parseFile(filepath);
    if (json.isNull()) {
        LOG_ERROR("Failed to parse settings file");
        return false;
    }
    
    settings = deserializeSettings(json.serialize());
    LOG_INFO("Settings loaded");
    return true;
}

bool SaveSystem::saveExists(const std::string& slot) {
    return std::filesystem::exists(getSaveFilePath(slot));
}

bool SaveSystem::deleteSave(const std::string& slot) {
    std::string filepath = getSaveFilePath(slot);
    
    if (std::filesystem::remove(filepath)) {
        LOG_INFO("Save deleted: " + slot);
        return true;
    }
    
    LOG_WARNING("Failed to delete save: " + slot);
    return false;
}

std::vector<std::string> SaveSystem::getSaveSlots() {
    std::vector<std::string> slots;
    
    if (!std::filesystem::exists(m_saveDirectory)) {
        return slots;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(m_saveDirectory)) {
        if (entry.path().extension() == ".json") {
            std::string filename = entry.path().stem().string();
            if (filename != "settings") {
                slots.push_back(filename);
            }
        }
    }
    
    return slots;
}

bool SaveSystem::quickSave(const SaveData& data) {
    return saveGame(data, "quicksave");
}

bool SaveSystem::quickLoad(SaveData& data) {
    return loadGame(data, "quicksave");
}

void SaveSystem::setSaveDirectory(const std::string& directory) {
    m_saveDirectory = directory;
    std::filesystem::create_directories(m_saveDirectory);
}

std::string SaveSystem::getSaveDirectory() const {
    return m_saveDirectory;
}

std::string SaveSystem::getSaveFilePath(const std::string& slot) {
    return m_saveDirectory + "/" + slot + ".json";
}

std::string SaveSystem::getSettingsFilePath() {
    return m_saveDirectory + "/settings.json";
}

std::string SaveSystem::serializeSaveData(const SaveData& data) {
    JsonValue json;
    json["currentLevel"] = JsonValue(data.currentLevel);
    json["maxLevelUnlocked"] = JsonValue(data.maxLevelUnlocked);
    json["totalCoins"] = JsonValue(data.totalCoins);
    json["totalPlayTime"] = JsonValue(data.totalPlayTime);
    json["totalDeaths"] = JsonValue(data.totalDeaths);
    json["totalLevelsCompleted"] = JsonValue(data.totalLevelsCompleted);
    json["currentDifficulty"] = JsonValue(static_cast<int>(data.currentDifficulty));
    json["lastCheckpointX"] = JsonValue(data.lastCheckpoint.x);
    json["lastCheckpointY"] = JsonValue(data.lastCheckpoint.y);
    
    JsonValue::Array highScores;
    for (int32 i = 0; i < 5; ++i) {
        highScores.push_back(JsonValue(data.highScores[i]));
    }
    json["highScores"] = JsonValue(highScores);
    
    JsonValue::Array achievements;
    for (const auto& achievement : data.unlockedAchievements) {
        achievements.push_back(JsonValue(achievement));
    }
    json["achievements"] = JsonValue(achievements);
    
    return json.serialize(2);
}

SaveData SaveSystem::deserializeSaveData(const std::string& json) {
    SaveData data;
    JsonValue parsed = JsonParser::parse(json);
    
    data.currentLevel = static_cast<int32>(parsed["currentLevel"].getNumber());
    data.maxLevelUnlocked = static_cast<int32>(parsed["maxLevelUnlocked"].getNumber());
    data.totalCoins = static_cast<int32>(parsed["totalCoins"].getNumber());
    data.totalPlayTime = parsed["totalPlayTime"].getNumber();
    data.totalDeaths = static_cast<int32>(parsed["totalDeaths"].getNumber());
    data.totalLevelsCompleted = static_cast<int32>(parsed["totalLevelsCompleted"].getNumber());
    data.currentDifficulty = static_cast<DifficultyLevel>(parsed["currentDifficulty"].getNumber());
    data.lastCheckpoint.x = parsed["lastCheckpointX"].getNumber();
    data.lastCheckpoint.y = parsed["lastCheckpointY"].getNumber();
    
    JsonValue::Array highScores = parsed["highScores"].getArray();
    for (size_t i = 0; i < highScores.size() && i < 5; ++i) {
        data.highScores[i] = static_cast<int32>(highScores[i].getNumber());
    }
    
    JsonValue::Array achievements = parsed["achievements"].getArray();
    for (const auto& achievement : achievements) {
        data.unlockedAchievements.push_back(achievement.getString());
    }
    
    return data;
}

std::string SaveSystem::serializeSettings(const SettingsData& settings) {
    JsonValue json;
    json["masterVolume"] = JsonValue(settings.masterVolume);
    json["musicVolume"] = JsonValue(settings.musicVolume);
    json["sfxVolume"] = JsonValue(settings.sfxVolume);
    json["actionVolume"] = JsonValue(settings.actionVolume);
    json["audioEnabled"] = JsonValue(settings.audioEnabled);
    json["fullscreen"] = JsonValue(settings.fullscreen);
    json["resolutionWidth"] = JsonValue(settings.resolutionWidth);
    json["resolutionHeight"] = JsonValue(settings.resolutionHeight);
    json["vsync"] = JsonValue(settings.vsync);
    json["showFPS"] = JsonValue(settings.showFPS);
    json["defaultDifficulty"] = JsonValue(static_cast<int>(settings.defaultDifficulty));
    
    return json.serialize(2);
}

SettingsData SaveSystem::deserializeSettings(const std::string& json) {
    SettingsData settings;
    JsonValue parsed = JsonParser::parse(json);
    
    if (parsed.hasKey("masterVolume")) settings.masterVolume = static_cast<float32>(parsed["masterVolume"].getNumber());
    if (parsed.hasKey("musicVolume")) settings.musicVolume = static_cast<float32>(parsed["musicVolume"].getNumber());
    if (parsed.hasKey("sfxVolume")) settings.sfxVolume = static_cast<float32>(parsed["sfxVolume"].getNumber());
    if (parsed.hasKey("actionVolume")) settings.actionVolume = static_cast<float32>(parsed["actionVolume"].getNumber());
    if (parsed.hasKey("audioEnabled")) settings.audioEnabled = parsed["audioEnabled"].getBool();
    if (parsed.hasKey("fullscreen")) settings.fullscreen = parsed["fullscreen"].getBool();
    if (parsed.hasKey("resolutionWidth")) settings.resolutionWidth = static_cast<int32>(parsed["resolutionWidth"].getNumber());
    if (parsed.hasKey("resolutionHeight")) settings.resolutionHeight = static_cast<int32>(parsed["resolutionHeight"].getNumber());
    if (parsed.hasKey("vsync")) settings.vsync = parsed["vsync"].getBool();
    if (parsed.hasKey("showFPS")) settings.showFPS = parsed["showFPS"].getBool();
    if (parsed.hasKey("defaultDifficulty")) settings.defaultDifficulty = static_cast<DifficultyLevel>(static_cast<int>(parsed["defaultDifficulty"].getNumber()));
    
    return settings;
}

// AutoSaveManager Implementation
AutoSaveManager::AutoSaveManager(SaveSystem* saveSystem)
    : m_saveSystem(saveSystem)
    , m_enabled(true)
    , m_interval(60.0f)
    , m_timer(0.0f)
{
}

void AutoSaveManager::setEnabled(bool enabled) {
    m_enabled = enabled;
}

void AutoSaveManager::triggerAutoSave(const SaveData& data) {
    if (m_enabled && m_saveSystem) {
        m_saveSystem->quickSave(data);
        LOG_INFO("Auto-save triggered");
    }
}

void AutoSaveManager::setInterval(float32 interval) {
    m_interval = interval;
}

void AutoSaveManager::update(float32 deltaTime) {
    if (!m_enabled) return;
    
    m_timer += deltaTime;
    if (m_timer >= m_interval) {
        m_timer = 0.0f;
        // Would trigger auto-save with current game data
        LOG_INFO("Auto-save interval reached");
    }
}

} // namespace APLG
