# ============================================================
# VisualTileRenderer.gd  —  Applies procedural textures to TileMap
# ============================================================
# Integrates TileVisualGenerator with the level rendering system
# Applies theme-based textures to generated level tiles

extends Node
class_name VisualTileRenderer

# References
var _tile_map: TileMap = null
var _tile_visual_generator: TileVisualGenerator = null
var _art_director: ArtDirector = null

# Tile size
const TILE_SIZE: int = 32

# Tile atlas (generated textures)
var _tile_atlas: Dictionary = {}

# Tile type mapping to atlas IDs
var _tile_mapping: Dictionary = {}

func _ready() -> void:
	# Initialize systems
	_tile_visual_generator = TileVisualGenerator.new()
	add_child(_tile_visual_generator)
	
	_art_director = get_node_or_null("/root/ArtDirector")
	if not _art_director:
		_art_director = ArtDirector.new()
		_art_director.name = "ArtDirector"
		get_tree().root.call_deferred("add_child", _art_director)
	
	_tile_visual_generator.set_art_director(_art_director)
	
	# Connect to theme changes
	_art_director.theme_changed.connect(_on_theme_changed)

func set_tile_map(tile_map: TileMap) -> void:
	_tile_map = tile_map
	_generate_tile_atlas()
	_apply_theme_to_tilemap()

func _generate_tile_atlas() -> void:
	_tile_atlas.clear()
	_tile_mapping.clear()
	
	# Generate all tile textures for current theme
	var platform_texture := _tile_visual_generator.generate_platform_tile(0)
	var platform_dark_texture := _tile_visual_generator.generate_platform_tile(1)
	var platform_light_texture := _tile_visual_generator.generate_platform_tile(2)
	var hazard_texture := _tile_visual_generator.generate_hazard_tile()
	
	# Store in atlas
	_tile_atlas["platform"] = platform_texture
	_tile_atlas["platform_dark"] = platform_dark_texture
	_tile_atlas["platform_light"] = platform_light_texture
	_tile_atlas["hazard"] = hazard_texture

func _apply_theme_to_tilemap() -> void:
	if not _tile_map:
		return
	
	# Apply background color
	var bg_color := _art_director.get_background_color()
	_tile_map.modulate = Color(1, 1, 1, 1)
	
	# Update tile set with new textures
	_update_tileset()

func _update_tileset() -> void:
	if not _tile_map:
		return
	
	var tile_set := TileSet.new()
	var source_id := 0
	
	# Create tile source from generated textures
	var tile_atlas_texture := _create_combined_atlas()
	var atlas_source := TileSetAtlasSource.new()
	atlas_source.texture = tile_atlas_texture
	atlas_source.texture_region_size = Vector2i(TILE_SIZE, TILE_SIZE)
	
	# Add tiles to atlas source
	var atlas_coords := Vector2i.ZERO
	for tile_name in _tile_atlas.keys():
		atlas_source.create_tile(atlas_coords, Vector2i(0, 0))
		_tile_mapping[tile_name] = atlas_coords
		atlas_coords.x += 1
	
	tile_set.add_source(atlas_source, source_id)
	_tile_map.tile_set = tile_set

func _create_combined_atlas() -> Texture2D:
	var atlas_size := Vector2i(_tile_atlas.size() * TILE_SIZE, TILE_SIZE)
	var atlas_image := Image.create(atlas_size.x, atlas_size.y, false, Image.FORMAT_RGBA8)
	atlas_image.fill(Color.TRANSPARENT)
	
	var x_offset := 0
	for tile_name in _tile_atlas.keys():
		var texture: Texture2D = _tile_atlas[tile_name]
		var tile_image := texture.get_image()
		
		# Blit tile image to atlas
		for y in range(TILE_SIZE):
			for x in range(TILE_SIZE):
				var pixel := tile_image.get_pixel(x, y)
				atlas_image.set_pixel(x_offset + x, y, pixel)
		
		x_offset += TILE_SIZE
	
	return ImageTexture.create_from_image(atlas_image)

func _on_theme_changed(theme: ArtDirector.LevelTheme) -> void:
	_tile_visual_generator.clear_cache()
	_generate_tile_atlas()
	_apply_theme_to_tilemap()

func get_tile_texture(tile_type: String) -> Texture2D:
	if _tile_atlas.has(tile_type):
		return _tile_atlas[tile_type]
	return null

func apply_tile_to_cell(tile_coords: Vector2i, tile_type: String) -> void:
	if not _tile_map or not _tile_mapping.has(tile_type):
		return
	
	var source_id: int = 0
	var atlas_coords: Vector2i = _tile_mapping[tile_type]
	var alternative_tile: int = 0
	
	_tile_map.set_cell(0, tile_coords, source_id, atlas_coords, alternative_tile)

func apply_theme_to_level(level_data: Array) -> void:
	if not _tile_map:
		return
	
	# Clear existing tiles
	_tile_map.clear()
	
	# Apply new tiles based on level data and theme
	for y in range(level_data.size()):
		for x in range(level_data[y].size()):
			var tile_type: int = level_data[y][x]
			var tile_coords := Vector2i(x, y)
			
			match tile_type:
				1: # SOLID
					apply_tile_to_cell(tile_coords, "platform")
				2: # PLATFORM
					apply_tile_to_cell(tile_coords, "platform_light")
				3: # HAZARD
					apply_tile_to_cell(tile_coords, "hazard")
				_: # SPAWN(4), EXIT(5), CHECKPOINT(6), COIN(7), BUFF(8) - never render as tile blocks
					pass
