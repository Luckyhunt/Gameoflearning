#pragma once

#include "IPhysicsBody.h"
#include <vector>
#include <memory>
#include <functional>

namespace APLG {

/**
 * @brief Collision result data
 */
struct CollisionResult {
    bool collided;
    Vec2 normal;      // Collision normal (direction of collision)
    float32 penetration; // Depth of penetration
    IPhysicsBody* bodyA;
    IPhysicsBody* bodyB;
    
    CollisionResult() 
        : collided(false)
        , normal(0, 0)
        , penetration(0.0f)
        , bodyA(nullptr)
        , bodyB(nullptr)
    {
    }
};

/**
 * @brief Collision detection system
 * 
 * Provides efficient collision detection between physics bodies.
 * Uses spatial partitioning for performance optimization.
 */
class CollisionDetection {
public:
    /**
     * @brief Check collision between two AABB bodies
     */
    static bool checkAABB(const AABBBody& a, const AABBBody& b) {
        Rect rectA = a.getAABB();
        Rect rectB = b.getAABB();
        return rectA.intersects(rectB);
    }
    
    /**
     * @brief Get detailed collision result for AABB vs AABB
     */
    static CollisionResult getAABBCollision(const AABBBody& a, const AABBBody& b) {
        CollisionResult result;
        result.bodyA = const_cast<AABBBody*>(&a);
        result.bodyB = const_cast<AABBBody*>(&b);
        
        Rect rectA = a.getAABB();
        Rect rectB = b.getAABB();
        
        if (!rectA.intersects(rectB)) {
            result.collided = false;
            return result;
        }
        
        result.collided = true;
        
        // Calculate penetration depth and normal
        Vec2 centerA(rectA.position.x + rectA.size.x / 2, 
                     rectA.position.y + rectA.size.y / 2);
        Vec2 centerB(rectB.position.x + rectB.size.x / 2, 
                     rectB.position.y + rectB.size.y / 2);
        
        Vec2 diff = centerA - centerB;
        Vec2 overlap(
            (rectA.size.x + rectB.size.x) / 2 - std::abs(diff.x),
            (rectA.size.y + rectB.size.y) / 2 - std::abs(diff.y)
        );
        
        // Use the smaller overlap for the collision normal
        if (overlap.x < overlap.y) {
            result.penetration = overlap.x;
            result.normal = Vec2(diff.x > 0 ? 1 : -1, 0);
        } else {
            result.penetration = overlap.y;
            result.normal = Vec2(0, diff.y > 0 ? 1 : -1);
        }
        
        return result;
    }
    
    /**
     * @brief Check collision between two circle bodies
     */
    static bool checkCircle(const CircleBody& a, const CircleBody& b) {
        Vec2 diff = a.getPosition() - b.getPosition();
        float32 distanceSquared = diff.lengthSquared();
        float32 radiusSum = a.getRadius() + b.getRadius();
        return distanceSquared <= radiusSum * radiusSum;
    }
    
    /**
     * @brief Get detailed collision result for Circle vs Circle
     */
    static CollisionResult getCircleCollision(const CircleBody& a, const CircleBody& b) {
        CollisionResult result;
        result.bodyA = const_cast<CircleBody*>(&a);
        result.bodyB = const_cast<CircleBody*>(&b);
        
        Vec2 diff = a.getPosition() - b.getPosition();
        float32 distanceSquared = diff.lengthSquared();
        float32 radiusSum = a.getRadius() + b.getRadius();
        
        if (distanceSquared > radiusSum * radiusSum) {
            result.collided = false;
            return result;
        }
        
        result.collided = true;
        float32 distance = std::sqrt(distanceSquared);
        
        if (distance > 0.0001f) {
            result.normal = diff / distance;
            result.penetration = radiusSum - distance;
        } else {
            // Circles are at the same position
            result.normal = Vec2(1, 0);
            result.penetration = radiusSum;
        }
        
        return result;
    }
    
    /**
     * @brief Check collision between AABB and Circle
     */
    static bool checkAABBCircle(const AABBBody& aabb, const CircleBody& circle) {
        Vec2 circleCenter = circle.getPosition();
        Rect aabbRect = aabb.getAABB();
        
        // Find closest point on AABB to circle center
        Vec2 closest(
            std::max(aabbRect.position.x, std::min(circleCenter.x, aabbRect.position.x + aabbRect.size.x)),
            std::max(aabbRect.position.y, std::min(circleCenter.y, aabbRect.position.y + aabbRect.size.y))
        );
        
        Vec2 diff = circleCenter - closest;
        float32 distanceSquared = diff.lengthSquared();
        
        return distanceSquared <= circle.getRadius() * circle.getRadius();
    }
    
