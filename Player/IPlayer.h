#pragma once

#include "../Include/Common.h"
#include "../Physics/IPhysicsBody.h"
#include <memory>

namespace APLG {

/**
 * @brief Player state enumeration
 */
enum class PlayerState {
    Idle,
    Walking,
    Running,
    Jumping,
    Falling,
    DoubleJumping,
    WallSliding,
    WallJumping,
    Dashing,
    Climbing,
    Hurt,
    Dead
};

/**
 * @brief Player abilities configuration
 */
struct PlayerAbilities {
    bool canDoubleJump;
    bool canWallJump;
    bool canDash;
    bool canClimb;
    
    float32 jumpForce;
    float32 doubleJumpForce;
    float32 wallJumpForce;
    float32 dashForce;
    float32 dashDuration;
    float32 climbSpeed;
    float32 moveSpeed;
    float32 runSpeed;
    
    PlayerAbilities()
        : canDoubleJump(true)
        , canWallJump(true)
        , canDash(true)
        , canClimb(true)
        , jumpForce(15.0f)
        , doubleJumpForce(12.0f)
        , wallJumpForce(12.0f)
        , dashForce(30.0f)
        , dashDuration(0.2f)
        , climbSpeed(5.0f)
        , moveSpeed(8.0f)
        , runSpeed(12.0f)
    {
    }
};

/**
 * @brief Player health and status
 */
struct PlayerStatus {
    int32 maxHealth;
    int32 currentHealth;
    int32 lives;
    bool invincible;
    float32 invincibilityTime;
    
    PlayerStatus()
        : maxHealth(3)
        , currentHealth(3)
        , lives(3)
        , invincible(false)
        , invincibilityTime(0.0f)
    {
    }
    
    void takeDamage(int32 amount) {
        if (!invincible) {
            currentHealth = std::max(0, currentHealth - amount);
            invincible = true;
            invincibilityTime = 1.0f; // 1 second invincibility
        }
    }
    
    void heal(int32 amount) {
        currentHealth = std::min(maxHealth, currentHealth + amount);
    }
    
    bool isDead() const {
        return currentHealth <= 0;
    }
    
    void respawn() {
        currentHealth = maxHealth;
        lives--;
        invincible = true;
        invincibilityTime = 2.0f;
    }
    
    void update(float32 deltaTime) {
        if (invincible) {
            invincibilityTime -= deltaTime;
            if (invincibilityTime <= 0.0f) {
                invincible = false;
            }
        }
    }
};

/**
 * @brief Player movement input
 */
struct PlayerInput {
    bool moveLeft;
    bool moveRight;
    bool moveUp;
    bool moveDown;
    bool jump;
    bool jumpHeld;
    bool dash;
    bool attack;
    bool interact;
    
    PlayerInput()
        : moveLeft(false)
        , moveRight(false)
        , moveUp(false)
        , moveDown(false)
        , jump(false)
        , jumpHeld(false)
        , dash(false)
        , attack(false)
        , interact(false)
    {
    }
    
    void reset() {
        moveLeft = false;
        moveRight = false;
        moveUp = false;
        moveDown = false;
        jump = false;
        dash = false;
        attack = false;
        interact = false;
    }
};

/**
 * @brief Interface for player controller
 * 
 * Manages player movement, abilities, and state.
 * Integrates with physics system for collision and movement.
 */
class IPlayer {
public:
    virtual ~IPlayer() = default;
    
    /**
     * @brief Get the player's physics body
     */
    virtual std::shared_ptr<IPhysicsBody> getPhysicsBody() const = 0;
    
    /**
     * @brief Get current player state
     */
    virtual PlayerState getState() const = 0;
    
    /**
     * @brief Get player position
     */
    virtual Vec2 getPosition() const = 0;
    
    /**
     * @brief Set player position
     */
    virtual void setPosition(const Vec2& position) = 0;
    
    /**
     * @brief Get player velocity
     */
    virtual Vec2 getVelocity() const = 0;
    
    /**
     * @brief Process input for this frame
     */
    virtual void processInput(const PlayerInput& input) = 0;
    
