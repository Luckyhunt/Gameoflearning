#include "IEntity.h"
#include "../Engine/IModule.h"
#include "../Utilities/ILogger.h"

namespace APLG {

/**
 * @brief Gameplay module implementation
 * 
 * Manages game entities (enemies, collectables, powerups).
 */
class GameplayModule : public ModuleBase {
public:
    GameplayModule() : ModuleBase("Gameplay") {}
    
    /**
     * @brief Get the entity manager instance
     */
    EntityManager* getEntityManager() {
        return &m_entityManager;
    }
    
protected:
    bool onInitialize() override {
        LOG_INFO("Gameplay module initializing...");
        
        // Initialize entity manager
        LOG_INFO("Gameplay module initialized");
        return true;
    }
    
    void onShutdown() override {
        LOG_INFO("Gameplay module shutting down...");
        m_entityManager.clear();
        LOG_INFO("Gameplay module shutdown complete");
    }
    
    void update(float32 deltaTime) override {
        if (isInitialized()) {
            m_entityManager.update(deltaTime);
        }
    }
    
private:
    EntityManager m_entityManager;
};

// Global gameplay module instance
static GameplayModule* g_gameplayModule = nullptr;

/**
 * @brief Get the global gameplay module
 */
GameplayModule* getGameplayModule() {
    return g_gameplayModule;
}

// Module registration function
extern "C" {
    void registerGameplayModule() {
        auto module = std::make_shared<GameplayModule>();
        g_gameplayModule = module.get();
        ModuleManager::instance().registerModule(module);
    }
}

} // namespace APLG
