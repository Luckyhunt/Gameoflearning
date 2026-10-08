# ============================================================
# TileVisualGenerator.gd  —  Procedural Tile Visual Generation
# ============================================================
# Generates tile textures programmatically using ArtDirector color palettes
# Creates consistent visual language across all themes without external assets

extends Node
class_name TileVisualGenerator

# Reference to ArtDirector
var _art_director: ArtDirector = null

# Tile size
const TILE_SIZE: int = 32

# Generated texture cache
var _texture_cache: Dictionary = {}

func _ready() -> void:
	# Find or create ArtDirector
	_art_director = get_node_or_null("/root/ArtDirector")
	if not _art_director:
		_art_director = ArtDirector.new()
		get_tree().root.add_child(_art_director)
		_art_director.name = "ArtDirector"

func set_art_director(director: ArtDirector) -> void:
	_art_director = director

# Generate platform tile texture
func generate_platform_tile(variant: int = 0) -> Texture2D:
	var tex_path: String = "res://Assets/sprites/world_tileset.png"
	if ResourceLoader.exists(tex_path):
		return load(tex_path) as Texture2D

	var cache_key := "platform_%d_%d" % [_art_director.get_current_theme(), variant]
	if _texture_cache.has(cache_key):
		return _texture_cache[cache_key]
	
	var image := Image.create(TILE_SIZE, TILE_SIZE, false, Image.FORMAT_RGBA8)
	var base_color := _art_director.get_platform_color(variant)
	
	image.fill(base_color)
	_add_platform_texture(image, base_color, variant)
	
	var texture := ImageTexture.create_from_image(image)
	_texture_cache[cache_key] = texture
	return texture

# Generate hazard tile texture
func generate_hazard_tile() -> Texture2D:
	var cache_key := "hazard_%d" % _art_director.get_current_theme()
	if _texture_cache.has(cache_key):
		return _texture_cache[cache_key]
	
	var image := Image.create(TILE_SIZE, TILE_SIZE, false, Image.FORMAT_RGBA8)
	var hazard_color := _art_director.get_hazard_color()
	
	image.fill(hazard_color)
	_add_hazard_pattern(image, hazard_color)
	
	var texture := ImageTexture.create_from_image(image)
	_texture_cache[cache_key] = texture
	return texture

# Generate coin tile texture
func generate_coin_tile() -> Texture2D:
	var tex_path: String = "res://Assets/sprites/coin.png"
	if ResourceLoader.exists(tex_path):
		return load(tex_path) as Texture2D

	var cache_key := "coin_%d" % _art_director.get_current_theme()
	if _texture_cache.has(cache_key):
		return _texture_cache[cache_key]
	
	var image := Image.create(TILE_SIZE, TILE_SIZE, false, Image.FORMAT_RGBA8)
	image.fill(Color.TRANSPARENT)
	
	var coin_color := _art_director.get_coin_color()
	_add_coin_pattern(image, coin_color)
	
	var texture := ImageTexture.create_from_image(image)
	_texture_cache[cache_key] = texture
	return texture

# Generate checkpoint tile texture
func generate_checkpoint_tile() -> Texture2D:
	var cache_key := "checkpoint_%d" % _art_director.get_current_theme()
	if _texture_cache.has(cache_key):
		return _texture_cache[cache_key]
	
	var image := Image.create(TILE_SIZE, TILE_SIZE, false, Image.FORMAT_RGBA8)
	image.fill(Color.TRANSPARENT)
	
	var primary_color := _art_director.get_color("primary")
	
	# Draw checkpoint flag
	_add_checkpoint_pattern(image, primary_color)
	
	var texture := ImageTexture.create_from_image(image)
	_texture_cache[cache_key] = texture
	return texture

# Generate exit tile texture
func generate_exit_tile() -> Texture2D:
	var cache_key := "exit_%d" % _art_director.get_current_theme()
	if _texture_cache.has(cache_key):
		return _texture_cache[cache_key]
	
	var image := Image.create(TILE_SIZE, TILE_SIZE, false, Image.FORMAT_RGBA8)
	image.fill(Color.TRANSPARENT)
	
	var accent_color := _art_director.get_color("accent")
	
	# Draw exit portal
	_add_exit_pattern(image, accent_color)
	
	var texture := ImageTexture.create_from_image(image)
	_texture_cache[cache_key] = texture
	return texture

# Generate spawn tile texture
func generate_spawn_tile() -> Texture2D:
	var cache_key := "spawn_%d" % _art_director.get_current_theme()
	if _texture_cache.has(cache_key):
		return _texture_cache[cache_key]
	
	var image := Image.create(TILE_SIZE, TILE_SIZE, false, Image.FORMAT_RGBA8)
	image.fill(Color.TRANSPARENT)
	
	var primary_color := _art_director.get_color("primary")
	
	# Draw spawn marker
	_add_spawn_pattern(image, primary_color)
	
	var texture := ImageTexture.create_from_image(image)
	_texture_cache[cache_key] = texture
	return texture

# Clear texture cache (call when theme changes)
func clear_cache() -> void:
	_texture_cache.clear()

