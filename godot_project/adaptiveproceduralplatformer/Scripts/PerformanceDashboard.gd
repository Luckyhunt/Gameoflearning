# ============================================================
# PerformanceDashboard.gd — Dynamic Statistics Modal
# ============================================================
# Connects to EngineBridge to display:
# - Levels Completed, Enemies Eliminated, Attack & Jump Accuracy,
#   Damage Taken, Time Per Level, Skill Rating, Adaptive Difficulty,
#   Highest Combo, and Overall Progress.

extends CanvasLayer

signal closed

var bridge: EngineBridge = null

var _panel: Panel = null
var _vbox: VBoxContainer = null
var _stats_grid: GridContainer = null

func _ready() -> void:
	layer = 80
	process_mode = Node.PROCESS_MODE_ALWAYS
	_build_ui()

func setup(p_bridge: EngineBridge) -> void:
	bridge = p_bridge
	update_stats()

func _build_ui() -> void:
	# Semi-transparent background overlay
	var bg := ColorRect.new()
	bg.color = Color(0.05, 0.05, 0.1, 0.85)
	bg.set_anchors_preset(Control.PRESET_FULL_RECT)
	add_child(bg)

	_panel = Panel.new()
	_panel.custom_minimum_size = Vector2(540, 420)
	_panel.position = (get_viewport().get_visible_rect().size - _panel.custom_minimum_size) / 2.0
	add_child(_panel)

	_vbox = VBoxContainer.new()
	_vbox.set_anchors_preset(Control.PRESET_FULL_RECT)
	_vbox.add_theme_constant_override("separation", 16)
	_panel.add_child(_vbox)

	# Header Title
	var title := Label.new()
	title.text = "📊 PERFORMANCE STATISTICS DASHBOARD"
	title.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	title.add_theme_font_size_override("font_size", 20)
	_vbox.add_child(title)

	var hsep := HSeparator.new()
	_vbox.add_child(hsep)

	# Grid for key/value pairs
	_stats_grid = GridContainer.new()
	_stats_grid.columns = 2
	_stats_grid.add_theme_constant_override("h_separation", 24)
	_stats_grid.add_theme_constant_override("v_separation", 12)
	_vbox.add_child(_stats_grid)

	# Close button
	var close_btn := Button.new()
	close_btn.text = "Close  (ESC)"
	close_btn.custom_minimum_size = Vector2(160, 36)
	close_btn.pressed.connect(_on_close_pressed)
	
	var btn_center := CenterContainer.new()
	btn_center.add_child(close_btn)
	_vbox.add_child(btn_center)

func update_stats() -> void:
	if _stats_grid == null:
		return

	# Clear previous entries
	for child in _stats_grid.get_children():
		child.queue_free()

	var stats: Dictionary = {}
	if bridge and bridge.has_method("get_performance_dashboard_stats"):
		stats = bridge.get_performance_dashboard_stats() as Dictionary

	_add_stat_row("Levels Completed:", str(int(stats.get("levels_completed", 0))))
	_add_stat_row("Enemies Eliminated:", str(int(stats.get("enemies_eliminated", 0))))
	_add_stat_row("Attack Accuracy:", "%.1f%%" % (float(stats.get("attack_accuracy", 1.0)) * 100.0))
	_add_stat_row("Jump Success Rate:", "%.1f%%" % (float(stats.get("jump_success_rate", 1.0)) * 100.0))
	_add_stat_row("Total Damage Taken:", str(int(stats.get("damage_taken", 0))))
	_add_stat_row("Avg Time / Level:", "%.1fs" % float(stats.get("time_per_level", 0.0)))
	_add_stat_row("Skill Rating:", "%.0f%%" % (float(stats.get("skill_rating", 0.5)) * 100.0))
	_add_stat_row("Current Difficulty:", str(stats.get("current_difficulty_name", "Normal")).to_upper())
	_add_stat_row("Highest Combo:", str(int(stats.get("highest_combo", 0))))
	_add_stat_row("Overall Progress:", "%.0f%%" % (float(stats.get("overall_progress", 0.0)) * 100.0))

func _add_stat_row(key: String, value: String) -> void:
	var lbl_key := Label.new()
	lbl_key.text = key
	lbl_key.horizontal_alignment = HORIZONTAL_ALIGNMENT_RIGHT
	lbl_key.size_flags_horizontal = Control.SIZE_EXPAND_FILL

	var lbl_val := Label.new()
	lbl_val.text = value
	lbl_val.horizontal_alignment = HORIZONTAL_ALIGNMENT_LEFT
	lbl_val.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	lbl_val.add_theme_font_size_override("font_size", 16)

	_stats_grid.add_child(lbl_key)
	_stats_grid.add_child(lbl_val)

func _unhandled_input(event: InputEvent) -> void:
	if visible and (event.is_action_pressed("ui_cancel") or (event is InputEventKey and event.pressed and event.keycode == KEY_ESCAPE)):
		_on_close_pressed()

func _on_close_pressed() -> void:
	visible = false
	emit_signal("closed")
	queue_free()
