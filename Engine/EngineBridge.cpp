// ============================================================
// EngineBridge.cpp
//
// Implementation of the GDExtension boundary class.
// Declared at global scope to satisfy the GDCLASS macro —
// see EngineBridge.h for the full explanation.
//
// Responsibilities:
//   - Own the APLG::IEngine instance via RAII
//   - Translate Godot types <-> C++ types
//   - Delegate all logic to APLG:: engine classes
//   - Register all Godot-callable methods via _bind_methods()
//
// What does NOT belong here:
//   - Game logic of any kind
//   - Difficulty calculations
//   - Procedural generation algorithms
//   - Analytics processing
// ============================================================

#include "EngineBridge.h"
#include "../Generation/IGenerator.h"
#include "../Utilities/Json.h"
#include "../Validation/LevelValidator.h"
#include "../Validation/PathFinder.h"
#include "../Validation/DifficultyEstimator.h"
#include "../Validation/MetricsCollector.h"
#include "../AI/IPlayerModel.h"
#include "PlatformerLevelEngine.h"
#include "LevelGenerationPipeline.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <sstream>
#include <chrono>

// ──────────────────────────────────────────────────────────────
// Construction / Destruction
// ──────────────────────────────────────────────────────────────

EngineBridge::EngineBridge()
    : m_engine(APLG::EngineFactory::createEngine())
{
    // Engine is created but NOT initialized here.
    // Initialization happens in _ready() when the node
    // enters the Godot scene tree and all Godot services are live.
}

EngineBridge::~EngineBridge()
{
    // m_engine (unique_ptr) destructs here -> calls Engine::shutdown().
    // RAII: no manual cleanup needed.
}

// ──────────────────────────────────────────────────────────────
// Godot Lifecycle
// ──────────────────────────────────────────────────────────────

void EngineBridge::_ready()
{
    if (m_engine) {
        m_engine->initialize();
    }

    // Enable adaptive difficulty by default
    m_difficultyManager.setAdaptive(true);
    m_difficultyManager.setAdaptationRate(0.5f);

    godot::UtilityFunctions::print(
        "EngineBridge: C++ engine initialized. Version: ", get_engine_version());
}

// ──────────────────────────────────────────────────────────────
// Milestone 1: Verification Methods
// ──────────────────────────────────────────────────────────────

godot::String EngineBridge::hello() const
{
    return godot::String("APLG Engine v1.0 operational. GDExtension bridge active.");
}

godot::String EngineBridge::get_engine_version() const
{
    return godot::String("1.0.0");
}

int EngineBridge::add(int a, int b) const
{
    // Proves integer round-trip: GDScript int -> C++ int -> back.
    return a + b;
}

// ──────────────────────────────────────────────────────────────
// Internal helper: run generation + validation pipeline and
// pack the result into a Godot Dictionary.
// ──────────────────────────────────────────────────────────────

godot::Dictionary EngineBridge::buildLevelDictionary(
    const APLG::LevelData& level,
    double genTimeMs, double valTimeMs,
    int platformCount, int jumpCount,
    float difficultyScore,
    const std::string& exportJson,
    bool valid, const std::string& reason,
    int attempts) const
{
    godot::Dictionary dict;
    dict["width"]   = level.width;
    dict["height"]  = level.height;
    dict["spawn_x"] = level.spawnPosition.x;
    dict["spawn_y"] = level.spawnPosition.y;
    dict["exit_x"]  = level.exitPosition.x;
    dict["exit_y"]  = level.exitPosition.y;

    godot::Array tiles_array;
    for (int y = 0; y < level.height; ++y) {
        godot::Array row;
        for (int x = 0; x < level.width; ++x) {
            row.append(static_cast<int>(level.tiles[y][x]));
        }
        tiles_array.append(row);
    }
    dict["tiles"] = tiles_array;

    godot::Dictionary stats;
    stats["seed"]               = m_seed;
    stats["generation_time_ms"] = genTimeMs + valTimeMs;
    stats["platform_count"]     = platformCount;
    stats["jump_count"]         = jumpCount;
    stats["difficulty_score"]   = difficultyScore;
    stats["export_json"]        = godot::String(exportJson.c_str());
    stats["valid"]              = valid;
    stats["reason"]             = godot::String(reason.c_str());
    stats["attempts"]           = attempts;
    dict["stats"] = stats;

    return dict;
}

