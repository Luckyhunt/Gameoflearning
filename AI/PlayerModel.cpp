#include "IPlayerModel.h"
#include "../Utilities/ILogger.h"
#include <algorithm>
#include <cmath>

namespace APLG {

// PlayerModel Implementation
PlayerModel::PlayerModel()
    : m_analysisWindow(100)
    , m_playstyle(Playstyle::Careful)
    , m_confidence(0.0f)
    , m_lastPosition(0, 0)
    , m_totalDistance(0.0f)
    , m_uniquePositions(0)
{
    m_playstyleProbabilities = {
        {Playstyle::Explorer, 0.2f},
        {Playstyle::Speedrunner, 0.2f},
        {Playstyle::Aggressive, 0.2f},
        {Playstyle::Careful, 0.2f},
        {Playstyle::Collector, 0.2f}
    };
}

void PlayerModel::recordAction(PlayerAction action, Vec2 position, float32 timestamp) {
    ActionRecord record(action, timestamp, position);
    m_actionHistory.push_back(record);
    
    // Calculate distance traveled
    float32 dist = (position - m_lastPosition).length();
    m_totalDistance += dist;
    
    // Track unique positions (simplified)
    if ((position - m_lastPosition).length() > 1.0f) {
        m_uniquePositions++;
    }
    
    m_lastPosition = position;
    
    // Keep history within window
    while (m_actionHistory.size() > m_analysisWindow) {
        m_actionHistory.pop_front();
    }
}

void PlayerModel::analyze() {
    LOG_INFO("Analyzing player behavior...");
    
    m_stats = calculateStatistics();
    detectPlaystyle();
    
    LOG_INFO("Playstyle detected: " + std::to_string(static_cast<int>(m_playstyle)));
    LOG_INFO("Confidence: " + std::to_string(m_confidence));
}

BehaviorStats PlayerModel::calculateStatistics() const {
    BehaviorStats stats;
    stats.totalActions = static_cast<int32>(m_actionHistory.size());
    
    if (m_actionHistory.empty()) return stats;
    
    // Count action types
    for (const auto& record : m_actionHistory) {
        switch (record.action) {
            case PlayerAction::Jump:
                stats.jumpCount++;
                break;
            case PlayerAction::DoubleJump:
                stats.doubleJumpCount++;
                break;
            case PlayerAction::Dash:
                stats.dashCount++;
                break;
            case PlayerAction::Attack:
                stats.attackCount++;
                break;
            default:
                break;
        }
    }
    
    // Calculate average movement speed
    float32 totalTime = m_actionHistory.back().timestamp - m_actionHistory.front().timestamp;
    if (totalTime > 0.0f) {
        stats.averageMovementSpeed = m_totalDistance / totalTime;
    }
    
    // Calculate exploration percentage (unique positions / total actions)
    if (stats.totalActions > 0) {
        stats.explorationPercentage = static_cast<float32>(m_uniquePositions) / 
                                       static_cast<float32>(stats.totalActions);
    }
    
    // Calculate combat engagement (attacks / total actions)
    if (stats.totalActions > 0) {
        stats.combatEngagement = static_cast<float32>(stats.attackCount) / 
                                  static_cast<float32>(stats.totalActions);
    }
    
    // Calculate risk taking (dashes + double jumps / total actions)
    int32 riskyActions = stats.dashCount + stats.doubleJumpCount + stats.wallJumpCount;
    if (stats.totalActions > 0) {
        stats.riskTaking = static_cast<float32>(riskyActions) / 
                            static_cast<float32>(stats.totalActions);
    }
    
    // Calculate efficiency (movement speed / time spent idle)
    int32 idleCount = 0;
    for (const auto& record : m_actionHistory) {
        if (record.action == PlayerAction::Idle) {
            idleCount++;
        }
    }
    if (stats.totalActions > 0) {
        float32 idleRatio = static_cast<float32>(idleCount) / static_cast<float32>(stats.totalActions);
        stats.efficiency = stats.averageMovementSpeed * (1.0f - idleRatio);
    }

    return stats;
}

void PlayerModel::detectPlaystyle() {
    // Calculate scores for each playstyle
    std::vector<std::pair<Playstyle, float32>> scores;
    
    for (const auto& [playstyle, _] : m_playstyleProbabilities) {
        float32 score = calculatePlaystyleScore(playstyle);
        scores.push_back({playstyle, score});
    }
    
    // Sort by score
    std::sort(scores.begin(), scores.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Update probabilities
    m_playstyleProbabilities = scores;
    
    // Normalize scores
    float32 total = 0.0f;
    for (auto& [_, score] : m_playstyleProbabilities) {
        total += score;
    }
    for (auto& [_, score] : m_playstyleProbabilities) {
        score /= total;
    }
    
    // Set dominant playstyle
    m_playstyle = m_playstyleProbabilities[0].first;
    m_confidence = m_playstyleProbabilities[0].second;
}

float32 PlayerModel::calculatePlaystyleScore(Playstyle playstyle) {
    float32 score = 0.0f;
    
    switch (playstyle) {
        case Playstyle::Explorer:
            // High exploration, moderate speed, low combat
            score += m_stats.explorationPercentage * 0.4f;
            score += (1.0f - m_stats.combatEngagement) * 0.3f;
            score += (1.0f - m_stats.riskTaking) * 0.2f;
            score += m_stats.efficiency * 0.1f;
            break;
            
        case Playstyle::Speedrunner:
            // High speed, high efficiency, high risk
            score += m_stats.averageMovementSpeed * 0.4f;
            score += m_stats.efficiency * 0.3f;
            score += m_stats.riskTaking * 0.2f;
            score += (1.0f - m_stats.explorationPercentage) * 0.1f;
            break;
            
        case Playstyle::Aggressive:
            // High combat, high risk, moderate speed
            score += m_stats.combatEngagement * 0.4f;
            score += m_stats.riskTaking * 0.3f;
            score += m_stats.averageMovementSpeed * 0.2f;
            score += (1.0f - m_stats.explorationPercentage) * 0.1f;
            break;
            
        case Playstyle::Careful:
            // Low risk, high exploration, low speed
            score += (1.0f - m_stats.riskTaking) * 0.4f;
            score += m_stats.explorationPercentage * 0.3f;
            score += (1.0f - m_stats.averageMovementSpeed) * 0.2f;
            score += (1.0f - m_stats.combatEngagement) * 0.1f;
            break;
            
        case Playstyle::Collector:
            // High exploration, moderate speed, low combat
            score += m_stats.explorationPercentage * 0.5f;
            score += (1.0f - m_stats.combatEngagement) * 0.3f;
            score += m_stats.efficiency * 0.2f;
            break;
    }
    
    return score;
}

void PlayerModel::clear() {
    m_actionHistory.clear();
    m_stats = BehaviorStats();
    m_playstyle = Playstyle::Careful;
    m_confidence = 0.0f;
    m_totalDistance = 0.0f;
    m_uniquePositions = 0;
    m_lastPosition = Vec2(0, 0);
    
    // Reset probabilities
    m_playstyleProbabilities = {
        {Playstyle::Explorer, 0.2f},
        {Playstyle::Speedrunner, 0.2f},
        {Playstyle::Aggressive, 0.2f},
        {Playstyle::Careful, 0.2f},
        {Playstyle::Collector, 0.2f}
    };
}

// AdaptiveGenerator Implementation
GenerationConfig AdaptiveGenerator::getConfigForPlaystyle(Playstyle playstyle,
                                                          const GenerationConfig& baseConfig) {
    GenerationConfig config = baseConfig;
    config.enemyDensity = 0.0f;
    config.hazardDensity = 0.0f;
    
    switch (playstyle) {
        case Playstyle::Explorer:
            config.platformDensity = 0.4f;
            config.coinDensity = 0.15f;
            config.enemyDensity = 0.0f;
            config.hazardDensity = 0.0f;
            config.maxGapSize = 4;
            break;
            
        case Playstyle::Speedrunner:
            config.platformDensity = 0.25f;
            config.coinDensity = 0.05f;
            config.enemyDensity = 0.0f;
            config.hazardDensity = 0.0f;
            config.maxGapSize = 7;
            config.useMovingPlatforms = true;
            break;
            
        case Playstyle::Aggressive:
            config.platformDensity = 0.3f;
            config.coinDensity = 0.08f;
            config.enemyDensity = 0.0f;
            config.hazardDensity = 0.0f;
            config.powerupSpawnRate = 0.02f;
            break;
            
        case Playstyle::Careful:
            config.platformDensity = 0.35f;
            config.coinDensity = 0.1f;
            config.enemyDensity = 0.0f;
            config.hazardDensity = 0.0f;
            config.maxGapSize = 3;
            config.minPlatformWidth = 4;
            break;
            
        case Playstyle::Collector:
            config.platformDensity = 0.3f;
            config.coinDensity = 0.2f;
            config.enemyDensity = 0.0f;
            config.hazardDensity = 0.0f;
            config.powerupSpawnRate = 0.03f;
            break;
    }
    
    return config;
}

GenerationConfig AdaptiveGenerator::adjustConfig(const PlayerModel& model,
                                               const GenerationConfig& baseConfig) {
    Playstyle playstyle = model.getPlaystyle();
    float32 confidence = model.getConfidence();
    
    GenerationConfig config = getConfigForPlaystyle(playstyle, baseConfig);
    
    // Blend with base config based on confidence
    if (confidence < 0.7f) {
        float32 blendFactor = confidence;
        config.platformDensity = baseConfig.platformDensity * (1.0f - blendFactor) + 
                               config.platformDensity * blendFactor;
        config.enemyDensity = baseConfig.enemyDensity * (1.0f - blendFactor) + 
                            config.enemyDensity * blendFactor;
        config.coinDensity = baseConfig.coinDensity * (1.0f - blendFactor) + 
                           config.coinDensity * blendFactor;
    }
    
    return config;
}

std::string AdaptiveGenerator::recommendGenerator(Playstyle playstyle) {
    switch (playstyle) {
        case Playstyle::Explorer:
            return "Room Graph"; // More exploration opportunities
        case Playstyle::Speedrunner:
            return "Random Walk"; // Linear paths
        case Playstyle::Aggressive:
            return "Constraint-Based"; // Combat-focused
        case Playstyle::Careful:
            return "Cellular Automata"; // Predictable caves
        case Playstyle::Collector:
            return "Room Graph"; // More areas to explore
        default:
            return "Cellular Automata";
    }
}

// SessionAnalytics Implementation
void SessionAnalytics::startSession() {
    m_sessionStartTime = 0.0f; // Would use actual time
    m_levelTimes.clear();
    m_levelDeaths.clear();
    m_levelCoins.clear();
    m_playstyleHistory.clear();
    
    LOG_INFO("Session started");
}

void SessionAnalytics::endSession() {
    m_sessionEndTime = 0.0f; // Would use actual time
    
    LOG_INFO("Session ended");
}

void SessionAnalytics::recordLevelCompletion(float32 time, int32 deaths, int32 coins) {
    m_levelTimes.push_back(time);
    m_levelDeaths.push_back(deaths);
    m_levelCoins.push_back(coins);
    
    LOG_INFO("Level completed - Time: " + std::to_string(time) + 
             ", Deaths: " + std::to_string(deaths) + 
             ", Coins: " + std::to_string(coins));
}

SessionAnalytics::SessionStats SessionAnalytics::getSessionStats() const {
    SessionStats stats;
    
    stats.levelsCompleted = static_cast<int32>(m_levelTimes.size());
    
    float32 totalTime = 0.0f;
    int32 totalDeaths = 0;
    int32 totalCoins = 0;
    
    for (size_t i = 0; i < m_levelTimes.size(); ++i) {
        totalTime += m_levelTimes[i];
        totalDeaths += m_levelDeaths[i];
        totalCoins += m_levelCoins[i];
    }
    
    stats.totalPlayTime = totalTime;
    stats.totalDeaths = totalDeaths;
    stats.totalCoins = totalCoins;
    
    if (stats.levelsCompleted > 0) {
        stats.averageCompletionTime = totalTime / static_cast<float32>(stats.levelsCompleted);
    }
    
    // Find dominant playstyle
    if (!m_playstyleHistory.empty()) {
        std::map<Playstyle, int32> counts;
        for (Playstyle p : m_playstyleHistory) {
            counts[p]++;
        }
        
        Playstyle dominant = Playstyle::Careful;
        int32 maxCount = 0;
        for (const auto& [playstyle, count] : counts) {
            if (count > maxCount) {
                maxCount = count;
                dominant = playstyle;
            }
        }
        stats.dominantPlaystyle = dominant;
    }
    
    stats.playstyleHistory = m_playstyleHistory;
    
    return stats;
}

std::string SessionAnalytics::exportToJson() const {
    SessionStats stats = getSessionStats();
    
    std::string json = "{\n";
    json += "  \"levelsCompleted\": " + std::to_string(stats.levelsCompleted) + ",\n";
    json += "  \"totalPlayTime\": " + std::to_string(stats.totalPlayTime) + ",\n";
    json += "  \"totalDeaths\": " + std::to_string(stats.totalDeaths) + ",\n";
    json += "  \"totalCoins\": " + std::to_string(stats.totalCoins) + ",\n";
    json += "  \"averageCompletionTime\": " + std::to_string(stats.averageCompletionTime) + ",\n";
    json += "  \"dominantPlaystyle\": " + std::to_string(static_cast<int>(stats.dominantPlaystyle)) + "\n";
    json += "}";
    
    return json;
}

} // namespace APLG
