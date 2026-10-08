#pragma once

#include "../Include/Common.h"
#include <cmath>

namespace APLG {

/**
 * @brief Physics controller for managing variable and inverted gravity dynamics.
 */
class AntigravityController {
public:
    AntigravityController()
        : m_gravityMagnitude(980.0f)
        , m_gravityDirection(0.0f, -1.0f) // Standard downward gravity in screen/world space
        , m_inverted(false)
    {
    }

    AntigravityController(float32 gravityMagnitude, const Vec2& direction)
        : m_gravityMagnitude(gravityMagnitude)
        , m_gravityDirection(direction.normalized())
        , m_inverted(false)
    {
    }

    /**
     * @brief Set the magnitude of gravity
     */
    void setGravityMagnitude(float32 magnitude) {
        m_gravityMagnitude = magnitude;
    }

    float32 getGravityMagnitude() const {
        return m_gravityMagnitude;
    }

    /**
     * @brief Invert the gravity direction (180 degree flip)
     */
    void invertGravity() {
        m_gravityDirection = m_gravityDirection * -1.0f;
        m_inverted = !m_inverted;
    }

    /**
     * @brief Set an arbitrary gravity direction vector
     */
    void setGravityDirection(const Vec2& direction) {
        if (direction.lengthSquared() > 0.0001f) {
            m_gravityDirection = direction.normalized();
        }
    }

    Vec2 getGravityDirection() const {
        return m_gravityDirection;
    }

    /**
     * @brief Get full gravity acceleration vector (magnitude * direction)
     */
    Vec2 getGravityVector() const {
        return m_gravityDirection * m_gravityMagnitude;
    }

    bool isInverted() const {
        return m_inverted;
    }

    /**
     * @brief Decompose velocity into components parallel and perpendicular to gravity
     */
    void decomposeVelocity(const Vec2& velocity, Vec2& outParallel, Vec2& outPerpendicular) const {
        float32 parallelMagnitude = velocity.dot(m_gravityDirection);
        outParallel = m_gravityDirection * parallelMagnitude;
        outPerpendicular = velocity - outParallel;
    }

    /**
     * @brief Calculate jump impulse vector opposing current gravity direction
     */
    Vec2 calculateJumpImpulse(float32 jumpSpeed) const {
        return m_gravityDirection * (-jumpSpeed);
    }

    /**
     * @brief Check if a surface normal constitutes a walkable floor relative to gravity direction
     */
    bool isGroundedOnSurface(const Vec2& surfaceNormal, float32 maxSlopeAngleDegrees = 45.0f) const {
        if (surfaceNormal.lengthSquared() < 0.0001f) return false;
        Vec2 norm = surfaceNormal.normalized();
        // Floor normal opposes gravity direction: dot(norm, gravityDir) <= -cos(maxSlopeAngle)
        float32 dotProd = norm.dot(m_gravityDirection);
        float32 minCos = std::cos((180.0f - maxSlopeAngleDegrees) * 3.14159265f / 180.0f);
        return dotProd <= minCos;
    }

    /**
     * @brief Calculate target rotation angle (in radians) for sprite/collider surface alignment
     */
    float32 calculateTargetRotation() const {
        // Standard standing: facing up when gravity is down (0, -1) -> rotation 0
        // Facing opposite to gravity direction
        Vec2 upVector = m_gravityDirection * -1.0f;
        return std::atan2(upVector.x, -upVector.y);
    }

private:
    float32 m_gravityMagnitude;
    Vec2 m_gravityDirection;
    bool m_inverted;
};

} // namespace APLG
