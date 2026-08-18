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
class IEnemy : public IEntity {
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
class ICollectable : public IEntity {
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
class IPowerup : public ICollectable {
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
