# ============================================================
# MainMenu.gd  —  Professional Commercial Game Entrance Scene
# ============================================================
# Built with Brackeys 2D Assets:
# - Animated Knight Hero (breathing idle frames)
# - Animated Gold Coin (12-frame rotating animation)
# - Mossy Stone Platform & Ancient Portal
# - PixelOperator8 typography & ambient mote particles
# - Audio integration (BGM & SFX)
# ============================================================

extends Control
class_name MainMenu

const GameManagerScript = preload("res://Scripts/GameManager.gd")

signal play_pressed
signal difficulty_pressed
signal settings_pressed
signal statistics_pressed
signal credits_pressed
signal quit_pressed

# Button nodes
@onready var btn_play: Button = $CenterContainer/VBoxContainer/BtnPlay
@onready var btn_difficulty: Button = $CenterContainer/VBoxContainer/BtnDifficulty
@onready var btn_settings: Button = $CenterContainer/VBoxContainer/BtnSettings
@onready var btn_statistics: Button = $CenterContainer/VBoxContainer/BtnStatistics
@onready var btn_credits: Button = $CenterContainer/VBoxContainer/BtnCredits
@onready var btn_quit: Button = $CenterContainer/VBoxContainer/BtnQuit

# Animation nodes
@onready var title_label: Label = $TitleContainer/TitleLabel
@onready var background_panel: Panel = $Background

# Button list for navigation
var _buttons: Array[Button] = []
var _current_button_index: int = 0

# State
var _difficulties: Array[String] = ["BEGINNER", "MODERATE", "ADVANCED", "EXPERT"]
var _difficulty_idx: int = 0
var _info_popup: Panel = null
var _info_label: Label = null

# Sprite animation references
var _knight_spr: Sprite2D = null
var _coin_spr: Sprite2D = null
var _slime_spr: Sprite2D = null
var _knight_timer: float = 0.0
var _knight_frame: int = 0
var _coin_timer: float = 0.0
var _coin_frame: int = 0
var _slime_timer: float = 0.0
var _slime_frame: int = 0

func _ready() -> void:
	# Collect buttons
	_buttons = [btn_play, btn_difficulty, btn_settings, btn_statistics, btn_credits, btn_quit]
	
	# Hook button audio
	if has_node("/root/AudioManager"):
		var audio = get_node("/root/AudioManager")
		audio.hook_buttons(self)
		audio.play_music()

	# Connect button signals
	for i in range(_buttons.size()):
		_buttons[i].mouse_entered.connect(_on_button_hovered.bind(i))
		_buttons[i].pressed.connect(_on_button_pressed.bind(i))
	
	_create_info_popup()
	_setup_game_entrance_visuals()

	# Start entrance animations
	_animate_title_in()
	_animate_buttons_in()
	_animate_background()

func _process(delta: float) -> void:
	# 1) Knight idle animation: 4 frames at ~6.5 FPS
	if _knight_spr:
		_knight_timer += delta
		if _knight_timer >= 0.15:
			_knight_timer = 0.0
			_knight_frame = (_knight_frame + 1) % 4
			_knight_spr.frame = _knight_frame

	# 2) Coin spinning animation: 12 frames at ~12.5 FPS
	if _coin_spr:
		_coin_timer += delta
		if _coin_timer >= 0.08:
			_coin_timer = 0.0
			_coin_frame = (_coin_frame + 1) % 12
			_coin_spr.frame = _coin_frame

	# 3) Slime enemy idle breathing animation
	if _slime_spr:
		_slime_timer += delta
		if _slime_timer >= 0.15:
			_slime_timer = 0.0
			_slime_frame = (_slime_frame + 1) % 4
			_slime_spr.frame = _slime_frame

