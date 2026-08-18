#include "../../Player/IPlayer.h"
#include <iostream>

using namespace APLG;

/**
 * @brief Test suite for Player module
 */
int main() {
    std::cout << "=== Player Module Test ===" << std::endl;
    
    int passed = 0;
    int failed = 0;
    
    // Test 1: Player creation
    std::cout << "\n--- Test 1: Player Creation ---" << std::endl;
    {
        Player player;
        
        if (player.getPhysicsBody() != nullptr) {
            std::cout << "PASS: Player physics body created" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Player physics body not created" << std::endl;
            failed++;
        }
    }
    
    // Test 2: Player state
    std::cout << "\n--- Test 2: Player State ---" << std::endl;
    {
        Player player;
        
        if (player.getState() == PlayerState::Idle) {
            std::cout << "PASS: Player starts in Idle state" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Player not in Idle state" << std::endl;
            failed++;
        }
    }
    
    // Test 3: Player position
    std::cout << "\n--- Test 3: Player Position ---" << std::endl;
    {
        Player player;
        Vec2 testPos(10.0f, 20.0f);
        player.setPosition(testPos);
        
        Vec2 pos = player.getPosition();
        if (std::abs(pos.x - testPos.x) < 0.01f && std::abs(pos.y - testPos.y) < 0.01f) {
            std::cout << "PASS: Player position set correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Player position not set correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 4: Player abilities
    std::cout << "\n--- Test 4: Player Abilities ---" << std::endl;
    {
        Player player;
        const PlayerAbilities& abilities = player.getAbilities();
        
        if (abilities.canDoubleJump && abilities.canWallJump && abilities.canDash && abilities.canClimb) {
            std::cout << "PASS: Player has all default abilities" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Player missing default abilities" << std::endl;
            failed++;
        }
    }
    
    // Test 5: Custom abilities
    std::cout << "\n--- Test 5: Custom Abilities ---" << std::endl;
    {
        Player player;
        PlayerAbilities customAbilities;
        customAbilities.canDoubleJump = false;
        customAbilities.jumpForce = 20.0f;
        player.setAbilities(customAbilities);
        
        const PlayerAbilities& abilities = player.getAbilities();
        if (!abilities.canDoubleJump && std::abs(abilities.jumpForce - 20.0f) < 0.01f) {
            std::cout << "PASS: Custom abilities set correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Custom abilities not set correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 6: Player status
    std::cout << "\n--- Test 6: Player Status ---" << std::endl;
    {
        Player player;
        const PlayerStatus& status = player.getStatus();
        
        if (status.maxHealth == 3 && status.currentHealth == 3 && status.lives == 3) {
            std::cout << "PASS: Player status initialized correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Player status not initialized correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 7: Take damage
    std::cout << "\n--- Test 7: Take Damage ---" << std::endl;
    {
        Player player;
        PlayerStatus& status = const_cast<PlayerStatus&>(player.getStatus());
        status.takeDamage(1);
        
        if (status.currentHealth == 2 && status.invincible) {
            std::cout << "PASS: Player takes damage and becomes invincible" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Damage not applied correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 8: Heal
    std::cout << "\n--- Test 8: Heal ---" << std::endl;
    {
        Player player;
        PlayerStatus& status = const_cast<PlayerStatus&>(player.getStatus());
        status.takeDamage(2);
        status.heal(1);
        
        if (status.currentHealth == 2) {
            std::cout << "PASS: Player heals correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Healing not applied correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 9: Player death
    std::cout << "\n--- Test 9: Player Death ---" << std::endl;
    {
        Player player;
        PlayerStatus& status = const_cast<PlayerStatus&>(player.getStatus());
        status.takeDamage(3);
        
        if (status.isDead()) {
            std::cout << "PASS: Player dies when health reaches 0" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Player death not detected" << std::endl;
            failed++;
        }
    }
    
    // Test 10: Respawn
    std::cout << "\n--- Test 10: Respawn ---" << std::endl;
    {
        Player player;
        Vec2 checkpoint(5.0f, 10.0f);
        player.setCheckpoint(checkpoint);
        
        const_cast<PlayerStatus&>(player.getStatus()).takeDamage(3);
        player.respawn(checkpoint);
        
        Vec2 pos = player.getPosition();
        const PlayerStatus& status = player.getStatus();
        
        if (std::abs(pos.x - checkpoint.x) < 0.01f && 
            std::abs(pos.y - checkpoint.y) < 0.01f &&
            status.currentHealth == status.maxHealth &&
            status.lives == 2) {
            std::cout << "PASS: Player respawns correctly" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Respawn not working correctly" << std::endl;
            failed++;
        }
    }
    
    // Test 11: Input processing
    std::cout << "\n--- Test 11: Input Processing ---" << std::endl;
    {
        Player player;
        PlayerInput input;
        input.moveRight = true;
        input.jump = true;
        
        player.processInput(input);
        player.update(0.016f); // 60 FPS
        
        Vec2 vel = player.getVelocity();
        if (vel.x > 0.0f) {
            std::cout << "PASS: Player responds to input" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Player not responding to input" << std::endl;
            failed++;
        }
    }
    
    // Test 12: Checkpoint system
    std::cout << "\n--- Test 12: Checkpoint System ---" << std::endl;
    {
        Player player;
        Vec2 checkpoint1(10.0f, 20.0f);
        Vec2 checkpoint2(15.0f, 25.0f);
        
        player.setCheckpoint(checkpoint1);
        Vec2 retrieved = player.getLastCheckpoint();
        
        if (std::abs(retrieved.x - checkpoint1.x) < 0.01f && 
            std::abs(retrieved.y - checkpoint1.y) < 0.01f) {
            std::cout << "PASS: Checkpoint system working" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Checkpoint system not working" << std::endl;
            failed++;
        }
    }
    
    // Test 13: Jump ability
    std::cout << "\n--- Test 13: Jump Ability ---" << std::endl;
    {
        Player player;
        // Simulate grounded state
        const_cast<Player&>(player).m_grounded = true;
        
        if (player.canJumpNow()) {
            std::cout << "PASS: Player can jump when grounded" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Player cannot jump when grounded" << std::endl;
            failed++;
        }
    }
    
    // Test 14: Double jump ability
    std::cout << "\n--- Test 14: Double Jump Ability ---" << std::endl;
    {
        Player player;
        // Simulate air state with jumps remaining
        const_cast<Player&>(player).m_grounded = false;
        const_cast<Player&>(player).m_jumpsRemaining = 1;
        
        if (player.canDoubleJumpNow()) {
            std::cout << "PASS: Player can double jump in air" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Player cannot double jump" << std::endl;
            failed++;
        }
    }
    
    // Test 15: Dash ability
    std::cout << "\n--- Test 15: Dash Ability ---" << std::endl;
    {
        Player player;
        
        if (player.canDashNow()) {
            std::cout << "PASS: Player can dash" << std::endl;
            passed++;
        } else {
            std::cout << "FAIL: Player cannot dash" << std::endl;
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
