#include "IPlayerModel.h"
#include "../Engine/IModule.h"
#include "../Utilities/ILogger.h"

namespace APLG {

/**
 * @brief AI module implementation
 * 
 * Manages player behavior analytics and playstyle detection.
 */
class AIModule : public ModuleBase {
public:
    AIModule() : ModuleBase("AI") {}
    
    /**
     * @brief Get the player model instance
     */
    PlayerModel* getPlayerModel() {
        return &m_playerModel;
    }
    
    /**
     * @brief Get the session analytics instance
     */
    SessionAnalytics* getSessionAnalytics() {
        return &m_sessionAnalytics;
    }
    
protected:
    bool onInitialize() override {
        LOG_INFO("AI module initializing...");
        
        // Initialize player model
        m_playerModel.setAnalysisWindow(100);
        
        // Start session
        m_sessionAnalytics.startSession();
        
        LOG_INFO("AI module initialized");
        return true;
    }
    
    void onShutdown() override {
        LOG_INFO("AI module shutting down...");
        
        // End session
        m_sessionAnalytics.endSession();
        
        LOG_INFO("AI module shutdown complete");
    }
    
private:
    PlayerModel m_playerModel;
    SessionAnalytics m_sessionAnalytics;
};

// Global AI module instance
static AIModule* g_aiModule = nullptr;

/**
 * @brief Get the global AI module
 */
AIModule* getAIModule() {
    return g_aiModule;
}

// Module registration function
extern "C" {
    void registerAIModule() {
        auto module = std::make_shared<AIModule>();
        g_aiModule = module.get();
        ModuleManager::instance().registerModule(module);
    }
}

} // namespace APLG
