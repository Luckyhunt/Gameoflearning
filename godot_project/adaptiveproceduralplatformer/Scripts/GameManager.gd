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
class_name GameManager

signal request_next_level
signal request_restart
signal request_pause_toggle

# ── State ────────────────────────────────────────────────────────
static var current_difficulty: String = "BEGINNER"

var session_lives: int  = 3
var level_number:  int  = 1
var is_paused:     bool = false
var is_game_over:  bool = false
var _is_transitioning: bool = false

# ── References set by Main.gd ────────────────────────────────────
var hud_node:    Node = null
var fade_rect:   ColorRect = null

# ─────────────────────────────────────────────────────────────────
func on_player_died(_remaining_lives: int = 3) -> void:
	session_lives = _remaining_lives
	if session_lives <= 0:
		_trigger_game_over()

# ─────────────────────────────────────────────────────────────────
func _trigger_game_over() -> void:
	is_game_over = true
	print("[GameManager] GAME OVER at level %d" % level_number)
	if hud_node and hud_node.has_method("show_game_over"):
		hud_node.show_game_over()

# ─────────────────────────────────────────────────────────────────
# Called when player completes the level (strictly sequential)
# ─────────────────────────────────────────────────────────────────
func on_level_complete() -> void:
	if _is_transitioning or is_game_over:
		return
	_is_transitioning = true
	is_game_over = false
	level_number += 1
	print("[GameManager] Level complete! Advancing sequentially to Level %d" % level_number)
	_do_fade_transition()

# ─────────────────────────────────────────────────────────────────
func restart_level() -> void:
	is_game_over = false
	_is_transitioning = false
	if is_paused:
		toggle_pause()
	emit_signal("request_restart")

# ─────────────────────────────────────────────────────────────────
func toggle_pause() -> void:
	is_paused = !is_paused
	get_tree().paused = is_paused
	if has_node("/root/AudioManager"):
		var audio = get_node("/root/AudioManager")
		if is_paused:
			audio.pause_music()
		else:
			audio.resume_music()
	emit_signal("request_pause_toggle")

# ─────────────────────────────────────────────────────────────────
func reset_for_new_level(_lives: int = 3) -> void:
	is_game_over = false
	session_lives = _lives
	if hud_node and hud_node.has_method("hide_game_over"):
		hud_node.hide_game_over()

# ─────────────────────────────────────────────────────────────────
func _do_fade_transition() -> void:
	var completed_level := level_number - 1  # level_number already incremented

	if fade_rect:
		var tw := create_tween()

		# Step 1 — show "LEVEL CLEARED!" overlay
		var cleared_overlay := _build_level_cleared_overlay(completed_level)
		if hud_node:
			hud_node.add_child(cleared_overlay)

		# Step 2 — hold for 1.5s
		tw.tween_interval(1.5)

		# Step 3 — fade to black
		tw.tween_property(fade_rect, "color:a", 1.0, 0.22)

		# Step 4 — remove overlay and load next level
		tw.tween_callback(func():
			cleared_overlay.queue_free()
			emit_signal("request_next_level")
		)

		# Step 5 — fade back in
		tw.tween_interval(0.1)
		tw.tween_property(fade_rect, "color:a", 0.0, 0.28)
		tw.tween_callback(func():
			_is_transitioning = false
		)
	else:
		emit_signal("request_next_level")
		_is_transitioning = false

