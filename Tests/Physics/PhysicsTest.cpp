#include "../../Physics/IPhysicsBody.h"
#include "../../Physics/CollisionDetection.h"
#include <iostream>

using namespace APLG;

/**
 * @brief Test suite for Physics module
 */
int main() {
    std::cout << "=== Physics Module Test ===" << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    // Test 1: AABB vs AABB collision
    std::cout << "\n--- Test 1: AABB vs AABB Collision ---" << std::endl;
    {
        AABBBody box1(Vec2(0, 0), Vec2(10, 10));
        AABBBody box2(Vec2(5, 5), Vec2(10, 10));
        
        if (CollisionDetection::checkAABB(box1, box2)) {
            std::cout << "PASS: Overlapping AABBs detected" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Overlapping AABBs not detected" << std::endl;
            failed++;
        }
    }
    
    // Test 2: Non-overlapping AABB
    std::cout << "\n--- Test 2: Non-overlapping AABB ---" << std::endl;
    {
        AABBBody box1(Vec2(0, 0), Vec2(10, 10));
        AABBBody box2(Vec2(20, 20), Vec2(10, 10));
        
        if (!CollisionDetection::checkAABB(box1, box2)) {
            std::cout << "PASS: Non-overlapping AABBs correctly identified" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Non-overlapping AABBs incorrectly reported as colliding" << std::endl;
            failed++;
        }
    }
    
    // Test 3: Circle vs Circle collision
    std::cout << "\n--- Test 3: Circle vs Circle Collision ---" << std::endl;
    {
        CircleBody circle1(Vec2(0, 0), 5.0f);
        CircleBody circle2(Vec2(3, 0), 5.0f);
        
        if (CollisionDetection::checkCircle(circle1, circle2)) {
            std::cout << "PASS: Overlapping circles detected" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Overlapping circles not detected" << std::endl;
            failed++;
        }
    }
    
    // Test 4: Non-overlapping circles
    std::cout << "\n--- Test 4: Non-overlapping Circles ---" << std::endl;
    {
        CircleBody circle1(Vec2(0, 0), 5.0f);
        CircleBody circle2(Vec2(20, 0), 5.0f);
        
        if (!CollisionDetection::checkCircle(circle1, circle2)) {
            std::cout << "PASS: Non-overlapping circles correctly identified" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Non-overlapping circles incorrectly reported as colliding" << std::endl;
            failed++;
        }
    }
    
    // Test 5: AABB vs Circle collision
    std::cout << "\n--- Test 5: AABB vs Circle Collision ---" << std::endl;
    {
        AABBBody box(Vec2(0, 0), Vec2(10, 10));
        CircleBody circle(Vec2(5, 5), 3.0f);
        
        if (CollisionDetection::checkAABBCircle(box, circle)) {
            std::cout << "PASS: AABB-Circle collision detected" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: AABB-Circle collision not detected" << std::endl;
            failed++;
        }
    }
    
    // Test 6: AABB vs Circle no collision
    std::cout << "\n--- Test 6: AABB vs Circle No Collision ---" << std::endl;
    {
        AABBBody box(Vec2(0, 0), Vec2(10, 10));
        CircleBody circle(Vec2(20, 20), 3.0f);
        
        if (!CollisionDetection::checkAABBCircle(box, circle)) {
            std::cout << "PASS: Non-colliding AABB-Circle correctly identified" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Non-colliding AABB-Circle incorrectly reported as colliding" << std::endl;
            failed++;
        }
    }
    
    // Test 7: Collision result details
    std::cout << "\n--- Test 7: Collision Result Details ---" << std::endl;
    {
        AABBBody box1(Vec2(0, 0), Vec2(10, 10));
        AABBBody box2(Vec2(8, 0), Vec2(10, 10));
        
        CollisionResult result = CollisionDetection::getAABBCollision(box1, box2);
        
        if (result.collided && result.penetration > 0) {
            std::cout << "PASS: Collision result contains valid data" << std::endl;
            std::cout << "  Normal: (" << result.normal.x << ", " << result.normal.y << ")" << std::endl;
            std::cout << "  Penetration: " << result.penetration << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Collision result invalid" << std::endl;
            failed++;
        }
    }
    
    // Test 8: Physics World
    std::cout << "\n--- Test 8: Physics World ---" << std::endl;
    {
        PhysicsWorld world;
        world.setGravity(Vec2(0, -10.0f));
        
        auto body1 = std::make_shared<AABBBody>(Vec2(0, 10), Vec2(2, 2));
        body1->setMass(1.0f);
        body1->setStatic(false);
        
        auto body2 = std::make_shared<AABBBody>(Vec2(0, 0), Vec2(10, 1));
        body2->setMass(0.0f);
        body2->setStatic(true);
        
        world.addBody(body1);
        world.addBody(body2);
        
        Vec2 initialPos = body1->getPosition();
        world.update(0.1f);
        Vec2 finalPos = body1->getPosition();
        
        // Body should have moved due to gravity
        if (finalPos.y < initialPos.y) {
            std::cout << "PASS: Physics world applies gravity correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Physics world did not apply gravity" << std::endl;
            failed++;
        }
    }
    
    // Test 9: Collision layers
    std::cout << "\n--- Test 9: Collision Layers ---" << std::endl;
    {
        AABBBody box1(Vec2(0, 0), Vec2(10, 10));
        AABBBody box2(Vec2(5, 5), Vec2(10, 10));
        
        box1.setCollisionLayer(1);
        box1.setCollisionMask(1);
        box2.setCollisionLayer(2);
        box2.setCollisionMask(2);
        
        CollisionResult result = CollisionDetection::checkCollision(&box1, &box2);
        
        if (!result.collided) {
            std::cout << "PASS: Collision layers filter correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Collision layers did not filter" << std::endl;
            failed++;
        }
    }
    
    // Test 10: Static bodies
    std::cout << "\n--- Test 10: Static Bodies ---" << std::endl;
    {
        AABBBody staticBody(Vec2(0, 0), Vec2(10, 10));
        staticBody.setStatic(true);
        staticBody.setVelocity(Vec2(10, 10));
        
        staticBody.update(0.1f);
        
        if (staticBody.getVelocity().x == 10.0f && staticBody.getVelocity().y == 10.0f) {
            std::cout << "PASS: Static body maintains velocity (no update)" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Static body velocity changed" << std::endl;
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
