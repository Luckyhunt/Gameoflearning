# ============================================================
# VisualManager.gd  —  Visual System Integration Manager
# ============================================================
# Coordinates all visual systems (ArtDirector, TileVisualGenerator,
# BackgroundParallax, ParticleSystem) with existing game architecture
# Maintains modular, signal-based communication

extends Node
class_name VisualManager

# Visual system components
var _art_director: ArtDirector = null
var _tile_visual_generator: TileVisualGenerator = null
var _visual_tile_renderer: VisualTileRenderer = null
var _background_parallax: BackgroundParallax = null
var _particle_system: ParticleSystem = null

# Game manager reference
var _game_manager: Node = null

# Current theme
var _current_theme: ArtDirector.LevelTheme = ArtDirector.LevelTheme.FOREST

# Signals
signal theme_changed(theme: ArtDirector.LevelTheme)
signal visual_ready()

func _ready() -> void:
	# Initialize visual systems
	_initialize_visual_systems()
	
	# Connect to game manager
	_connect_to_game_manager()
	
	# Set default theme
	set_theme(ArtDirector.LevelTheme.FOREST)
	
	emit_signal("visual_ready")

func _initialize_visual_systems() -> void:
	# Art Director (single instance in scene tree)
	_art_director = get_node_or_null("/root/ArtDirector")
	if not _art_director:
		_art_director = ArtDirector.new()
		get_tree().root.add_child(_art_director)
		_art_director.name = "ArtDirector"
	
	# Tile Visual Generator
	_tile_visual_generator = TileVisualGenerator.new()
	_tile_visual_generator.set_art_director(_art_director)
	add_child(_tile_visual_generator)
	
	# Visual Tile Renderer
	_visual_tile_renderer = VisualTileRenderer.new()
	add_child(_visual_tile_renderer)
	
	# Background Parallax
	_background_parallax = BackgroundParallax.new()
	add_child(_background_parallax)
	
	# Particle System
	_particle_system = ParticleSystem.new()
	add_child(_particle_system)
	
	# Connect theme changes
	_art_director.theme_changed.connect(_on_theme_changed)

func _connect_to_game_manager() -> void:
	# Find game manager
	_game_manager = get_node_or_null("/root/GameManager")
	
	if not _game_manager:
		# Try to find it in the scene tree
		_game_manager = get_tree().get_first_node_in_group("game_manager")
	
	if _game_manager:
		# Connect to game manager signals
		if _game_manager.has_signal("level_loaded"):
			_game_manager.connect("level_loaded", _on_level_loaded)
		if _game_manager.has_signal("level_started"):
			_game_manager.connect("level_started", _on_level_started)
		if _game_manager.has_signal("level_completed"):
			_game_manager.connect("level_completed", _on_level_completed)
		if _game_manager.has_signal("player_spawned"):
			_game_manager.connect("player_spawned", _on_player_spawned)

# Theme management
func set_theme(theme: ArtDirector.LevelTheme) -> void:
	_current_theme = theme
	if _art_director:
		_art_director.set_theme(theme)
	emit_signal("theme_changed", theme)

func get_current_theme() -> ArtDirector.LevelTheme:
	return _current_theme

func get_theme_name() -> String:
	if _art_director:
		return _art_director.get_theme_name()
	return "UNKNOWN"

func _on_theme_changed(theme: ArtDirector.LevelTheme) -> void:
	_current_theme = theme
	emit_signal("theme_changed", theme)

# Level integration
func _on_level_loaded(level_data) -> void:
	# Apply visual theme to level
	if _visual_tile_renderer:
		_visual_tile_renderer.apply_theme_to_level(level_data.tiles)

func _on_level_started() -> void:
	# Set background parallax scroll
	if _background_parallax:
		_background_parallax.scroll_offset = Vector2.ZERO

func _on_level_completed() -> void:
	# Spawn completion particles
	if _particle_system:
		var player = get_tree().get_first_node_in_group("player")
		if player:
			_particle_system.spawn_powerup(player.global_position)

func _on_player_spawned(_player_position: Vector2) -> void:
	pass

# Particle effect integration
func spawn_dust(position: Vector2) -> void:
	if _particle_system:
		_particle_system.spawn_dust(position)

func spawn_landing(position: Vector2) -> void:
	if _particle_system:
		_particle_system.spawn_landing(position)

func spawn_jump(position: Vector2) -> void:
	if _particle_system:
		_particle_system.spawn_jump(position)

func spawn_coin(position: Vector2) -> void:
	if _particle_system:
		_particle_system.spawn_coin(position)

func spawn_checkpoint(position: Vector2) -> void:
	if _particle_system:
		_particle_system.spawn_checkpoint(position)

func spawn_hazard(position: Vector2) -> void:
	if _particle_system:
		_particle_system.spawn_hazard(position)

func spawn_death(position: Vector2) -> void:
	if _particle_system:
		_particle_system.spawn_death(position)

func spawn_powerup(position: Vector2) -> void:
	if _particle_system:
		_particle_system.spawn_powerup(position)

# Tile renderer integration
func set_tile_map(tile_map: TileMap) -> void:
	if _visual_tile_renderer:
		_visual_tile_renderer.set_tile_map(tile_map)

# Background integration
func set_background_scroll(offset: Vector2) -> void:
	if _background_parallax:
		_background_parallax.set_background_scroll(offset)

# Color access for other systems
func get_color(color_name: String) -> Color:
	return _art_director.get_color(color_name)

func get_palette() -> Dictionary:
	return _art_director.get_palette()

func get_platform_color(variant: int = 0) -> Color:
	return _art_director.get_platform_color(variant)

func get_hazard_color() -> Color:
	return _art_director.get_hazard_color()

func get_coin_color() -> Color:
	return _art_director.get_coin_color()

func get_background_color() -> Color:
	return _art_director.get_background_color()

func get_ui_accent_color() -> Color:
	return _art_director.get_ui_accent_color()

# Theme cycling for testing
func cycle_theme() -> void:
	var themes = ArtDirector.LevelTheme.values()
	var current_index := themes.find(_current_theme)
	var next_index := (current_index + 1) % themes.size()
	set_theme(themes[next_index])

# Theme selection based on level/difficulty
func select_theme_for_level(level_number: int, difficulty: int) -> void:
	var theme := ArtDirector.LevelTheme.FOREST
	
	# Cycle through themes based on level
	match level_number % 8:
		0: theme = ArtDirector.LevelTheme.FOREST
		1: theme = ArtDirector.LevelTheme.ICE
		2: theme = ArtDirector.LevelTheme.LAVA
		3: theme = ArtDirector.LevelTheme.CASTLE
		4: theme = ArtDirector.LevelTheme.FACTORY
		5: theme = ArtDirector.LevelTheme.CAVE
		6: theme = ArtDirector.LevelTheme.RUINS
		7: theme = ArtDirector.LevelTheme.UNDERGROUND
	
	# Adjust theme based on difficulty (higher difficulty = darker themes)
	if difficulty >= 4:  # Expert+
		match theme:
			ArtDirector.LevelTheme.FOREST: theme = ArtDirector.LevelTheme.CAVE
			ArtDirector.LevelTheme.ICE: theme = ArtDirector.LevelTheme.UNDERGROUND
			ArtDirector.LevelTheme.LAVA: theme = ArtDirector.LevelTheme.FACTORY
	
	set_theme(theme)