// ──────────────────────────────────────────────────────────────
// Core generation helper — shared by generate_test_grid and
// generate_adaptive_level.
// ──────────────────────────────────────────────────────────────

static godot::Dictionary runGenerationPipeline(
    APLG::GenerationConfig config,
    const std::string& generatorType,
    int& seedInOut)
{
    // Resolve seed
    int actual_seed = seedInOut;
    if (actual_seed <= 0) {
        auto now = std::chrono::high_resolution_clock::now();
        actual_seed = static_cast<int>(now.time_since_epoch().count() & 0x7FFFFFFF);
        if (actual_seed <= 0) actual_seed = 1;
        seedInOut = actual_seed;
    }

    auto generator = APLG::GeneratorFactory::createGenerator(generatorType);

    APLG::Validation::PlayerCapabilities caps;
    APLG::Validation::ValidatedLevelGenerator validatedGenerator(std::move(generator));

    APLG::Validation::ValidationResult valResult;
    double genTimeMs = 0.0, valTimeMs = 0.0;

    APLG::LevelData level = validatedGenerator.generateValid(
        config, static_cast<uint32_t>(seedInOut), caps, valResult, genTimeMs, valTimeMs);

    if (valResult.valid) {
        seedInOut = valResult.seedUsed;
    }

    // Count platforms
    int platform_count = 0;
    for (int y = 0; y < level.height; ++y)
        for (int x = 0; x < level.width; ++x)
            if (level.tiles[y][x] == APLG::TileType::Solid ||
                level.tiles[y][x] == APLG::TileType::Platform)
                platform_count++;

    // Find path and count jumps
    std::vector<APLG::Vec2i> path = APLG::Validation::PathFinder::findPath(
        level, level.spawnPosition, level.exitPosition, caps);

    int jump_count = 0;
    if (valResult.valid && !path.empty()) {
        for (size_t i = 0; i < path.size() - 1; ++i) {
            APLG::Vec2i pos  = path[i];
            APLG::Vec2i next = path[i + 1];
            if (pos.y - next.y > 0 || std::abs(next.x - pos.x) > 1)
                jump_count++;
        }
    }

    float difficulty_score = APLG::Validation::DifficultyEstimator::estimate(level, path, caps);

    // JSON export
    APLG::JsonValue::Object jsonObj;
    jsonObj["seed"]               = static_cast<double>(seedInOut);
    jsonObj["generation_time_ms"] = genTimeMs;
    jsonObj["platform_count"]     = static_cast<double>(platform_count);
    jsonObj["jump_count"]         = static_cast<double>(jump_count);
    jsonObj["difficulty_score"]   = static_cast<double>(difficulty_score);
    jsonObj["width"]              = static_cast<double>(level.width);
    jsonObj["height"]             = static_cast<double>(level.height);
    jsonObj["spawn_x"]            = static_cast<double>(level.spawnPosition.x);
    jsonObj["spawn_y"]            = static_cast<double>(level.spawnPosition.y);
    jsonObj["exit_x"]             = static_cast<double>(level.exitPosition.x);
    jsonObj["exit_y"]             = static_cast<double>(level.exitPosition.y);

    APLG::JsonValue::Array tilesJsonArray;
    for (int y = 0; y < level.height; ++y) {
        APLG::JsonValue::Array rowArr;
        for (int x = 0; x < level.width; ++x)
            rowArr.push_back(APLG::JsonValue(static_cast<int32_t>(level.tiles[y][x])));
        tilesJsonArray.push_back(APLG::JsonValue(rowArr));
    }
    jsonObj["tiles"] = tilesJsonArray;
    std::string jsonStr = APLG::JsonValue(jsonObj).serialize(2);

    // Save metrics
    APLG::Validation::LevelMetrics metrics;
    metrics.seed              = seedInOut;
    metrics.generationTimeMs  = genTimeMs;
    metrics.validationTimeMs  = valTimeMs;
    metrics.reachable         = valResult.valid;
    metrics.platformCount     = platform_count;
    metrics.jumpCount         = jump_count;
    metrics.coins             = static_cast<int32_t>(level.coinPositions.size());
    metrics.enemies           = static_cast<int32_t>(level.enemyPositions.size());
    metrics.difficultyScore   = difficulty_score;
    metrics.attempts          = valResult.attempts;
    APLG::Validation::MetricsCollector::save("metrics.json", metrics);

    // Build dictionary
    godot::Dictionary dict;
    dict["width"]   = level.width;
    dict["height"]  = level.height;
    dict["spawn_x"] = level.spawnPosition.x;
    dict["spawn_y"] = level.spawnPosition.y;
    dict["exit_x"]  = level.exitPosition.x;
    dict["exit_y"]  = level.exitPosition.y;

    godot::Array tiles_array;
    for (int y = 0; y < level.height; ++y) {
        godot::Array row;
        for (int x = 0; x < level.width; ++x)
            row.append(static_cast<int>(level.tiles[y][x]));
        tiles_array.append(row);
    }
    dict["tiles"] = tiles_array;

    godot::Dictionary stats;
    stats["seed"]               = seedInOut;
    stats["generation_time_ms"] = genTimeMs + valTimeMs;
    stats["platform_count"]     = platform_count;
    stats["jump_count"]         = jump_count;
    stats["difficulty_score"]   = difficulty_score;
    stats["export_json"]        = godot::String(jsonStr.c_str());
    stats["valid"]              = valResult.valid;
    stats["reason"]             = godot::String(valResult.reason.c_str());
    stats["attempts"]           = valResult.attempts;
    dict["stats"] = stats;

    return dict;
}

