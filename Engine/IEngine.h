#pragma once

#include "../Include/Common.h"
#include <memory>

namespace APLG {

/**
 * @brief Main engine interface for the game framework
 * 
 * This interface defines the core engine functionality including:
 * - Initialization and shutdown
 * - Main game loop
 * - Module management
 * - Time management
 */
class IEngine {
public:
    virtual ~IEngine() = default;
    
    /**
     * @brief Initialize the engine and all subsystems
     * @return true if initialization successful
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Shutdown the engine and cleanup resources
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Main game loop - called every frame
     * @param deltaTime Time elapsed since last frame in seconds
     */
    virtual void update(float32 deltaTime) = 0;
    
    /**
     * @brief Render the current frame
     */
    virtual void render() = 0;
    
    /**
     * @brief Get the current engine time in seconds
     * @return Current engine time
     */
    virtual float32 getEngineTime() const = 0;
    
    /**
     * @brief Get the current frame rate
     * @return Current FPS
     */
    virtual uint32 getFPS() const = 0;
    
    /**
     * @brief Check if the engine is running
     * @return true if engine is running
     */
    virtual bool isRunning() const = 0;
    
    /**
     * @brief Request engine shutdown
     */
    virtual void requestShutdown() = 0;
};

/**
 * @brief Factory for creating engine instances
 */
class EngineFactory {
public:
    static std::unique_ptr<IEngine> createEngine();
};

} // namespace APLG
