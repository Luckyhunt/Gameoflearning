#include "IRenderer.h"
#include "../Utilities/ILogger.h"
#include <algorithm>

namespace APLG {

// GodotRenderer Implementation
GodotRenderer::GodotRenderer()
    : m_godotNode(nullptr)
{
}

bool GodotRenderer::initialize() {
    LOG_INFO("Godot Renderer initializing...");
    
    // Would initialize Godot-specific rendering resources
    // For now, this is a placeholder for GDExtension integration
    
    LOG_INFO("Godot Renderer initialized");
    return true;
}

void GodotRenderer::shutdown() {
    LOG_INFO("Godot Renderer shutting down...");
    
    // Would cleanup Godot resources
    
    LOG_INFO("Godot Renderer shutdown complete");
}

void GodotRenderer::beginFrame() {
    // Would call Godot's frame begin
}

void GodotRenderer::endFrame() {
    // Would call Godot's frame end
}

void GodotRenderer::drawSprite(const SpriteData& sprite) {
    // Would draw sprite using Godot's Sprite2D or similar
    // For now, just log the action
    LOG_DEBUG("Drawing sprite at: (" + std::to_string(sprite.position.x) + ", " + 
             std::to_string(sprite.position.y) + ")");
}

void GodotRenderer::drawTilemap(const TilemapData& tilemap) {
    // Would draw tilemap using Godot's TileMap or TileMapLayer
    LOG_DEBUG("Drawing tilemap with " + std::to_string(tilemap.tiles.size()) + " rows");
}

void GodotRenderer::updateCamera(const CameraData& camera) {
    m_camera = camera;
    
    // Would update Godot Camera2D
    if (m_camera.followPlayer) {
        updateCameraFollow(0.016f); // Assume 60 FPS
    }
}

void GodotRenderer::setCameraPosition(Vec2 position) {
    m_camera.position = position;
    m_camera.targetPosition = position;
}

void GodotRenderer::setCameraZoom(float32 zoom) {
    m_camera.zoom = std::clamp(zoom, 0.1f, 5.0f);
}

void GodotRenderer::clearScreen(uint32 color) {
    // Would clear screen with specified color
    LOG_DEBUG("Clearing screen with color: " + std::to_string(color));
}

void GodotRenderer::setRenderTarget(const std::string& target) {
    m_currentRenderTarget = target;
    // Would set Godot render target
}

void GodotRenderer::resetRenderTarget() {
    m_currentRenderTarget.clear();
    // Would reset to default render target
}

void GodotRenderer::updateCameraFollow(float32 deltaTime) {
    // Smooth camera follow
    Vec2 diff = m_camera.targetPosition - m_camera.position;
    float32 distance = diff.length();
    
    if (distance > 0.1f) {
        Vec2 direction = diff.normalized();
        float32 moveAmount = m_camera.followSpeed * deltaTime;
        
        if (moveAmount >= distance) {
            m_camera.position = m_camera.targetPosition;
        } else {
            m_camera.position += direction * moveAmount;
        }
    }
    
    // Apply bounds if enabled
    if (m_camera.boundsEnabled) {
        m_camera.position.x = std::clamp(m_camera.position.x, m_camera.bounds.x(), 
                                        m_camera.bounds.x() + m_camera.bounds.width());
        m_camera.position.y = std::clamp(m_camera.position.y, m_camera.bounds.y(), 
                                        m_camera.bounds.y() + m_camera.bounds.height());
    }
}

// SpriteBatch Implementation
SpriteBatch::SpriteBatch()
    : m_batching(false)
{
}

void SpriteBatch::begin() {
    m_batching = true;
    m_sprites.clear();
}

void SpriteBatch::addSprite(const SpriteData& sprite) {
    if (m_batching) {
        m_sprites.push_back(sprite);
    }
}

void SpriteBatch::end(IRenderer* renderer) {
    if (!m_batching) return;
    
    m_batching = false;
    
    // Sort sprites by layer for proper rendering order
    std::sort(m_sprites.begin(), m_sprites.end(),
              [](const SpriteData& a, const SpriteData& b) {
                  return static_cast<int>(a.layer) < static_cast<int>(b.layer);
              });
    
    // Draw all sprites
    for (const auto& sprite : m_sprites) {
        renderer->drawSprite(sprite);
    }
    
    m_sprites.clear();
}

void SpriteBatch::clear() {
    m_sprites.clear();
    m_batching = false;
}

// LevelRenderer Implementation
void LevelRenderer::renderLevel(const LevelData& level) {
    if (!m_renderer) return;
    
    m_renderer->beginFrame();
    
    // Render tilemap
    TilemapData tilemap;
    // Convert TileType to int32 for tilemap
    std::vector<std::vector<int32>> intTiles;
    for (const auto& row : level.tiles) {
        std::vector<int32> intRow;
        for (const auto& tile : row) {
            intRow.push_back(static_cast<int32>(tile));
        }
        intTiles.push_back(intRow);
    }
    tilemap.tiles = intTiles;
    tilemap.tileSize = 32;
    tilemap.position = Vec2(0, 0);
    tilemap.layer = RenderLayer::Tilemap;
    
    m_renderer->drawTilemap(tilemap);
    
    // Render spawn and exit
    SpriteData spawnSprite;
    spawnSprite.position = Vec2(static_cast<float32>(level.spawnPosition.x) * 32, 
                               static_cast<float32>(level.spawnPosition.y) * 32);
    spawnSprite.size = Vec2(32, 32);
    spawnSprite.color = 0x00FF00; // Green
    spawnSprite.layer = RenderLayer::Platforms;
    m_renderer->drawSprite(spawnSprite);
    
    SpriteData exitSprite;
    exitSprite.position = Vec2(static_cast<float32>(level.exitPosition.x) * 32, 
                              static_cast<float32>(level.exitPosition.y) * 32);
    exitSprite.size = Vec2(32, 32);
    exitSprite.color = 0xFF0000; // Red
    exitSprite.layer = RenderLayer::Platforms;
    m_renderer->drawSprite(exitSprite);
    
    m_renderer->endFrame();
}

void LevelRenderer::renderEntities(const std::vector<SpriteData>& entitySprites) {
    if (!m_renderer) return;
    
    m_spriteBatch.begin();
    
    for (const auto& sprite : entitySprites) {
        m_spriteBatch.addSprite(sprite);
    }
    
    m_spriteBatch.end(m_renderer);
}

void LevelRenderer::setRenderer(IRenderer* renderer) {
    m_renderer = renderer;
}

} // namespace APLG