// ──────────────────────────────────────────────────────────────
// Milestone 1: generate_test_grid (unchanged behaviour)
// ──────────────────────────────────────────────────────────────

godot::Dictionary EngineBridge::generate_test_grid() const
{
    APLG::GenerationConfig config;
    config.minWidth        = 20;
    config.maxWidth        = 20;
    config.minHeight       = 10;
    config.maxHeight       = 10;
    config.platformDensity = 0.3f;
    config.enemyDensity    = 0.05f;
    config.coinDensity     = 0.1f;

    // Seed determinism: verify on the second call from GDScript
    if (m_seed <= 0) {
        auto now = std::chrono::high_resolution_clock::now();
        m_seed = static_cast<int>(now.time_since_epoch().count() & 0x7FFFFFFF);
        if (m_seed <= 0) m_seed = 1;
    }

    return runGenerationPipeline(config, "Constraint-Based", m_seed);
}

// ──────────────────────────────────────────────────────────────
// Seeding
// ──────────────────────────────────────────────────────────────

void EngineBridge::set_seed(int p_seed)
{
    m_seed = p_seed < 0 ? 0 : p_seed;
}

int EngineBridge::get_seed() const
{
    return m_seed;
}

// ──────────────────────────────────────────────────────────────
// Physics capabilities (Single Source of Truth)
// ──────────────────────────────────────────────────────────────

float EngineBridge::get_player_speed() const
{
    APLG::Validation::PlayerCapabilities caps;
    return caps.runSpeed;
}

float EngineBridge::get_player_jump_velocity() const
{
    APLG::Validation::PlayerCapabilities caps;
    return caps.jumpVelocity;
}

float EngineBridge::get_player_gravity() const
{
    APLG::Validation::PlayerCapabilities caps;
    return caps.gravity;
}

// ──────────────────────────────────────────────────────────────
// Milestone 4: Adaptive Difficulty
// ──────────────────────────────────────────────────────────────

