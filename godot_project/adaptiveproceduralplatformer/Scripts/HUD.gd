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
	if hbox_lives: (hbox_lives as Control).visible = false
	if hbox_deaths: (hbox_deaths as Control).visible = false
	
	# Get visual manager
	_visual_manager = get_node_or_null("/root/VisualManager")
	if _visual_manager:
		_visual_manager.connect("theme_changed", _on_theme_changed)
	if _checkpoint_lbl:  _checkpoint_lbl.modulate.a = 0.0

func _on_theme_changed(theme: int) -> void:
	# Update HUD colors based on theme
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
	lbl_objective.text = "CLEAR FACULTY / ALIENS"

	if bridge:
		var diff_name: String = bridge.get_difficulty_name()
		var skill: float = bridge.get_skill_score()
		var diff_level: int = bridge.get_difficulty_level()

		lbl_difficulty.text = diff_name.to_upper()
		lbl_skill.text = "%.0f%%" % (skill * 100.0)

		if diff_level != _prev_difficulty:
			_prev_difficulty = diff_level
			_flash_difficulty()
	else:
		lbl_difficulty.text = "NORMAL"
		lbl_skill.text = "0%"

	# Right panel - Player stats
	if lbl_coins: lbl_coins.text = str(coins_collected)

	var total_sec := int(elapsed_time)
	var mm := total_sec / 60.0
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
	_checkpoint_flash = 2.0
	if _checkpoint_lbl:
		_checkpoint_lbl.text = "CHECKPOINT"
		_checkpoint_lbl.modulate.a = 1.0

func show_game_over() -> void:
	if _game_over_panel:
		_game_over_panel.visible = true
		if lbl_coins_value:
			lbl_coins_value.text = str(coins_collected)

func hide_game_over() -> void:
	if _game_over_panel:
		_game_over_panel.visible = false

# ─────────────────────────────────────────────────────────────────
func _flash_difficulty() -> void:
	if lbl_difficulty == null: return
	var tween := create_tween()
	tween.tween_property(lbl_difficulty, "modulate", Color(2.0, 2.0, 0.4), 0.12)
	tween.tween_property(lbl_difficulty, "modulate", Color.WHITE, 0.4)
