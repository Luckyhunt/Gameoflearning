## Analytics.gd
## ─────────────────────────────────────────────────────────────────
## Owns all per-level and per-session analytics.
## Reports to the C++ bridge after each level completion.
## ─────────────────────────────────────────────────────────────────

extends Node

# Set from Main.gd
var bridge: EngineBridge = null

# ─────────────────────────────────────────────────────────────────
# Called by Main.gd when player.player_died signal fires.
# tile_pos = tile-space coordinates of the death position.
# ─────────────────────────────────────────────────────────────────
func record_death(tile_pos: Vector2i) -> void:
	if bridge == null:
		return
	bridge.record_death_position(tile_pos.x, tile_pos.y)

# ─────────────────────────────────────────────────────────────────
# Called by Main.gd when player.level_complete fires.
# stats = the Dictionary emitted by the player signal.
# ─────────────────────────────────────────────────────────────────
func report_level_complete(stats: Dictionary, level_num: int = 1) -> void:
	if bridge == null:
		return

	if bridge.has_method("report_performance_metrics"):
		bridge.report_performance_metrics(
			level_num,
			int(stats.get("enemies_killed", 0)),
			int(stats.get("damage_taken", 0)),
			float(stats.get("time_sec", 0.0)),
			int(stats.get("attacks_attempted", 0)),
			int(stats.get("attacks_landed", 0)),
			int(stats.get("jumps_attempted", 0)),
			int(stats.get("jumps_landed", 0)),
			int(stats.get("highest_combo", 0))
		)
	else:
		bridge.report_level_metrics(
			int(stats.get("deaths", 0)),
			float(stats.get("time_sec", 0.0)),
			int(stats.get("coins", 0)),
			float(stats.get("jump_accuracy", 0.5)),
			float(stats.get("attack_accuracy", 0.5))
		)

	bridge.report_extended_metrics(
		int(stats.get("deaths_hazard", 0)),
		int(stats.get("deaths_fall", 0)),
		int(stats.get("deaths_enemy", 0)),
		0.5,
		bool(stats.get("used_checkpoint", false))
	)

	print("[Analytics] Level complete. EnemiesKilled:%d Time:%.1fs DamageTaken:%d JumpAcc:%.0f%% AtkAcc:%.0f%%" % [
		int(stats.get("enemies_killed", 0)),
		float(stats.get("time_sec", 0.0)),
		int(stats.get("damage_taken", 0)),
		float(stats.get("jump_accuracy", 0.0)) * 100.0,
		float(stats.get("attack_accuracy", 0.0)) * 100.0,
	])

# ─────────────────────────────────────────────────────────────────
func export_session_json() -> String:
	if bridge == null:
		return "{}"
	return bridge.export_session_json()
