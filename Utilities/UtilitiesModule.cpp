#include "ILogger.h"
#include "../Engine/IModule.h"

namespace APLG {

/**
 * @brief Utilities module implementation
 * 
 * This module provides logging, JSON parsing, and CSV handling
 * for the entire framework.
 */
class UtilitiesModule : public ModuleBase {
public:
    UtilitiesModule() : ModuleBase("Utilities") {}
    
protected:
    bool onInitialize() override {
        LOG_INFO("Utilities module initializing...");
        
        // Initialize logger with default settings
        Logger::instance().setLogLevel(LogLevel::Info);
        Logger::instance().enableConsoleLogging(true);
        Logger::instance().enableFileLogging(false);
        
        LOG_INFO("Utilities module initialized");
        return true;
    }
    
    void onShutdown() override {
        LOG_INFO("Utilities module shutting down...");
        Logger::instance().enableFileLogging(false);
        LOG_INFO("Utilities module shutdown complete");
    }
};

// Module registration function
extern "C" {
    void registerUtilitiesModule() {
        ModuleManager::instance().registerModule(
            std::make_shared<UtilitiesModule>()
        );
    }
}

} // namespace APLG
