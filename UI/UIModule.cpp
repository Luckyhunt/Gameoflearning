#include "IUI.h"
#include "../Engine/IModule.h"
#include "../Utilities/ILogger.h"

namespace APLG {

/**
 * @brief UI module implementation
 * 
 * Manages UI elements, HUD, and menus.
 */
class UIModule : public ModuleBase {
public:
    UIModule() : ModuleBase("UI") {}
    
    /**
     * @brief Get the UI manager instance
     */
    IUIManager* getUIManager() {
        return ::APLG::getUIManager();
    }
    
protected:
    bool onInitialize() override {
        LOG_INFO("UI module initializing...");
        
        // Initialize UI manager
        ::APLG::getUIManager();
        
        LOG_INFO("UI module initialized");
        return true;
    }
    
    void onShutdown() override {
        LOG_INFO("UI module shutting down...");
        ::APLG::cleanupUIManager();
        LOG_INFO("UI module shutdown complete");
    }
    
    void update(float32 deltaTime) override {
        if (isInitialized()) {
            auto uiManager = ::APLG::getUIManager();
            if (uiManager) {
                uiManager->update(deltaTime);
            }
        }
    }
};

// Global UI module instance
static UIModule* g_uiModule = nullptr;

/**
 * @brief Get the global UI module
 */
UIModule* getUIModule() {
    return g_uiModule;
}

// Module registration function
extern "C" {
    void registerUIModule() {
        auto module = std::make_shared<UIModule>();
        g_uiModule = module.get();
        ModuleManager::instance().registerModule(module);
    }
}

} // namespace APLG
