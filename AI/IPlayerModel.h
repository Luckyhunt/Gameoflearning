#pragma once

#include "../Include/Common.h"
#include <vector>
#include <deque>

namespace APLG {

/**
 * @brief Player action types for analytics
 */
enum class PlayerAction {
    MoveLeft,
    MoveRight,
    Jump,
    DoubleJump,
    WallJump,
    Dash,
    ClimbUp,
    ClimbDown,
    Attack,
    Interact,
    Idle
};

/**
 * @brief Recorded player action with timestamp
 */
struct ActionRecord {
    PlayerAction action;
    float32 timestamp;
    Vec2 position;
    
    ActionRecord() : action(PlayerAction::Idle), timestamp(0.0f), position(0, 0) {}
    ActionRecord(PlayerAction a, float32 t, Vec2 p) : action(a), timestamp(t), position(p) {}
};

/**
 * @brief Player behavior statistics
 */
struct BehaviorStats {
    int32 totalActions;
    int32 jumpCount;
    int32 doubleJumpCount;
    int32 wallJumpCount;
    int32 dashCount;
    int32 attackCount;
    float32 averageMovementSpeed;
    float32 explorationPercentage;
    float32 combatEngagement;
    float32 riskTaking;
    float32 efficiency;
    
    BehaviorStats()
        : totalActions(0)
        , jumpCount(0)
        , doubleJumpCount(0)
        , wallJumpCount(0)
        , dashCount(0)
        , attackCount(0)
        , averageMovementSpeed(0.0f)
        , explorationPercentage(0.0f)
        , combatEngagement(0.0f)
        , riskTaking(0.0f)
        , efficiency(0.0f)
    {
    }
};

/**
 * @brief Player model interface
 * 
 * Analyzes player behavior to determine playstyle and preferences.
 */
class IPlayerModel {
public:
    virtual ~IPlayerModel() = default;
    
    /**
     * @brief Record a player action
     */
    virtual void recordAction(PlayerAction action, Vec2 position, float32 timestamp) = 0;
    
    /**
     * @brief Analyze recorded behavior
     */
    virtual void analyze() = 0;
    
    /**
     * @brief Get detected playstyle
     */
    virtual Playstyle getPlaystyle() const = 0;
    
    /**
     * @brief Get behavior statistics
     */
    virtual BehaviorStats getStatistics() const = 0;
    
    /**
     * @brief Clear recorded data
     */
    virtual void clear() = 0;
    
    /**
     * @brief Set analysis window (number of recent actions to consider)
     */
    virtual void setAnalysisWindow(size_t windowSize) = 0;
};

/**
 * @brief Concrete player model implementation
 * 
 * Uses statistical analysis to determine player playstyle.
 */
class PlayerModel : public IPlayerModel {
public:
    PlayerModel();
    
    void recordAction(PlayerAction action, Vec2 position, float32 timestamp) override;
    void analyze() override;
    Playstyle getPlaystyle() const override { return m_playstyle; }
    BehaviorStats getStatistics() const override { return calculateStatistics(); }
    void clear() override;
    void setAnalysisWindow(size_t windowSize) override { m_analysisWindow = windowSize; }
    
    /**
     * @brief Get confidence in playstyle detection (0.0 to 1.0)
     */
    float32 getConfidence() const { return m_confidence; }
    
    /**
     * @brief Get playstyle probabilities
     */
    const std::vector<std::pair<Playstyle, float32>>& getPlaystyleProbabilities() const {
        return m_playstyleProbabilities;
    }
    
private:
    BehaviorStats calculateStatistics() const;
    void detectPlaystyle();
    float32 calculatePlaystyleScore(Playstyle playstyle);
    
    std::deque<ActionRecord> m_actionHistory;
    size_t m_analysisWindow;
    
    Playstyle m_playstyle;
    float32 m_confidence;
    BehaviorStats m_stats;
    std::vector<std::pair<Playstyle, float32>> m_playstyleProbabilities;
    
    Vec2 m_lastPosition;
    float32 m_totalDistance;
    int32 m_uniquePositions;
};

/**
 * @brief Adaptive generator based on player model
 * 
 * Adjusts generation parameters based on detected playstyle.
 */
class AdaptiveGenerator {
public:
    /**
     * @brief Get generation parameters for playstyle
     */
    static GenerationConfig getConfigForPlaystyle(Playstyle playstyle, 
                                                   const GenerationConfig& baseConfig);
    
    /**
     * @brief Adjust config based on player model
     */
    static GenerationConfig adjustConfig(const PlayerModel& model,
                                       const GenerationConfig& baseConfig);
    
    /**
     * @brief Recommend generator algorithm for playstyle
     */
    static std::string recommendGenerator(Playstyle playstyle);
};

/**
 * @brief Session analytics
 * 
 * Tracks player behavior across entire play session.
 */
class SessionAnalytics {
public:
    /**
     * @brief Start a new session
     */
    void startSession();
    
    /**
     * @brief End current session
     */
    void endSession();
    
    /**
     * @brief Record level completion
     */
    void recordLevelCompletion(float32 time, int32 deaths, int32 coins);
    
    /**
     * @brief Get session statistics
     */
    struct SessionStats {
        int32 levelsCompleted;
        float32 totalPlayTime;
        int32 totalDeaths;
        int32 totalCoins;
        float32 averageCompletionTime;
        Playstyle dominantPlaystyle;
        std::vector<Playstyle> playstyleHistory;
    };
    
    SessionStats getSessionStats() const;
    
    /**
     * @brief Export session data to JSON
     */
    std::string exportToJson() const;
    
private:
    float32 m_sessionStartTime;
    float32 m_sessionEndTime;
    std::vector<float32> m_levelTimes;
    std::vector<int32> m_levelDeaths;
    std::vector<int32> m_levelCoins;
    std::vector<Playstyle> m_playstyleHistory;
};

} // namespace APLG
