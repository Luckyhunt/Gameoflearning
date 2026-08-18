#pragma once

#include "../Include/Common.h"
#include <string>

namespace APLG {

/**
 * @brief Sound types
 */
enum class SoundType {
    Music,
    SFX,
    Ambient
};

/**
 * @brief Sound data structure
 */
struct SoundData {
    std::string filePath;
    float32 volume;
    float32 pitch;
    bool loop;
    bool spatial;
    Vec2 position; // For spatial audio
    float32 maxDistance; // For spatial audio falloff
    
    SoundData()
        : volume(1.0f)
        , pitch(1.0f)
        , loop(false)
        , spatial(false)
        , position(0, 0)
        , maxDistance(100.0f)
    {
    }
};

/**
 * @brief Audio interface
 * 
 * Provides audio playback functionality.
 */
class IAudioSystem {
public:
    virtual ~IAudioSystem() = default;
    
    /**
     * @brief Initialize audio system
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Shutdown audio system
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Play sound
     * @return Sound ID for control
     */
    virtual int32 playSound(const SoundData& sound, SoundType type = SoundType::SFX) = 0;
    
    /**
     * @brief Stop sound by ID
     */
    virtual void stopSound(int32 soundId) = 0;
    
    /**
     * @brief Stop all sounds of type
     */
    virtual void stopAllSounds(SoundType type) = 0;
    
    /**
     * @brief Stop all sounds
     */
    virtual void stopAll() = 0;
    
    /**
     * @brief Set master volume
     */
    virtual void setMasterVolume(float32 volume) = 0;
    
    /**
     * @brief Get master volume
     */
    virtual float32 getMasterVolume() const = 0;
    
    /**
     * @brief Set music volume
     */
    virtual void setMusicVolume(float32 volume) = 0;
    
    /**
     * @brief Get music volume
     */
    virtual float32 getMusicVolume() const = 0;
    
    /**
     * @brief Set SFX volume
     */
    virtual void setSFXVolume(float32 volume) = 0;
    
    /**
     * @brief Get SFX volume
     */
    virtual float32 getSFXVolume() const = 0;
    
    /**
     * @brief Pause audio
     */
    virtual void pause() = 0;
    
    /**
     * @brief Resume audio
     */
    virtual void resume() = 0;
    
    /**
     * @brief Update audio (for spatial audio, etc.)
     */
    virtual void update(float32 deltaTime) = 0;
    
    /**
     * @brief Check if sound is playing
     */
    virtual bool isPlaying(int32 soundId) const = 0;
};

/**
 * @brief Audio manager implementation
 * 
 * Manages audio playback with volume control.
 */
class AudioManager : public IAudioSystem {
public:
    AudioManager();
    
    bool initialize() override;
    void shutdown() override;
    int32 playSound(const SoundData& sound, SoundType type = SoundType::SFX) override;
    void stopSound(int32 soundId) override;
    void stopAllSounds(SoundType type) override;
    void stopAll() override;
    void setMasterVolume(float32 volume) override;
    float32 getMasterVolume() const override { return m_masterVolume; }
    void setMusicVolume(float32 volume) override;
    float32 getMusicVolume() const override { return m_musicVolume; }
    void setSFXVolume(float32 volume) override;
    float32 getSFXVolume() const override { return m_sfxVolume; }
    void pause() override;
    void resume() override;
    void update(float32 deltaTime) override;
    bool isPlaying(int32 soundId) const override;
    
    /**
     * @brief Set listener position (for spatial audio)
     */
    void setListenerPosition(Vec2 position);
    
    /**
     * @brief Get listener position
     */
    Vec2 getListenerPosition() const { return m_listenerPosition; }
    
private:
    float32 calculateSpatialVolume(const SoundData& sound);
    
    float32 m_masterVolume;
    float32 m_musicVolume;
    float32 m_sfxVolume;
    bool m_paused;
    int32 m_nextSoundId;
    Vec2 m_listenerPosition;
    
    // Would store active sounds in a real implementation
    // For now, this is a placeholder
};

/**
 * @brief Sound library
 * 
 * Manages sound assets and provides easy access.
 */
class SoundLibrary {
public:
    /**
     * @brief Register sound
     */
    void registerSound(const std::string& name, const std::string& filePath);
    
    /**
     * @brief Get sound data by name
     */
    SoundData getSound(const std::string& name) const;
    
    /**
     * @brief Check if sound exists
     */
    bool hasSound(const std::string& name) const;
    
    /**
     * @brief Load sounds from config file
     */
    bool loadFromConfig(const std::string& configPath);
    
private:
    std::map<std::string, SoundData> m_sounds;
};

} // namespace APLG
