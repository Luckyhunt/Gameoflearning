#include "AssetManager.h"
#include "ILogger.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <mmsystem.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "winmm.lib")

namespace APLG {

namespace fs = std::filesystem;

AssetManager& AssetManager::instance() {
    static AssetManager inst;
    return inst;
}

AssetManager::~AssetManager() {
    shutdown();
}

std::string AssetManager::findAssetRoot() const {
    std::vector<std::string> searchPaths = {
        "brackeys_platformer_assets",
        "../brackeys_platformer_assets",
        "../../brackeys_platformer_assets",
        "D:/testing/Adaptive Procedural Level Generation/brackeys_platformer_assets"
    };

    for (const auto& path : searchPaths) {
        if (fs::exists(path) && fs::is_directory(path)) {
            return fs::canonical(path).string();
        }
    }
    return "";
}

std::string AssetManager::getAssetPath(const std::string& relativePath) const {
    if (m_assetRoot.empty()) return "";
    fs::path fullPath = fs::path(m_assetRoot) / relativePath;
    if (fs::exists(fullPath)) {
        return fullPath.string();
    }
    return "";
}

bool AssetManager::initialize() {
    if (m_initialized) return true;

    // Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::Status status = Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
    if (status != Gdiplus::Ok) {
        LOG_ERROR("AssetManager: Failed to initialize GDI+");
        return false;
    }

    m_assetRoot = findAssetRoot();
    if (m_assetRoot.empty()) {
        LOG_WARNING("AssetManager: brackeys_platformer_assets directory not found! Fallback rendering will be used.");
    } else {
        LOG_INFO("AssetManager: Asset root located at: " + m_assetRoot);
    }

    // Load textures if asset root exists
    if (!m_assetRoot.empty()) {
        auto loadTex = [this](const std::string& key, const std::string& relPath) {
            std::string fullPath = getAssetPath(relPath);
            if (!fullPath.empty()) {
                std::wstring wPath(fullPath.begin(), fullPath.end());
                auto bitmap = std::make_unique<Gdiplus::Bitmap>(wPath.c_str());
                if (bitmap && bitmap->GetLastStatus() == Gdiplus::Ok) {
                    m_textures[key] = std::move(bitmap);
                    LOG_INFO("AssetManager: Loaded texture " + key);
                } else {
                    LOG_WARNING("AssetManager: Failed to load texture " + relPath);
                }
            }
        };

        loadTex("knight", "sprites/knight.png");
        loadTex("world_tileset", "sprites/world_tileset.png");
        loadTex("platforms", "sprites/platforms.png");
        loadTex("slime_green", "sprites/slime_green.png");
        loadTex("slime_purple", "sprites/slime_purple.png");
        loadTex("coin", "sprites/coin.png");
        loadTex("fruit", "sprites/fruit.png");

        m_soundFiles[SoundEffect::Jump] = getAssetPath("sounds/jump.wav");
        m_soundFiles[SoundEffect::Coin] = getAssetPath("sounds/coin.wav");
        m_soundFiles[SoundEffect::Hurt] = getAssetPath("sounds/hurt.wav");
        m_soundFiles[SoundEffect::Explosion] = getAssetPath("sounds/explosion.wav");
        m_soundFiles[SoundEffect::PowerUp] = getAssetPath("sounds/power_up.wav");
        m_soundFiles[SoundEffect::Tap] = getAssetPath("sounds/tap.wav");
        
        m_musicFile = getAssetPath("music/time_for_adventure.mp3");
    }

    m_initialized = true;
    return true;
}

void AssetManager::shutdown() {
    if (!m_initialized) return;
    stopMusic();
    m_textures.clear();
    if (m_gdiplusToken != 0) {
        Gdiplus::GdiplusShutdown(m_gdiplusToken);
        m_gdiplusToken = 0;
    }
    m_initialized = false;
}

Gdiplus::Bitmap* AssetManager::getTexture(const std::string& key) {
    auto it = m_textures.find(key);
    if (it != m_textures.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool AssetManager::drawSprite(HDC hdc, const std::string& key, int destX, int destY, int destW, int destH, int srcX, int srcY, int srcW, int srcH, bool flipX) {
    Gdiplus::Bitmap* bmp = getTexture(key);
    if (!bmp) return false;

    Gdiplus::Graphics graphics(hdc);
    graphics.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

    if (flipX) {
        Gdiplus::Rect destRect(destX + destW, destY, -destW, destH);
        graphics.DrawImage(bmp, destRect, srcX, srcY, srcW, srcH, Gdiplus::UnitPixel);
    } else {
        Gdiplus::Rect destRect(destX, destY, destW, destH);
        graphics.DrawImage(bmp, destRect, srcX, srcY, srcW, srcH, Gdiplus::UnitPixel);
    }
    return true;
}

void AssetManager::playSound(SoundEffect effect) {
    if (!m_audioEnabled) return;
    bool isAction = (effect == SoundEffect::Jump || effect == SoundEffect::Hurt || effect == SoundEffect::Explosion);
    float effectiveVol = m_masterVolume * m_sfxVolume * (isAction ? m_actionVolume : 1.0f);
    effectiveVol = std::clamp(effectiveVol, 0.0f, 1.0f);
    if (effectiveVol <= 0.001f) return;

    auto it = m_soundFiles.find(effect);
    if (it != m_soundFiles.end() && !it->second.empty()) {
        WORD volWord = static_cast<WORD>(effectiveVol * 65535.0f);
        DWORD waveVol = MAKELONG(volWord, volWord);
        waveOutSetVolume(NULL, waveVol);
        PlaySoundA(it->second.c_str(), NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
    }
}

void AssetManager::startMusic() {
    if (m_musicFile.empty() || !m_audioEnabled) return;
    std::string cmdOpen = "open \"" + m_musicFile + "\" type mpegvideo alias bgm";
    mciSendStringA(cmdOpen.c_str(), NULL, 0, NULL);
    
    int mciVol = static_cast<int>(std::clamp(m_masterVolume * m_musicVolume, 0.0f, 1.0f) * 1000.0f);
    std::string cmdVol = "setaudio bgm volume to " + std::to_string(mciVol);
    mciSendStringA(cmdVol.c_str(), NULL, 0, NULL);
    mciSendStringA("play bgm repeat", NULL, 0, NULL);
}

void AssetManager::stopMusic() {
    mciSendStringA("stop bgm", NULL, 0, NULL);
    mciSendStringA("close bgm", NULL, 0, NULL);
}

void AssetManager::setMasterVolume(float vol) {
    m_masterVolume = std::clamp(vol, 0.0f, 1.0f);
    int mciVol = static_cast<int>(std::clamp(m_masterVolume * m_musicVolume, 0.0f, 1.0f) * 1000.0f);
    std::string cmdVol = "setaudio bgm volume to " + std::to_string(mciVol);
    mciSendStringA(cmdVol.c_str(), NULL, 0, NULL);
}

void AssetManager::setMusicVolume(float vol) {
    m_musicVolume = std::clamp(vol, 0.0f, 1.0f);
    int mciVol = static_cast<int>(std::clamp(m_masterVolume * m_musicVolume, 0.0f, 1.0f) * 1000.0f);
    std::string cmdVol = "setaudio bgm volume to " + std::to_string(mciVol);
    mciSendStringA(cmdVol.c_str(), NULL, 0, NULL);
}

void AssetManager::setSFXVolume(float vol) {
    m_sfxVolume = std::clamp(vol, 0.0f, 1.0f);
}

void AssetManager::setActionVolume(float vol) {
    m_actionVolume = std::clamp(vol, 0.0f, 1.0f);
}

void AssetManager::setAudioEnabled(bool enabled) {
    m_audioEnabled = enabled;
    if (!m_audioEnabled) {
        mciSendStringA("setaudio bgm volume to 0", NULL, 0, NULL);
    } else {
        int mciVol = static_cast<int>(std::clamp(m_masterVolume * m_musicVolume, 0.0f, 1.0f) * 1000.0f);
        std::string cmdVol = "setaudio bgm volume to " + std::to_string(mciVol);
        mciSendStringA(cmdVol.c_str(), NULL, 0, NULL);
    }
}

} // namespace APLG