# ─────────────────────────────────────────────────────────────────
# Pixel-art "LEVEL CLEARED!" overlay — always centered in camera view.
#
# Architecture: CanvasLayer (screen-space) contains a full-rect
# Control root, which contains a CenterContainer. CenterContainer
# auto-centers its child panel using Godot layout — no manual
# position math, works at any window resolution.
# ─────────────────────────────────────────────────────────────────
func _build_level_cleared_overlay(completed_level: int) -> CanvasLayer:
	var font_bold = load("res://Assets/fonts/PixelOperator8-Bold.ttf") as Font
	var font_reg  = load("res://Assets/fonts/PixelOperator8.ttf")  as Font

	var canvas := CanvasLayer.new()
	canvas.layer = 60  # above HUD and minimap

	# Full-rect Control — fills the screen in screen-space
	var screen_root := Control.new()
	screen_root.set_anchors_preset(Control.PRESET_FULL_RECT)
	screen_root.mouse_filter = Control.MOUSE_FILTER_IGNORE
	canvas.add_child(screen_root)

	# CenterContainer — auto-centers child in viewport (camera view)
	var center := CenterContainer.new()
	center.set_anchors_preset(Control.PRESET_FULL_RECT)
	center.mouse_filter = Control.MOUSE_FILTER_IGNORE
	screen_root.add_child(center)

	# Panel backing with fixed minimum size
	var bg := ColorRect.new()
	bg.color = Color(0.04, 0.06, 0.10, 0.88)
	bg.custom_minimum_size = Vector2(340, 170)
	center.add_child(bg)

	# Pixel-green border (square corners — pixel-art style)
	var border := Panel.new()
	var border_style := StyleBoxFlat.new()
	border_style.bg_color              = Color(0, 0, 0, 0)
	border_style.border_color          = Color(0.38, 0.82, 0.38, 1.0)
	border_style.border_width_left     = 3
	border_style.border_width_top      = 3
	border_style.border_width_right    = 3
	border_style.border_width_bottom   = 3
	border_style.corner_radius_top_left     = 0
	border_style.corner_radius_top_right    = 0
	border_style.corner_radius_bottom_right = 0
	border_style.corner_radius_bottom_left  = 0
	border.add_theme_stylebox_override("panel", border_style)
	border.set_anchors_preset(Control.PRESET_FULL_RECT)
	bg.add_child(border)

	# Centered VBox for text
	var vbox := VBoxContainer.new()
	vbox.set_anchors_preset(Control.PRESET_FULL_RECT)
	vbox.offset_left   = 20
	vbox.offset_top    = 20
	vbox.offset_right  = -20
	vbox.offset_bottom = -20
	vbox.alignment = BoxContainer.ALIGNMENT_CENTER
	vbox.add_theme_constant_override("separation", 10)
	bg.add_child(vbox)

	# "LEVEL X" header
	var lbl_lvl := Label.new()
	lbl_lvl.text = "LEVEL %d" % completed_level
	lbl_lvl.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	lbl_lvl.modulate = Color(0.75, 0.92, 1.0)
	if font_bold:
		lbl_lvl.add_theme_font_override("font", font_bold)
		lbl_lvl.add_theme_font_size_override("font_size", 13)
	vbox.add_child(lbl_lvl)

	var sep1 := ColorRect.new()
	sep1.color = Color(0.38, 0.82, 0.38, 0.8)
	sep1.custom_minimum_size = Vector2(0, 2)
	vbox.add_child(sep1)

	# "CLEARED!" main label
	var lbl_cleared := Label.new()
	lbl_cleared.text = "CLEARED!"
	lbl_cleared.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	lbl_cleared.modulate = Color(0.3, 1.0, 0.42)
	if font_bold:
		lbl_cleared.add_theme_font_override("font", font_bold)
		lbl_cleared.add_theme_font_size_override("font_size", 28)
	vbox.add_child(lbl_cleared)

	var sep2 := ColorRect.new()
	sep2.color = Color(0.38, 0.82, 0.38, 0.4)
	sep2.custom_minimum_size = Vector2(0, 2)
	vbox.add_child(sep2)

	# "NEXT LEVEL..." sub-text
	var lbl_next := Label.new()
	lbl_next.text = "NEXT LEVEL..."
	lbl_next.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	lbl_next.modulate = Color(0.85, 0.85, 0.85, 0.9)
	if font_reg:
		lbl_next.add_theme_font_override("font", font_reg)
		lbl_next.add_theme_font_size_override("font_size", 9)
	vbox.add_child(lbl_next)

	# Entrance animation: fade + pop scale
	bg.modulate.a = 0.0
	bg.scale = Vector2(0.85, 0.85)
	bg.pivot_offset = Vector2(170, 85)
	var anim := bg.create_tween()
	anim.set_parallel(true)
	anim.tween_property(bg, "modulate:a", 1.0, 0.22)
	anim.tween_property(bg, "scale", Vector2(1.0, 1.0), 0.18)

	return canvas

