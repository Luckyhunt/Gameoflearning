#include "IEntity.h"
#include "../Utilities/ILogger.h"
#include <algorithm>

namespace APLG {

/**
 * @brief Base entity implementation
 */
class Entity : public IEntity {
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
class Enemy : public IEnemy, public Entity {
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
        
        Entity::update(deltaTime);
        
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

// EntityManager Implementation
EntityManager::EntityManager()
    : m_nextId(1)
{
}

void EntityManager::addEntity(std::shared_ptr<IEntity> entity) {
    entity->setId(m_nextId++);
    m_entities.push_back(entity);
    
    LOG_INFO("Entity added: ID " + std::to_string(entity->getId()));
}

void EntityManager::removeEntity(uint32 id) {
    auto it = std::remove_if(m_entities.begin(), m_entities.end(),
                           [id](const std::shared_ptr<IEntity>& e) {
                               return e->getId() == id;
                           });
    
    if (it != m_entities.end()) {
        m_entities.erase(it, m_entities.end());
        LOG_INFO("Entity removed: ID " + std::to_string(id));
    }
}

std::shared_ptr<IEntity> EntityManager::getEntity(uint32 id) {
    for (auto& entity : m_entities) {
        if (entity->getId() == id) {
            return entity;
        }
    }
    return nullptr;
}

const std::vector<std::shared_ptr<IEntity>>& EntityManager::getAllEntities() const {
    return m_entities;
}

std::vector<std::shared_ptr<IEntity>> EntityManager::getEntitiesByType(EntityType type) {
    std::vector<std::shared_ptr<IEntity>> result;
    for (auto& entity : m_entities) {
        if (entity->getType() == type) {
            result.push_back(entity);
        }
    }
    return result;
}

std::vector<std::shared_ptr<IEnemy>> EntityManager::getEnemies() {
    std::vector<std::shared_ptr<IEnemy>> result;
    for (auto& entity : m_entities) {
        if (entity->getType() == EntityType::Enemy) {
            result.push_back(std::static_pointer_cast<IEnemy>(entity));
        }
    }
    return result;
}

std::vector<std::shared_ptr<ICollectable>> EntityManager::getCollectables() {
    std::vector<std::shared_ptr<ICollectable>> result;
    for (auto& entity : m_entities) {
        if (entity->getType() == EntityType::Collectable) {
            result.push_back(std::static_pointer_cast<ICollectable>(entity));
        }
    }
    return result;
}

void EntityManager::update(float32 deltaTime) {
    for (auto& entity : m_entities) {
        if (entity->isActive()) {
            entity->update(deltaTime);
        }
    }
    
    // Remove dead entities
    auto it = std::remove_if(m_entities.begin(), m_entities.end(),
                           [](const std::shared_ptr<IEntity>& e) {
                               auto enemy = std::dynamic_pointer_cast<IEnemy>(e);
                               return enemy && enemy->isDead();
                           });
    
    if (it != m_entities.end()) {
        m_entities.erase(it, m_entities.end());
    }
}

void EntityManager::clear() {
    m_entities.clear();
    m_nextId = 1;
    
    LOG_INFO("All entities cleared");
}

} // namespace APLG
