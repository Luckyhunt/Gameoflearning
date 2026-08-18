#pragma once

#include "../Include/Common.h"
#include <vector>
#include <functional>

namespace APLG {

/**
 * @brief Difficulty parameters for level generation
 * 
 * These parameters are adjusted based on player performance.
 */
struct DifficultyParams {
    float32 enemyCountMultiplier;
    float32 enemySpeedMultiplier;
    float32 hazardDensityMultiplier;
    bool useMovingPlatforms;
    float32 platformWidthMultiplier;
    float32 jumpDistanceMultiplier;
    float32 coinDensityMultiplier;
    float32 powerupSpawnRateMultiplier;
    int32 maxGapSize;
    
    DifficultyParams()
        : enemyCountMultiplier(0.0f)
        , enemySpeedMultiplier(0.0f)
        , hazardDensityMultiplier(0.0f)
        , useMovingPlatforms(false)
        , platformWidthMultiplier(1.0f)
        , jumpDistanceMultiplier(1.0f)
        , coinDensityMultiplier(1.0f)
        , powerupSpawnRateMultiplier(1.0f)
        , maxGapSize(5)
    {
    }
    
    /**
     * @brief Apply difficulty level to parameters
     */
    void applyDifficultyLevel(DifficultyLevel level) {
        switch (level) {
            case DifficultyLevel::Easy:
                enemyCountMultiplier = 0.0f;
                enemySpeedMultiplier = 0.0f;
                hazardDensityMultiplier = 0.0f;
                useMovingPlatforms = false;
                platformWidthMultiplier = 1.3f;
                jumpDistanceMultiplier = 0.8f;
                coinDensityMultiplier = 1.5f;
                powerupSpawnRateMultiplier = 2.0f;
                maxGapSize = 3;
                break;
                
            case DifficultyLevel::Normal:
                enemyCountMultiplier = 0.0f;
                enemySpeedMultiplier = 0.0f;
                hazardDensityMultiplier = 0.0f;
                useMovingPlatforms = false;
                platformWidthMultiplier = 1.0f;
                jumpDistanceMultiplier = 1.0f;
                coinDensityMultiplier = 1.0f;
                powerupSpawnRateMultiplier = 1.0f;
                maxGapSize = 5;
                break;
                
            case DifficultyLevel::Hard:
                enemyCountMultiplier = 0.0f;
                enemySpeedMultiplier = 0.0f;
                hazardDensityMultiplier = 0.0f;
                useMovingPlatforms = true;
                platformWidthMultiplier = 0.8f;
                jumpDistanceMultiplier = 1.2f;
                coinDensityMultiplier = 0.8f;
                powerupSpawnRateMultiplier = 0.5f;
                maxGapSize = 6;
                break;
                
            case DifficultyLevel::Expert:
                enemyCountMultiplier = 0.0f;
                enemySpeedMultiplier = 0.0f;
                hazardDensityMultiplier = 0.0f;
                useMovingPlatforms = true;
                platformWidthMultiplier = 0.6f;
                jumpDistanceMultiplier = 1.4f;
                coinDensityMultiplier = 0.5f;
                powerupSpawnRateMultiplier = 0.3f;
                maxGapSize = 7;
                break;
                
            case DifficultyLevel::Nightmare:
                enemyCountMultiplier = 0.0f;
                enemySpeedMultiplier = 0.0f;
                hazardDensityMultiplier = 0.0f;
                useMovingPlatforms = true;
                platformWidthMultiplier = 0.5f;
                jumpDistanceMultiplier = 1.6f;
                coinDensityMultiplier = 0.3f;
                powerupSpawnRateMultiplier = 0.1f;
                maxGapSize = 8;
                break;
        }
    }
};

/**
 * @brief Skill score calculator
 * 
 * Calculates player skill based on performance metrics.
 */
class SkillScoreCalculator {
public:
    /**
     * @brief Calculate skill score from metrics
     * @param metrics Player performance metrics
     * @return Calculated skill score
     */
    static SkillScore calculate(const PlayerMetrics& metrics);
    
