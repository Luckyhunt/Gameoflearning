#pragma once

// ============================================================
// EngineBridge.h
//
// GDExtension boundary class.
// This is the ONLY place where Godot headers are mixed with
// our C++ engine headers.
//
// IMPORTANT — Why this class is NOT in namespace APLG:
//   The GDCLASS(ClassName, ParentClass) macro from godot-cpp
//   internally uses reinterpret_cast on member-function pointers
//   between the declared class and an internal stub class.
//   MSVC rejects this cast when the class lives inside a user
//   namespace. This is a known godot-cpp constraint — wrapper
//   classes must be declared at global scope (or in the godot
//   namespace). All pure-C++ engine classes remain in APLG::.
//
// Architecture rule:
//   Godot calls EngineBridge methods.
//   EngineBridge translates to/from Godot types.
//   EngineBridge delegates ALL logic to APLG:: C++ classes.
//
// This class must remain a thin adapter — zero game logic here.
// ============================================================

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>

#include "IEngine.h"         // APLG::IEngine, APLG::EngineFactory
#include "../Include/Common.h"
#include "../Difficulty/IDifficultyManager.h"
#include "../AI/IPlayerModel.h"
#include "PlatformerLevelEngine.h"
#include "GameplayDecorator.h"
#include "LevelGenerationPipeline.h"

/**
 * @brief GDExtension bridge exposing the C++ engine to Godot.
 *
 * Inherits godot::Node so it can be placed in any scene.
 * Uses the Adapter pattern: Godot types <-> C++ engine types.
 *
 * Declared at global scope — see header comment for rationale.
 *
 * Milestone 1 API (verification only):
 *   hello()               → confirms DLL is loaded and callable
 *   get_engine_version()  → returns version string from C++ side
 *   add(a, b)             → proves int round-trip across boundary
 *   generate_test_grid()  → calls real ConstraintGenerator, returns level data dictionary
 *
 * Milestone 4 API (adaptive gameplay loop):
 *   report_level_metrics()    → feed completion data into DifficultyManager
 *   generate_adaptive_level() → generate next level using adaptive config
 *   get_difficulty_level()    → current difficulty as integer (0=Easy..4=Nightmare)
 *   get_difficulty_name()     → human-readable difficulty string
 *   get_skill_score()         → overall skill score 0.0..1.0
 *   record_player_action()    → feed action into PlayerModel
 *   get_detected_playstyle()  → detected playstyle as int (0=Explorer..4=Collector)
 *   get_playstyle_confidence()→ confidence in playstyle detection 0.0..1.0
 *   get_playstyle_name()      → human-readable playstyle string
 *   export_session_json()     → dump full session analytics JSON string
 */
class EngineBridge : public godot::Node {
    GDCLASS(EngineBridge, godot::Node)

public:
    EngineBridge();
    ~EngineBridge() override;

    // Godot lifecycle
    void _ready() override;

    // ── Milestone 1: verification methods ──────────────────
    godot::String hello() const;
    godot::String get_engine_version() const;
    int           add(int a, int b) const;
    godot::Dictionary generate_test_grid() const;

    // Seeding methods
    void set_seed(int p_seed);
    int  get_seed() const;

    // Physics capabilities (Single Source of Truth)
    float get_player_speed() const;
    float get_player_jump_velocity() const;
    float get_player_gravity() const;

    // ── Milestone 4: Adaptive Difficulty ───────────────────

    /**
     * @brief Report level completion metrics to the DifficultyManager.
     *        Call this when the player reaches the exit.
     * @param deaths        Number of times the player died this level
     * @param time_sec      Level completion time in seconds
     * @param coins         Coins collected this level
     * @param jump_accuracy Fraction of successful jumps (0.0–1.0)
     * @param enemy_hit_rate Fraction of enemy encounters won (0.0–1.0)
     */
    void  report_level_metrics(int deaths, float time_sec, int coins,
                                float jump_accuracy, float enemy_hit_rate);

    /**
     * @brief Generate a level whose GenerationConfig is shaped by current difficulty.
     *        Internally calls AdaptiveGenerator::adjustConfig() then ValidatedLevelGenerator.
     * @return Same dictionary format as generate_test_grid()
     */
    godot::Dictionary generate_adaptive_level() const;

    /**
     * @brief Generate a guaranteed-playable level using PlatformerLevelEngine.
     *        This engine builds the level by construction — no retries, no
     *        validation failures, no overlapping tiles.  Use this whenever the
     *        adaptive generator produces a non-playable result.
     * @return Same dictionary format as generate_test_grid()
     */
    godot::Dictionary generate_guaranteed_level() const;

