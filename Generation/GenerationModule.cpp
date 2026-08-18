#include "IGenerator.h"
#include "../Engine/IModule.h"
#include "../Utilities/ILogger.h"

namespace APLG {

/**
 * @brief Generation module implementation
 * 
 * Manages procedural level generation with multiple algorithms.
 */
class GenerationModule : public ModuleBase {
public:
    GenerationModule() : ModuleBase("Generation") {}
    
    /**
     * @brief Generate a level using the specified algorithm
     */
    LevelData generateLevel(const std::string& algorithm, const GenerationConfig& config) {
        auto generator = GeneratorFactory::createGenerator(algorithm);
        if (generator) {
            return generator->generate(config);
        }
        
        LOG_ERROR("Failed to create generator: " + algorithm);
        return LevelData();
    }
    
    /**
     * @brief Get available generators
     */
    std::vector<std::string> getAvailableGenerators() {
        return GeneratorFactory::getAvailableGenerators();
    }
    
protected:
    bool onInitialize() override {
        LOG_INFO("Generation module initializing...");
        
        // Log available generators
        auto generators = getAvailableGenerators();
        LOG_INFO("Available generators:");
        for (const auto& gen : generators) {
            LOG_INFO("  - " + gen);
        }
        
        LOG_INFO("Generation module initialized");
        return true;
    }
    
    void onShutdown() override {
        LOG_INFO("Generation module shutting down...");
        LOG_INFO("Generation module shutdown complete");
    }
};

// Global generation module instance
static GenerationModule* g_generationModule = nullptr;

/**
 * @brief Get the global generation module
 */
GenerationModule* getGenerationModule() {
    return g_generationModule;
}

// Module registration function
extern "C" {
    void registerGenerationModule() {
        auto module = std::make_shared<GenerationModule>();
        g_generationModule = module.get();
        ModuleManager::instance().registerModule(module);
    }
}

} // namespace APLG