    /**
     * @brief Update player logic
     * @param deltaTime Time elapsed since last frame
     */
    virtual void update(float32 deltaTime) = 0;
    
    /**
     * @brief Get player status (health, lives, etc.)
     */
    virtual const PlayerStatus& getStatus() const = 0;
    
    /**
     * @brief Get player abilities configuration
     */
    virtual const PlayerAbilities& getAbilities() const = 0;
    
    /**
     * @brief Set player abilities
     */
    virtual void setAbilities(const PlayerAbilities& abilities) = 0;
    
    /**
     * @brief Check if player is grounded
     */
    virtual bool isGrounded() const = 0;
    
    /**
     * @brief Check if player is touching a wall
     */
    virtual bool isTouchingWall() const = 0;
    
    /**
     * @brief Get wall direction (1 for right, -1 for left)
     */
    virtual int32 getWallDirection() const = 0;
    
    /**
     * @brief Check if player can jump
     */
    virtual bool canJumpNow() const = 0;
    
    /**
     * @brief Check if player can double jump
     */
    virtual bool canDoubleJumpNow() const = 0;
    
    /**
     * @brief Check if player can dash
     */
    virtual bool canDashNow() const = 0;
    
    /**
     * @brief Respawn player at checkpoint
     */
    virtual void respawn(const Vec2& checkpoint) = 0;
    
    /**
     * @brief Kill player
     */
    virtual void kill() = 0;
};

/**
 * @brief Concrete implementation of IPlayer
 * 
 * Implements full player movement with all abilities.
 */
class Player : public IPlayer {
public:
    Player() 
        : m_state(PlayerState::Idle)
        , m_grounded(false)
        , m_touchingWall(false)
        , m_wallDirection(0)
        , m_jumpsRemaining(0)
        , m_maxJumps(2)
        , m_canDash(true)
        , m_dashCooldown(0.0f)
        , m_dashTimer(0.0f)
        , m_facingDirection(1)
        , m_coyoteTime(0.0f)
        , m_jumpBufferTime(0.0f)
    {
        // Create physics body
        m_physicsBody = std::make_shared<AABBBody>(Vec2(0, 0), Vec2(0.8f, 1.8f));
        m_physicsBody->setMass(1.0f);
        m_physicsBody->setStatic(false);
        m_physicsBody->setCollisionLayer(1); // Player layer
        m_physicsBody->setCollisionMask(0xFFFFFFFF);
        
        // Physics material
        PhysicsMaterial material;
        material.friction = 0.0f; // No friction for platformer
        material.restitution = 0.0f;
        m_physicsBody->setMaterial(material);
    }
    
    std::shared_ptr<IPhysicsBody> getPhysicsBody() const override {
        return m_physicsBody;
    }
    
    PlayerState getState() const override { return m_state; }
    
    Vec2 getPosition() const override {
        return m_physicsBody->getPosition();
    }
    
    void setPosition(const Vec2& position) override {
        m_physicsBody->setPosition(position);
    }
    
    Vec2 getVelocity() const override {
        return m_physicsBody->getVelocity();
    }
    
    void processInput(const PlayerInput& input) override {
        m_input = input;
        
        // Jump buffering
        if (input.jump) {
            m_jumpBufferTime = 0.1f;
        }
    }
    
    void update(float32 deltaTime) override {
        // Update status
        m_status.update(deltaTime);
        
        // Update timers
        updateTimers(deltaTime);
        
        // Check ground and wall collision
        checkCollisionState();
        
        // Process movement
        processMovement(deltaTime);
        
        // Process abilities
        processAbilities(deltaTime);
        
        // Update state
        updateState();
        
        // Update physics body
        m_physicsBody->update(deltaTime);
    }
    
    const PlayerStatus& getStatus() const override {
        return m_status;
    }
    
    const PlayerAbilities& getAbilities() const override {
        return m_abilities;
    }
    
    void setAbilities(const PlayerAbilities& abilities) override {
        m_abilities = abilities;
        m_maxJumps = abilities.canDoubleJump ? 2 : 1;
    }
    
