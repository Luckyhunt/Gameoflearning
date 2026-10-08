# ============================================================
# LevelCompleteScreen.gd  —  Professional Level Complete Screen
# ============================================================
# Features: Statistics display, animated XP bar, stars rating,
# skill rating, smooth transitions

extends Control
class_name LevelCompleteScreen

signal next_level_pressed
signal main_menu_pressed
signal retry_pressed

# Statistics nodes
@onready var lbl_difficulty: Label = $CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsLeft/LblDifficulty
@onready var lbl_coins_value: Label = $CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsCenter/LblCoinsValue
@onready var lbl_deaths_value: Label = $CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsRight/LblDeathsValue
@onready var lbl_time_value: Label = $CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsCenter/LblTimeValue
@onready var lbl_accuracy_value: Label = $CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsRight/LblAccuracyValue
@onready var lbl_secrets_value: Label = $CenterContainer/VBoxContainer/StatsContainer/HBoxStats/VBoxStatsLeft/LblSecretsValue

# Rating nodes
@onready var lbl_skill_rating: Label = $CenterContainer/VBoxContainer/RatingContainer/LblSkillRating
@onready var star_container: HBoxContainer = $CenterContainer/VBoxContainer/RatingContainer/StarContainer

# XP bar
@onready var xp_bar: ProgressBar = $CenterContainer/VBoxContainer/XPContainer/ProgressBar
@onready var lbl_xp_percent: Label = $CenterContainer/VBoxContainer/XPContainer/HBoxXP/LblXPPercent

# Button nodes
@onready var btn_next_level: Button = $CenterContainer/VBoxContainer/ButtonContainer/HBoxButtons/BtnNextLevel
@onready var btn_retry: Button = $CenterContainer/VBoxContainer/ButtonContainer/HBoxButtons/BtnRetry
@onready var btn_main_menu: Button = $CenterContainer/VBoxContainer/ButtonContainer/HBoxButtons/BtnMainMenu

# Background
@onready var background_panel: Panel = $Background

# Star textures
var _star_filled: Texture2D = null
var _star_empty: Texture2D = null

func _ready() -> void:
	# Connect button signals
	btn_next_level.pressed.connect(_on_next_level_pressed)
	btn_retry.pressed.connect(_on_retry_pressed)
	btn_main_menu.pressed.connect(_on_main_menu_pressed)
	
	if has_node("/root/AudioManager"):
		get_node("/root/AudioManager").hook_buttons(self)

	# Create star textures
	_create_star_textures()
	
	# Start animations
	_animate_in()

func set_statistics(difficulty: String, _coins: int, deaths: int, time: float, accuracy: float, _secrets: int) -> void:
	lbl_difficulty.text = difficulty.to_upper()
	if lbl_coins_value and lbl_coins_value.get_parent():
		(lbl_coins_value.get_parent() as Control).visible = false
	if lbl_secrets_value and lbl_secrets_value.get_parent():
		(lbl_secrets_value.get_parent() as Control).visible = false
	lbl_deaths_value.text = str(deaths)
	
	var total_sec := int(time)
	var mm := total_sec / 60
	var ss := total_sec % 60
	lbl_time_value.text = "%02d:%02d" % [mm, ss]
	
	lbl_accuracy_value.text = "%.1f%%" % (accuracy * 100.0)

func set_skill_rating(rating: float) -> void:
	lbl_skill_rating.text = "SKILL RATING: %.0f%%" % (rating * 100.0)
	_update_stars(rating)

func set_xp_progress(value: float) -> void:
	value = clampf(value, 0.0, 1.0)
	xp_bar.value = value * 100.0
	lbl_xp_percent.text = "%.0f%%" % (value * 100.0)

func _create_star_textures() -> void:
	# Create simple star textures using code
	_star_filled = _create_star_texture(true)
	_star_empty = _create_star_texture(false)
	
	# Initialize stars
	_update_stars(0.0)

func _create_star_texture(filled: bool) -> Texture2D:
	var image := Image.create(32, 32, false, Image.FORMAT_RGBA8)
	image.fill(Color.TRANSPARENT)
	
	var color := Color(0.94, 0.76, 0.13, 1) if filled else Color(0.3, 0.3, 0.3, 1)
	
	# Draw star shape
	var center := Vector2(16, 16)
	var points: PackedVector2Array = []
	
	for i in range(10):
		var angle := deg_to_rad(i * 36 - 90)
		var radius := 12.0 if i % 2 == 0 else 6.0
		var point := center + Vector2(cos(angle), sin(angle)) * radius
		points.append(point)
	
	for i in range(points.size()):
		var next_i := (i + 1) % points.size()
		image.draw_line(points[i], points[next_i], color)
	
	var texture := ImageTexture.create_from_image(image)
	return texture

func _update_stars(rating: float) -> void:
	# Clear existing stars
	for child in star_container.get_children():
		child.queue_free()
	
	# Determine star count (0-5 stars based on rating)
	var star_count := int(rating * 5.0)
	star_count = clampi(star_count, 0, 5)
	
	# Create star nodes
	for i in range(5):
		var texture_rect := TextureRect.new()
		texture_rect.custom_minimum_size = Vector2(32, 32)
		texture_rect.texture = _star_filled if i < star_count else _star_empty
		texture_rect.stretch_mode = TextureRect.STRETCH_KEEP
		star_container.add_child(texture_rect)

func _animate_in() -> void:
	visible = true
	modulate.a = 0.0
	
	# Animate background
	if background_panel:
		background_panel.modulate.a = 0.0
		var tween := create_tween()
		tween.tween_property(background_panel, "modulate:a", 1.0, 0.4)
	
	# Animate content
	var tween := create_tween()
	tween.tween_property(self, "modulate:a", 1.0, 0.5)
	
	# Animate XP bar
	_animate_xp_bar()
	
	# Animate stars
	_animate_stars()

func _animate_xp_bar() -> void:
	var target_value := xp_bar.value
	xp_bar.value = 0.0
	
	var tween := create_tween()
	tween.tween_interval(0.4)
	tween.tween_property(xp_bar, "value", target_value, 1.0).set_ease(Tween.EASE_OUT)

func _animate_stars() -> void:
	var stars := star_container.get_children()
	for i in range(stars.size()):
		var star: TextureRect = stars[i]
		star.modulate.a = 0.0
		star.scale = Vector2(0.5, 0.5)
		
		var tween := create_tween()
		tween.set_parallel(false)
		tween.tween_interval(0.6 + i * 0.1)
		tween.set_parallel(true)
		tween.tween_property(star, "modulate:a", 1.0, 0.3)
		tween.tween_property(star, "scale", Vector2(1.0, 1.0), 0.3).set_ease(Tween.EASE_OUT).set_trans(Tween.TRANS_BACK)

func _on_next_level_pressed() -> void:
	emit_signal("next_level_pressed")
	animate_out()

func _on_retry_pressed() -> void:
	emit_signal("retry_pressed")
	animate_out()

func _on_main_menu_pressed() -> void:
	emit_signal("main_menu_pressed")
	animate_out()

func animate_out() -> void:
	var tween := create_tween()
	tween.tween_property(self, "modulate:a", 0.0, 0.4)
	await tween.finished
	visible = false
