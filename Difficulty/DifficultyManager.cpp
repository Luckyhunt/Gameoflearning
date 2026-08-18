#include "IDifficultyManager.h"
#include "../Utilities/ILogger.h"
#include <algorithm>
#include <cmath>

namespace APLG {

// Static member initialization
float32 SkillScoreCalculator::s_deathsWeight = 0.0f;
float32 SkillScoreCalculator::s_timeWeight = 0.40f;
float32 SkillScoreCalculator::s_accuracyWeight = 0.30f;
float32 SkillScoreCalculator::s_hitRateWeight = 0.30f;

SkillScore SkillScoreCalculator::calculate(const PlayerMetrics& metrics) {
    SkillScore score;
    
    // Normalize completion time (faster = higher score)
    float32 normalizedTime = normalize(metrics.completionTime, 30.0f, 300.0f);
    score.overall += (1.0f - normalizedTime) * s_timeWeight;
    
    // Normalize jump accuracy (higher = better)
    float32 normalizedAccuracy = normalize(metrics.jumpAccuracy, 0.0f, 1.0f);
    score.overall += normalizedAccuracy * s_accuracyWeight;
    
    // Normalize enemy hit rate / efficiency (higher = better)
    float32 normalizedHitRate = normalize(metrics.enemyHitRate, 0.0f, 1.0f);
    score.overall += normalizedHitRate * s_hitRateWeight;
    
    // Calculate sub-scores
    score.combat = normalizedHitRate * 1.0f;
    score.platforming = normalizedAccuracy * 0.6f + (1.0f - static_cast<float32>(metrics.platformMisses) / 50.0f) * 0.4f;
    score.exploration = static_cast<float32>(metrics.coinsCollected) / 100.0f;
    score.speed = (1.0f - normalizedTime) * 0.8f + normalizedAccuracy * 0.2f;
    
    // Clamp overall score
    score.overall = std::clamp(score.overall, 0.0f, 1.0f);
    
    return score;
}

void SkillScoreCalculator::setWeights(float32 deathsWeight, float32 timeWeight,
                                      float32 accuracyWeight, float32 hitRateWeight) {
    (void)deathsWeight;
    float32 total = timeWeight + accuracyWeight + hitRateWeight;
    s_deathsWeight = 0.0f;
    s_timeWeight = timeWeight / total;
    s_accuracyWeight = accuracyWeight / total;
    s_hitRateWeight = hitRateWeight / total;
}

float32 SkillScoreCalculator::normalize(float32 value, float32 min, float32 max) {
    if (max - min < 0.0001f) return 0.5f;
    return std::clamp((value - min) / (max - min), 0.0f, 1.0f);
}

// DifficultyManager Implementation
DifficultyManager::DifficultyManager()
    : m_currentDifficulty(DifficultyLevel::Normal)
    , m_adaptive(true)
    , m_adaptationRate(0.5f)
    , m_callback(nullptr)
{
    m_currentParams.applyDifficultyLevel(m_currentDifficulty);
    m_skillScore.overall = 0.5f;
}

void DifficultyManager::updateDifficulty(const PlayerMetrics& metrics) {
    if (!m_adaptive) return;
    
    LOG_INFO("Updating difficulty based on player performance...");
    
    // Calculate skill score
    m_skillScore = SkillScoreCalculator::calculate(metrics);
    
    LOG_INFO("Skill score: " + std::to_string(m_skillScore.overall));
    
    // Adjust difficulty based on skill
    adjustDifficulty(m_skillScore.overall);
    
    // Record history
    m_history.push_back({m_skillScore.overall, m_currentDifficulty});
    
    // Keep history manageable
    if (m_history.size() > 100) {
        m_history.erase(m_history.begin());
    }
}

void DifficultyManager::adjustDifficulty(float32 skillScore) {
    DifficultyLevel newDifficulty = m_currentDifficulty;
    
    // Skill score thresholds for difficulty levels
    const float32 NIGHTMARE_THRESHOLD = 0.9f;
    const float32 EXPERT_THRESHOLD = 0.75f;
    const float32 HARD_THRESHOLD = 0.6f;
    const float32 NORMAL_THRESHOLD = 0.4f;
    const float32 EASY_THRESHOLD = 0.2f;
    
    // Apply adaptation rate for smooth transitions
    // If adaptation rate is low, require more extreme skill changes to adjust difficulty
    
    if (skillScore >= NIGHTMARE_THRESHOLD) {
        newDifficulty = DifficultyLevel::Nightmare;
    } else if (skillScore >= EXPERT_THRESHOLD) {
        newDifficulty = DifficultyLevel::Expert;
    } else if (skillScore >= HARD_THRESHOLD) {
        newDifficulty = DifficultyLevel::Hard;
    } else if (skillScore >= NORMAL_THRESHOLD) {
        newDifficulty = DifficultyLevel::Normal;
    } else if (skillScore >= EASY_THRESHOLD) {
        newDifficulty = DifficultyLevel::Easy;
    } else {
        newDifficulty = DifficultyLevel::Easy;
    }
    
    // Only change difficulty if significantly different
    if (newDifficulty != m_currentDifficulty) {
        // Check if change is warranted based on adaptation rate
        int32 currentLevel = static_cast<int32>(m_currentDifficulty);
        int32 newLevel = static_cast<int32>(newDifficulty);
        
        // With low adaptation rate, only change if skill is far from current difficulty
        float32 currentDifficultyCenter = (currentLevel + 1) / 5.0f;
        float32 skillDifference = std::abs(skillScore - currentDifficultyCenter);
        
        if (skillDifference > (1.0f - m_adaptationRate) * 0.3f) {
            setDifficulty(newDifficulty);
        }
    }
}

void DifficultyManager::setDifficulty(DifficultyLevel level) {
    if (m_currentDifficulty == level) return;
    
    LOG_INFO("Difficulty changed to: " + std::to_string(static_cast<int>(level)));
    
    m_currentDifficulty = level;
    m_currentParams.applyDifficultyLevel(level);
    
    notifyDifficultyChange();
}

void DifficultyManager::notifyDifficultyChange() {
    if (m_callback) {
        m_callback(m_currentDifficulty);
    }
}

// DifficultyPresets Implementation
DifficultyParams DifficultyPresets::getPresetForPlaystyle(Playstyle playstyle) {
    DifficultyParams params;
    
    switch (playstyle) {
        case Playstyle::Explorer:
            // More coins, secrets, less combat
            params.enemyCountMultiplier = 0.7f;
            params.coinDensityMultiplier = 1.5f;
            params.powerupSpawnRateMultiplier = 1.5f;
            params.platformWidthMultiplier = 1.2f;
            break;
            
        case Playstyle::Speedrunner:
            // Faster enemies, more hazards, fewer coins
            params.enemySpeedMultiplier = 1.5f;
            params.hazardDensityMultiplier = 1.3f;
            params.coinDensityMultiplier = 0.7f;
            params.jumpDistanceMultiplier = 1.3f;
            params.platformWidthMultiplier = 0.9f;
            break;
            
        case Playstyle::Aggressive:
            // More enemies, more combat-focused
            params.enemyCountMultiplier = 1.5f;
            params.enemySpeedMultiplier = 1.2f;
            params.powerupSpawnRateMultiplier = 1.3f;
            params.coinDensityMultiplier = 0.8f;
            break;
            
        case Playstyle::Careful:
            // Easier platforming, more checkpoints
            params.enemyCountMultiplier = 0.8f;
            params.hazardDensityMultiplier = 0.7f;
            params.platformWidthMultiplier = 1.3f;
            params.maxGapSize = 4;
            break;
            
        case Playstyle::Collector:
            // Maximum coins and collectables
            params.coinDensityMultiplier = 2.0f;
            params.powerupSpawnRateMultiplier = 2.0f;
            params.enemyCountMultiplier = 0.9f;
            break;
    }
    
    return params;
}

DifficultyParams DifficultyPresets::getPresetForLevel(DifficultyLevel level) {
    DifficultyParams params;
    params.applyDifficultyLevel(level);
    return params;
}

DifficultyParams DifficultyPresets::createCustomPreset(
    float32 enemyCount, float32 enemySpeed, float32 hazardDensity,
    bool movingPlatforms, float32 platformWidth, float32 jumpDist,
    float32 coinDensity, float32 powerupRate, int32 maxGap)
{
    DifficultyParams params;
    params.enemyCountMultiplier = enemyCount;
    params.enemySpeedMultiplier = enemySpeed;
    params.hazardDensityMultiplier = hazardDensity;
    params.useMovingPlatforms = movingPlatforms;
    params.platformWidthMultiplier = platformWidth;
    params.jumpDistanceMultiplier = jumpDist;
    params.coinDensityMultiplier = coinDensity;
    params.powerupSpawnRateMultiplier = powerupRate;
    params.maxGapSize = maxGap;
    return params;
}

} // namespace APLG
