## ============================================================
## AudioManager.gd — Global Audio & Sound Management System
## ============================================================
## Handles:
## - Background Music (BGM) playback with volume and pause controls
## - Polyphonic Sound Effects (SFX) pooling for zero cutoff
## - UI button audio binding (hover & click)
## - Volume sliders and mute toggling
## ============================================================

extends Node

# Audio buses / volume settings
var master_volume: float = 1.0
var music_volume: float = 0.8
var sfx_volume: float = 0.9
var is_muted: bool = false

# Nodes
var _music_player: AudioStreamPlayer
var _sfx_pool: Array[AudioStreamPlayer] = []
const SFX_POOL_SIZE: int = 12

# Loaded sound streams
var _sounds: Dictionary = {}
var _music_stream: AudioStream = null

func _ready() -> void:
	process_mode = Node.PROCESS_MODE_ALWAYS # Continue audio during pause
	
	# Create music player
	_music_player = AudioStreamPlayer.new()
	_music_player.name = "MusicPlayer"
	_music_player.bus = "Master"
	add_child(_music_player)
	
	# Create SFX pool
	for i in range(SFX_POOL_SIZE):
		var player := AudioStreamPlayer.new()
		player.name = "SFXPlayer_%d" % i
		player.bus = "Master"
		add_child(player)
		_sfx_pool.append(player)
		
	# Preload sounds
	_load_sound("jump", "res://Assets/sounds/jump.wav")
	_load_sound("coin", "res://Assets/sounds/coin.wav")
	_load_sound("hurt", "res://Assets/sounds/hurt.wav")
	_load_sound("explosion", "res://Assets/sounds/explosion.wav")
	_load_sound("power_up", "res://Assets/sounds/power_up.wav")
	_load_sound("tap", "res://Assets/sounds/tap.wav")
	
	# Preload music
	var music_path := "res://Assets/music/time_for_adventure.mp3"
	if ResourceLoader.exists(music_path):
		_music_stream = load(music_path)
		
	print("[AudioManager] Initialized with %d SFX channels" % SFX_POOL_SIZE)

func _load_sound(key: String, path: String) -> void:
	if ResourceLoader.exists(path):
		var stream: AudioStream = load(path)
		if stream:
			_sounds[key] = stream
			print("[AudioManager] Loaded sound: %s -> %s" % [key, path])
	else:
		push_warning("[AudioManager] Sound file not found: " + path)

## Play a named sound effect
func play_sound(key: String, pitch_scale: float = 1.0, volume_offset_db: float = 0.0) -> void:
	if is_muted or sfx_volume <= 0.001:
		return
	if not _sounds.has(key):
		return
		
	var stream: AudioStream = _sounds[key]
	var player: AudioStreamPlayer = _get_available_sfx_player()
	if player:
		player.stream = stream
		player.pitch_scale = pitch_scale
		var db_vol: float = linear_to_db(sfx_volume * master_volume) + volume_offset_db
		player.volume_db = clampf(db_vol, -80.0, 6.0)
		player.play()

## Play background music
func play_music(loop: bool = true, volume_offset_db: float = -6.0) -> void:
	if _music_stream == null or _music_player == null:
		return
	if _music_player.stream != _music_stream:
		_music_player.stream = _music_stream
	_update_music_volume(volume_offset_db)
	if not _music_player.playing:
		_music_player.play()

func stop_music() -> void:
	if _music_player:
		_music_player.stop()

func pause_music() -> void:
	if _music_player:
		_music_player.stream_paused = true

func resume_music() -> void:
	if _music_player:
		_music_player.stream_paused = false

func toggle_mute() -> bool:
	is_muted = !is_muted
	if is_muted:
		if _music_player: _music_player.volume_db = -80.0
	else:
		_update_music_volume()
	return is_muted

func set_master_volume(vol: float) -> void:
	master_volume = clampf(vol, 0.0, 1.0)
	_update_music_volume()

func set_music_volume(vol: float) -> void:
	music_volume = clampf(vol, 0.0, 1.0)
	_update_music_volume()

func set_sfx_volume(vol: float) -> void:
	sfx_volume = clampf(vol, 0.0, 1.0)

func _update_music_volume(offset_db: float = -6.0) -> void:
	if _music_player == null:
		return
	if is_muted or music_volume <= 0.001 or master_volume <= 0.001:
		_music_player.volume_db = -80.0
	else:
		var db_vol: float = linear_to_db(music_volume * master_volume) + offset_db
		_music_player.volume_db = clampf(db_vol, -80.0, 6.0)

func _get_available_sfx_player() -> AudioStreamPlayer:
	for p in _sfx_pool:
		if not p.playing:
			return p
	# Steal first if none free
	return _sfx_pool[0] if _sfx_pool.size() > 0 else null

## Recursively binds button hover and click sound to all Buttons in a node subtree
func hook_buttons(root: Node) -> void:
	if root is Button:
		_bind_button(root as Button)
	for child in root.get_children():
		hook_buttons(child)

func _bind_button(btn: Button) -> void:
	if not btn.is_connected("mouse_entered", Callable(self, "_on_btn_hover")):
		btn.mouse_entered.connect(_on_btn_hover)
	if not btn.is_connected("pressed", Callable(self, "_on_btn_pressed")):
		btn.pressed.connect(_on_btn_pressed)

func _on_btn_hover() -> void:
	play_sound("tap", 1.2, -4.0)

func _on_btn_pressed() -> void:
	play_sound("tap", 1.0, 0.0)
