#include "IRenderer.h"
#include "../Engine/IModule.h"
#include "../Utilities/ILogger.h"

namespace APLG {

/**
 * @brief Rendering module implementation
 * 
 * Manages rendering and Godot integration.
 */
class RenderingModule : public ModuleBase {
public:
    RenderingModule() : ModuleBase("Rendering") {}
    
    /**
     * @brief Get the renderer instance
     */
    GodotRenderer* getRenderer() {
        return &m_renderer;
    }
    
    /**
     * @brief Get the level renderer instance
     */
    LevelRenderer* getLevelRenderer() {
        return &m_levelRenderer;
    }
    
protected:
    bool onInitialize() override {
        LOG_INFO("Rendering module initializing...");
        
        // Initialize renderer
        if (!m_renderer.initialize()) {
            LOG_ERROR("Failed to initialize renderer");
            return false;
        }
        
        // Set up level renderer
        m_levelRenderer.setRenderer(&m_renderer);
        
        LOG_INFO("Rendering module initialized");
        return true;
    }
    
    void onShutdown() override {
        LOG_INFO("Rendering module shutting down...");
        m_renderer.shutdown();
        LOG_INFO("Rendering module shutdown complete");
    }
    
    void update(float32 deltaTime) override {
        if (isInitialized()) {
            // Update camera if following player
            CameraData camera = m_renderer.getCamera();
            if (camera.followPlayer) {
                m_renderer.updateCamera(camera);
            }
        }
    }
    
private:
    GodotRenderer m_renderer;
    LevelRenderer m_levelRenderer;
};

// Global rendering module instance
static RenderingModule* g_renderingModule = nullptr;

/**
 * @brief Get the global rendering module
 */
RenderingModule* getRenderingModule() {
    return g_renderingModule;
}

// Module registration function
extern "C" {
    void registerRenderingModule() {
        auto module = std::make_shared<RenderingModule>();
        g_renderingModule = module.get();
        ModuleManager::instance().registerModule(module);
    }
}

} // namespace APLG
