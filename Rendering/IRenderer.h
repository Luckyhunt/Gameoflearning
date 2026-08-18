#pragma once

#include "../Include/Common.h"
#include <memory>

namespace APLG {

/**
 * @brief Render layer types
 */
enum class RenderLayer {
    Background = 0,
    Tilemap = 1,
    Platforms = 2,
    Collectables = 3,
    Enemies = 4,
    Player = 5,
    Foreground = 6,
    UI = 7
};

/**
 * @brief Sprite data structure
 */
struct SpriteData {
    std::string texturePath;
    Vec2 position;
    Vec2 size;
    Vec2 pivot; // 0.5, 0.5 for center
    float32 rotation;
    uint32 color; // ARGB
    bool flippedH;
    bool flippedV;
    RenderLayer layer;
    
    SpriteData()
        : position(0, 0)
        , size(1, 1)
        , pivot(0.5f, 0.5f)
        , rotation(0.0f)
        , color(0xFFFFFFFF)
        , flippedH(false)
        , flippedV(false)
        , layer(RenderLayer::Player)
    {
    }
};

/**
 * @brief Tilemap data structure
 */
struct TilemapData {
    std::string tilesetPath;
    int32 tileSize;
    std::vector<std::vector<int32>> tiles; // Tile IDs
    Vec2 position;
    RenderLayer layer;
    
    TilemapData()
        : tileSize(32)
        , position(0, 0)
        , layer(RenderLayer::Tilemap)
    {
    }
};

/**
 * @brief Camera data structure
 */
struct CameraData {
    Vec2 position;
    Vec2 targetPosition;
    float32 zoom;
    float32 rotation;
    bool followPlayer;
    float32 followSpeed;
    Rect bounds; // Camera bounds
    bool boundsEnabled;
    
    CameraData()
        : position(0, 0)
        , targetPosition(0, 0)
        , zoom(1.0f)
        , rotation(0.0f)
        , followPlayer(true)
        , followSpeed(5.0f)
        , boundsEnabled(false)
    {
    }
};

/**
 * @brief Renderer interface
 * 
 * Provides abstraction for Godot rendering integration.
 */
class IRenderer {
public:
    virtual ~IRenderer() = default;
    
    /**
     * @brief Initialize renderer
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Shutdown renderer
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Begin frame
     */
    virtual void beginFrame() = 0;
    
    /**
     * @brief End frame
     */
    virtual void endFrame() = 0;
    
    /**
     * @brief Draw sprite
     */
    virtual void drawSprite(const SpriteData& sprite) = 0;
    
    /**
     * @brief Draw tilemap
     */
    virtual void drawTilemap(const TilemapData& tilemap) = 0;
    
    /**
     * @brief Update camera
     */
    virtual void updateCamera(const CameraData& camera) = 0;
    
    /**
     * @brief Get current camera data
     */
    virtual CameraData getCamera() const = 0;
    
    /**
     * @brief Set camera position
     */
    virtual void setCameraPosition(Vec2 position) = 0;
    
    /**
     * @brief Set camera zoom
     */
    virtual void setCameraZoom(float32 zoom) = 0;
    
    /**
     * @brief Clear screen
     */
    virtual void clearScreen(uint32 color) = 0;
    
    /**
     * @brief Set render target
     */
    virtual void setRenderTarget(const std::string& target) = 0;
    
    /**
     * @brief Reset render target
     */
    virtual void resetRenderTarget() = 0;
};

/**
 * @brief Godot renderer implementation
 * 
 * Bridges the C++ engine with Godot's rendering system.
 */
class GodotRenderer : public IRenderer {
public:
    GodotRenderer();
    
    bool initialize() override;
    void shutdown() override;
    void beginFrame() override;
    void endFrame() override;
    void drawSprite(const SpriteData& sprite) override;
    void drawTilemap(const TilemapData& tilemap) override;
    void updateCamera(const CameraData& camera) override;
    CameraData getCamera() const override { return m_camera; }
    void setCameraPosition(Vec2 position) override;
    void setCameraZoom(float32 zoom) override;
    void clearScreen(uint32 color) override;
    void setRenderTarget(const std::string& target) override;
    void resetRenderTarget() override;
    
    /**
     * @brief Get Godot node pointer (for GDExtension integration)
     */
    void* getGodotNode() const { return m_godotNode; }
    
    /**
     * @brief Set Godot node pointer
     */
    void setGodotNode(void* node) { m_godotNode = node; }
    
private:
    void updateCameraFollow(float32 deltaTime);
    
    CameraData m_camera;
    void* m_godotNode; // Pointer to Godot Node2D or similar
    std::string m_currentRenderTarget;
};

/**
 * @brief Sprite batch renderer
 * 
 * Optimizes sprite rendering by batching similar sprites.
 */
class SpriteBatch {
public:
    SpriteBatch();
    
    /**
     * @brief Begin batch
     */
    void begin();
    
    /**
     * @brief Add sprite to batch
     */
    void addSprite(const SpriteData& sprite);
    
    /**
     * @brief End batch and render
     */
    void end(IRenderer* renderer);
    
    /**
     * @brief Clear batch
     */
    void clear();
    
    /**
     * @brief Get batch size
     */
    size_t getBatchSize() const { return m_sprites.size(); }
    
private:
    std::vector<SpriteData> m_sprites;
    bool m_batching;
};

/**
 * @brief Level renderer
 * 
 * Handles rendering of complete level data.
 */
class LevelRenderer {
public:
    LevelRenderer() : m_renderer(nullptr) {}
    LevelRenderer(IRenderer* renderer) : m_renderer(renderer) {}
    
    /**
     * @brief Render level
     */
    void renderLevel(const LevelData& level);
    
    /**
     * @brief Render entities
     */
    void renderEntities(const std::vector<SpriteData>& entitySprites);
    
    /**
     * @brief Set renderer
     */
    void setRenderer(IRenderer* renderer);
    
private:
    IRenderer* m_renderer;
    SpriteBatch m_spriteBatch;
};

} // namespace APLG
