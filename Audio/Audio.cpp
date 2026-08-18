#include "IAudio.h"
#include "../Utilities/ILogger.h"
#include "../Utilities/Json.h"
#include <algorithm>

namespace APLG {

// AudioManager Implementation
AudioManager::AudioManager()
    : m_masterVolume(1.0f)
    , m_musicVolume(0.8f)
    , m_sfxVolume(0.8f)
    , m_paused(false)
    , m_nextSoundId(1)
    , m_listenerPosition(0, 0)
{
}

bool AudioManager::initialize() {
    LOG_INFO("Audio Manager initializing...");
    
    // Would initialize audio backend (SDL_mixer, FMOD, etc.)
    // For now, this is a placeholder
    
    LOG_INFO("Audio Manager initialized");
    return true;
}

void AudioManager::shutdown() {
    LOG_INFO("Audio Manager shutting down...");
    stopAll();
    LOG_INFO("Audio Manager shutdown complete");
}

int32 AudioManager::playSound(const SoundData& sound, SoundType type) {
    if (m_paused) return -1;
    
    int32 soundId = m_nextSoundId++;
    
    // Calculate final volume based on type and master volume
    float32 finalVolume = sound.volume * m_masterVolume;
    if (type == SoundType::Music) {
        finalVolume *= m_musicVolume;
    } else if (type == SoundType::SFX) {
        finalVolume *= m_sfxVolume;
    }
    
    // Apply spatial audio if enabled
    if (sound.spatial) {
        finalVolume *= calculateSpatialVolume(sound);
    }
    
    // Would play sound using audio backend
    LOG_DEBUG("Playing sound: " + sound.filePath + " (ID: " + std::to_string(soundId) + 
             ", Volume: " + std::to_string(finalVolume) + ")");
    
    return soundId;
}

void AudioManager::stopSound(int32 soundId) {
    LOG_DEBUG("Stopping sound ID: " + std::to_string(soundId));
    // Would stop specific sound
}

void AudioManager::stopAllSounds(SoundType type) {
    std::string typeName = (type == SoundType::Music) ? "Music" : 
                          (type == SoundType::SFX) ? "SFX" : "Ambient";
    LOG_INFO("Stopping all " + typeName + " sounds");
    // Would stop all sounds of type
}

void AudioManager::stopAll() {
    LOG_INFO("Stopping all sounds");
    // Would stop all sounds
}

void AudioManager::setMasterVolume(float32 volume) {
    m_masterVolume = std::clamp(volume, 0.0f, 1.0f);
    LOG_INFO("Master volume set to: " + std::to_string(m_masterVolume));
}

void AudioManager::setMusicVolume(float32 volume) {
    m_musicVolume = std::clamp(volume, 0.0f, 1.0f);
    LOG_INFO("Music volume set to: " + std::to_string(m_musicVolume));
}

void AudioManager::setSFXVolume(float32 volume) {
    m_sfxVolume = std::clamp(volume, 0.0f, 1.0f);
    LOG_INFO("SFX volume set to: " + std::to_string(m_sfxVolume));
}

void AudioManager::pause() {
    m_paused = true;
    LOG_INFO("Audio paused");
}

void AudioManager::resume() {
    m_paused = false;
    LOG_INFO("Audio resumed");
}

void AudioManager::update(float32 deltaTime) {
    // Would update spatial audio, streaming, etc.
}

bool AudioManager::isPlaying(int32 soundId) const {
    // Would check if sound is still playing
    return false;
}

void AudioManager::setListenerPosition(Vec2 position) {
    m_listenerPosition = position;
}

float32 AudioManager::calculateSpatialVolume(const SoundData& sound) {
    float32 distance = (sound.position - m_listenerPosition).length();
    
    if (distance >= sound.maxDistance) {
        return 0.0f;
    }
    
    // Linear falloff
    return 1.0f - (distance / sound.maxDistance);
}

// SoundLibrary Implementation
void SoundLibrary::registerSound(const std::string& name, const std::string& filePath) {
    SoundData sound;
    sound.filePath = filePath;
    m_sounds[name] = sound;
    
    LOG_INFO("Registered sound: " + name + " -> " + filePath);
}

SoundData SoundLibrary::getSound(const std::string& name) const {
    auto it = m_sounds.find(name);
    if (it != m_sounds.end()) {
        return it->second;
    }
    
    LOG_WARNING("Sound not found: " + name);
    return SoundData();
}

bool SoundLibrary::hasSound(const std::string& name) const {
    return m_sounds.find(name) != m_sounds.end();
}

bool SoundLibrary::loadFromConfig(const std::string& configPath) {
    JsonValue json = JsonParser::parseFile(configPath);
    if (json.isNull()) {
        LOG_ERROR("Failed to load sound config: " + configPath);
        return false;
    }
    
    JsonValue::Object sounds = json.getObject();
    for (const auto& [name, value] : sounds) {
        JsonValue::Object soundData = value.getObject();
        SoundData sound;
        
        if (soundData.find("path") != soundData.end()) {
            sound.filePath = soundData.at("path").getString();
        }
        if (soundData.find("volume") != soundData.end()) {
            sound.volume = soundData.at("volume").getNumber();
        }
        if (soundData.find("pitch") != soundData.end()) {
            sound.pitch = soundData.at("pitch").getNumber();
        }
        if (soundData.find("loop") != soundData.end()) {
            sound.loop = soundData.at("loop").getBool();
        }
        
        m_sounds[name] = sound;
    }
    
    LOG_INFO("Loaded " + std::to_string(m_sounds.size()) + " sounds from config");
    return true;
}

} // namespace APLG