void EngineBridge::report_level_metrics(int deaths, float time_sec, int coins,
                                         float jump_accuracy, float enemy_hit_rate)
{
    APLG::PlayerMetrics metrics;
    metrics.deaths          = deaths;
    metrics.completionTime  = time_sec;
    metrics.coinsCollected  = coins;
    metrics.jumpAccuracy    = jump_accuracy;
    metrics.enemyHitRate    = enemy_hit_rate;

    // Feed into difficulty manager (updates skill score + adjusts difficulty)
    m_difficultyManager.updateDifficulty(metrics);

    // Also trigger behaviour analysis on every level completion
    m_playerModel.analyze();

    godot::UtilityFunctions::print(
        "[APLG] Level metrics reported. Skill: ",
        m_difficultyManager.getSkillScore().overall,
        " | Difficulty: ", get_difficulty_name());
}

// ──────────────────────────────────────────────────────────────
// Shared marshalling: C++ pipeline result → Godot Dictionary
// ──────────────────────────────────────────────────────────────

godot::Dictionary EngineBridge::marshalLevelResult(
    const APLG::LevelGenerationResult& result,
    const godot::String& generatorLabel) const
{
    m_lastLevel      = result.level;
    m_hasLevel       = true;
    m_currentProfile = result.profile;
    m_seed           = result.seed;

    const APLG::LevelData& level = result.level;

    godot::Dictionary dict;
    dict["width"]      = level.width;
    dict["height"]     = level.height;
    dict["spawn_x"]    = level.spawnPosition.x;
    dict["spawn_y"]    = level.spawnPosition.y;
    dict["exit_x"]     = level.exitPosition.x;
    dict["exit_y"]     = level.exitPosition.y;
    dict["guaranteed"] = result.stats.valid;
    dict["lives"]      = 3;

    godot::Array tiles_array;
    for (int y = 0; y < level.height; ++y) {
        godot::Array row;
        for (int x = 0; x < level.width; ++x) {
            row.append(static_cast<int>(level.tiles[y][x]));
        }
        tiles_array.append(row);
    }
    dict["tiles"] = tiles_array;

    godot::Array enemies;
    for (size_t i = 0; i < level.enemyPositions.size(); ++i) {
        godot::Dictionary e;
        e["x"]     = level.enemyPositions[i].x;
        e["y"]     = level.enemyPositions[i].y;
        e["type"]  = static_cast<int>(level.enemyTypes[i]);
        e["speed"] = (i < level.enemySpeeds.size()) ? level.enemySpeeds[i] : 1.5f;
        enemies.append(e);
    }
    dict["enemies"] = enemies;

    godot::Array checkpoints;
    for (const auto& cp : level.checkpointPositions) {
        godot::Dictionary c;
        c["x"] = cp.x;
        c["y"] = cp.y;
        checkpoints.append(c);
    }
    dict["checkpoints"] = checkpoints;

    godot::Array moving_platforms;
    for (const auto& p : level.movingPlatformPositions) {
        godot::Dictionary pd;
        pd["x"] = p.x;
        pd["y"] = p.y;
        pd["variant"] = 10;
        moving_platforms.append(pd);
    }
    dict["moving_platforms"] = moving_platforms;

    godot::Array falling_platforms;
    for (const auto& p : level.fallingPlatformPositions) {
        godot::Dictionary pd;
        pd["x"] = p.x;
        pd["y"] = p.y;
        pd["variant"] = 11;
        falling_platforms.append(pd);
    }
    dict["falling_platforms"] = falling_platforms;

    godot::Array bounce_pads;
    for (const auto& p : level.bouncePadPositions) {
        godot::Dictionary pd;
        pd["x"] = p.x;
        pd["y"] = p.y;
        pd["variant"] = 12;
        bounce_pads.append(pd);
    }
    dict["bounce_pads"] = bounce_pads;

    godot::Dictionary stats;
    stats["seed"]             = result.seed;
    stats["jump_count"]       = result.stats.jumpCount;
    stats["platform_count"]   = result.stats.platformCount;
    stats["difficulty_score"] = result.stats.difficultyScore;
    stats["valid"]            = result.stats.valid;
    stats["reason"]           = godot::String(result.stats.reason.c_str());
    stats["generator"]        = generatorLabel;
    stats["difficulty_name"]  = get_difficulty_name();
    stats["playstyle"]        = get_playstyle_name();
    stats["enemies"]          = static_cast<int>(level.enemyPositions.size());
    stats["coins"]            = static_cast<int>(level.coinPositions.size());
    stats["checkpoints"]      = static_cast<int>(level.checkpointPositions.size());
    stats["lives"]            = 3;
    stats["attempts"]         = 1;
    dict["stats"] = stats;

    godot::UtilityFunctions::print(
        "[APLG] Level ready: ", level.width, "x", level.height,
        " | Enemies:", static_cast<int>(level.enemyPositions.size()),
        " | Coins:", static_cast<int>(level.coinPositions.size()),
        " | Checkpoints:", static_cast<int>(level.checkpointPositions.size()));

    return dict;
}

