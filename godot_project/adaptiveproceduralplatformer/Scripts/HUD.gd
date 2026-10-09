## HUD.gd
## ============================================================
## Professional Adaptive Platform Generation Framework — HUD
##
## Professional Design:
##   Clean, modern dark fantasy aesthetic
##   Consistent color palette
##   Smooth animations
##   Proper visual hierarchy
##
## Panels:
##   Top-left  - Level / Difficulty / Theme / Objective
##   Top-right - Lives / Coins / Time / Deaths
##   Bottom center - Progress bar
##   Bottom left - Skill rating
##   Bottom right - Combo
##   Checkpoint indicator (flashes on activation)
##   Game-Over overlay (shown by Main.gd)
## ============================================================

extends CanvasLayer

# Set from Main.gd
var bridge: EngineBridge = null

# Updated each frame by Main.gd
var level_number:    int   = 1
var coins_collected: int   = 0
var elapsed_time:    float = 0.0
var deaths:          int   = 0
var lives:           int   = 3
var combo:           int   = 0
var progress:        float = 0.0
var enemies_remaining: int = 0

# Internal
var _prev_difficulty: int  = -1
var _checkpoint_flash: float = 0.0
var _current_theme: String = "FOREST"

# Visual manager reference
var _visual_manager: Node = null

# ── Node refs ─────────────────────────────────────────────────────
@onready var lbl_level      : Label = $PanelLeft/VBox/LblLevel
@onready var lbl_difficulty : Label = $PanelLeft/VBox/LblDifficulty
@onready var lbl_theme      : Label = $PanelLeft/VBox/LblTheme
@onready var lbl_objective  : Label = $PanelLeft/VBox/HBoxObjective/LblObjective

@onready var hbox_lives : Node = get_node_or_null("PanelRight/VBox/HBoxLives")
@onready var hbox_deaths : Node = get_node_or_null("PanelRight/VBox/HBoxDeaths")
@onready var lbl_lives  : Label = get_node_or_null("PanelRight/VBox/HBoxLives/LblLives") as Label
@onready var lbl_deaths : Label = get_node_or_null("PanelRight/VBox/HBoxDeaths/LblDeaths") as Label
@onready var lbl_coins  : Label = $PanelRight/VBox/HBoxCoins/LblCoins
@onready var lbl_timer  : Label = $PanelRight/VBox/HBoxTime/LblTimer

@onready var lbl_progress_percent : Label = $PanelProgress/VBoxProgress/HBoxProgressHeader/LblProgressPercent
@onready var progress_bar        : ProgressBar = $PanelProgress/VBoxProgress/ProgressBar

@onready var lbl_skill : Label = $PanelSkill/VBoxSkill/LblSkill
@onready var lbl_combo : Label = $PanelMiniStats/VBoxMiniStats/LblCombo

@onready var _checkpoint_lbl   : Label = $CheckpointFlash
@onready var _game_over_panel  : Panel = $GameOverPanel
@onready var lbl_coins_value   : Label = $GameOverPanel/VBox/HBoxStats/VBoxStatsRight/LblCoinsValue

# ─────────────────────────────────────────────────────────────────
func _ready() -> void:
	if _game_over_panel: _game_over_panel.visible = false
	if hbox_lives: (hbox_lives as Control).visible = true
	if hbox_deaths: (hbox_deaths as Control).visible = true
	
	# Hide generic AI dashboard panels
	var hbox_c := get_node_or_null("PanelRight/VBox/HBoxCoins") as Control
	if hbox_c: hbox_c.visible = false
	var pmini := get_node_or_null("PanelMiniStats") as Control
	if pmini: pmini.visible = false
	var pprog := get_node_or_null("PanelProgress") as Control
	if pprog: pprog.visible = false
	var pskill := get_node_or_null("PanelSkill") as Control
	if pskill: pskill.visible = false
	
	# Get visual manager
	_visual_manager = get_node_or_null("/root/VisualManager")
	if _visual_manager:
		_visual_manager.connect("theme_changed", _on_theme_changed)
	if _checkpoint_lbl:  _checkpoint_lbl.modulate.a = 0.0

	_apply_pixel_game_aesthetic()

var minimap_node: Minimap = null

func setup_minimap(ld: Dictionary, player: Node2D, camera: CameraController = null) -> void:
	if minimap_node == null:
		_create_minimap()
	if minimap_node:
		minimap_node.setup_level(ld, player, camera)