func _setup_game_entrance_visuals() -> void:
	var font_bold = load("res://Assets/fonts/PixelOperator8-Bold.ttf") as Font
	var font_reg = load("res://Assets/fonts/PixelOperator8.ttf") as Font
	
	# Sky background with gradient, drifting pixel clouds, and distant mountain silhouettes
	var sky_rect := TextureRect.new()
	sky_rect.name = "SkyBackground"
	sky_rect.size = Vector2(1280, 720)
	sky_rect.expand_mode = TextureRect.EXPAND_IGNORE_SIZE

	var sky_img := Image.create(1280, 720, false, Image.FORMAT_RGBA8)
	var top_sky := Color(0.12, 0.24, 0.48, 1.0)
	var mid_sky := Color(0.26, 0.48, 0.74, 1.0)
	var horizon := Color(0.60, 0.76, 0.90, 1.0)

	for y in range(720):
		var t := float(y) / 720.0
		var c: Color = top_sky.lerp(mid_sky, t / 0.6) if t < 0.6 else mid_sky.lerp(horizon, (t - 0.6) / 0.4)
		for x in range(1280):
			sky_img.set_pixel(x, y, c)

	# Fluffy pixel clouds
	var cloud_pts: Array[Vector2i] = [
		Vector2i(120, 100), Vector2i(320, 150), Vector2i(560, 90),
		Vector2i(780, 130), Vector2i(1020, 105), Vector2i(1200, 150)
	]
	for pt in cloud_pts:
		for dy in range(-18, 19):
			for dx in range(-48, 49):
				if (float(dx) / 48.0) ** 2.0 + (float(dy) / 18.0) ** 2.0 <= 1.0:
					var px: int = pt.x + dx
					var py: int = pt.y + dy
					if px >= 0 and px < 1280 and py >= 0 and py < 720:
						var col: Color = Color(0.70, 0.82, 0.92, 0.65) if dy > 4 else Color(0.96, 0.98, 1.0, 0.78)
						sky_img.set_pixel(px, py, col)

	# Distant mountain ridge across the lower portion
	var mtn_col := Color(0.18, 0.28, 0.44, 0.9)
	for x in range(1280):
		var h: float = sin(x * 0.008) * 80.0 + sin(x * 0.02) * 40.0 + cos(x * 0.05) * 20.0
		var top_y: int = int(520 - 100 + h)
		for y in range(clampi(top_y, 0, 719), 720):
			sky_img.set_pixel(x, y, mtn_col)

	sky_rect.texture = ImageTexture.create_from_image(sky_img)
	add_child(sky_rect)
	move_child(sky_rect, 0)

	if background_panel:
		background_panel.visible = false
		
	# Ambient floating particles (glowing motes of magical dust)
	var ambient_particles := CPUParticles2D.new()
	ambient_particles.name = "AmbientMotes"
	ambient_particles.position = Vector2(640, 720)
	ambient_particles.amount = 32
	ambient_particles.lifetime = 6.0
	ambient_particles.emission_shape = CPUParticles2D.EMISSION_SHAPE_RECTANGLE
	ambient_particles.emission_rect_extents = Vector2(640, 20)
	ambient_particles.direction = Vector2(0, -1)
	ambient_particles.spread = 24.0
	ambient_particles.gravity = Vector2(0, -8)
	ambient_particles.initial_velocity_min = 25.0
	ambient_particles.initial_velocity_max = 60.0
	ambient_particles.scale_amount_min = 2.0
	ambient_particles.scale_amount_max = 4.0
	ambient_particles.color = Color(0.45, 0.85, 1.0, 0.45)
	add_child(ambient_particles)
	move_child(ambient_particles, 1)

	# Title styling
	if title_label and font_bold:
		title_label.add_theme_font_override("font", font_bold)
		title_label.add_theme_font_size_override("font_size", 28)
		title_label.modulate = Color(1.0, 0.88, 0.38)
		title_label.text = "ADAPTIVE PLATFORMER"
		
		# Add Subtitle without symbols
		var tc := title_label.get_parent()
		if tc and not tc.has_node("SubtitleLabel"):
			var sub := Label.new()
			sub.name = "SubtitleLabel"
			sub.text = "THE EXPEDITION"
			sub.add_theme_font_override("font", font_reg)
			sub.add_theme_font_size_override("font_size", 11)
			sub.modulate = Color(0.40, 0.88, 1.0, 0.95)
			sub.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
			tc.add_child(sub)
			sub.position = Vector2(0, 52)
			
	# Textured pixel-art stone button styles matching authentic game aesthetics
	var tex_normal = load("res://Assets/sprites/btn_stone_normal.png") as Texture2D
	var tex_hover = load("res://Assets/sprites/btn_stone_hover.png") as Texture2D
	var tex_pressed = load("res://Assets/sprites/btn_stone_pressed.png") as Texture2D

	var btn_normal: StyleBox
	var btn_hover: StyleBox
	var btn_pressed: StyleBox

	if tex_normal and tex_hover:
		var s_norm := StyleBoxTexture.new()
		s_norm.texture = tex_normal
		s_norm.texture_margin_left = 6.0
		s_norm.texture_margin_right = 6.0
		s_norm.texture_margin_top = 6.0
		s_norm.texture_margin_bottom = 6.0
		s_norm.content_margin_left = 24.0
		s_norm.content_margin_right = 24.0
		s_norm.content_margin_top = 10.0
		s_norm.content_margin_bottom = 10.0
		btn_normal = s_norm

		var s_hov := StyleBoxTexture.new()
		s_hov.texture = tex_hover
		s_hov.texture_margin_left = 6.0
		s_hov.texture_margin_right = 6.0
		s_hov.texture_margin_top = 6.0
		s_hov.texture_margin_bottom = 6.0
		s_hov.content_margin_left = 24.0
		s_hov.content_margin_right = 24.0
		s_hov.content_margin_top = 10.0
		s_hov.content_margin_bottom = 10.0
		btn_hover = s_hov

		var s_press := StyleBoxTexture.new()
		s_press.texture = tex_pressed if tex_pressed else tex_hover
		s_press.texture_margin_left = 6.0
		s_press.texture_margin_right = 6.0
		s_press.texture_margin_top = 6.0
		s_press.texture_margin_bottom = 6.0
		s_press.content_margin_left = 24.0
		s_press.content_margin_right = 24.0
		s_press.content_margin_top = 11.0
		s_press.content_margin_bottom = 9.0
		btn_pressed = s_press
	else:
		var fb := StyleBoxFlat.new()
		fb.bg_color = Color(0.12, 0.16, 0.24, 0.95)
		fb.border_width_left = 2
		fb.border_width_top = 2
		fb.border_width_right = 2
		fb.border_width_bottom = 2
		fb.border_color = Color(0.35, 0.45, 0.60, 1.0)
		btn_normal = fb
		btn_hover = fb
		btn_pressed = fb

	var btn_texts := [
		"PLAY",
		"DIFFICULTY: " + _difficulties[_difficulty_idx],
		"AUDIO: ON",
		"ARCHITECTURE",
		"CREDITS",
		"QUIT"
	]

	for i in range(_buttons.size()):
		var btn = _buttons[i]
		if btn:
			btn.text = btn_texts[i]
			btn.add_theme_stylebox_override("normal", btn_normal)
			btn.add_theme_stylebox_override("hover", btn_hover)
			btn.add_theme_stylebox_override("pressed", btn_pressed)
			btn.add_theme_stylebox_override("focus", btn_hover)
			btn.add_theme_color_override("font_color", Color(0.88, 0.92, 0.98))
			btn.add_theme_color_override("font_hover_color", Color(1.0, 0.90, 0.35))
			btn.add_theme_color_override("font_pressed_color", Color(0.72, 0.80, 0.92))
			btn.add_theme_color_override("font_focus_color", Color(1.0, 0.90, 0.35))
			if font_bold:
				btn.add_theme_font_override("font", font_bold)
				btn.add_theme_font_size_override("font_size", 10)
				
	# Hero Stage Vignette (Knight hero and Enemy Slime standing on mossy platform)
	var stage := Control.new()
	stage.name = "HeroStage"
	stage.position = Vector2(260, 470)
	add_child(stage)

	# Clean Floating Mossy Platform (from platforms.png)
	var plat_tex = load("res://Assets/sprites/platforms.png") as Texture2D
	if plat_tex:
		var plat_spr := Sprite2D.new()
		plat_spr.texture = plat_tex
		plat_spr.region_enabled = true
		plat_spr.region_rect = Rect2(0, 0, 48, 16)
		plat_spr.scale = Vector2(3.2, 3.2)
		plat_spr.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
		stage.add_child(plat_spr)
		
	# Animated Enemy Slime preview (facing the knight)
	var slime_tex = load("res://Assets/sprites/slime_green.png") as Texture2D
	if slime_tex:
		_slime_spr = Sprite2D.new()
		_slime_spr.name = "SlimeHeroStage"
		_slime_spr.texture = slime_tex
		_slime_spr.hframes = 4
		_slime_spr.vframes = 3
		_slime_spr.frame = 0
		_slime_spr.scale = Vector2(2.6, 2.6)
		_slime_spr.position = Vector2(36, -34)
		_slime_spr.flip_h = true
		_slime_spr.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
		stage.add_child(_slime_spr)
		
	# Animated Knight hero (large, crisp nearest-neighbor, authentic idle frame animation)
	var knight_tex = load("res://Assets/sprites/knight.png") as Texture2D
	if knight_tex:
		_knight_spr = Sprite2D.new()
		_knight_spr.name = "KnightHero"
		_knight_spr.texture = knight_tex
		_knight_spr.hframes = 8
		_knight_spr.vframes = 8
		_knight_spr.frame = 0
		_knight_spr.scale = Vector2(3.2, 3.2)
		_knight_spr.position = Vector2(-28, -44)
		_knight_spr.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
		stage.add_child(_knight_spr)
		
		# Gentle breathing bob
		var tw := create_tween().set_loops()
		tw.tween_property(_knight_spr, "position:y", -47.0, 0.9).set_trans(Tween.TRANS_SINE)
		tw.tween_property(_knight_spr, "position:y", -44.0, 0.9).set_trans(Tween.TRANS_SINE)
		
	# Animated Gold Coin (12 frames spinning smoothly, positioned cleanly above Knight)
	var coin_tex = load("res://Assets/sprites/coin.png") as Texture2D
	if coin_tex:
		_coin_spr = Sprite2D.new()
		_coin_spr.name = "CoinHero"
		_coin_spr.texture = coin_tex
		_coin_spr.hframes = 12
		_coin_spr.frame = 0
		_coin_spr.scale = Vector2(2.6, 2.6)
		_coin_spr.position = Vector2(-28, -108) # Clear view to the left above the knight
		_coin_spr.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
		stage.add_child(_coin_spr)
		var ctw := create_tween().set_loops()
		ctw.tween_property(_coin_spr, "position:y", -114.0, 0.7).set_trans(Tween.TRANS_SINE)
		ctw.tween_property(_coin_spr, "position:y", -108.0, 0.7).set_trans(Tween.TRANS_SINE)

	# Bottom prompt: PRESS SPACE OR ENTER
	if font_reg:
		var prompt := Label.new()
		prompt.name = "StartPrompt"
		prompt.text = "[ PRESS SPACE OR ENTER TO PLAY ]"
		prompt.add_theme_font_override("font", font_reg)
		prompt.add_theme_font_size_override("font_size", 10)
		prompt.modulate = Color(0.85, 0.9, 1.0, 0.85)
		prompt.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
		prompt.position = Vector2(0, 664)
		prompt.size = Vector2(1280, 30)
		add_child(prompt)
		var ptw := create_tween().set_loops()
		ptw.tween_property(prompt, "modulate:a", 0.35, 0.7)
		ptw.tween_property(prompt, "modulate:a", 1.0, 0.7)

