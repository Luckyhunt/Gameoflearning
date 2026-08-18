#include "PlatformConnectivity.h"
#include "PathFinder.h"

namespace APLG::Validation {

bool PlatformConnectivity::analyze(const LevelData& level, const PlayerCapabilities& caps, std::string& outReason) {
    // 1. Find all standable positions that can be reached from the Spawn point
    std::vector<Vec2i> reachableFromSpawn = PathFinder::getReachableArea(level, level.spawnPosition, caps);

    // 2. Soft-lock validation: every reachable tile must have a path leading to the Exit
    for (const auto& pos : reachableFromSpawn) {
        if (pos == level.exitPosition) {
            continue;
        }
        
        // If the player can stand here, but has no physical path to reach the exit door,
        // it is a dead-end/soft-lock trap.
        if (!PathFinder::isReachable(level, pos, level.exitPosition, caps)) {
            outReason = "Soft-lock trap detected at cell (" + 
                        std::to_string(pos.x) + ", " + std::to_string(pos.y) + 
                        ") - exit is unreachable from this location.";
            return false;
        }
    }

    return true;
}

} // namespace APLG::Validation
