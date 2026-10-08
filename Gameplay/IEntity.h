#pragma once

#include "../Include/Common.h"
#include "../Physics/IPhysicsBody.h"
#include <memory>

namespace APLG {

/**
 * @brief Entity types
 */
enum class EntityType {
    Player,
    Enemy,
    Collectable,
    Powerup,
    Hazard,
    Checkpoint,
    Trigger
};

/**
 * @brief Base interface for all game entities
 */
class IEntity {
public:
    virtual ~IEntity() = default;
    
    /**
     * @brief Get entity type
     */
    virtual EntityType getType() const = 0;
    
    /**
     * @brief Get physics body
     */
    virtual std::shared_ptr<IPhysicsBody> getPhysicsBody() const = 0;
    
    /**
     * @brief Get entity position
     */
    virtual Vec2 getPosition() const = 0;
    
    /**
     * @brief Set entity position
     */
    virtual void setPosition(const Vec2& position) = 0;
    
    /**
     * @brief Update entity
     */
    virtual void update(float32 deltaTime) = 0;
    
    /**
     * @brief Check if entity is active
     */
    virtual bool isActive() const = 0;
    
    /**
     * @brief Set entity active state
     */
    virtual void setActive(bool active) = 0;
    
    /**
     * @brief Get entity ID
     */
    virtual uint32 getId() const = 0;
    
    /**
     * @brief Set entity ID
     */
    virtual void setId(uint32 id) = 0;
};

/**
 * @brief Enemy AI behavior types
 */
enum class EnemyBehavior {
    Patrol,
    Chase,
    Flying,
    Stationary,
    Boss
};

/**
 * @brief Enemy entity
 */
class IEnemy : public virtual IEntity {
public:
    virtual ~IEnemy() = default;
    
    /**
     * @brief Get enemy behavior type
     */
    virtual EnemyBehavior getBehavior() const = 0;
    
    /**
     * @brief Set target for chase AI
     */
    virtual void setTarget(Vec2 target) = 0;
    
    /**
     * @brief Get current target
     */
    virtual Vec2 getTarget() const = 0;
    
    /**
     * @brief Get enemy health
     */
    virtual int32 getHealth() const = 0;
    
    /**
     * @brief Set enemy health
     */
    virtual void setHealth(int32 health) = 0;
    
    /**
     * @brief Damage enemy
     */
    virtual void takeDamage(int32 amount) = 0;
    
    /**
     * @brief Check if enemy is dead
     */
    virtual bool isDead() const = 0;
    
    /**
     * @brief Get patrol points
     */
    virtual const std::vector<Vec2>& getPatrolPoints() const = 0;
    
    /**
     * @brief Set patrol points
     */
    virtual void setPatrolPoints(const std::vector<Vec2>& points) = 0;
};

/**
 * @brief Collectable types
 */
enum class CollectableType {
    Coin,
    Gem,
    Key,
    Health,
    Ammo,
    Secret
};

/**
 * @brief Collectable entity
 */
class ICollectable : public virtual IEntity {
public:
    virtual ~ICollectable() = default;
    
    /**
     * @brief Get collectable type
     */
    virtual CollectableType getCollectableType() const = 0;
    
    /**
     * @brief Get value (points, amount, etc.)
     */
    virtual int32 getValue() const = 0;
    
    /**
     * @brief Set value
     */
    virtual void setValue(int32 value) = 0;
    
    /**
     * @brief Check if collected
     */
    virtual bool isCollected() const = 0;
    
    /**
     * @brief Mark as collected
     */
    virtual void collect() = 0;
    
    /**
     * @brief Respawn collectable
     */
    virtual void respawn() = 0;
    
    /**
     * @brief Get respawn time
     */
    virtual float32 getRespawnTime() const = 0;
    
    /**
     * @brief Set respawn time
     */
    virtual void setRespawnTime(float32 time) = 0;
};

/**
 * @brief Powerup entity
 */
class IPowerup : public virtual ICollectable {
public:
    virtual ~IPowerup() = default;
    
    /**
     * @brief Get powerup type
     */
    virtual PowerupType getPowerupType() const = 0;
    
    /**
     * @brief Get duration
     */
    virtual float32 getDuration() const = 0;
    
    /**
     * @brief Set duration
     */
    virtual void setDuration(float32 duration) = 0;
};

/**
 * @brief Base entity implementation
 */
class Entity : public virtual IEntity {
public:
    Entity(EntityType type, Vec2 position)
        : m_type(type)
        , m_active(true)
        , m_id(0)
    {
        m_physicsBody = std::make_shared<AABBBody>(position, Vec2(1, 1));
    }
    
