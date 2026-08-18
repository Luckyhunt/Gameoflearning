#include "IDifficultyManager.h"
#include "../Engine/IModule.h"
#include "../Utilities/ILogger.h"

namespace APLG {

/**
 * @brief Difficulty module implementation
 * 
 * Manages adaptive difficulty based on player performance.
 */
class DifficultyModule : public ModuleBase {
public:
    DifficultyModule() : ModuleBase("Difficulty") {}
    
    /**
     * @brief Get the difficulty manager instance
     */
    DifficultyManager* getManager() {
        return &m_manager;
    }
    
protected:
    bool onInitialize() override {
        LOG_INFO("Difficulty module initializing...");
        
        // Initialize with normal difficulty
        m_manager.setDifficulty(DifficultyLevel::Normal);
        m_manager.setAdaptive(true);
        
        LOG_INFO("Difficulty module initialized");
        return true;
    }
    
    void onShutdown() override {
        LOG_INFO("Difficulty module shutting down...");
        m_manager.clearHistory();
        LOG_INFO("Difficulty module shutdown complete");
    }
    
private:
    DifficultyManager m_manager;
};

// Global difficulty module instance
static DifficultyModule* g_difficultyModule = nullptr;

/**
 * @brief Get the global difficulty module
 */
DifficultyModule* getDifficultyModule() {
    return g_difficultyModule;
}

// Module registration function
extern "C" {
    void registerDifficultyModule() {
        auto module = std::make_shared<DifficultyModule>();
        g_difficultyModule = module.get();
        ModuleManager::instance().registerModule(module);
    }
}

} // namespace APLG
