#pragma once

#include "../Include/Common.h"

namespace APLG {

/**
 * @brief Collision shape types
 */
enum class CollisionShape {
    None,
    AABB,       // Axis-Aligned Bounding Box
    Circle,
    OBB,        // Oriented Bounding Box
    Polygon
};

/**
 * @brief Collision layer/mask for filtering
 */
struct CollisionLayers {
    uint32 player : 1;
    uint32 enemy : 1;
    uint32 terrain : 1;
    uint32 hazard : 1;
    uint32 collectable : 1;
    uint32 trigger : 1;
    uint32 reserved : 26;
    
    CollisionLayers() : player(1), enemy(1), terrain(1), hazard(1), 
                        collectable(1), trigger(1), reserved(0) {}
    
    uint32 toMask() const {
        return (player << 0) | (enemy << 1) | (terrain << 2) | 
               (hazard << 3) | (collectable << 4) | (trigger << 5);
    }
};

/**
 * @brief Physics material properties
 */
struct PhysicsMaterial {
    float32 friction;
    float32 restitution; // Bounciness (0-1)
    float32 density;
    
    PhysicsMaterial() : friction(0.5f), restitution(0.0f), density(1.0f) {}
    PhysicsMaterial(float32 f, float32 r, float32 d) 
        : friction(f), restitution(r), density(d) {}
};

/**
 * @brief Base class for all physics bodies
 * 
 * Provides collision detection and response capabilities.
 * Implements the Component pattern for flexible attachment to game objects.
 */
class IPhysicsBody {
public:
    virtual ~IPhysicsBody() = default;
    
    /**
     * @brief Get the collision shape type
     */
    virtual CollisionShape getShape() const = 0;
    
    /**
     * @brief Get the position of the body
     */
    virtual Vec2 getPosition() const = 0;
    
    /**
     * @brief Set the position of the body
     */
    virtual void setPosition(const Vec2& position) = 0;
    
    /**
     * @brief Get the velocity of the body
     */
    virtual Vec2 getVelocity() const = 0;
    
    /**
     * @brief Set the velocity of the body
     */
    virtual void setVelocity(const Vec2& velocity) = 0;
    
    /**
     * @brief Apply a force to the body
     */
    virtual void applyForce(const Vec2& force) = 0;
    
    /**
     * @brief Apply an impulse to the body
     */
    virtual void applyImpulse(const Vec2& impulse) = 0;
    
    /**
     * @brief Get the mass of the body
     */
    virtual float32 getMass() const = 0;
    
    /**
     * @brief Set the mass of the body
     */
    virtual void setMass(float32 mass) = 0;
    
    /**
     * @brief Check if the body is static (immovable)
     */
    virtual bool isStatic() const = 0;
    
    /**
     * @brief Set whether the body is static
     */
    virtual void setStatic(bool isStatic) = 0;
    
    /**
     * @brief Get the collision layer mask
     */
    virtual uint32 getCollisionLayer() const = 0;
    
    /**
     * @brief Set the collision layer mask
     */
    virtual void setCollisionLayer(uint32 layer) = 0;
    
    /**
     * @brief Get the collision mask (what layers to collide with)
     */
    virtual uint32 getCollisionMask() const = 0;
    
    /**
     * @brief Set the collision mask
     */
    virtual void setCollisionMask(uint32 mask) = 0;
    
    /**
     * @brief Get the physics material
     */
    virtual const PhysicsMaterial& getMaterial() const = 0;
    
    /**
     * @brief Set the physics material
     */
    virtual void setMaterial(const PhysicsMaterial& material) = 0;
    
    /**
     * @brief Update the physics body
     * @param deltaTime Time elapsed since last frame
     */
    virtual void update(float32 deltaTime) = 0;
    
    /**
     * @brief Get the AABB bounding box
     */
    virtual Rect getAABB() const = 0;
};

/**
 * @brief AABB (Axis-Aligned Bounding Box) physics body
 * 
 * Simplest collision shape, efficient for platformer games.
 */
class AABBBody : public IPhysicsBody {
public:
    AABBBody() 
        : m_position(0, 0)
        , m_velocity(0, 0)
        , m_size(1, 1)
        , m_mass(1.0f)
        , m_static(false)
        , m_collisionLayer(1)
        , m_collisionMask(0xFFFFFFFF)
        , m_material()
    {
    }
    
    AABBBody(const Vec2& position, const Vec2& size)
        : m_position(position)
        , m_velocity(0, 0)
        , m_size(size)
        , m_mass(1.0f)
        , m_static(false)
        , m_collisionLayer(1)
        , m_collisionMask(0xFFFFFFFF)
        , m_material()
    {
    }
    
    CollisionShape getShape() const override { return CollisionShape::AABB; }
    