// ──────────────────────────────────────────────────────────────
// generate_guaranteed_level: first-level intro (pinned to Easy)
// ──────────────────────────────────────────────────────────────

godot::Dictionary EngineBridge::generate_guaranteed_level() const
{
    APLG::Validation::PlayerCapabilities caps;

    APLG::LevelGenerationPipeline::Request request;
    request.difficulty = APLG::DifficultyLevel::Easy;
    request.playstyle  = m_playerModel.getPlaystyle();
    request.seed       = m_seed;

    godot::UtilityFunctions::print(
        "[APLG] Generating guaranteed level (Easy intro) via unified pipeline");

    const APLG::LevelGenerationResult result =
        APLG::LevelGenerationPipeline::generate(request, caps);

    return marshalLevelResult(result, godot::String("PLE+Decorator"));
}

// ──────────────────────────────────────────────────────────────
// generate_adaptive_level: current difficulty + playstyle
// ──────────────────────────────────────────────────────────────

godot::Dictionary EngineBridge::generate_adaptive_level() const
{
    APLG::Validation::PlayerCapabilities caps;

    APLG::LevelGenerationPipeline::Request request;
    request.difficulty = m_difficultyManager.getCurrentDifficulty();
    request.playstyle  = m_playerModel.getPlaystyle();
    request.seed       = 0;

    godot::UtilityFunctions::print(
        "[APLG] Generating adaptive level via unified pipeline | ",
        get_difficulty_name(),
        " | Playstyle: ", get_playstyle_name());

    const APLG::LevelGenerationResult result =
        APLG::LevelGenerationPipeline::generate(request, caps);

    return marshalLevelResult(result, godot::String("PLE+Decorator"));
}

int EngineBridge::get_difficulty_level() const
{
    return static_cast<int>(m_difficultyManager.getCurrentDifficulty());
}

godot::String EngineBridge::get_difficulty_name() const
{
    switch (m_difficultyManager.getCurrentDifficulty()) {
        case APLG::DifficultyLevel::Easy:      return godot::String("Easy");
        case APLG::DifficultyLevel::Normal:    return godot::String("Normal");
        case APLG::DifficultyLevel::Hard:      return godot::String("Hard");
        case APLG::DifficultyLevel::Expert:    return godot::String("Expert");
        case APLG::DifficultyLevel::Nightmare: return godot::String("Nightmare");
    }
    return godot::String("Normal");
}

float EngineBridge::get_skill_score() const
{
    return m_difficultyManager.getSkillScore().overall;
}

// ──────────────────────────────────────────────────────────────
// Milestone 4: Player Analytics
// ──────────────────────────────────────────────────────────────

void EngineBridge::record_player_action(int action_type, float x, float y, float timestamp)
{
    if (action_type < 0 || action_type > static_cast<int>(APLG::PlayerAction::Idle))
        return;

    APLG::PlayerAction action = static_cast<APLG::PlayerAction>(action_type);
    m_playerModel.recordAction(action, APLG::Vec2(x, y), timestamp);
}

