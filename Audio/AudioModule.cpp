#include "IAudio.h"
#include "../Engine/IModule.h"
#include "../Utilities/ILogger.h"

namespace APLG {

/**
 * @brief Audio module implementation
 * 
 * Manages audio playback and sound library.
 */
class AudioModule : public ModuleBase {
public:
    AudioModule() : ModuleBase("Audio") {}
    
    /**
     * @brief Get the audio manager instance
     */
    AudioManager* getAudioManager() {
        return &m_audioManager;
    }
    
    /**
     * @brief Get the sound library instance
     */
    SoundLibrary* getSoundLibrary() {
        return &m_soundLibrary;
    }
    
protected:
    bool onInitialize() override {
        LOG_INFO("Audio module initializing...");
        
        // Initialize audio manager
        if (!m_audioManager.initialize()) {
            LOG_ERROR("Failed to initialize audio manager");
            return false;
        }
        
        LOG_INFO("Audio module initialized");
        return true;
    }
    
    void onShutdown() override {
        LOG_INFO("Audio module shutting down...");
        m_audioManager.shutdown();
        LOG_INFO("Audio module shutdown complete");
    }
    
    void update(float32 deltaTime) override {
        if (isInitialized()) {
            m_audioManager.update(deltaTime);
        }
    }
    
private:
    AudioManager m_audioManager;
    SoundLibrary m_soundLibrary;
};

// Global audio module instance
static AudioModule* g_audioModule = nullptr;

/**
 * @brief Get the global audio module
 */
AudioModule* getAudioModule() {
    return g_audioModule;
}

// Module registration function
extern "C" {
    void registerAudioModule() {
        auto module = std::make_shared<AudioModule>();
        g_audioModule = module.get();
        ModuleManager::instance().registerModule(module);
    }
}

} // namespace APLG