    EntityType getType() const override { return m_type; }
    std::shared_ptr<IPhysicsBody> getPhysicsBody() const override { return m_physicsBody; }
    Vec2 getPosition() const override { return m_physicsBody->getPosition(); }
    void setPosition(const Vec2& position) override { m_physicsBody->setPosition(position); }
    void update(float32 deltaTime) override { m_physicsBody->update(deltaTime); }
    bool isActive() const override { return m_active; }
    void setActive(bool active) override { m_active = active; }
    uint32 getId() const override { return m_id; }
    void setId(uint32 id) override { m_id = id; }
    
protected:
    EntityType m_type;
    std::shared_ptr<IPhysicsBody> m_physicsBody;
    bool m_active;
    uint32 m_id;
};

/**
 * @brief Enemy implementation
 */
class Enemy : public Entity, public IEnemy {
public:
    Enemy(EnemyBehavior behavior, Vec2 position)
        : Entity(EntityType::Enemy, position)
        , m_behavior(behavior)
        , m_health(1)
        , m_dead(false)
        , m_currentPatrolIndex(0)
        , m_patrolSpeed(3.0f)
        , m_chaseSpeed(5.0f)
        , m_detectionRange(10.0f)
    {
        m_physicsBody->setCollisionLayer(2); // Enemy layer
        m_physicsBody->setCollisionMask(0xFFFFFFFF);
    }
    
    EnemyBehavior getBehavior() const override { return m_behavior; }
    void setTarget(Vec2 target) override { m_target = target; }
    Vec2 getTarget() const override { return m_target; }
    int32 getHealth() const override { return m_health; }
    void setHealth(int32 health) override { m_health = health; }
    void takeDamage(int32 amount) override {
        m_health -= amount;
        if (m_health <= 0) {
            m_dead = true;
            m_active = false;
        }
    }
    bool isDead() const override { return m_dead; }
    const std::vector<Vec2>& getPatrolPoints() const override { return m_patrolPoints; }
    void setPatrolPoints(const std::vector<Vec2>& points) override { m_patrolPoints = points; }
    
    void update(float32 deltaTime) override {
        if (!m_active || m_dead) return;
        
        switch (m_behavior) {
            case EnemyBehavior::Patrol:
                updatePatrol(deltaTime);
                break;
            case EnemyBehavior::Chase:
                updateChase(deltaTime);
                break;
            case EnemyBehavior::Flying:
                updateFlying(deltaTime);
                break;
            case EnemyBehavior::Stationary:
                // Do nothing
                break;
            case EnemyBehavior::Boss:
                updateBoss(deltaTime);
                break;
        }
        
        Entity::update(deltaTime);
    }
    
private:
    void updatePatrol(float32 deltaTime) {
        if (m_patrolPoints.empty()) return;
        
        Vec2 currentPos = Entity::getPosition();
        Vec2 target = m_patrolPoints[m_currentPatrolIndex];
        Vec2 direction = (target - currentPos).normalized();
        
        Vec2 vel = m_physicsBody->getVelocity();
        vel.x = direction.x * m_patrolSpeed;
        m_physicsBody->setVelocity(vel);
        
        // Check if reached patrol point
        float32 dist = (target - currentPos).length();
        if (dist < 0.5f) {
            m_currentPatrolIndex = (m_currentPatrolIndex + 1) % m_patrolPoints.size();
        }
    }
    
    void updateChase(float32 deltaTime) {
        Vec2 currentPos = Entity::getPosition();
        Vec2 direction = (m_target - currentPos).normalized();
        
        // Check if target is in range
        float32 dist = (m_target - currentPos).length();
        if (dist < m_detectionRange) {
            Vec2 vel = m_physicsBody->getVelocity();
            vel.x = direction.x * m_chaseSpeed;
            m_physicsBody->setVelocity(vel);
        } else {
            // Stop if target out of range
            m_physicsBody->setVelocity(Vec2(0, 0));
        }
    }
    
    void updateFlying(float32 deltaTime) {
        Vec2 currentPos = Entity::getPosition();
        Vec2 direction = (m_target - currentPos).normalized();
        
        Vec2 vel = m_physicsBody->getVelocity();
        vel = direction * m_chaseSpeed;
        m_physicsBody->setVelocity(vel);
    }
    
