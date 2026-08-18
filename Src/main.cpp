#include "../Engine/IEngine.h"
#include "../Engine/IModule.h"
#include <iostream>

using namespace APLG;

/**
 * @brief Main entry point for the GDExtension
 * 
 * This function is called by Godot when the extension is loaded.
 * It initializes the engine and all modules.
 */
extern "C" {
    // Godot GDExtension entry points
    void* godot_extension_init() {
        std::cout << "APLG GDExtension: Initializing..." << std::endl;
        
        // Create and initialize engine
        auto engine = EngineFactory::createEngine();
        if (!engine->initialize()) {
            std::cerr << "Failed to initialize engine" << std::endl;
            return nullptr;
        }
        
        // Initialize all modules
        if (!ModuleManager::instance().initializeAll()) {
            std::cerr << "Failed to initialize modules" << std::endl;
            return nullptr;
        }
        
        std::cout << "APLG GDExtension: Initialization complete" << std::endl;
        return engine.release();
    }
    
    void godot_extension_shutdown(void* handle) {
        std::cout << "APLG GDExtension: Shutting down..." << std::endl;
        
        IEngine* engine = static_cast<IEngine*>(handle);
        if (engine) {
            ModuleManager::instance().shutdownAll();
            engine->shutdown();
            delete engine;
        }
        
        std::cout << "APLG GDExtension: Shutdown complete" << std::endl;
    }
    
    void godot_extension_update(void* handle, float deltaTime) {
        IEngine* engine = static_cast<IEngine*>(handle);
        if (engine && engine->isRunning()) {
            engine->update(deltaTime);
            ModuleManager::instance().updateAll(deltaTime);
        }
    }
}