func _create_info_popup() -> void:
	_info_popup = Panel.new()
	_info_popup.size = Vector2(440, 260)
	_info_popup.position = (Vector2(1280, 720) - _info_popup.size) / 2.0
	_info_popup.visible = false
	_info_popup.z_index = 30
	
	var pop_style := StyleBoxFlat.new()
	pop_style.bg_color = Color(0.06, 0.08, 0.14, 0.98)
	pop_style.border_width_left = 2
	pop_style.border_width_top = 2
	pop_style.border_width_right = 2
	pop_style.border_width_bottom = 2
	pop_style.border_color = Color(0.35, 0.75, 0.95, 0.9)
	pop_style.shadow_size = 8
	_info_popup.add_theme_stylebox_override("panel", pop_style)
	add_child(_info_popup)

	var vbox := VBoxContainer.new()
	vbox.set_anchors_preset(Control.PRESET_FULL_RECT)
	vbox.offset_left = 24
	vbox.offset_top = 24
	vbox.offset_right = -24
	vbox.offset_bottom = -24
	vbox.add_theme_constant_override("separation", 14)
	_info_popup.add_child(vbox)

	_info_label = Label.new()
	_info_label.autowrap_mode = TextServer.AUTOWRAP_WORD
	_info_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	var font_reg = load("res://Assets/fonts/PixelOperator8.ttf") as Font
	if font_reg:
		_info_label.add_theme_font_override("font", font_reg)
		_info_label.add_theme_font_size_override("font_size", 9)
	vbox.add_child(_info_label)

	var close_btn := Button.new()
	close_btn.text = "CLOSE"
	close_btn.custom_minimum_size = Vector2(120, 36)
	close_btn.size_flags_horizontal = Control.SIZE_SHRINK_CENTER
	if font_reg:
		close_btn.add_theme_font_override("font", font_reg)
		close_btn.add_theme_font_size_override("font_size", 9)
	close_btn.pressed.connect(func(): _info_popup.visible = false)
	vbox.add_child(close_btn)
	if has_node("/root/AudioManager"):
		get_node("/root/AudioManager").hook_buttons(close_btn)