void EngineBridge::analyze_player_behavior()
{
    m_playerModel.analyze();
}

int EngineBridge::get_detected_playstyle() const
{
    return static_cast<int>(m_playerModel.getPlaystyle());
}

float EngineBridge::get_playstyle_confidence() const
{
    return m_playerModel.getConfidence();
}

godot::String EngineBridge::get_playstyle_name() const
{
    switch (m_playerModel.getPlaystyle()) {
        case APLG::Playstyle::Explorer:    return godot::String("Explorer");
        case APLG::Playstyle::Speedrunner: return godot::String("Speedrunner");
        case APLG::Playstyle::Aggressive:  return godot::String("Aggressive");
        case APLG::Playstyle::Careful:     return godot::String("Careful");
        case APLG::Playstyle::Collector:   return godot::String("Collector");
    }
    return godot::String("Careful");
}

godot::String EngineBridge::export_session_json() const
{
    // Build a compact JSON object capturing the current session state
    APLG::JsonValue::Object root;

    // Difficulty state
    root["difficulty_level"] = static_cast<double>(get_difficulty_level());
    root["skill_score"]      = static_cast<double>(get_skill_score());

    // Playstyle state
    root["playstyle"]            = static_cast<double>(get_detected_playstyle());
    root["playstyle_confidence"] = static_cast<double>(get_playstyle_confidence());

    // Behaviour stats
    APLG::BehaviorStats bs = m_playerModel.getStatistics();
    APLG::JsonValue::Object statsObj;
    statsObj["total_actions"]         = static_cast<double>(bs.totalActions);
    statsObj["jump_count"]            = static_cast<double>(bs.jumpCount);
    statsObj["dash_count"]            = static_cast<double>(bs.dashCount);
    statsObj["attack_count"]          = static_cast<double>(bs.attackCount);
    statsObj["exploration_pct"]       = static_cast<double>(bs.explorationPercentage);
    statsObj["combat_engagement"]     = static_cast<double>(bs.combatEngagement);
    statsObj["risk_taking"]           = static_cast<double>(bs.riskTaking);
    root["behavior_stats"] = APLG::JsonValue(statsObj);

    std::string jsonStr = APLG::JsonValue(root).serialize(2);
    return godot::String(jsonStr.c_str());
}

// ──────────────────────────────────────────────────────────────
// Phase 2: Analytics + Heatmap
// ──────────────────────────────────────────────────────────────

void EngineBridge::record_death_position(int tile_x, int tile_y)
{
    if (!m_hasLevel) return;
    if (tile_x < 0 || tile_x >= m_lastLevel.width) return;
    if (tile_y < 0 || tile_y >= m_lastLevel.height) return;
    m_lastLevel.deathHeatmap[tile_y][tile_x]++;
}

godot::Array EngineBridge::get_death_heatmap() const
{
    godot::Array result;
    if (!m_hasLevel) return result;
    for (int y = 0; y < m_lastLevel.height; ++y) {
        godot::Array row;
        for (int x = 0; x < m_lastLevel.width; ++x)
            row.append(m_lastLevel.deathHeatmap[y][x]);
        result.append(row);
    }
    return result;
}

godot::Dictionary EngineBridge::get_difficulty_profile() const
{
    godot::Dictionary d;
    d["map_width"]           = m_currentProfile.mapWidth;
    d["map_height"]          = m_currentProfile.mapHeight;
    d["enemy_count"]         = m_currentProfile.enemyCount;
    d["checkpoint_count"]    = m_currentProfile.checkpointCount;
    d["lives"]               = m_currentProfile.lives;
    d["coin_density"]        = m_currentProfile.coinDensity;
    d["hazard_density"]      = m_currentProfile.hazardDensity;
    d["powerups_per_level"]  = m_currentProfile.powerupsPerLevel;
    d["has_moving_platforms"]= m_currentProfile.movingPlatformRate > 0.0f;
    d["has_falling_platforms"]= m_currentProfile.fallingPlatformRate > 0.0f;
    return d;
}