func _create_minimap() -> void:
	var pright := get_node_or_null("PanelRight") as Control
	var pright_vbox := get_node_or_null("PanelRight/VBox") as VBoxContainer
	if pright and pright_vbox and minimap_node == null:
		minimap_node = Minimap.new()
		minimap_node.name = "RadarMinimap"
		minimap_node.custom_minimum_size = Vector2(200, 92)
		pright_vbox.add_child(minimap_node)
		pright_vbox.move_child(minimap_node, 0)
		pright.size = Vector2(224, 154)
		pright.position.x = 1280.0 - 240.0

func _apply_pixel_game_aesthetic() -> void:
	_create_minimap()

	var font_bold = load("res://Assets/fonts/PixelOperator8-Bold.ttf") as Font
	var font_reg = load("res://Assets/fonts/PixelOperator8.ttf") as Font
	
	# Pixel border stylebox
	var pixel_style := StyleBoxFlat.new()
	pixel_style.bg_color = Color(0.07, 0.08, 0.12, 0.94)
	pixel_style.border_width_left = 2
	pixel_style.border_width_top = 2
	pixel_style.border_width_right = 2
	pixel_style.border_width_bottom = 2
	pixel_style.border_color = Color(0.32, 0.44, 0.62, 0.95)
	pixel_style.corner_radius_top_left = 0
	pixel_style.corner_radius_top_right = 0
	pixel_style.corner_radius_bottom_right = 0
	pixel_style.corner_radius_bottom_left = 0
	pixel_style.shadow_color = Color(0, 0, 0, 0.6)
	pixel_style.shadow_size = 4
	pixel_style.content_margin_left = 12.0
	pixel_style.content_margin_top = 10.0
	pixel_style.content_margin_right = 12.0
	pixel_style.content_margin_bottom = 10.0
	
	for panel_name in ["PanelLeft", "PanelRight"]:
		var p := get_node_or_null(panel_name) as Panel
		if p:
			p.add_theme_stylebox_override("panel", pixel_style)

	# Apply font to all labels
	if font_bold and font_reg:
		for lbl in [lbl_level, lbl_difficulty, lbl_theme, lbl_objective]:
			if lbl:
				lbl.add_theme_font_override("font", font_bold if lbl == lbl_level or lbl == lbl_difficulty else font_reg)
				lbl.add_theme_font_size_override("font_size", 11 if lbl == lbl_level else 8)
		for lbl in [lbl_lives, lbl_deaths, lbl_timer]:
			if lbl:
				lbl.add_theme_font_override("font", font_bold if lbl == lbl_lives or lbl == lbl_deaths else font_reg)
				lbl.add_theme_font_size_override("font_size", 9)
		var obj_lbl := get_node_or_null("PanelLeft/VBox/HBoxObjective/LblObjectiveLabel") as Label
		if obj_lbl:
			obj_lbl.add_theme_font_override("font", font_reg)
			obj_lbl.add_theme_font_size_override("font_size", 8)
		for p_lbl in ["PanelRight/VBox/HBoxLives/LblLivesLabel", "PanelRight/VBox/HBoxDeaths/LblDeathsLabel", "PanelRight/VBox/HBoxTime/LblTimeLabel"]:
			var node := get_node_or_null(p_lbl) as Label
			if node:
				node.add_theme_font_override("font", font_reg)
				node.add_theme_font_size_override("font_size", 8)

	# Add Knight portrait inside PanelLeft
	var panel_left_vbox := get_node_or_null("PanelLeft/VBox") as VBoxContainer
	if panel_left_vbox and not panel_left_vbox.has_node("HBoxKnightHeader") and lbl_level and lbl_difficulty:
		var knight_tex = load("res://Assets/sprites/knight.png") as Texture2D
		if knight_tex:
			var hbox_hdr := HBoxContainer.new()
			hbox_hdr.name = "HBoxKnightHeader"
			hbox_hdr.add_theme_constant_override("separation", 8)
			
			var avatar := TextureRect.new()
			avatar.name = "KnightAvatar"
			var atlas := AtlasTexture.new()
			atlas.atlas = knight_tex
			atlas.region = Rect2(0, 0, 32, 32)
			avatar.texture = atlas
			avatar.custom_minimum_size = Vector2(28, 28)
			avatar.stretch_mode = TextureRect.STRETCH_KEEP_ASPECT_CENTERED
			avatar.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
			
			hbox_hdr.add_child(avatar)
			var sub_vbox := VBoxContainer.new()
			sub_vbox.add_theme_constant_override("separation", 2)
			panel_left_vbox.remove_child(lbl_level)
			panel_left_vbox.remove_child(lbl_difficulty)
			sub_vbox.add_child(lbl_level)
			sub_vbox.add_child(lbl_difficulty)
			hbox_hdr.add_child(sub_vbox)
			
			panel_left_vbox.add_child(hbox_hdr)
			panel_left_vbox.move_child(hbox_hdr, 0)
			

