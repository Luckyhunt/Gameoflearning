#include "IEntity.h"
#include "../Utilities/ILogger.h"
#include <algorithm>

namespace APLG {

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
            auto enemy = std::dynamic_pointer_cast<IEnemy>(entity);
            if (enemy) {
                result.push_back(enemy);
            }
        }
    }
    return result;
}

std::vector<std::shared_ptr<ICollectable>> EntityManager::getCollectables() {
    std::vector<std::shared_ptr<ICollectable>> result;
    for (auto& entity : m_entities) {
        if (entity->getType() == EntityType::Collectable) {
            auto collectable = std::dynamic_pointer_cast<ICollectable>(entity);
            if (collectable) {
                result.push_back(collectable);
            }
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
