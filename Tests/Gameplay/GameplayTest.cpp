#include "../../Gameplay/IEntity.h"
#include <iostream>

using namespace APLG;

/**
 * @brief Test suite for Gameplay module
 */
int main() {
    std::cout << "=== Gameplay Module Test ===" << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    // Test 1: Entity manager initialization
    std::cout << "\n--- Test 1: Entity Manager Initialization ---" << std::endl;
    {
        EntityManager manager;
        
        if (manager.getEntityCount() == 0) {
            std::cout << "PASS: Entity manager initialized empty" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Entity manager not empty" << std::endl;
            failed++;
        }
    }
    
    // Test 2: Add entity
    std::cout << "\n--- Test 2: Add Entity ---" << std::endl;
    {
        EntityManager manager;
        auto enemy = std::make_shared<Enemy>(EnemyBehavior::Patrol, Vec2(0, 0));
        manager.addEntity(enemy);
        
        if (manager.getEntityCount() == 1) {
            std::cout << "PASS: Entity added successfully" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Entity not added" << std::endl;
            failed++;
        }
    }
    
    // Test 3: Get entity by ID
    std::cout << "\n--- Test 3: Get Entity by ID ---" << std::endl;
    {
        EntityManager manager;
        auto enemy = std::make_shared<Enemy>(EnemyBehavior::Patrol, Vec2(0, 0));
        manager.addEntity(enemy);
        
        uint32 id = enemy->getId();
        auto retrieved = manager.getEntity(id);
        
        if (retrieved && retrieved->getId() == id) {
            std::cout << "PASS: Entity retrieved by ID" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Entity not retrieved" << std::endl;
            failed++;
        }
    }
    
    // Test 4: Remove entity
    std::cout << "\n--- Test 4: Remove Entity ---" << std::endl;
    {
        EntityManager manager;
        auto enemy = std::make_shared<Enemy>(EnemyBehavior::Patrol, Vec2(0, 0));
        manager.addEntity(enemy);
        uint32 id = enemy->getId();
        
        manager.removeEntity(id);
        
        if (manager.getEntityCount() == 0) {
            std::cout << "PASS: Entity removed successfully" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Entity not removed" << std::endl;
            failed++;
        }
    }
    
    // Test 5: Get entities by type
    std::cout << "\n--- Test 5: Get Entities by Type ---" << std::endl;
    {
        EntityManager manager;
        manager.addEntity(std::make_shared<Enemy>(EnemyBehavior::Patrol, Vec2(0, 0)));
        manager.addEntity(std::make_shared<Enemy>(EnemyBehavior::Chase, Vec2(1, 0)));
        manager.addEntity(std::make_shared<Collectable>(CollectableType::Coin, Vec2(2, 0), 1));
        
        auto enemies = manager.getEntitiesByType(EntityType::Enemy);
        
        if (enemies.size() == 2) {
            std::cout << "PASS: Entities filtered by type" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Type filtering failed" << std::endl;
            failed++;
        }
    }
    
    // Test 6: Get enemies
    std::cout << "\n--- Test 6: Get Enemies ---" << std::endl;
    {
        EntityManager manager;
        manager.addEntity(std::make_shared<Enemy>(EnemyBehavior::Patrol, Vec2(0, 0)));
        manager.addEntity(std::make_shared<Collectable>(CollectableType::Coin, Vec2(1, 0), 1));
        
        auto enemies = manager.getEnemies();
        
        if (enemies.size() == 1) {
            std::cout << "PASS: Enemies retrieved" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Enemy retrieval failed" << std::endl;
            failed++;
        }
    }
    
    // Test 7: Get collectables
    std::cout << "\n--- Test 7: Get Collectables ---" << std::endl;
    {
        EntityManager manager;
        manager.addEntity(std::make_shared<Enemy>(EnemyBehavior::Patrol, Vec2(0, 0)));
        manager.addEntity(std::make_shared<Collectable>(CollectableType::Coin, Vec2(1, 0), 1));
        manager.addEntity(std::make_shared<Collectable>(CollectableType::Gem, Vec2(2, 0), 5));
        
        auto collectables = manager.getCollectables();
        
        if (collectables.size() == 2) {
            std::cout << "PASS: Collectables retrieved" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Collectable retrieval failed" << std::endl;
            failed++;
        }
    }
    
    // Test 8: Enemy patrol behavior
    std::cout << "\n--- Test 8: Enemy Patrol Behavior ---" << std::endl;
    {
        auto enemy = std::make_shared<Enemy>(EnemyBehavior::Patrol, Vec2(0, 0));
        std::vector<Vec2> patrolPoints = {Vec2(5, 0), Vec2(10, 0), Vec2(0, 0)};
        enemy->setPatrolPoints(patrolPoints);
        
        if (enemy->getBehavior() == EnemyBehavior::Patrol && 
            enemy->getPatrolPoints().size() == 3) {
            std::cout << "PASS: Enemy patrol behavior set" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Patrol behavior not set" << std::endl;
            failed++;
        }
    }
    
    // Test 9: Enemy chase behavior
    std::cout << "\n--- Test 9: Enemy Chase Behavior ---" << std::endl;
    {
        auto enemy = std::make_shared<Enemy>(EnemyBehavior::Chase, Vec2(0, 0));
        enemy->setTarget(Vec2(10, 0));
        
        if (enemy->getBehavior() == EnemyBehavior::Chase && 
            enemy->getTarget().x == 10.0f) {
            std::cout << "PASS: Enemy chase behavior set" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Chase behavior not set" << std::endl;
            failed++;
        }
    }
    
    // Test 10: Enemy health and damage
    std::cout << "\n--- Test 10: Enemy Health and Damage ---" << std::endl;
    {
        auto enemy = std::make_shared<Enemy>(EnemyBehavior::Patrol, Vec2(0, 0));
        enemy->setHealth(5);
        enemy->takeDamage(2);
        
        if (enemy->getHealth() == 3 && !enemy->isDead()) {
            std::cout << "PASS: Enemy takes damage correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Damage not applied correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 11: Enemy death
    std::cout << "\n--- Test 11: Enemy Death ---" << std::endl;
    {
        auto enemy = std::make_shared<Enemy>(EnemyBehavior::Patrol, Vec2(0, 0));
        enemy->setHealth(1);
        enemy->takeDamage(1);
        
        if (enemy->isDead()) {
            std::cout << "PASS: Enemy dies when health reaches 0" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Enemy did not die" << std::endl;
            failed++;
        }
    }
    
    // Test 12: Collectable collection
    std::cout << "\n--- Test 12: Collectable Collection ---" << std::endl;
    {
        auto collectable = std::make_shared<Collectable>(CollectableType::Coin, Vec2(0, 0), 10);
        
        if (!collectable->isCollected()) {
            collectable->collect();
            if (collectable->isCollected()) {
                std::cout << "PASS: Collectable marked as collected" << std::endl;
                passed++;
            } else {
                std::cout << "FAIL: Collectable not marked" << std::endl;
                failed++;
            }
        } else {
            std::cout << "FAIL: Collectable already collected" << std::endl;
            failed++;
        }
    }
    
    // Test 13: Collectable respawn
    std::cout << "\n--- Test 13: Collectable Respawn ---" << std::endl;
    {
        auto collectable = std::make_shared<Collectable>(CollectableType::Coin, Vec2(0, 0), 10);
        collectable->collect();
        collectable->respawn();
        
        if (!collectable->isCollected()) {
            std::cout << "PASS: Collectable respawned" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Collectable not respawned" << std::endl;
            failed++;
        }
    }
    
    // Test 14: Entity update
    std::cout << "\n--- Test 14: Entity Update ---" << std::endl;
    {
        EntityManager manager;
        auto enemy = std::make_shared<Enemy>(EnemyBehavior::Patrol, Vec2(0, 0));
        manager.addEntity(enemy);
        
        Vec2 initialPos = enemy->getPosition();
        manager.update(0.1f);
        Vec2 finalPos = enemy->getPosition();
        
        // Position should change due to physics
        if (finalPos.y < initialPos.y) {
            std::cout << "PASS: Entity updated with physics" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Entity not updated" << std::endl;
            failed++;
        }
    }
    
    // Test 15: Clear entities
    std::cout << "\n--- Test 15: Clear Entities ---" << std::endl;
    {
        EntityManager manager;
        manager.addEntity(std::make_shared<Enemy>(EnemyBehavior::Patrol, Vec2(0, 0)));
        manager.addEntity(std::make_shared<Collectable>(CollectableType::Coin, Vec2(1, 0), 1));
        
        manager.clear();
        
        if (manager.getEntityCount() == 0) {
            std::cout << "PASS: All entities cleared" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Entities not cleared" << std::endl;
            failed++;
        }
    }
    
    // Summary
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    std::cout << "Total:  " << (passed + failed) << std::endl;
    
    return (failed == 0) ? 0 : 1;
}
