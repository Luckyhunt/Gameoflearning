#pragma once

#include "../Include/Common.h"
#include <string>

namespace APLG {

/**
 * @brief Base interface for all game modules
 * 
 * All modules (Physics, Generation, AI, etc.) inherit from this interface
 * to provide consistent initialization, update, and shutdown behavior.
 * This follows the Open/Closed Principle and Interface Segregation Principle.
 */
class IModule {
public:
    virtual ~IModule() = default;
    
    /**
     * @brief Get the module name
     * @return Module name as string
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Initialize the module
     * @return true if initialization successful
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Update the module
     * @param deltaTime Time elapsed since last frame in seconds
     */
    virtual void update(float32 deltaTime) = 0;
    
    /**
     * @brief Shutdown the module and cleanup resources
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Check if module is initialized
     * @return true if module is initialized
     */
    virtual bool isInitialized() const = 0;
};

/**
 * @brief Base class for modules providing common functionality
 * 
 * Implements the Template Method pattern for consistent module behavior.
 */
class ModuleBase : public IModule {
public:
    ModuleBase(const std::string& name) 
        : m_name(name)
        , m_initialized(false)
    {
    }
    
    virtual ~ModuleBase() = default;
    
    std::string getName() const override { return m_name; }
    
    bool initialize() override {
        if (m_initialized) return true;
        
        if (onInitialize()) {
            m_initialized = true;
            return true;
        }
        return false;
    }
    
    void shutdown() override {
        if (m_initialized) {
            onShutdown();
            m_initialized = false;
        }
    }
    
    bool isInitialized() const override { return m_initialized; }
    
    void update(float32 deltaTime) override {
        // Default empty implementation
        // Override in derived classes if needed
    }
    
protected:
    /**
     * @brief Override this for module-specific initialization
     * @return true if successful
     */
    virtual bool onInitialize() = 0;
    
    /**
     * @brief Override this for module-specific shutdown
     */
    virtual void onShutdown() = 0;
    
    std::string m_name;
    bool m_initialized;
};

/**
 * @brief Manager for all game modules
 * 
 * Handles module registration, initialization order,
 * and coordinated updates/shutdown.
 */
class ModuleManager {
public:
    static ModuleManager& instance() {
        static ModuleManager inst;
        return inst;
    }
    
    /**
     * @brief Register a module
     * @param module Module to register
     */
    void registerModule(std::shared_ptr<IModule> module) {
        m_modules.push_back(module);
    }
    
    /**
     * @brief Initialize all registered modules
     * @return true if all modules initialized successfully
     */
    bool initializeAll() {
        for (auto& module : m_modules) {
            if (!module->initialize()) {
                std::cerr << "Failed to initialize module: " << module->getName() << std::endl;
                return false;
            }
            std::cout << "Module initialized: " << module->getName() << std::endl;
        }
        return true;
    }
    
    /**
     * @brief Update all modules
     * @param deltaTime Time elapsed since last frame
     */
    void updateAll(float32 deltaTime) {
        for (auto& module : m_modules) {
            if (module->isInitialized()) {
                module->update(deltaTime);
            }
        }
    }
    
    /**
     * @brief Shutdown all modules in reverse order
     */
    void shutdownAll() {
        for (auto it = m_modules.rbegin(); it != m_modules.rend(); ++it) {
            (*it)->shutdown();
            std::cout << "Module shutdown: " << (*it)->getName() << std::endl;
        }
        m_modules.clear();
    }
    
private:
    ModuleManager() = default;
    std::vector<std::shared_ptr<IModule>> m_modules;
};

} // namespace APLG
