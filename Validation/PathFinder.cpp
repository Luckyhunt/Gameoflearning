#include "PathFinder.h"
#include <queue>
#include <cmath>
#include <algorithm>
#include <unordered_set>

namespace APLG::Validation {

bool PathFinder::isValidPosition(const LevelData& level, Vec2i pos) {
    return pos.x >= 0 && pos.x < level.width && 
           pos.y >= 0 && pos.y < level.height;
}

bool PathFinder::isSolid(const LevelData& level, Vec2i pos) {
    if (!isValidPosition(level, pos)) return true; // Borders are solid boundaries
    TileType t = level.tiles[pos.y][pos.x];
    return t == TileType::Solid
        || t == TileType::MovingPlatform
        || t == TileType::FallingPlatform
        || t == TileType::BouncePad
        || t == TileType::IcePlatform
        || t == TileType::OneWayPlatform;
}

bool PathFinder::isPlatform(const LevelData& level, Vec2i pos) {
    if (!isValidPosition(level, pos)) return false;
    TileType t = level.tiles[pos.y][pos.x];
    return t == TileType::Platform
        || t == TileType::MovingPlatform
        || t == TileType::FallingPlatform
        || t == TileType::BouncePad
        || t == TileType::IcePlatform
        || t == TileType::OneWayPlatform;
}

bool PathFinder::isStandable(const LevelData& level, Vec2i pos) {
    if (!isValidPosition(level, pos)) return false;
    if (isSolid(level, pos)) return false;
    
    // In platform games, a position is standable if the tile below is Solid or a Platform
    Vec2i below(pos.x, pos.y + 1);
    return isSolid(level, below) || isPlatform(level, below);
}

bool PathFinder::hasHeadroom(const LevelData& level, Vec2i pos, int32 height) {
    for (int32 h = 0; h < height; ++h) {
        Vec2i check(pos.x, pos.y - h);
        if (isSolid(level, check)) return false;
    }
    return true;
}

bool PathFinder::checkJumpArcClear(const LevelData& level, Vec2i from, Vec2i to, const PlayerCapabilities& caps) {
    int32 dx = to.x - from.x;
    int32 dy = from.y - to.y; // Positive means jumping up

    int32 absDx = std::abs(dx);
    int32 maxH = std::max(caps.maxJumpHeight, dy + 1);

    // Filter impossible jumps
    if (absDx > caps.maxJumpDistance || -dy > caps.maxJumpHeight) {
        return false;
    }

    // Parabolic interpolation steps
    int32 steps = std::max(5, absDx * 2);
    for (int32 i = 0; i <= steps; ++i) {
        float32 t = static_cast<float32>(i) / steps;
        // Parabolic trajectory formula
        float32 yOffset = 4.0f * maxH * t * (1.0f - t) + static_cast<float32>(dy) * t;
        
        int32 currX = from.x + static_cast<int32>(std::round(dx * t));
        int32 currY = from.y - static_cast<int32>(std::round(yOffset));

        Vec2i checkPos(currX, currY);
        
        // Body cannot pass through Solid terrain
        if (isSolid(level, checkPos)) {
            return false;
        }
        // Clearance headroom must be maintained
        if (!hasHeadroom(level, checkPos, caps.minimumHeadroom)) {
            return false;
        }
    }
    return true;
}

std::vector<Vec2i> PathFinder::getPlatformerNeighbors(const LevelData& level, Vec2i pos, const PlayerCapabilities& caps, const std::vector<Vec2i>& allStandable) {
    std::vector<Vec2i> neighbors;

    // 1. Walk transitions (left/right)
    for (int32 dir : {-1, 1}) {
        Vec2i walkPos(pos.x + dir, pos.y);
        if (isStandable(level, walkPos) && hasHeadroom(level, walkPos, caps.minimumHeadroom)) {
            neighbors.push_back(walkPos);
        }
    }

    // 2. Fall transitions (falling off platforms with horizontal drift)
    for (int32 dir : {-1, 1}) {
        Vec2i side(pos.x + dir, pos.y);
        // Fall trigger: moving into walkable space that has no floor under it
        if (isValidPosition(level, side) && !isSolid(level, side) && !isStandable(level, side)) {
            Vec2i fallPos = side;
            int32 fallDist = 0;
            bool hitSolid = false;
            
            while (isValidPosition(level, fallPos) && !hitSolid && fallDist <= caps.maximumFallDistance) {
                if (isStandable(level, fallPos)) {
                    neighbors.push_back(fallPos);
                    break;
                }
                if (isSolid(level, fallPos)) {
                    hitSolid = true;
                    break;
                }
                
                fallPos.y++;
                fallDist++;
                
                // Allow drift (going diagonally down-outwards)
                Vec2i driftPos(fallPos.x + dir, fallPos.y);
                if (isValidPosition(level, driftPos) && !isSolid(level, driftPos) && isStandable(level, driftPos)) {
                    neighbors.push_back(driftPos);
                }
            }
        }
    }

    // 3. Jump transitions (parabolic trajectory validation)
    for (const auto& target : allStandable) {
        if (target == pos) continue;
        int32 dx = target.x - pos.x;
        int32 dy = pos.y - target.y; // Positive means climbing higher
        
        if (std::abs(dx) <= caps.maxJumpDistance && dy <= caps.maxJumpHeight && -dy <= caps.maximumFallDistance) {
            if (checkJumpArcClear(level, pos, target, caps)) {
                neighbors.push_back(target);
            }
        }
    }

    return neighbors;
}

std::vector<Vec2i> PathFinder::findPath(const LevelData& level, Vec2i start, Vec2i end, const PlayerCapabilities& caps) {
    // Bring spawn points down to floor if spawning in empty cells
    Vec2i realStart = start;
    while (isValidPosition(level, realStart) && !isStandable(level, realStart) && !isSolid(level, realStart)) {
        realStart.y++;
    }
    Vec2i realEnd = end;
    while (isValidPosition(level, realEnd) && !isStandable(level, realEnd) && !isSolid(level, realEnd)) {
        realEnd.y++;
    }

    if (!isStandable(level, realStart) || !isStandable(level, realEnd)) {
        return {};
    }

    // Gather all standable cells
    std::vector<Vec2i> allStandable;
    for (int32 y = 0; y < level.height; ++y) {
        for (int32 x = 0; x < level.width; ++x) {
            Vec2i pos(x, y);
            if (isStandable(level, pos)) {
                allStandable.push_back(pos);
            }
        }
    }

    // BFS Search
    std::queue<Vec2i> queue;
    std::vector<std::vector<Vec2i>> parent(level.height, std::vector<Vec2i>(level.width, Vec2i(-1, -1)));
    std::vector<std::vector<bool>> visited(level.height, std::vector<bool>(level.width, false));

    queue.push(realStart);
    visited[realStart.y][realStart.x] = true;

    bool found = false;
    while (!queue.empty()) {
        Vec2i current = queue.front();
        queue.pop();

        if (current == realEnd) {
            found = true;
            break;
        }

        std::vector<Vec2i> neighbors = getPlatformerNeighbors(level, current, caps, allStandable);
        for (const auto& neighbor : neighbors) {
            if (!visited[neighbor.y][neighbor.x]) {
                visited[neighbor.y][neighbor.x] = true;
                parent[neighbor.y][neighbor.x] = current;
                queue.push(neighbor);
            }
        }
    }

    if (!found) return {};

    // Reconstruct path
    std::vector<Vec2i> path;
    Vec2i curr = realEnd;
    while (curr != Vec2i(-1, -1)) {
        path.push_back(curr);
        curr = parent[curr.y][curr.x];
    }
    std::reverse(path.begin(), path.end());
    return path;
}

bool PathFinder::isReachable(const LevelData& level, Vec2i start, Vec2i target, const PlayerCapabilities& caps) {
    std::vector<Vec2i> path = findPath(level, start, target, caps);
    return !path.empty();
}

std::vector<Vec2i> PathFinder::getReachableArea(const LevelData& level, Vec2i start, const PlayerCapabilities& caps) {
    Vec2i realStart = start;
    while (isValidPosition(level, realStart) && !isStandable(level, realStart) && !isSolid(level, realStart)) {
        realStart.y++;
    }

    if (!isStandable(level, realStart)) {
        return {};
    }

    std::vector<Vec2i> allStandable;
    for (int32 y = 0; y < level.height; ++y) {
        for (int32 x = 0; x < level.width; ++x) {
            Vec2i pos(x, y);
            if (isStandable(level, pos)) {
                allStandable.push_back(pos);
            }
        }
    }

    std::vector<Vec2i> reachable;
    std::queue<Vec2i> queue;
    std::vector<std::vector<bool>> visited(level.height, std::vector<bool>(level.width, false));

    queue.push(realStart);
    visited[realStart.y][realStart.x] = true;

    while (!queue.empty()) {
        Vec2i current = queue.front();
        queue.pop();
        reachable.push_back(current);

        std::vector<Vec2i> neighbors = getPlatformerNeighbors(level, current, caps, allStandable);
        for (const auto& neighbor : neighbors) {
            if (!visited[neighbor.y][neighbor.x]) {
                visited[neighbor.y][neighbor.x] = true;
                queue.push(neighbor);
            }
        }
    }

    return reachable;
}

} // namespace APLG::Validation