    /** @brief Current difficulty as integer: 0=Easy, 1=Normal, 2=Hard, 3=Expert, 4=Nightmare */
    int           get_difficulty_level() const;

    /** @brief Human-readable difficulty name */
    godot::String get_difficulty_name() const;

    /** @brief Overall skill score 0.0–1.0 */
    float         get_skill_score() const;

    // ── Milestone 4: Player Analytics ──────────────────────

    /**
     * @brief Record a player action for playstyle analysis.
     * @param action_type  0=MoveLeft,1=MoveRight,2=Jump,3=DoubleJump,4=WallJump,
     *                     5=Dash,6=ClimbUp,7=ClimbDown,8=Attack,9=Interact,10=Idle
     * @param x            World X position at time of action
     * @param y            World Y position at time of action
     * @param timestamp    Elapsed game time in seconds
     */
    void  record_player_action(int action_type, float x, float y, float timestamp);

    /**
     * @brief Trigger playstyle analysis on buffered action history.
     *        Call periodically (e.g. every level completion).
     */
    void  analyze_player_behavior();

    /** @brief Detected playstyle: 0=Explorer,1=Speedrunner,2=Aggressive,3=Careful,4=Collector */
    int           get_detected_playstyle() const;

    /** @brief Confidence in playstyle detection 0.0–1.0 */
    float         get_playstyle_confidence() const;

    /** @brief Human-readable playstyle name */
    godot::String get_playstyle_name() const;

    /** @brief Full session analytics as a compact JSON string */
    godot::String export_session_json() const;

    // ── Phase 2: Full adaptive pipeline ────────────────────

    /**
     * @brief Record the tile position where the player died (for heatmap).
     * @param tile_x  Tile column of death position
     * @param tile_y  Tile row of death position
     */
    void record_death_position(int tile_x, int tile_y);

    /**
     * @brief Get the death heatmap as a 2D Godot Array.
     * @return Array of Array of int, same dimensions as current level
     */
    godot::Array get_death_heatmap() const;

    /**
     * @brief Get current difficulty profile parameters as a Dictionary.
     * Useful for the HUD to display what the current profile looks like.
     */
    godot::Dictionary get_difficulty_profile() const;

    /**
     * @brief Report extended per-level analytics.
     */
    void report_extended_metrics(int deaths_hazard, int deaths_fall, int deaths_enemy,
                                  float exploration_pct, bool used_checkpoint);

    /**
     * @brief Report consolidated level performance metrics for dashboard and adaptive progression.
     */
    void report_performance_metrics(int level_num, int enemies_eliminated, int damage_taken,
                                      float time_sec, int attacks_attempted, int attacks_landed,
                                      int jumps_attempted, int jumps_landed, int highest_combo);

    /**
     * @brief Get consolidated Performance Dashboard statistics as a Dictionary.
     */
    godot::Dictionary get_performance_dashboard_stats() const;

protected:
    // Required by GDCLASS macro — registers all method bindings
    static void _bind_methods();

private:
    // Owned C++ engine instance (RAII)
    std::unique_ptr<APLG::IEngine> m_engine;

    // Adaptive difficulty manager
    APLG::DifficultyManager m_difficultyManager;

    // Player behaviour analytics model
    APLG::PlayerModel m_playerModel;

    // Configurable seed (0 means randomized on generation)
    mutable int m_seed = 0;

    // Last generated+decorated level (stored for heatmap accumulation)
    mutable APLG::LevelData  m_lastLevel;
    mutable bool             m_hasLevel = false;

    // Current difficulty profile
    mutable APLG::DifficultyProfile m_currentProfile;

    // Performance dashboard accumulation metrics
    mutable int m_totalLevelsCompleted = 0;
    mutable int m_totalEnemiesEliminated = 0;
    mutable int m_totalDamageTaken = 0;
    mutable int m_highestCombo = 0;
    mutable float m_accumulatedTime = 0.0f;
    mutable int m_totalAttacks = 0;
    mutable int m_attacksLanded = 0;
    mutable int m_totalJumps = 0;
    mutable int m_jumpsLanded = 0;

    // ── helpers ───────────────────────────────────────────
    godot::Dictionary buildLevelDictionary(const APLG::LevelData& level,
                                           double genTimeMs, double valTimeMs,
                                           int platformCount, int jumpCount,
                                           float difficultyScore,
                                           const std::string& exportJson,
                                           bool valid, const std::string& reason,
                                           int attempts) const;

    godot::Dictionary marshalLevelResult(
        const APLG::LevelGenerationResult& result,
        const godot::String& generatorLabel) const;
};