func _on_theme_changed(_theme: int) -> void:
	if _visual_manager:
		var accent_color: Color = _visual_manager.call("get_ui_accent_color")
		if lbl_difficulty:
			lbl_difficulty.modulate = accent_color

func _process(delta: float) -> void:
	_refresh()
	if _checkpoint_flash > 0.0:
		_checkpoint_flash -= delta
		if _checkpoint_lbl:
			_checkpoint_lbl.modulate.a = clampf(_checkpoint_flash * 2.0, 0.0, 1.0)

# ─────────────────────────────────────────────────────────────────
func _refresh() -> void:
	if lbl_level == null: return

	# Left panel - Level progression
	lbl_level.text = "LEVEL %d" % level_number
	lbl_theme.text = _current_theme
	if enemies_remaining > 0:
		lbl_objective.text = "DEFEAT ENEMIES: %d" % enemies_remaining
		lbl_objective.modulate = Color(1.0, 0.45, 0.35)
	else:
		lbl_objective.text = "ENEMIES CLEARED!"
		lbl_objective.modulate = Color(0.4, 1.0, 0.5)

	# 4 Level Difficulty Classification: BEGINNER, MODERATE, ADVANCED, EXPERT
	var diff_str := "BEGINNER"
	var gm_script = load("res://Scripts/GameManager.gd")
	if gm_script and "current_difficulty" in gm_script and str(gm_script.get("current_difficulty")) != "":
		diff_str = str(gm_script.get("current_difficulty")).to_upper()
	elif bridge:
		var d_lvl: int = bridge.get_difficulty_level()
		match d_lvl:
			0, 1: diff_str = "BEGINNER"
			2: diff_str = "MODERATE"
			3: diff_str = "ADVANCED"
			_: diff_str = "EXPERT"
	lbl_difficulty.text = "[ %s ]" % diff_str

	# Right panel - Player stats
	if lbl_lives:
		var hearts := ""
		for i in range(clampi(lives, 0, 5)):
			hearts += "♥ "
		lbl_lives.text = hearts.strip_edges() if not hearts.is_empty() else "0"
		lbl_lives.modulate = Color(1.0, 0.3, 0.35)

	if lbl_deaths:
		lbl_deaths.text = str(deaths)
	if lbl_coins:
		lbl_coins.text = str(coins_collected)

	var total_sec := int(elapsed_time)
	var mm := total_sec / 60
	var ss := total_sec % 60
	lbl_timer.text = "%02d:%02d" % [mm, ss]

	# Bottom center - Progress bar
	progress_bar.value = progress * 100.0
	lbl_progress_percent.text = "%.0f%%" % (progress * 100.0)

	# Bottom left - Skill rating
	if lbl_skill:
		lbl_skill.text = "%.0f%%" % (bridge.get_skill_score() * 100.0 if bridge else 0.0)

	# Bottom right - Combo
	lbl_combo.text = "x%d" % combo

# ─────────────────────────────────────────────────────────────────
# Public methods called by Main.gd
# ─────────────────────────────────────────────────────────────────

func set_combo(value: int) -> void:
	combo = value

func set_progress(value: float) -> void:
	progress = clampf(value, 0.0, 1.0)

func set_theme(theme_name: String) -> void:
	_current_theme = theme_name.to_upper()

func flash_death() -> void:
	var tween := create_tween()
	tween.tween_property($PanelRight, "modulate", Color(1.4, 0.4, 0.4), 0.08)
	tween.tween_property($PanelRight, "modulate", Color.WHITE, 0.25)

func flash_checkpoint() -> void:
	pass

