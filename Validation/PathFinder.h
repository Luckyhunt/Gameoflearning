#pragma once

#include "../Include/Common.h"
#include "PlayerCapabilities.h"
#include <vector>

namespace APLG::Validation {

class PathFinder {
public:
    /**
     * @brief Find path from start to end using platformer kinematics.
     * @param level Level data
     * @param start Start position
     * @param end End position
     * @param caps Player capabilities model
     * @return Path as vector of positions, empty if no path
     */
    static std::vector<Vec2i> findPath(const LevelData& level, Vec2i start, Vec2i end, const PlayerCapabilities& caps);

    /**
     * @brief Check if target position is reachable from start
     */
    static bool isReachable(const LevelData& level, Vec2i start, Vec2i target, const PlayerCapabilities& caps);

    /**
     * @brief Get all reachable standable positions from start
     */
    static std::vector<Vec2i> getReachableArea(const LevelData& level, Vec2i start, const PlayerCapabilities& caps);

    // Grid status helpers
    static bool isValidPosition(const LevelData& level, Vec2i pos);
    static bool isStandable(const LevelData& level, Vec2i pos);
    static bool isSolid(const LevelData& level, Vec2i pos);
    static bool isPlatform(const LevelData& level, Vec2i pos);
    static bool hasHeadroom(const LevelData& level, Vec2i pos, int32 height);

private:
    static std::vector<Vec2i> getPlatformerNeighbors(const LevelData& level, Vec2i pos, const PlayerCapabilities& caps, const std::vector<Vec2i>& allStandable);
    static bool checkJumpArcClear(const LevelData& level, Vec2i from, Vec2i to, const PlayerCapabilities& caps);
};

} // namespace APLG::Validation