    /**
     * @brief Get detailed collision result for AABB vs Circle
     */
    static CollisionResult getAABBCircleCollision(const AABBBody& aabb, const CircleBody& circle) {
        CollisionResult result;
        result.bodyA = const_cast<AABBBody*>(&aabb);
        result.bodyB = const_cast<CircleBody*>(&circle);
        
        Vec2 circleCenter = circle.getPosition();
        Rect aabbRect = aabb.getAABB();
        
        // Find closest point on AABB to circle center
        Vec2 closest(
            std::max(aabbRect.position.x, std::min(circleCenter.x, aabbRect.position.x + aabbRect.size.x)),
            std::max(aabbRect.position.y, std::min(circleCenter.y, aabbRect.position.y + aabbRect.size.y))
        );
        
        Vec2 diff = circleCenter - closest;
        float32 distanceSquared = diff.lengthSquared();
        
        if (distanceSquared > circle.getRadius() * circle.getRadius()) {
            result.collided = false;
            return result;
        }
        
        result.collided = true;
        float32 distance = std::sqrt(distanceSquared);
        
        if (distance > 0.0001f) {
            result.normal = diff / distance;
            result.penetration = circle.getRadius() - distance;
        } else {
            // Circle center is inside AABB
            result.normal = Vec2(0, 1);
            result.penetration = circle.getRadius();
        }
        
        return result;
    }
    
    /**
     * @brief Generic collision check between any two bodies
     */
    static CollisionResult checkCollision(IPhysicsBody* a, IPhysicsBody* b) {
        if (!a || !b) return CollisionResult();
        
        // Check collision layers
        if ((a->getCollisionLayer() & b->getCollisionMask()) == 0 &&
            (b->getCollisionLayer() & a->getCollisionMask()) == 0) {
            return CollisionResult();
        }
        
        CollisionShape shapeA = a->getShape();
        CollisionShape shapeB = b->getShape();
        
        // AABB vs AABB
        if (shapeA == CollisionShape::AABB && shapeB == CollisionShape::AABB) {
            return getAABBCollision(
                *static_cast<AABBBody*>(a),
                *static_cast<AABBBody*>(b)
            );
        }
        
        // Circle vs Circle
        if (shapeA == CollisionShape::Circle && shapeB == CollisionShape::Circle) {
            return getCircleCollision(
                *static_cast<CircleBody*>(a),
                *static_cast<CircleBody*>(b)
            );
        }
        
        // AABB vs Circle
        if (shapeA == CollisionShape::AABB && shapeB == CollisionShape::Circle) {
            return getAABBCircleCollision(
                *static_cast<AABBBody*>(a),
                *static_cast<CircleBody*>(b)
            );
        }
        
        // Circle vs AABB
        if (shapeA == CollisionShape::Circle && shapeB == CollisionShape::AABB) {
            CollisionResult result = getAABBCircleCollision(
                *static_cast<AABBBody*>(b),
                *static_cast<CircleBody*>(a)
            );
            // Swap bodies and invert normal
            std::swap(result.bodyA, result.bodyB);
            result.normal = result.normal * -1;
            return result;
        }
        
        return CollisionResult();
    }
};

/**
 * @brief Collision callback type
 */
using CollisionCallback = std::function<void(const CollisionResult&)>;

/**
 * @brief Physics world for managing physics bodies and collisions
 */
class PhysicsWorld {
public:
    PhysicsWorld() 
        : m_gravity(0, -9.8f)
        , m_enabled(true)
    {
    }
    
    /**
     * @brief Add a physics body to the world
     */
    void addBody(std::shared_ptr<IPhysicsBody> body) {
        m_bodies.push_back(body);
    }
    
    /**
     * @brief Remove a physics body from the world
     */
    void removeBody(std::shared_ptr<IPhysicsBody> body) {
        auto it = std::remove(m_bodies.begin(), m_bodies.end(), body);
        m_bodies.erase(it, m_bodies.end());
    }
    