    void updateBoss(float32 deltaTime) {
        // Simple boss behavior - move towards target slowly
        Vec2 currentPos = Entity::getPosition();
        Vec2 direction = (m_target - currentPos).normalized();
        
        Vec2 vel = m_physicsBody->getVelocity();
        vel = direction * (m_chaseSpeed * 0.5f);
        m_physicsBody->setVelocity(vel);
    }
    
    EnemyBehavior m_behavior;
    int32 m_health;
    bool m_dead;
    Vec2 m_target;
    std::vector<Vec2> m_patrolPoints;
    size_t m_currentPatrolIndex;
    float32 m_patrolSpeed;
    float32 m_chaseSpeed;
    float32 m_detectionRange;
};

/**
 * @brief Collectable implementation
 */
class Collectable : public ICollectable, public Entity {
public:
    Collectable(CollectableType type, Vec2 position, int32 value)
        : Entity(EntityType::Collectable, position)
        , m_collectableType(type)
        , m_value(value)
        , m_collected(false)
        , m_respawnTime(5.0f)
        , m_respawnTimer(0.0f)
    {
        m_physicsBody = std::make_shared<CircleBody>(position, 0.3f);
        m_physicsBody->setCollisionLayer(4); // Collectable layer
        m_physicsBody->setCollisionMask(1); // Only collide with player
    }
    
    CollectableType getCollectableType() const override { return m_collectableType; }
    int32 getValue() const override { return m_value; }
    void setValue(int32 value) override { m_value = value; }
    bool isCollected() const override { return m_collected; }
    void collect() override { 
        m_collected = true; 
        m_active = false;
        m_respawnTimer = m_respawnTime;
    }
    void respawn() override {
        m_collected = false;
        m_active = true;
        m_respawnTimer = 0.0f;
    }
    float32 getRespawnTime() const override { return m_respawnTime; }
    void setRespawnTime(float32 time) override { m_respawnTime = time; }
    
    void update(float32 deltaTime) override {
        if (m_collected) {
            m_respawnTimer -= deltaTime;
            if (m_respawnTimer <= 0.0f) {
                respawn();
            }
            return;
        }
        
        Entity::update(deltaTime);
        
        // Simple floating animation
        Vec2 pos = Entity::getPosition();
        pos.y += std::sin(m_respawnTimer) * 0.01f;
        m_physicsBody->setPosition(pos);
        m_respawnTimer += deltaTime * 3.0f;
    }
    
private:
    CollectableType m_collectableType;
    int32 m_value;
    bool m_collected;
    float32 m_respawnTime;
    float32 m_respawnTimer;
};

/**
 * @brief Powerup implementation
 */
class Powerup : public IPowerup, public Collectable {
public:
    Powerup(PowerupType type, Vec2 position, float32 duration)
        : Collectable(CollectableType::Health, position, 1)
        , m_powerupType(type)
        , m_duration(duration)
    {
    }
    
    PowerupType getPowerupType() const override { return m_powerupType; }
    float32 getDuration() const override { return m_duration; }
    void setDuration(float32 duration) override { m_duration = duration; }
    
private:
    PowerupType m_powerupType;
    float32 m_duration;
};

/**
 * @brief Entity manager
 * 
 * Manages all game entities in a level.
 */
class EntityManager {
public:
    EntityManager();
    
    /**
     * @brief Add entity to manager
     */
    void addEntity(std::shared_ptr<IEntity> entity);
    
    /**
     * @brief Remove entity by ID
     */
    void removeEntity(uint32 id);
    
    /**
     * @brief Get entity by ID
     */
    std::shared_ptr<IEntity> getEntity(uint32 id);
    
    /**
     * @brief Get all entities
     */
    const std::vector<std::shared_ptr<IEntity>>& getAllEntities() const;
    
    /**
     * @brief Get entities by type
     */
    std::vector<std::shared_ptr<IEntity>> getEntitiesByType(EntityType type);
    
    /**
     * @brief Get all enemies
     */
    std::vector<std::shared_ptr<IEnemy>> getEnemies();
    
    /**
     * @brief Get all collectables
     */
    std::vector<std::shared_ptr<ICollectable>> getCollectables();
    
    /**
     * @brief Update all entities
     */
    void update(float32 deltaTime);
    
    /**
     * @brief Clear all entities
     */
    void clear();
    
    /**
     * @brief Get entity count
     */
    size_t getEntityCount() const { return m_entities.size(); }
    
private:
    std::vector<std::shared_ptr<IEntity>> m_entities;
    uint32 m_nextId;
};

} // namespace APLG