    /**
     * @brief Set custom weights for skill calculation
     */
    static void setWeights(float32 deathsWeight, float32 timeWeight, 
                          float32 accuracyWeight, float32 hitRateWeight);
    
private:
    static float32 normalize(float32 value, float32 min, float32 max);
    
    static float32 s_deathsWeight;
    static float32 s_timeWeight;
    static float32 s_accuracyWeight;
    static float32 s_hitRateWeight;
};

/**
 * @brief Difficulty manager
 * 
 * Manages adaptive difficulty based on player performance.
 */
class IDifficultyManager {
public:
    virtual ~IDifficultyManager() = default;
    
    /**
     * @brief Update difficulty based on player metrics
     * @param metrics Player performance metrics
     */
    virtual void updateDifficulty(const PlayerMetrics& metrics) = 0;
    
    /**
     * @brief Get current difficulty level
     */
    virtual DifficultyLevel getCurrentDifficulty() const = 0;
    
    /**
     * @brief Get current difficulty parameters
     */
    virtual DifficultyParams getCurrentParams() const = 0;
    
    /**
     * @brief Set difficulty manually
     */
    virtual void setDifficulty(DifficultyLevel level) = 0;
    
    /**
     * @brief Get skill score
     */
    virtual SkillScore getSkillScore() const = 0;
    
    /**
     * @brief Enable/disable adaptive difficulty
     */
    virtual void setAdaptive(bool enabled) = 0;
    
    /**
     * @brief Check if adaptive difficulty is enabled
     */
    virtual bool isAdaptive() const = 0;
    
    /**
     * @brief Register difficulty change callback
     */
    virtual void setDifficultyChangeCallback(DifficultyCallback callback) = 0;
};

/**
 * @brief Concrete implementation of difficulty manager
 */
class DifficultyManager : public IDifficultyManager {
public:
    DifficultyManager();
    
    void updateDifficulty(const PlayerMetrics& metrics) override;
    DifficultyLevel getCurrentDifficulty() const override { return m_currentDifficulty; }
    DifficultyParams getCurrentParams() const override { return m_currentParams; }
    void setDifficulty(DifficultyLevel level) override;
    SkillScore getSkillScore() const override { return m_skillScore; }
    void setAdaptive(bool enabled) override { m_adaptive = enabled; }
    bool isAdaptive() const override { return m_adaptive; }
    void setDifficultyChangeCallback(DifficultyCallback callback) override { m_callback = callback; }
    
    /**
     * @brief Get difficulty history
     */
    const std::vector<std::pair<float32, DifficultyLevel>>& getHistory() const {
        return m_history;
    }
    
    /**
     * @brief Clear difficulty history
     */
    void clearHistory() {
        m_history.clear();
    }
    
    /**
     * @brief Set adaptation rate (how quickly difficulty changes)
     * @param rate 0.0 to 1.0, higher = faster adaptation
     */
    void setAdaptationRate(float32 rate) {
        m_adaptationRate = std::clamp(rate, 0.0f, 1.0f);
    }
    
private:
    void adjustDifficulty(float32 skillScore);
    void notifyDifficultyChange();
    
    DifficultyLevel m_currentDifficulty;
    DifficultyParams m_currentParams;
    SkillScore m_skillScore;
    bool m_adaptive;
    float32 m_adaptationRate;
    DifficultyCallback m_callback;
    
    std::vector<std::pair<float32, DifficultyLevel>> m_history; // (skill, difficulty)
};

/**
 * @brief Difficulty presets for different playstyles
 */
class DifficultyPresets {
public:
    /**
     * @brief Get preset for playstyle
     */
    static DifficultyParams getPresetForPlaystyle(Playstyle playstyle);
    
    /**
     * @brief Get preset for specific difficulty level
     */
    static DifficultyParams getPresetForLevel(DifficultyLevel level);
    
    /**
     * @brief Create custom preset
     */
    static DifficultyParams createCustomPreset(
        float32 enemyCount, float32 enemySpeed, float32 hazardDensity,
        bool movingPlatforms, float32 platformWidth, float32 jumpDist,
        float32 coinDensity, float32 powerupRate, int32 maxGap);
};

} // namespace APLG