# Private: Add platform texture details
func _add_platform_texture(image: Image, base_color: Color, variant: int) -> void:
	var dark_color := base_color.darkened(0.2)
	var light_color := base_color.lightened(0.15)
	
	match variant:
		0: # Standard platform
			# Add top highlight
			for x in range(TILE_SIZE):
				image.set_pixel(x, 0, light_color)
				image.set_pixel(x, 1, light_color.lerp(base_color, 0.5))
			
			# Add bottom shadow
			for x in range(TILE_SIZE):
				image.set_pixel(x, TILE_SIZE - 1, dark_color)
				image.set_pixel(x, TILE_SIZE - 2, dark_color.lerp(base_color, 0.5))
		
		1: # Dark platform
			# Add subtle texture
			for x in range(0, TILE_SIZE, 4):
				for y in range(0, TILE_SIZE, 4):
					if (x + y) % 8 == 0:
						image.set_pixel(x, y, dark_color)
		
		2: # Light platform
			# Add highlight pattern
			for x in range(0, TILE_SIZE, 3):
				for y in range(0, TILE_SIZE, 3):
					if (x + y) % 6 == 0:
						image.set_pixel(x, y, light_color)

# Private: Add hazard pattern (spikes)
func _add_hazard_pattern(image: Image, hazard_color: Color) -> void:
	var dark_color := hazard_color.darkened(0.3)
	var light_color := hazard_color.lightened(0.2)
	
	# Draw triangular spikes
	for y in range(TILE_SIZE):
		for x in range(TILE_SIZE):
			var spike_height: float = float(TILE_SIZE) / 2.0
			var spike_width: int = TILE_SIZE / 3
			
			var spike_x: int = (x % spike_width)
			var spike_y: float = y - spike_height + abs(float(spike_x) - float(spike_width) / 2.0) * 2
			
			if spike_y >= 0 and spike_y < 2:
				image.set_pixel(x, y, light_color)
			elif spike_y >= 2 and spike_y < 4:
				image.set_pixel(x, y, hazard_color)
			elif spike_y >= 4:
				image.set_pixel(x, y, dark_color)

# Private: Add coin pattern
func _add_coin_pattern(image: Image, coin_color: Color) -> void:
	var center := Vector2(TILE_SIZE / 2.0, TILE_SIZE / 2.0)
	var radius := TILE_SIZE / 3.0
	
	# Draw coin circle
	for y in range(TILE_SIZE):
		for x in range(TILE_SIZE):
			var dist := center.distance_to(Vector2(x, y))
			if dist < radius:
				var shine := 1.0 - (dist / radius)
				var pixel_color := coin_color.lightened(shine * 0.3)
				image.set_pixel(x, y, pixel_color)
			elif dist < radius + 1:
				image.set_pixel(x, y, coin_color.darkened(0.2))

# Private: Add checkpoint pattern
func _add_checkpoint_pattern(image: Image, primary_color: Color) -> void:
	var flag_color := primary_color
	var pole_color := Color(0.4, 0.3, 0.2)
	
	# Draw pole
	for y in range(TILE_SIZE):
		image.set_pixel(int(TILE_SIZE / 2), y, pole_color)
	
	# Draw flag
	for y in range(int(TILE_SIZE / 2)):
		for x in range(int(TILE_SIZE / 2) + 1, TILE_SIZE):
			if x - int(TILE_SIZE / 2) < y + 2:
				image.set_pixel(x, y, flag_color)

# Private: Add exit pattern
func _add_exit_pattern(image: Image, accent_color: Color) -> void:
	var center := Vector2(TILE_SIZE / 2.0, TILE_SIZE / 2.0)
	var max_radius := TILE_SIZE / 2.0 - 2
	
	# Draw swirling portal
	for y in range(TILE_SIZE):
		for x in range(TILE_SIZE):
			var dist := center.distance_to(Vector2(x, y))
			var angle := atan2(y - center.y, x - center.x)
			
			if dist < max_radius:
				var spiral := sin(angle * 3 + dist * 0.5) * 0.5 + 0.5
				if spiral > 0.6:
					var alpha := 1.0 - (dist / max_radius)
					var pixel_color := accent_color.lightened(spiral * 0.3)
					pixel_color.a = alpha
					image.set_pixel(x, y, pixel_color)

# Private: Add spawn pattern
func _add_spawn_pattern(image: Image, primary_color: Color) -> void:
	var center := Vector2(TILE_SIZE / 2.0, TILE_SIZE / 2.0)
	var radius := 4
	
	# Draw cross marker
	for i in range(-radius, radius + 1):
		if center.x + i >= 0 and center.x + i < TILE_SIZE:
			image.set_pixel(int(center.x + i), int(center.y), primary_color)
		if center.y + i >= 0 and center.y + i < TILE_SIZE:
			image.set_pixel(int(center.x), int(center.y + i), primary_color)
	
	# Draw circle
	for y in range(TILE_SIZE):
		for x in range(TILE_SIZE):
			var dist := center.distance_to(Vector2(x, y))
			if abs(dist - radius) < 1:
				image.set_pixel(x, y, primary_color)