void EngineBridge::report_extended_metrics(int deaths_hazard, int deaths_fall,
                                            int deaths_enemy, float exploration_pct,
                                            bool used_checkpoint)
{
    godot::UtilityFunctions::print(
        "[APLG] Extended metrics | HazardDeaths:", deaths_hazard,
        " FallDeaths:", deaths_fall,
        " EnemyDeaths:", deaths_enemy,
        " Exploration:", exploration_pct,
        " UsedCheckpoint:", used_checkpoint);

    if (deaths_hazard > 2) {
        m_playerModel.recordAction(APLG::PlayerAction::Idle,
                                   APLG::Vec2(0,0), 0.0f);
    }
}

void EngineBridge::report_performance_metrics(int level_num, int enemies_eliminated, int damage_taken,
                                             float time_sec, int attacks_attempted, int attacks_landed,
                                             int jumps_attempted, int jumps_landed, int highest_combo)
{
    m_totalLevelsCompleted = std::max(m_totalLevelsCompleted, level_num);
    m_totalEnemiesEliminated += enemies_eliminated;
    m_totalDamageTaken += damage_taken;
    m_accumulatedTime += time_sec;
    m_totalAttacks += attacks_attempted;
    m_attacksLanded += attacks_landed;
    m_totalJumps += jumps_attempted;
    m_jumpsLanded += jumps_landed;
    m_highestCombo = std::max(m_highestCombo, highest_combo);

    float jump_acc = (jumps_attempted > 0) ? static_cast<float>(jumps_landed) / static_cast<float>(jumps_attempted) : 1.0f;
    float atk_acc = (attacks_attempted > 0) ? static_cast<float>(attacks_landed) / static_cast<float>(attacks_attempted) : 1.0f;

    report_level_metrics(0, time_sec, 0, jump_acc, atk_acc);
}

godot::Dictionary EngineBridge::get_performance_dashboard_stats() const
{
    godot::Dictionary dict;
    dict["levels_completed"] = m_totalLevelsCompleted;
    dict["enemies_eliminated"] = m_totalEnemiesEliminated;

    float atk_acc = (m_totalAttacks > 0) ? static_cast<float>(m_attacksLanded) / static_cast<float>(m_totalAttacks) : 1.0f;
    float jump_acc = (m_totalJumps > 0) ? static_cast<float>(m_jumpsLanded) / static_cast<float>(m_totalJumps) : 1.0f;

    dict["attack_accuracy"] = atk_acc;
    dict["jump_success_rate"] = jump_acc;
    dict["damage_taken"] = m_totalDamageTaken;

    float avg_time = (m_totalLevelsCompleted > 0) ? m_accumulatedTime / static_cast<float>(m_totalLevelsCompleted) : 0.0f;
    dict["time_per_level"] = avg_time;

    float skill = get_skill_score();
    dict["skill_rating"] = skill;
    dict["adaptive_difficulty_score"] = skill;
    dict["current_difficulty"] = get_difficulty_level();
    dict["current_difficulty_name"] = get_difficulty_name();
    dict["highest_combo"] = m_highestCombo;

    float progress = std::min(1.0f, static_cast<float>(m_totalLevelsCompleted) / 10.0f);
    dict["overall_progress"] = progress;

    return dict;
}

// ──────────────────────────────────────────────────────────────
// ClassDB Registration  (called once at extension load time)
// ──────────────────────────────────────────────────────────────

