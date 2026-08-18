## GameManager.gd
## ─────────────────────────────────────────────────────────────────
## Owns: lives, pause, game-over state, level transitions.
##
## Signals emitted upward to Main.gd:
##   request_next_level    — advance to next level
##   request_restart       — restart current level
##   request_pause_toggle  — toggle pause overlay
## ─────────────────────────────────────────────────────────────────

extends Node

signal request_next_level
signal request_restart
signal request_pause_toggle

# ── State ────────────────────────────────────────────────────────
var session_lives: int  = 3
var level_number:  int  = 1
var is_paused:     bool = false
var is_game_over:  bool = false

# ── References set by Main.gd ────────────────────────────────────
var hud_node:    Node = null    # typed as Node to avoid circular deps
var fade_rect:   ColorRect = null

# ─────────────────────────────────────────────────────────────────
# Called by Main.gd when player.player_died fires
# ─────────────────────────────────────────────────────────────────
func on_player_died(_remaining_lives: int = 3) -> void:
	pass

# ─────────────────────────────────────────────────────────────────
func _trigger_game_over() -> void:
	is_game_over = true
	print("[GameManager] GAME OVER at level %d" % level_number)
	if hud_node and hud_node.has_method("show_game_over"):
		hud_node.show_game_over()

# ─────────────────────────────────────────────────────────────────
# Called when player completes the level
# ─────────────────────────────────────────────────────────────────
func on_level_complete() -> void:
	is_game_over = false
	level_number += 1
	_do_fade_transition()

# ─────────────────────────────────────────────────────────────────
func restart_level() -> void:
	is_game_over = false
	if is_paused:
		toggle_pause()
	emit_signal("request_restart")

# ─────────────────────────────────────────────────────────────────
func toggle_pause() -> void:
	is_paused = !is_paused
	get_tree().paused = is_paused
	emit_signal("request_pause_toggle")

# ─────────────────────────────────────────────────────────────────
func reset_for_new_level(_lives: int = 3) -> void:
	is_game_over = false
	if hud_node and hud_node.has_method("hide_game_over"):
		hud_node.hide_game_over()

# ─────────────────────────────────────────────────────────────────
func _do_fade_transition() -> void:
	emit_signal("request_next_level")

# ─────────────────────────────────────────────────────────────────
# Build the pause overlay panel (programmatic, no scene needed)
# ─────────────────────────────────────────────────────────────────
func build_pause_overlay() -> CanvasLayer:
	var canvas := CanvasLayer.new()
	canvas.layer = 50
	canvas.process_mode = Node.PROCESS_MODE_ALWAYS
	canvas.visible = false

	var panel := Panel.new()
	panel.size = Vector2(300, 220)
	panel.position = (get_viewport().get_visible_rect().size - panel.size) / 2.0
	canvas.add_child(panel)

	var vbox := VBoxContainer.new()
	vbox.set_anchors_preset(Control.PRESET_FULL_RECT)
	vbox.add_theme_constant_override("separation", 14)
	panel.add_child(vbox)

	var title := Label.new()
	title.text = "⏸  PAUSED"
	title.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	title.add_theme_font_size_override("font_size", 20)
	vbox.add_child(title)

	var resume_btn := Button.new()
	resume_btn.text = "Resume  (ESC)"
	resume_btn.pressed.connect(toggle_pause)
	vbox.add_child(resume_btn)

	var restart_btn := Button.new()
	restart_btn.text = "Restart Level  (R)"
	restart_btn.pressed.connect(restart_level)
	vbox.add_child(restart_btn)

	var newlevel_btn := Button.new()
	newlevel_btn.text = "Generate New Level"
	newlevel_btn.pressed.connect(func() -> void:
		if is_paused:
			toggle_pause()
		level_number += 1
		emit_signal("request_next_level")
	)
	vbox.add_child(newlevel_btn)

	return canvas