    Vec2 getPosition() const override { return m_position; }
    void setPosition(const Vec2& position) override { m_position = position; }
    
    Vec2 getVelocity() const override { return m_velocity; }
    void setVelocity(const Vec2& velocity) override { m_velocity = velocity; }
    
    void applyForce(const Vec2& force) override {
        if (m_static || m_mass <= 0.0f) return;
        m_velocity += (force / m_mass);
    }
    
    void applyImpulse(const Vec2& impulse) override {
        if (m_static || m_mass <= 0.0f) return;
        m_velocity += (impulse / m_mass);
    }
    
    float32 getMass() const override { return m_mass; }
    void setMass(float32 mass) override { m_mass = mass; }
    
    bool isStatic() const override { return m_static; }
    void setStatic(bool isStatic) override { m_static = isStatic; }
    
    uint32 getCollisionLayer() const override { return m_collisionLayer; }
    void setCollisionLayer(uint32 layer) override { m_collisionLayer = layer; }
    
    uint32 getCollisionMask() const override { return m_collisionMask; }
    void setCollisionMask(uint32 mask) override { m_collisionMask = mask; }
    
    const PhysicsMaterial& getMaterial() const override { return m_material; }
    void setMaterial(const PhysicsMaterial& material) override { m_material = material; }
    
    void update(float32 deltaTime) override {
        if (m_static) return;
        
        // Apply gravity if enabled
        // m_velocity.y += gravity * deltaTime;
        
        // Update position
        m_position += m_velocity * deltaTime;
    }
    
    Rect getAABB() const override {
        return Rect(m_position, m_size);
    }
    
    Vec2 getSize() const { return m_size; }
    void setSize(const Vec2& size) { m_size = size; }
    
private:
    Vec2 m_position;
    Vec2 m_velocity;
    Vec2 m_size;
    float32 m_mass;
    bool m_static;
    uint32 m_collisionLayer;
    uint32 m_collisionMask;
    PhysicsMaterial m_material;
};

/**
 * @brief Circle physics body
 * 
 * Useful for round collectables and some enemies.
 */
class CircleBody : public IPhysicsBody {
public:
    CircleBody() 
        : m_position(0, 0)
        , m_velocity(0, 0)
        , m_radius(0.5f)
        , m_mass(1.0f)
        , m_static(false)
        , m_collisionLayer(1)
        , m_collisionMask(0xFFFFFFFF)
        , m_material()
    {
    }
    
    CircleBody(const Vec2& position, float32 radius)
        : m_position(position)
        , m_velocity(0, 0)
        , m_radius(radius)
        , m_mass(1.0f)
        , m_static(false)
        , m_collisionLayer(1)
        , m_collisionMask(0xFFFFFFFF)
        , m_material()
    {
    }
    
    CollisionShape getShape() const override { return CollisionShape::Circle; }
    
    Vec2 getPosition() const override { return m_position; }
    void setPosition(const Vec2& position) override { m_position = position; }
    
    Vec2 getVelocity() const override { return m_velocity; }
    void setVelocity(const Vec2& velocity) override { m_velocity = velocity; }
    
    void applyForce(const Vec2& force) override {
        if (m_static || m_mass <= 0.0f) return;
        m_velocity += (force / m_mass);
    }
    
    void applyImpulse(const Vec2& impulse) override {
        if (m_static || m_mass <= 0.0f) return;
        m_velocity += (impulse / m_mass);
    }
    
    float32 getMass() const override { return m_mass; }
    void setMass(float32 mass) override { m_mass = mass; }
    
    bool isStatic() const override { return m_static; }
    void setStatic(bool isStatic) override { m_static = isStatic; }
    
    uint32 getCollisionLayer() const override { return m_collisionLayer; }
    void setCollisionLayer(uint32 layer) override { m_collisionLayer = layer; }
    
    uint32 getCollisionMask() const override { return m_collisionMask; }
    void setCollisionMask(uint32 mask) override { m_collisionMask = mask; }
    
    const PhysicsMaterial& getMaterial() const override { return m_material; }
    void setMaterial(const PhysicsMaterial& material) override { m_material = material; }
    
    void update(float32 deltaTime) override {
        if (m_static) return;
        m_position += m_velocity * deltaTime;
    }
    
    Rect getAABB() const override {
        return Rect(
            Vec2(m_position.x - m_radius, m_position.y - m_radius),
            Vec2(m_radius * 2, m_radius * 2)
        );
    }
    
    float32 getRadius() const { return m_radius; }
    void setRadius(float32 radius) { m_radius = radius; }
    
private:
    Vec2 m_position;
    Vec2 m_velocity;
    float32 m_radius;
    float32 m_mass;
    bool m_static;
    uint32 m_collisionLayer;
    uint32 m_collisionMask;
    PhysicsMaterial m_material;
};

} // namespace APLG