func show_game_over() -> void:
	if _game_over_panel:
		# ── Populate stats BEFORE making visible ──────────────────────────
		# deaths and elapsed_time are synced every frame by Main.gd
		var lbl_dv := _game_over_panel.get_node_or_null(
			"CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsLeft/LblDeathsValue"
		) as Label
		if lbl_dv:
			lbl_dv.text = str(deaths)

		var lbl_tv := _game_over_panel.get_node_or_null(
			"CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsCenter/LblTimeValue"
		) as Label
		if lbl_tv:
			var total_sec := int(elapsed_time)
			lbl_tv.text = "%02d:%02d" % [total_sec / 60, total_sec % 60]

		# Apply pixel fonts to stat labels
		var font_bold = load("res://Assets/fonts/PixelOperator8-Bold.ttf") as Font
		var font_reg  = load("res://Assets/fonts/PixelOperator8.ttf") as Font
		for node_path in [
			"CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsLeft/LblDeathsValue",
			"CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsCenter/LblTimeValue",
		]:
			var lbl := _game_over_panel.get_node_or_null(node_path) as Label
			if lbl and font_bold:
				lbl.add_theme_font_override("font", font_bold)
				lbl.add_theme_font_size_override("font_size", 20)
		for node_path in [
			"CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsLeft/LblDeathsLabel",
			"CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsCenter/LblTimeLabel",
		]:
			var lbl := _game_over_panel.get_node_or_null(node_path) as Label
			if lbl and font_reg:
				lbl.add_theme_font_override("font", font_reg)
				lbl.add_theme_font_size_override("font_size", 9)

		_game_over_panel.visible = true

		var stats_right := _game_over_panel.get_node_or_null("VBox/HBoxStats/VBoxStatsRight") as Control
		if stats_right:
			stats_right.visible = false

		var lbl_go := _game_over_panel.get_node_or_null("VBox/LblGameOver") as Label
		if lbl_go and font_bold:
			lbl_go.add_theme_font_override("font", font_bold)
			lbl_go.add_theme_font_size_override("font_size", 28)

		var lbl_hint := _game_over_panel.get_node_or_null("VBox/LblHint") as Label
		if lbl_hint and font_reg:
			lbl_hint.add_theme_font_override("font", font_reg)
			lbl_hint.add_theme_font_size_override("font_size", 10)

		# Stone button styles matching game texture
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

		var vbox := _game_over_panel.get_node_or_null("VBox")
		if vbox and not vbox.has_node("BtnRetry"):
			var btn := Button.new()
			btn.name = "BtnRetry"
			btn.text = "RESTART (R)"
			btn.custom_minimum_size = Vector2(200, 44)
			btn.add_theme_stylebox_override("normal", btn_normal)
			btn.add_theme_stylebox_override("hover", btn_hover)
			btn.add_theme_stylebox_override("pressed", btn_hover)
			btn.add_theme_stylebox_override("focus", btn_hover)
			if font_bold:
				btn.add_theme_font_override("font", font_bold)
				btn.add_theme_font_size_override("font_size", 10)
			btn.pressed.connect(func():
				var main := get_parent()
				if main and main.has_method("_restart_level"):
					main.call("_restart_level")
			)
			vbox.add_child(btn)

			var btn_menu := Button.new()
			btn_menu.name = "BtnMainMenu"
			btn_menu.text = "MAIN MENU"
			btn_menu.custom_minimum_size = Vector2(200, 44)
			btn_menu.add_theme_stylebox_override("normal", btn_normal)
			btn_menu.add_theme_stylebox_override("hover", btn_hover)
			btn_menu.add_theme_stylebox_override("pressed", btn_hover)
			btn_menu.add_theme_stylebox_override("focus", btn_hover)
			if font_bold:
				btn_menu.add_theme_font_override("font", font_bold)
				btn_menu.add_theme_font_size_override("font_size", 10)
			btn_menu.pressed.connect(func():
				get_tree().change_scene_to_file("res://Scenes/MainMenu.tscn")
			)
			vbox.add_child(btn_menu)

			if has_node("/root/AudioManager"):
				get_node("/root/AudioManager").hook_buttons(btn)
				get_node("/root/AudioManager").hook_buttons(btn_menu)

func hide_game_over() -> void:
	if _game_over_panel:
		_game_over_panel.visible = false

# ─────────────────────────────────────────────────────────────────
func _flash_difficulty() -> void:
	if lbl_difficulty == null: return
	var tween := create_tween()
	tween.tween_property(lbl_difficulty, "modulate", Color(2.0, 2.0, 0.4), 0.12)
	tween.tween_property(lbl_difficulty, "modulate", Color.WHITE, 0.4)
