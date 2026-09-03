#pragma once

#define NOMINMAX
#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <unordered_map>
#include <memory>

namespace APLG {

enum class SoundEffect {
    Jump,
    Coin,
    Hurt,
    Explosion,
    PowerUp,
    Tap
};

class AssetManager {
public:
    static AssetManager& instance();

    bool initialize();
    void shutdown();

    Gdiplus::Bitmap* getTexture(const std::string& key);
    bool drawSprite(HDC hdc, const std::string& key, int destX, int destY, int destW, int destH, int srcX, int srcY, int srcW, int srcH, bool flipX = false);
    
    void playSound(SoundEffect effect);
    void startMusic();
    void stopMusic();

    void setMasterVolume(float vol);
    float getMasterVolume() const { return m_masterVolume; }
    void setMusicVolume(float vol);
    float getMusicVolume() const { return m_musicVolume; }
    void setSFXVolume(float vol);
    float getSFXVolume() const { return m_sfxVolume; }
    void setActionVolume(float vol);
    float getActionVolume() const { return m_actionVolume; }
    void setAudioEnabled(bool enabled);
    bool isAudioEnabled() const { return m_audioEnabled; }

    bool isLoaded() const { return m_initialized; }
    std::string getAssetPath(const std::string& relativePath) const;

private:
    AssetManager() = default;
    ~AssetManager();
    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    std::string findAssetRoot() const;

    ULONG_PTR m_gdiplusToken = 0;
    bool m_initialized = false;
    std::string m_assetRoot;

    float m_masterVolume = 1.0f;
    float m_musicVolume = 0.8f;
    float m_sfxVolume = 0.8f;
    float m_actionVolume = 0.9f;
    bool m_audioEnabled = true;

    std::unordered_map<std::string, std::unique_ptr<Gdiplus::Bitmap>> m_textures;
    std::unordered_map<SoundEffect, std::string> m_soundFiles;
    std::string m_musicFile;
};

} // namespace APLG
