# ============================================================
# GameOverScreen.gd  —  Professional Game Over Screen
# ============================================================
# Features: Statistics display, animated XP bar,
# difficulty recommendation, smooth transitions

extends Control
class_name GameOverScreen

signal retry_pressed
signal main_menu_pressed
signal next_difficulty_pressed

# Statistics nodes
@onready var lbl_deaths_value: Label = $CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsLeft/LblDeathsValue
@onready var lbl_coins_value: Label = $CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsRight/LblCoinsValue
@onready var lbl_time_value: Label = $CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsCenter/LblTimeValue
@onready var lbl_accuracy_value: Label = $CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsRight/LblAccuracyValue

# Recommendation nodes
@onready var lbl_recommendation: Label = $CenterContainer/VBoxContainer/RecommendationContainer/LblRecommendation

# Button nodes
@onready var btn_retry: Button = $CenterContainer/VBoxContainer/ButtonContainer/HBoxButtons/BtnRetry
@onready var btn_main_menu: Button = $CenterContainer/VBoxContainer/ButtonContainer/HBoxButtons/BtnMainMenu
@onready var btn_next_difficulty: Button = $CenterContainer/VBoxContainer/ButtonContainer/HBoxButtons/BtnNextDifficulty

# XP bar
@onready var xp_bar: ProgressBar = $CenterContainer/VBoxContainer/XPContainer/ProgressBar
@onready var lbl_xp_percent: Label = $CenterContainer/VBoxContainer/XPContainer/HBoxXP/LblXPPercent

# Background
@onready var background_panel: Panel = $Background

func _ready() -> void:
	# Connect button signals
	btn_retry.pressed.connect(_on_retry_pressed)
	btn_main_menu.pressed.connect(_on_main_menu_pressed)
	btn_next_difficulty.pressed.connect(_on_next_difficulty_pressed)
	
	if has_node("/root/AudioManager"):
		get_node("/root/AudioManager").hook_buttons(self)

	# Start animations
	_animate_in()

func set_statistics(deaths: int, _coins: int, time: float, accuracy: float) -> void:
	lbl_deaths_value.text = str(deaths)
	if lbl_coins_value and lbl_coins_value.get_parent():
		(lbl_coins_value.get_parent() as Control).visible = false
	
	var total_sec := int(time)
	var mm := total_sec / 60
	var ss := total_sec % 60
	lbl_time_value.text = "%02d:%02d" % [mm, ss]
	
	lbl_accuracy_value.text = "%.1f%%" % (accuracy * 100.0)

func set_recommendation(text: String) -> void:
	lbl_recommendation.text = text

func set_xp_progress(value: float) -> void:
	value = clampf(value, 0.0, 1.0)
	xp_bar.value = value * 100.0
	lbl_xp_percent.text = "%.0f%%" % (value * 100.0)

func _animate_in() -> void:
	visible = true
	modulate.a = 0.0
	
	# Animate background
	if background_panel:
		background_panel.modulate.a = 0.0
		var tween := create_tween()
		tween.tween_property(background_panel, "modulate:a", 1.0, 0.3)
	
	# Animate content
	var tween := create_tween()
	tween.tween_property(self, "modulate:a", 1.0, 0.4)
	
	# Animate XP bar
	_animate_xp_bar()

func _animate_xp_bar() -> void:
	var target_value := xp_bar.value
	xp_bar.value = 0.0
	
	var tween := create_tween()
	tween.tween_interval(0.3)
	tween.tween_property(xp_bar, "value", target_value, 0.8).set_ease(Tween.EASE_OUT)

func _on_retry_pressed() -> void:
	emit_signal("retry_pressed")
	animate_out()

func _on_main_menu_pressed() -> void:
	emit_signal("main_menu_pressed")
	animate_out()

func _on_next_difficulty_pressed() -> void:
	emit_signal("next_difficulty_pressed")
	animate_out()

func animate_out() -> void:
	var tween := create_tween()
	tween.tween_property(self, "modulate:a", 0.0, 0.3)
	await tween.finished
	visible = false