    bool isGrounded() const override { return m_grounded; }
    
    bool isTouchingWall() const override { return m_touchingWall; }
    
    int32 getWallDirection() const override { return m_wallDirection; }
    
    bool canJumpNow() const override {
        return (m_grounded || m_coyoteTime > 0.0f) && m_jumpsRemaining > 0;
    }
    
    bool canDoubleJumpNow() const override {
        return m_abilities.canDoubleJump && m_jumpsRemaining > 0 && !m_grounded;
    }
    
    bool canDashNow() const override {
        return m_abilities.canDash && m_canDash && m_dashCooldown <= 0.0f;
    }
    
    void respawn(const Vec2& checkpoint) override {
        setPosition(checkpoint);
        m_physicsBody->setVelocity(Vec2(0, 0));
        m_status.respawn();
        m_state = PlayerState::Idle;
        m_jumpsRemaining = m_maxJumps;
        m_canDash = true;
        m_dashCooldown = 0.0f;
    }
    
    void kill() override {
        m_status.currentHealth = 0;
        m_state = PlayerState::Dead;
    }
    
    void setCheckpoint(const Vec2& checkpoint) {
        m_lastCheckpoint = checkpoint;
    }
    
    Vec2 getLastCheckpoint() const {
        return m_lastCheckpoint;
    }
    
private:
    void updateTimers(float32 deltaTime) {
        // Coyote time (can jump shortly after leaving ground)
        if (m_grounded) {
            m_coyoteTime = 0.15f;
        } else {
            m_coyoteTime -= deltaTime;
        }
        
        // Jump buffer (remember jump input shortly before landing)
        if (m_jumpBufferTime > 0.0f) {
            m_jumpBufferTime -= deltaTime;
        }
        
        // Dash cooldown
        if (m_dashCooldown > 0.0f) {
            m_dashCooldown -= deltaTime;
        }
        
        // Dash timer
        if (m_dashTimer > 0.0f) {
            m_dashTimer -= deltaTime;
            if (m_dashTimer <= 0.0f) {
                // End dash
                Vec2 vel = m_physicsBody->getVelocity();
                m_physicsBody->setVelocity(vel * 0.5f); // Slow down after dash
            }
        }
    }
    
    void checkCollisionState() {
        // This would be implemented by checking collision with terrain
        // For now, we'll use a simple ground check based on velocity
        Vec2 vel = m_physicsBody->getVelocity();
        
        // Simple ground check (in real implementation, use collision detection)
        m_grounded = (vel.y >= -0.1f && vel.y <= 0.1f);
        
        if (m_grounded) {
            m_jumpsRemaining = m_maxJumps;
            m_canDash = true;
        }
        
        // Wall check (simplified)
        m_touchingWall = false;
        m_wallDirection = 0;
    }
    
    void processMovement(float32 deltaTime) {
        Vec2 vel = m_physicsBody->getVelocity();
        float32 targetSpeed = m_input.moveRight || m_input.moveLeft ? 
                            (m_input.moveRight ? 1.0f : -1.0f) * m_abilities.moveSpeed : 0.0f;
        
        // Apply horizontal movement
        if (m_state != PlayerState::Dashing) {
            vel.x = targetSpeed;
            
            // Update facing direction
            if (m_input.moveRight) {
                m_facingDirection = 1;
            } else if (m_input.moveLeft) {
                m_facingDirection = -1;
            }
        }
        
        // Variable jump height
        if (!m_input.jumpHeld && vel.y > 0.0f) {
            vel.y *= 0.5f; // Cut jump short
        }
        
        m_physicsBody->setVelocity(vel);
    }
    
    void processAbilities(float32 deltaTime) {
        // Jump
        if (m_input.jump && (m_jumpBufferTime > 0.0f || canJumpNow())) {
            performJump();
            m_jumpBufferTime = 0.0f;
        }
        
        // Double jump
        if (m_input.jump && canDoubleJumpNow()) {
            performDoubleJump();
        }
        
        // Wall jump
        if (m_input.jump && m_touchingWall && !m_grounded && m_abilities.canWallJump) {
            performWallJump();
        }
        
        // Dash
        if (m_input.dash && canDashNow()) {
            performDash();
        }
        
        // Climbing
        if (m_touchingWall && m_abilities.canClimb) {
            performClimb(deltaTime);
        }
    }
    
