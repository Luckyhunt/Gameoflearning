#include "IPlayer.h"
#include "../Engine/IModule.h"
#include "../Utilities/ILogger.h"

namespace APLG {

/**
 * @brief Player module implementation
 * 
 * Manages the player instance and provides global access.
 */
class PlayerModule : public ModuleBase {
public:
    PlayerModule() : ModuleBase("Player") {}
    
    /**
     * @brief Get the player instance
     */
    std::shared_ptr<IPlayer> getPlayer() {
        if (!m_player) {
            m_player = std::make_shared<Player>();
        }
        return m_player;
    }
    
    /**
     * @brief Set the player instance
     */
    void setPlayer(std::shared_ptr<IPlayer> player) {
        m_player = player;
    }
    
protected:
    bool onInitialize() override {
        LOG_INFO("Player module initializing...");
        
        // Create default player
        m_player = std::make_shared<Player>();
        
        LOG_INFO("Player module initialized");
        return true;
    }
    
    void onShutdown() override {
        LOG_INFO("Player module shutting down...");
        m_player.reset();
        LOG_INFO("Player module shutdown complete");
    }
    
    void update(float32 deltaTime) override {
        if (isInitialized() && m_player) {
            m_player->update(deltaTime);
        }
    }
    
private:
    std::shared_ptr<IPlayer> m_player;
};

// Global player module instance
static PlayerModule* g_playerModule = nullptr;

/**
 * @brief Get the global player module
 */
PlayerModule* getPlayerModule() {
    return g_playerModule;
}

// Module registration function
extern "C" {
    void registerPlayerModule() {
        auto module = std::make_shared<PlayerModule>();
        g_playerModule = module.get();
        ModuleManager::instance().registerModule(module);
    }
}

} // namespace APLG