    /**
     * @brief Update the physics world
     * @param deltaTime Time elapsed since last frame
     */
    void update(float32 deltaTime) {
        if (!m_enabled) return;
        
        // Update all bodies
        for (auto& body : m_bodies) {
            if (!body->isStatic()) {
                // Apply gravity
                body->applyForce(m_gravity * body->getMass());
                body->update(deltaTime);
            }
        }
        
        // Detect and resolve collisions
        detectCollisions();
    }
    
    /**
     * @brief Set the gravity vector
     */
    void setGravity(const Vec2& gravity) {
        m_gravity = gravity;
    }
    
    /**
     * @brief Get the gravity vector
     */
    Vec2 getGravity() const {
        return m_gravity;
    }
    
    /**
     * @brief Enable or disable physics simulation
     */
    void setEnabled(bool enabled) {
        m_enabled = enabled;
    }
    
    /**
     * @brief Check if physics is enabled
     */
    bool isEnabled() const {
        return m_enabled;
    }
    
    /**
     * @brief Set collision callback
     */
    void setCollisionCallback(CollisionCallback callback) {
        m_collisionCallback = callback;
    }
    
    /**
     * @brief Get all bodies in the world
     */
    const std::vector<std::shared_ptr<IPhysicsBody>>& getBodies() const {
        return m_bodies;
    }
    
    /**
     * @brief Query bodies in a region
     */
    std::vector<std::shared_ptr<IPhysicsBody>> queryRegion(const Rect& region) {
        std::vector<std::shared_ptr<IPhysicsBody>> result;
        for (auto& body : m_bodies) {
            if (region.intersects(body->getAABB())) {
                result.push_back(body);
            }
        }
        return result;
    }
    
private:
    void detectCollisions() {
        // Simple O(n^2) collision detection
        // TODO: Implement spatial partitioning for better performance
        for (size_t i = 0; i < m_bodies.size(); ++i) {
            for (size_t j = i + 1; j < m_bodies.size(); ++j) {
                CollisionResult result = CollisionDetection::checkCollision(
                    m_bodies[i].get(),
                    m_bodies[j].get()
                );
                
                if (result.collided) {
                    resolveCollision(result);
                    
                    if (m_collisionCallback) {
                        m_collisionCallback(result);
                    }
                }
            }
        }
    }
    
    void resolveCollision(const CollisionResult& result) {
        if (!result.collided) return;
        
        IPhysicsBody* bodyA = result.bodyA;
        IPhysicsBody* bodyB = result.bodyB;
        
        // Don't resolve if both are static
        if (bodyA->isStatic() && bodyB->isStatic()) return;
        
        // Separate bodies based on penetration
        Vec2 separation = result.normal * result.penetration;
        
        if (!bodyA->isStatic() && !bodyB->isStatic()) {
            // Both dynamic - separate equally
            Vec2 posA = bodyA->getPosition();
            Vec2 posB = bodyB->getPosition();
            bodyA->setPosition(posA - separation * 0.5f);
            bodyB->setPosition(posB + separation * 0.5f);
        } else if (!bodyA->isStatic()) {
            // Only A is dynamic
            Vec2 posA = bodyA->getPosition();
            bodyA->setPosition(posA - separation);
        } else {
            // Only B is dynamic
            Vec2 posB = bodyB->getPosition();
            bodyB->setPosition(posB + separation);
        }
        
        // Apply impulse response (simple elastic collision)
        const PhysicsMaterial& matA = bodyA->getMaterial();
        const PhysicsMaterial& matB = bodyB->getMaterial();
        float32 restitution = std::min(matA.restitution, matB.restitution);
        
        Vec2 relVel = bodyA->getVelocity() - bodyB->getVelocity();
        float32 velAlongNormal = relVel.dot(result.normal);
        
        // Only resolve if moving towards each other
        if (velAlongNormal > 0) return;
        
        float32 j = -(1 + restitution) * velAlongNormal;
        j /= (1 / bodyA->getMass() + 1 / bodyB->getMass());
        
        Vec2 impulse = result.normal * j;
        
        if (!bodyA->isStatic()) {
            Vec2 velA = bodyA->getVelocity();
            bodyA->setVelocity(velA + impulse / bodyA->getMass());
        }
        
        if (!bodyB->isStatic()) {
            Vec2 velB = bodyB->getVelocity();
            bodyB->setVelocity(velB - impulse / bodyB->getMass());
        }
    }
    
    Vec2 m_gravity;
    bool m_enabled;
    std::vector<std::shared_ptr<IPhysicsBody>> m_bodies;
    CollisionCallback m_collisionCallback;
};

} // namespace APLG
