// ============================================================
// PygamePlatformGenerator.cpp
// ============================================================
// Implementation of the C++ port of the Pygame-Platformer generator
// ============================================================

#include "PygamePlatformGenerator.h"
#include <cmath>
#include <algorithm>
#include <random>

namespace APLG {

namespace {

inline GeneratorPlatform createPlatform(int32 x, int32 y, int32 length, bool isStart = false, bool isEnd = false)
{
    const int32 BOUND_X = 1;
    const int32 BOUND_Y_UPPER = 2;
    const int32 BOUND_Y_LOWER = 1;

    GeneratorPlatform p;
    p.x           = x;
    p.y           = y;
    p.length      = length;
    p.connected   = 0;
    p.bound_x_min = x - BOUND_X;
    p.bound_x_max = x + length;
    p.bound_y_min = y + BOUND_Y_LOWER;
    p.bound_y_max = y - BOUND_Y_UPPER;
    p.isStart     = isStart;
    p.isEnd       = isEnd;
    p.isAlternate = false;
    return p;
}

} // anonymous namespace

bool PygamePlatformGenerator::validPlatform(int32 x, int32 y, int32 length,
                                            const std::vector<GeneratorPlatform>& platforms,
                                            int32 roomWidth, int32 roomHeight)
{
    const int32 BOUND_X = 1;
    const int32 BOUND_Y_UPPER = 2;
    const int32 BOUND_Y_LOWER = 1;

    if (x < 1 || x + length > roomWidth - 1) return false;
    if (y < 2 || y > roomHeight - 3) return false;

    for (const auto& p : platforms) {
        bool cond1 = (x + length - 1 >= p.bound_x_min && x <= p.bound_x_max) &&
                     (y <= p.bound_y_min && y >= p.bound_y_max);
        bool cond2 = (p.x + p.length - 1 >= x - BOUND_X && p.x <= x + length) &&
                     (p.y <= y + BOUND_Y_LOWER && p.y >= y - BOUND_Y_UPPER);
        if (cond1 || cond2) {
            return false;
        }
    }
    return true;
}

float32 PygamePlatformGenerator::jumpReach(int32 dy, float32 tileSize,
                                           float32 jumpStrength,
                                           float32 gravity,
                                           float32 speed)
{
    if (dy < 0) {
        float32 risePx = -static_cast<float32>(dy) * tileSize;
        float32 vy = jumpStrength;
        float32 x = 0.0f;
        float32 y = 0.0f;
        float32 lastX = 0.0f;

        for (int step = 0; step < 300; ++step) {
            vy += gravity;
            x += speed;
            y += vy;

            if (y <= -risePx) {
                lastX = x;
            }
            if (y > 0.0f) break;
        }
        return lastX / tileSize;
    } else {
        float32 dropPx = static_cast<float32>(dy) * tileSize;
        if (dropPx <= 0.0f) {
            float32 vy = jumpStrength;
            float32 x = 0.0f;
            float32 y = 0.0f;
            for (int step = 0; step < 300; ++step) {
                vy += gravity;
                x += speed;
                y += vy;
                if (y >= 0.0f && step > 3) break;
            }
            return x / tileSize;
        }
        float32 t = std::sqrt(2.0f * dropPx / std::max(gravity, 0.001f));
        return (speed * t) / tileSize;
    }
}

bool PygamePlatformGenerator::canReach(const GeneratorPlatform& p1, const GeneratorPlatform& p2,
                                       float32 tileSize, float32 jumpStrength,
                                       float32 gravity, float32 speed)
{
    int32 dy = p2.y - p1.y;
    float32 maxGap = jumpReach(dy, tileSize, jumpStrength, gravity, speed);

    int32 p1Left = p1.x;
    int32 p1Right = p1.x + p1.length - 1;
    int32 p2Left = p2.x;
    int32 p2Right = p2.x + p2.length - 1;

    int32 gapRight = p2Left - p1Right - 1;
    if (gapRight >= 0 && gapRight <= static_cast<int32>(maxGap)) return true;

    int32 gapLeft = p1Left - p2Right - 1;
    if (gapLeft >= 0 && gapLeft <= static_cast<int32>(maxGap)) return true;

    if (!(p2Right < p1Left || p2Left > p1Right)) return true;

    return false;
}

std::vector<PlatformNode> PygamePlatformGenerator::generatePlatforms(const PLEConfig& config)
{
    std::mt19937 rng(config.seed != 0 ? config.seed : 1337);

    auto randInt = [&](int32 low, int32 high) -> int32 {
        if (low > high) std::swap(low, high);
        std::uniform_int_distribution<int32> dist(low, high);
        return dist(rng);
    };

    auto randFloat = [&](float32 low, float32 high) -> float32 {
        std::uniform_real_distribution<float32> dist(low, high);
        return dist(rng);
    };

    std::vector<GeneratorPlatform> platforms;
    std::vector<size_t> activeIndices;

    // Start Platform
    int32 startY = std::clamp(config.height - 4, 3, config.height - 3);
    int32 firstLen = std::clamp(config.maxPlatformWidth, config.minPlatformWidth, config.width - 4);
    platforms.push_back(createPlatform(1, startY, firstLen, true, false));
    activeIndices.push_back(0);

    int32 cursorX = 1 + firstLen;
    int32 cursorY = startY;
    const float32 totalSpan = static_cast<float32>(config.width - 4);

    int32 minY = startY;
    int32 maxY = startY;

    // Primary Unidirectional Path
    while (cursorX < config.width - 3) {
        float32 progress = std::clamp(static_cast<float32>(cursorX) / totalSpan, 0.0f, 1.0f);

        // DDA Scaling
        int32 curMinW = config.minPlatformWidth;
        int32 curMaxW = config.maxPlatformWidth;
        if (config.progressiveDifficulty) {
            float32 wFactor = 1.0f - progress * 0.5f;
            curMaxW = std::max(curMinW, static_cast<int32>(std::round(config.maxPlatformWidth * wFactor)));
        }
        int32 platLen = randInt(curMinW, std::max(curMinW, curMaxW));

        int32 curMaxGap = config.maxGap;
        if (config.progressiveDifficulty) {
            float32 gFactor = 0.5f + progress * 0.5f;
            curMaxGap = std::clamp(static_cast<int32>(std::round(config.maxGap * gFactor)), config.minGap, config.maxGap);
        }
        int32 gap = randInt(config.minGap, curMaxGap);

        // Wave Elevation target
        int32 minAllowedY = 3;
        int32 maxAllowedY = config.height - 4;
        int32 spanY       = maxAllowedY - minAllowedY;
        int32 midY        = (minAllowedY + maxAllowedY) / 2;
        int32 requiredAmp = std::clamp(config.minVerticalProgression, 4, spanY);
        int32 halfAmp     = std::max(2, requiredAmp / 2);
        int32 peakY       = std::max(minAllowedY + 1, midY - halfAmp);
        int32 valleyY     = std::min(maxAllowedY, midY + halfAmp);
        if (valleyY - peakY < requiredAmp) {
            peakY   = std::max(minAllowedY, valleyY - requiredAmp);
            valleyY = std::min(maxAllowedY, peakY + requiredAmp);
        }

        int32 targetY = midY;
        if ((config.seed & 1) == 0) {
            if (progress < 0.20f)      targetY = std::min(maxAllowedY, midY + 1);
            else if (progress < 0.55f) targetY = peakY;
            else if (progress < 0.85f) targetY = valleyY;
            else                       targetY = midY;
        } else {
            if (progress < 0.20f)      targetY = std::max(minAllowedY, midY - 1);
            else if (progress < 0.55f) targetY = valleyY;
            else if (progress < 0.85f) targetY = peakY;
            else                       targetY = midY;
        }

        targetY += randInt(-1, 1);
        targetY = std::clamp(targetY, minAllowedY, maxAllowedY);

        int32 maxStep = std::min(config.maxVerticalChange, 2);
        platLen = std::max(platLen, 4);

        int32 deltaY = std::clamp(targetY - cursorY, -maxStep, maxStep);
        int32 nextX  = cursorX + gap;
        int32 nextY  = std::clamp(cursorY + deltaY, minAllowedY, maxAllowedY);

        if (nextX + config.minPlatformWidth > config.width - 2) {
            nextX   = std::max(cursorX + 1, config.width - 2 - config.minPlatformWidth);
            platLen = config.width - 2 - nextX;
        }

        platLen = std::clamp(platLen, 1, config.width - 2 - nextX);
        if (platLen < 1) break;

        bool isEnd = (nextX + platLen >= config.width - 3);
        GeneratorPlatform newPlat = createPlatform(nextX, nextY, platLen, false, isEnd);

        // Spatial and reachability validation
        if (validPlatform(nextX, nextY, platLen, platforms, config.width, config.height)) {
            size_t parentIdx = activeIndices.empty() ? 0 : activeIndices.back();
            platforms[parentIdx].connected++;
            if (platforms[parentIdx].connected >= 2 && activeIndices.size() > 1) {
                activeIndices.pop_back();
            }
            platforms.push_back(newPlat);
            activeIndices.push_back(platforms.size() - 1);

            cursorX = nextX + platLen;
            cursorY = nextY;
            minY = std::min(minY, nextY);
            maxY = std::max(maxY, nextY);
        } else {
            // Adjust coordinates to fit safely
            nextY = std::clamp(nextY + 1, minAllowedY, maxAllowedY);
            newPlat = createPlatform(nextX, nextY, platLen, false, isEnd);
            platforms.push_back(newPlat);
            cursorX = nextX + platLen;
            cursorY = nextY;
        }

        if (isEnd) break;
    }

    if (!platforms.empty()) {
        platforms.back().isEnd = true;
    }

    // Convert GeneratorPlatform list to PlatformNode vector
    std::vector<PlatformNode> nodes;
    nodes.reserve(platforms.size());
    for (const auto& p : platforms) {
        PlatformNode n;
        n.x           = p.x;
        n.y           = p.y;
        n.width       = p.length;
        n.isStart     = p.isStart;
        n.isEnd       = p.isEnd;
        n.isAlternate = p.isAlternate;
        nodes.push_back(n);
    }

    return nodes;
}

} // namespace APLG
