#include "IPhysicsBody.h"
#include "CollisionDetection.h"
#include "../Engine/IModule.h"
#include "../Utilities/ILogger.h"

namespace APLG {

/**
 * @brief Physics module implementation
 * 
 * Manages the physics world and collision detection system.
 */
class PhysicsModule : public ModuleBase {
public:
    PhysicsModule() : ModuleBase("Physics") {}
    
    /**
     * @brief Get the physics world instance
     */
    PhysicsWorld& getWorld() {
        return m_world;
    }
    
protected:
    bool onInitialize() override {
        LOG_INFO("Physics module initializing...");
        
        // Initialize physics world with default gravity
        m_world.setGravity(Vec2(0, -980.0f)); // Platformer gravity (scaled)
        m_world.setEnabled(true);
        
        LOG_INFO("Physics module initialized");
        return true;
    }
    
    void onShutdown() override {
        LOG_INFO("Physics module shutting down...");
        m_world.setEnabled(false);
        LOG_INFO("Physics module shutdown complete");
    }
    
    void update(float32 deltaTime) override {
        if (isInitialized()) {
            m_world.update(deltaTime);
        }
    }
    
private:
    PhysicsWorld m_world;
};

// Global physics module instance
static PhysicsModule* g_physicsModule = nullptr;

/**
 * @brief Get the global physics module
 */
PhysicsModule* getPhysicsModule() {
    return g_physicsModule;
}

// Module registration function
extern "C" {
    void registerPhysicsModule() {
        auto module = std::make_shared<PhysicsModule>();
        g_physicsModule = module.get();
        ModuleManager::instance().registerModule(module);
    }
}

} // namespace APLG
