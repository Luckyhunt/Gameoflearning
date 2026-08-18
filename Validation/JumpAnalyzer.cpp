#include "JumpAnalyzer.h"
#include "PathFinder.h"
#include <cmath>
#include <algorithm>

namespace APLG::Validation {

bool JumpAnalyzer::analyze(const LevelData& level, const PlayerCapabilities& caps, std::string& outReason) {
    // 1. Get the solution path
    std::vector<Vec2i> path = PathFinder::findPath(level, level.spawnPosition, level.exitPosition, caps);
    if (path.empty()) {
        outReason = "No solution path exists to validate jumps.";
        return false;
    }

    // 2. Validate headroom and jump limits along the solution path
    for (size_t i = 0; i < path.size(); ++i) {
        Vec2i pos = path[i];

        // Headroom clearance check
        if (!PathFinder::hasHeadroom(level, pos, caps.minimumHeadroom)) {
            outReason = "Ceiling headroom constraint violated at cell (" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ").";
            return false;
        }

        // Jump distance/height limit check between successive steps
        if (i < path.size() - 1) {
            Vec2i next = path[i + 1];
            int32 dx = next.x - pos.x;
            int32 dy = pos.y - next.y; // Positive means next is higher (jumping up)

            if (std::abs(dx) > caps.maxJumpDistance) {
                outReason = "Jump distance gap too wide between (" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ") and (" + std::to_string(next.x) + ", " + std::to_string(next.y) + ").";
                return false;
            }

            if (dy > caps.maxJumpHeight) {
                outReason = "Jump height target too high between (" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ") and (" + std::to_string(next.x) + ", " + std::to_string(next.y) + ").";
                return false;
            }
        }
    }

    return true;
}

} // namespace APLG::Validation