    void performJump() {
        Vec2 vel = m_physicsBody->getVelocity();
        vel.y = m_abilities.jumpForce;
        m_physicsBody->setVelocity(vel);
        m_jumpsRemaining--;
        m_state = PlayerState::Jumping;
    }
    
    void performDoubleJump() {
        Vec2 vel = m_physicsBody->getVelocity();
        vel.y = m_abilities.doubleJumpForce;
        m_physicsBody->setVelocity(vel);
        m_jumpsRemaining--;
        m_state = PlayerState::DoubleJumping;
    }
    
    void performWallJump() {
        Vec2 vel = m_physicsBody->getVelocity();
        vel.x = -m_wallDirection * m_abilities.wallJumpForce * 0.7f;
        vel.y = m_abilities.wallJumpForce;
        m_physicsBody->setVelocity(vel);
        m_jumpsRemaining = m_maxJumps; // Reset jumps after wall jump
        m_state = PlayerState::WallJumping;
    }
    
    void performDash() {
        Vec2 vel = m_physicsBody->getVelocity();
        vel.x = m_facingDirection * m_abilities.dashForce;
        vel.y = 0.0f; // Horizontal dash
        m_physicsBody->setVelocity(vel);
        m_dashTimer = m_abilities.dashDuration;
        m_dashCooldown = 1.0f; // 1 second cooldown
        m_canDash = false;
        m_state = PlayerState::Dashing;
    }
    
    void performClimb(float32 deltaTime) {
        if (m_input.moveUp) {
            Vec2 vel = m_physicsBody->getVelocity();
            vel.y = m_abilities.climbSpeed;
            m_physicsBody->setVelocity(vel);
            m_state = PlayerState::Climbing;
        } else if (m_input.moveDown) {
            Vec2 vel = m_physicsBody->getVelocity();
            vel.y = -m_abilities.climbSpeed;
            m_physicsBody->setVelocity(vel);
            m_state = PlayerState::Climbing;
        } else if (m_touchingWall) {
            // Wall slide
            Vec2 vel = m_physicsBody->getVelocity();
            vel.y = std::max(vel.y, -2.0f); // Slow fall
            m_physicsBody->setVelocity(vel);
            m_state = PlayerState::WallSliding;
        }
    }
    
    void updateState() {
        Vec2 vel = m_physicsBody->getVelocity();
        
        if (m_status.isDead()) {
            m_state = PlayerState::Dead;
            return;
        }
        
        if (m_status.invincible && m_status.currentHealth < m_status.maxHealth) {
            m_state = PlayerState::Hurt;
            return;
        }
        
        if (m_dashTimer > 0.0f) {
            m_state = PlayerState::Dashing;
            return;
        }
        
        if (m_grounded) {
            if (std::abs(vel.x) > 0.1f) {
                m_state = PlayerState::Walking;
            } else {
                m_state = PlayerState::Idle;
            }
        } else {
            if (vel.y < 0.0f) {
                m_state = PlayerState::Jumping;
            } else {
                m_state = PlayerState::Falling;
            }
        }
    }
    
    std::shared_ptr<IPhysicsBody> m_physicsBody;
    PlayerState m_state;
    PlayerAbilities m_abilities;
    PlayerStatus m_status;
    PlayerInput m_input;
    
    bool m_grounded;
    bool m_touchingWall;
    int32 m_wallDirection;
    
    int32 m_jumpsRemaining;
    int32 m_maxJumps;
    
    bool m_canDash;
    float32 m_dashCooldown;
    float32 m_dashTimer;
    
    int32 m_facingDirection;
    
    float32 m_coyoteTime;
    float32 m_jumpBufferTime;
    
    Vec2 m_lastCheckpoint;
};

} // namespace APLG