func _input(event: InputEvent) -> void:
	if not visible:
		return
	
	if event.is_action_pressed("ui_down"):
		_navigate_to_button(_current_button_index + 1)
	elif event.is_action_pressed("ui_up"):
		_navigate_to_button(_current_button_index - 1)
	elif event.is_action_pressed("ui_accept"):
		if _current_button_index >= 0 and _current_button_index < _buttons.size():
			_buttons[_current_button_index].pressed.emit()

func _navigate_to_button(index: int) -> void:
	index = wrapi(index, 0, _buttons.size())
	_current_button_index = index
	
	for i in range(_buttons.size()):
		if i == index:
			_buttons[i].grab_focus()
		else:
			_buttons[i].release_focus()

func _on_button_hovered(index: int) -> void:
	_current_button_index = index
	_animate_button_hover(_buttons[index])

func _on_button_pressed(index: int) -> void:
	_animate_button_press(_buttons[index])
	
	match index:
		0: 
			GameManagerScript.current_difficulty = _difficulties[_difficulty_idx]
			emit_signal("play_pressed")
			get_tree().change_scene_to_file("res://Scenes/Main.tscn")
		1: 
			_difficulty_idx = (_difficulty_idx + 1) % _difficulties.size()
			var selected_diff := _difficulties[_difficulty_idx]
			btn_difficulty.text = "DIFFICULTY: " + selected_diff
			GameManagerScript.current_difficulty = selected_diff
			emit_signal("difficulty_pressed")
		2: 
			if has_node("/root/AudioManager"):
				var is_muted: bool = get_node("/root/AudioManager").toggle_mute()
				btn_settings.text = "AUDIO: MUTED" if is_muted else "AUDIO: ON"
			emit_signal("settings_pressed")
		3: 
			_show_info("GAME SYSTEM ARCHITECTURE\n\n• C++ DDA Platform Generator Engine\n• Godot GDExtension v4.7 Architecture\n• Dynamic Difficulty Adjustment\n• Pure Retro Pixel Aesthetic")
			emit_signal("statistics_pressed")
		4: 
			_show_info("CREDITS & ASSETS\n\n• Artwork: Brackeys 2D Platformer Asset Pack\n• Audio: 'Time for Adventure' by Brackeys\n• Algorithm: C++ Pygame-Platformer Port\n• Engine: Godot 4.7 & C++ GDExtension")
			emit_signal("credits_pressed")
		5: 
			emit_signal("quit_pressed")
			get_tree().quit()

