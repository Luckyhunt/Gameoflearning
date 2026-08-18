#pragma once

#include "../Include/Common.h"

namespace APLG::Validation {

/**
 * @brief Player physics capability model
 * 
 * Defines physical parameters of the player to validate level layouts.
 */
struct PlayerCapabilities {
    // Kinematic limits in world pixels/seconds (matching Godot PlayerMovement.gd runtime physics)
    float32 runSpeed = 180.0f;
    float32 jumpVelocity = -420.0f;
    float32 gravity = 800.0f;
    float32 tileSize = 32.0f;

    // Derived discrete metrics in tile units
    int32 maxJumpHeight = 3;       // Maximum height player can reach vertically (3.44 tiles -> 3)
    int32 maxJumpDistance = 5;     // Maximum horizontal gap size player can cross (5.9 tiles -> 5)
    int32 minimumHeadroom = 2;     // Vertical empty spaces needed above platform tiles
    int32 maximumFallDistance = 8; // Maximum vertical height drop the player can survive

    /**
     * @brief Derive discrete tile jump metrics from continuous kinematic physics.
     */
    void deriveFromPhysics(float32 speed, float32 jumpVel, float32 grav, float32 tSize = 32.0f) {
        runSpeed = speed;
        jumpVelocity = jumpVel;
        gravity = grav;
        tileSize = tSize;

        float32 absJumpVel = std::abs(jumpVelocity);
        if (gravity > 0.0f && tileSize > 0.0f) {
            float32 hMaxPx = (absJumpVel * absJumpVel) / (2.0f * gravity);
            maxJumpHeight = static_cast<int32>(std::floor(hMaxPx / tileSize));

            float32 tAir = 2.0f * (absJumpVel / gravity);
            float32 dMaxPx = runSpeed * tAir;
            maxJumpDistance = static_cast<int32>(std::floor(dMaxPx / tileSize));
        }

        maxJumpHeight = std::clamp(maxJumpHeight, 1, 6);
        maxJumpDistance = std::clamp(maxJumpDistance, 1, 8);
    }
};

} // namespace APLG::Validation