# ─────────────────────────────────────────────────────────────────
# Build the pause overlay panel (programmatic, no scene needed)
# ─────────────────────────────────────────────────────────────────
func build_pause_overlay() -> CanvasLayer:
	var canvas := CanvasLayer.new()
	canvas.layer = 50
	canvas.process_mode = Node.PROCESS_MODE_ALWAYS
	canvas.visible = false

	var font_bold = load("res://Assets/fonts/PixelOperator8-Bold.ttf") as Font
	var font_reg = load("res://Assets/fonts/PixelOperator8.ttf") as Font

	# Stone frame style for pause panel
	var panel_style := StyleBoxFlat.new()
	panel_style.bg_color = Color(0.06, 0.08, 0.14, 0.96)
	panel_style.border_width_left = 3
	panel_style.border_width_top = 3
	panel_style.border_width_right = 3
	panel_style.border_width_bottom = 3
	panel_style.border_color = Color(0.35, 0.52, 0.75, 0.95)
	panel_style.shadow_color = Color(0, 0, 0, 0.7)
	panel_style.shadow_size = 8

	var panel := Panel.new()
	panel.size = Vector2(360, 310)
	panel.position = (get_viewport().get_visible_rect().size - panel.size) / 2.0
	panel.add_theme_stylebox_override("panel", panel_style)
	canvas.add_child(panel)

	var vbox := VBoxContainer.new()
	vbox.set_anchors_preset(Control.PRESET_FULL_RECT)
	vbox.offset_left = 20
	vbox.offset_top = 18
	vbox.offset_right = -20
	vbox.offset_bottom = -18
	vbox.add_theme_constant_override("separation", 10)
	panel.add_child(vbox)

	var title := Label.new()
	title.text = "PAUSED"
	title.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	if font_bold:
		title.add_theme_font_override("font", font_bold)
		title.add_theme_font_size_override("font_size", 14)
	title.modulate = Color(1.0, 0.88, 0.38)
	vbox.add_child(title)

	# Stone button styles
	var btn_normal := StyleBoxFlat.new()
	btn_normal.bg_color = Color(0.09, 0.12, 0.20, 0.95)
	btn_normal.border_width_left = 2
	btn_normal.border_width_top = 2
	btn_normal.border_width_right = 2
	btn_normal.border_width_bottom = 2
	btn_normal.border_color = Color(0.26, 0.38, 0.56, 0.9)
	btn_normal.content_margin_top = 8.0
	btn_normal.content_margin_bottom = 8.0

	var btn_hover := StyleBoxFlat.new()
	btn_hover.bg_color = Color(0.14, 0.20, 0.32, 1.0)
	btn_hover.border_width_left = 2
	btn_hover.border_width_top = 2
	btn_hover.border_width_right = 2
	btn_hover.border_width_bottom = 2
	btn_hover.border_color = Color(0.45, 0.88, 1.0, 1.0)
	btn_hover.content_margin_top = 8.0
	btn_hover.content_margin_bottom = 8.0

	var style_btn = func(b: Button, text: String):
		b.text = text
		b.add_theme_stylebox_override("normal", btn_normal)
		b.add_theme_stylebox_override("hover", btn_hover)
		b.add_theme_stylebox_override("pressed", btn_hover)
		b.add_theme_stylebox_override("focus", btn_hover)
		if font_reg:
			b.add_theme_font_override("font", font_reg)
			b.add_theme_font_size_override("font_size", 9)
		b.custom_minimum_size = Vector2(0, 36)

	var resume_btn := Button.new()
	style_btn.call(resume_btn, "RESUME  (ESC)")
	resume_btn.pressed.connect(toggle_pause)
	vbox.add_child(resume_btn)

	var restart_btn := Button.new()
	style_btn.call(restart_btn, "RESTART LEVEL  (R)")
	restart_btn.pressed.connect(restart_level)
	vbox.add_child(restart_btn)

	var newlevel_btn := Button.new()
	style_btn.call(newlevel_btn, "NEW LEVEL  (N)")
	newlevel_btn.pressed.connect(func() -> void:
		if is_paused:
			toggle_pause()
		level_number += 1
		emit_signal("request_next_level")
	)
	vbox.add_child(newlevel_btn)

	var audio_btn := Button.new()
	style_btn.call(audio_btn, "AUDIO: ON")
	audio_btn.pressed.connect(func():
		if has_node("/root/AudioManager"):
			var muted: bool = get_node("/root/AudioManager").toggle_mute()
			audio_btn.text = "AUDIO: MUTED" if muted else "AUDIO: ON"
	)
	vbox.add_child(audio_btn)

	var menu_btn := Button.new()
	style_btn.call(menu_btn, "MAIN MENU")
	menu_btn.pressed.connect(func() -> void:
		if is_paused:
			toggle_pause()
		get_tree().change_scene_to_file("res://Scenes/MainMenu.tscn")
	)
	vbox.add_child(menu_btn)

	if has_node("/root/AudioManager"):
		get_node("/root/AudioManager").hook_buttons(canvas)

	return canvas