func _show_info(text: String) -> void:
	if _info_label and _info_popup:
		_info_label.text = text
		_info_popup.visible = true

func _animate_title_in() -> void:
	if title_label:
		title_label.modulate.a = 0.0
		title_label.position.y = -50
		var tween := create_tween()
		tween.set_parallel(true)
		tween.tween_property(title_label, "modulate:a", 1.0, 0.8)
		var tween2 := create_tween()
		tween2.tween_property(title_label, "position:y", 0.0, 0.8)
		tween2.set_ease(Tween.EASE_OUT)
		tween2.set_trans(Tween.TRANS_BACK)

func _animate_buttons_in() -> void:
	for i in range(_buttons.size()):
		var button: Button = _buttons[i]
		button.modulate.a = 0.0
		button.position.x = -100
		var tween := create_tween()
		tween.set_parallel(false)
		tween.tween_interval(0.1 + i * 0.08)
		tween.tween_property(button, "modulate:a", 1.0, 0.4)
		var tween2 := create_tween()
		tween2.tween_property(button, "position:x", 0.0, 0.4)
		tween2.set_ease(Tween.EASE_OUT)
		tween2.set_trans(Tween.TRANS_BACK)

func _animate_background() -> void:
	if background_panel:
		background_panel.modulate.a = 0.0
		var tween := create_tween()
		tween.tween_property(background_panel, "modulate:a", 1.0, 1.2)

func _animate_button_hover(button: Button) -> void:
	var tween := create_tween()
	tween.tween_property(button, "scale", Vector2(1.04, 1.04), 0.12)
	tween.set_ease(Tween.EASE_OUT)

func _animate_button_press(button: Button) -> void:
	var tween := create_tween()
	tween.tween_property(button, "scale", Vector2(0.96, 0.96), 0.08)
	tween.set_ease(Tween.EASE_IN)
	var tween2 := create_tween()
	tween2.tween_property(button, "scale", Vector2(1.0, 1.0), 0.08)
	tween2.set_ease(Tween.EASE_OUT)