void EngineBridge::_bind_methods()
{
    // ── Milestone 1 ───────────────────────────────────────
    godot::ClassDB::bind_method(
        godot::D_METHOD("hello"),
        &EngineBridge::hello);

    godot::ClassDB::bind_method(
        godot::D_METHOD("get_engine_version"),
        &EngineBridge::get_engine_version);

    godot::ClassDB::bind_method(
        godot::D_METHOD("add", "a", "b"),
        &EngineBridge::add);

    godot::ClassDB::bind_method(
        godot::D_METHOD("generate_test_grid"),
        &EngineBridge::generate_test_grid);

    godot::ClassDB::bind_method(
        godot::D_METHOD("set_seed", "seed"),
        &EngineBridge::set_seed);

    godot::ClassDB::bind_method(
        godot::D_METHOD("get_seed"),
        &EngineBridge::get_seed);

    godot::ClassDB::bind_method(
        godot::D_METHOD("get_player_speed"),
        &EngineBridge::get_player_speed);

    godot::ClassDB::bind_method(
        godot::D_METHOD("get_player_jump_velocity"),
        &EngineBridge::get_player_jump_velocity);

    godot::ClassDB::bind_method(
        godot::D_METHOD("get_player_gravity"),
        &EngineBridge::get_player_gravity);

    // ── Milestone 4: Adaptive Difficulty ─────────────────
    godot::ClassDB::bind_method(
        godot::D_METHOD("report_level_metrics",
                        "deaths", "time_sec", "coins",
                        "jump_accuracy", "enemy_hit_rate"),
        &EngineBridge::report_level_metrics);

    godot::ClassDB::bind_method(
        godot::D_METHOD("generate_guaranteed_level"),
        &EngineBridge::generate_guaranteed_level);

    godot::ClassDB::bind_method(
        godot::D_METHOD("generate_adaptive_level"),
        &EngineBridge::generate_adaptive_level);

    godot::ClassDB::bind_method(
        godot::D_METHOD("get_difficulty_level"),
        &EngineBridge::get_difficulty_level);

    godot::ClassDB::bind_method(
        godot::D_METHOD("get_difficulty_name"),
        &EngineBridge::get_difficulty_name);

    godot::ClassDB::bind_method(
        godot::D_METHOD("get_skill_score"),
        &EngineBridge::get_skill_score);

    // ── Milestone 4: Player Analytics ────────────────────
    godot::ClassDB::bind_method(
        godot::D_METHOD("record_player_action",
                        "action_type", "x", "y", "timestamp"),
        &EngineBridge::record_player_action);

    godot::ClassDB::bind_method(
        godot::D_METHOD("analyze_player_behavior"),
        &EngineBridge::analyze_player_behavior);

    godot::ClassDB::bind_method(
        godot::D_METHOD("get_detected_playstyle"),
        &EngineBridge::get_detected_playstyle);

    godot::ClassDB::bind_method(
        godot::D_METHOD("get_playstyle_confidence"),
        &EngineBridge::get_playstyle_confidence);

    godot::ClassDB::bind_method(
        godot::D_METHOD("get_playstyle_name"),
        &EngineBridge::get_playstyle_name);

    godot::ClassDB::bind_method(
        godot::D_METHOD("export_session_json"),
        &EngineBridge::export_session_json);

    // ── Phase 2: Analytics + Heatmap ─────────────────────
    godot::ClassDB::bind_method(
        godot::D_METHOD("record_death_position", "tile_x", "tile_y"),
        &EngineBridge::record_death_position);

    godot::ClassDB::bind_method(
        godot::D_METHOD("get_death_heatmap"),
        &EngineBridge::get_death_heatmap);

    godot::ClassDB::bind_method(
        godot::D_METHOD("get_difficulty_profile"),
        &EngineBridge::get_difficulty_profile);

    godot::ClassDB::bind_method(
        godot::D_METHOD("report_extended_metrics",
                        "deaths_hazard", "deaths_fall", "deaths_enemy",
                        "exploration_pct", "used_checkpoint"),
        &EngineBridge::report_extended_metrics);

    godot::ClassDB::bind_method(
        godot::D_METHOD("report_performance_metrics",
                        "level_num", "enemies_eliminated", "damage_taken",
                        "time_sec", "attacks_attempted", "attacks_landed",
                        "jumps_attempted", "jumps_landed", "highest_combo"),
        &EngineBridge::report_performance_metrics);

    godot::ClassDB::bind_method(
        godot::D_METHOD("get_performance_dashboard_stats"),
        &EngineBridge::get_performance_dashboard_stats);
}
