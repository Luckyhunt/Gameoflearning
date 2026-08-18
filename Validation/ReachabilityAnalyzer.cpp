#include "ReachabilityAnalyzer.h"
#include "PathFinder.h"

namespace APLG::Validation {

bool ReachabilityAnalyzer::analyze(const LevelData& level, const PlayerCapabilities& caps, std::string& outReason) {
    // 1. Verify Spawn exists and is within bounds
    if (!PathFinder::isValidPosition(level, level.spawnPosition)) {
        outReason = "Spawn position is out of map bounds.";
        return false;
    }
    
    // 2. Verify Exit exists and is within bounds
    if (!PathFinder::isValidPosition(level, level.exitPosition)) {
        outReason = "Exit position is out of map bounds.";
        return false;
    }

    // 3. Verify Exit is reachable from Spawn
    if (!PathFinder::isReachable(level, level.spawnPosition, level.exitPosition, caps)) {
        outReason = "Exit is unreachable from the spawn point.";
        return false;
    }

    // 4. Verify all generated coin collectibles are reachable from Spawn
    for (const auto& coinPos : level.coinPositions) {
        if (PathFinder::isValidPosition(level, coinPos)) {
            if (!PathFinder::isReachable(level, level.spawnPosition, coinPos, caps)) {
                outReason = "One or more coins are physically unreachable.";
                return false;
            }
        }
    }

    return true;
}

} // namespace APLG::Validation
