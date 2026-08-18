# ============================================================
# BackgroundParallax.gd  —  Professional Parallax Background System
# ============================================================
# Creates layered parallax backgrounds with theme-based colors
# Adds depth and visual polish to the game world

extends ParallaxBackground
class_name BackgroundParallax

# Parallax layers
var _layers: Array[ParallaxLayer] = []
var _layer_speeds: Array[float] = []

# Art director reference
var _art_director: ArtDirector = null

# Layer configuration
const NUM_LAYERS: int = 4
const LAYER_SPACING: float = 100.0

func _ready() -> void:
	# Get art director
	_art_director = get_node_or_null("/root/ArtDirector")
	if not _art_director:
		_art_director = ArtDirector.new()
		get_tree().root.add_child(_art_director)
		_art_director.name = "ArtDirector"
	
	# Create parallax layers
	_create_parallax_layers()
	
	# Apply initial theme
	_apply_theme()
	
	# Connect to theme changes
	_art_director.theme_changed.connect(_on_theme_changed)

func _create_parallax_layers() -> void:
	# Clear existing layers
	for layer in _layers:
		layer.queue_free()
	_layers.clear()
	_layer_speeds.clear()
	
	# Create new layers
	for i in range(NUM_LAYERS):
		var layer := ParallaxLayer.new()
		add_child(layer)
		_layers.append(layer)
		
		# Calculate parallax speed (closer layers move slower)
		var speed := 1.0 - (float(i) / float(NUM_LAYERS))
		layer.motion_scale = Vector2(speed, speed * 0.5)
		_layer_speeds.append(speed)
		
		# Create background texture for this layer
		_create_layer_texture(layer, i)

func _create_layer_texture(layer: ParallaxLayer, layer_index: int) -> void:
	var texture_rect := TextureRect.new()
	texture_rect.name = "BackgroundTexture"
	texture_rect.anchors_preset = Control.PRESET_FULL_RECT
	texture_rect.expand_mode = TextureRect.EXPAND_IGNORE_SIZE
	
	# Generate procedural texture
	var texture: Texture2D = _generate_layer_texture(layer_index)
	texture_rect.texture = texture
	
	layer.add_child(texture_rect)

func _generate_layer_texture(layer_index: int) -> Texture2D:
	var bg_color := _art_director.get_background_color()
	var primary_color := _art_director.get_color("primary")
	var secondary_color := _art_director.get_color("secondary")
	
	var image_size := Vector2i(512, 512)
	var image := Image.create(image_size.x, image_size.y, false, Image.FORMAT_RGBA8)
	
	# Base color with variation based on layer depth
	var layer_color := bg_color.lerp(primary_color, 0.1 + layer_index * 0.05)
	image.fill(layer_color)
	
	# Add procedural details based on theme
	_add_theme_details(image, layer_index, layer_color, primary_color, secondary_color)
	
	var texture := ImageTexture.create_from_image(image)
	return texture

func _add_theme_details(image: Image, layer_index: int, base_color: Color, primary: Color, secondary: Color) -> void:
	var theme := _art_director.get_current_theme()
	var size := image.get_size()
	
	match theme:
		ArtDirector.LevelTheme.FOREST:
			_add_forest_details(image, layer_index, base_color, primary, secondary)
		ArtDirector.LevelTheme.ICE:
			_add_ice_details(image, layer_index, base_color, primary, secondary)
		ArtDirector.LevelTheme.LAVA:
			_add_lava_details(image, layer_index, base_color, primary, secondary)
		ArtDirector.LevelTheme.CASTLE:
			_add_castle_details(image, layer_index, base_color, primary, secondary)
		ArtDirector.LevelTheme.FACTORY:
			_add_factory_details(image, layer_index, base_color, primary, secondary)
		ArtDirector.LevelTheme.CAVE:
			_add_cave_details(image, layer_index, base_color, primary, secondary)
		ArtDirector.LevelTheme.RUINS:
			_add_ruins_details(image, layer_index, base_color, primary, secondary)
		ArtDirector.LevelTheme.UNDERGROUND:
			_add_underground_details(image, layer_index, base_color, primary, secondary)

func _add_forest_details(image: Image, layer_index: int, base: Color, primary: Color, secondary: Color) -> void:
	var size := image.get_size()
	
	# Add tree silhouettes (distant layers)
	if layer_index < 2:
		for i in range(20):
			var x := randi() % int(size.x)
			var height := 50 + randi() % 100
			var width := 20 + randi() % 30
			
			var tree_color := base.darkened(0.3 + layer_index * 0.1)
			_draw_tree_silhouette(image, x, int(size.y) - height, width, height, tree_color)
	
	# Add leaves/branches (closer layers)
	if layer_index >= 2:
		for i in range(30):
			var x := randi() % int(size.x)
			var y := randi() % int(size.y)
			var leaf_color := primary.lightened(0.2)
			leaf_color.a = 0.3
			_draw_leaf(image, x, y, leaf_color)

func _add_ice_details(image: Image, layer_index: int, base: Color, primary: Color, secondary: Color) -> void:
	var size := image.get_size()
	
	# Add ice crystals/structures
	for i in range(15):
		var x := randi() % int(size.x)
		var y := randi() % int(size.y)
		var crystal_size := 20 + randi() % 40
		
		var crystal_color := primary.lightened(0.3 - layer_index * 0.1)
		crystal_color.a = 0.4
		_draw_crystal(image, x, y, crystal_size, crystal_color)
	
	# Add snow particles
	if layer_index >= 2:
		for i in range(50):
			var x := randi() % int(size.x)
			var y := randi() % int(size.y)
			var snow_color := Color.WHITE
			snow_color.a = 0.2
			image.set_pixel(x, y, snow_color)

func _add_lava_details(image: Image, layer_index: int, base: Color, primary: Color, secondary: Color) -> void:
	var size := image.get_size()
	
	# Add rock formations
	for i in range(10):
		var x := randi() % int(size.x)
		var y := randi() % int(size.y)
		var rock_size := 30 + randi() % 50
		
		var rock_color := base.darkened(0.4)
		_draw_rock_formation(image, x, y, rock_size, rock_color)
	
	# Add lava glow (closer layers)
	if layer_index >= 2:
		for i in range(8):
			var x := randi() % int(size.x)
			var y := randi() % int(size.y)
			var glow_size := 40 + randi() % 30
			
			var glow_color := secondary
			glow_color.a = 0.3
			_draw_glow(image, x, y, glow_size, glow_color)

func _add_castle_details(image: Image, layer_index: int, base: Color, primary: Color, secondary: Color) -> void:
	var size := image.get_size()
	
	# Add architectural elements
	for i in range(12):
		var x := randi() % int(size.x)
		var y := randi() % int(size.y)
		var tower_size := 40 + randi() % 60
		
		var tower_color := base.darkened(0.3)
		_draw_tower_silhouette(image, x, y, tower_size, tower_color)
	
	# Add windows (closer layers)
	if layer_index >= 2:
		for i in range(20):
			var x := randi() % int(size.x)
			var y := randi() % int(size.y)
			var window_color := secondary
			window_color.a = 0.4
			_draw_window(image, x, y, window_color)

func _add_factory_details(image: Image, layer_index: int, base: Color, primary: Color, secondary: Color) -> void:
	var size := image.get_size()
	
	# Add industrial structures
	for i in range(15):
		var x := randi() % int(size.x)
		var y := randi() % int(size.y)
		var structure_size := 50 + randi() % 70
		
		var structure_color := base.darkened(0.35)
		_draw_factory_structure(image, x, y, structure_size, structure_color)
	
	# Add pipes/machinery (closer layers)
	if layer_index >= 2:
		for i in range(25):
			var x := randi() % int(size.x)
			var y := randi() % int(size.y)
			var pipe_color := primary
			pipe_color.a = 0.3
			_draw_pipe(image, x, y, pipe_color)

func _add_cave_details(image: Image, layer_index: int, base: Color, primary: Color, secondary: Color) -> void:
	var size := image.get_size()
	
	# Add stalactites/stalagmites
	for i in range(20):
		var x := randi() % int(size.x)
		var y := randi() % int(size.y)
		var formation_size := 30 + randi() % 50
		
		var formation_color := base.darkened(0.4)
		_draw_cave_formation(image, x, y, formation_size, formation_color)
	
	# Add crystal deposits (closer layers)
	if layer_index >= 2:
		for i in range(15):
			var x := randi() % int(size.x)
			var y := randi() % int(size.y)
			var crystal_color := secondary
			crystal_color.a = 0.3
			_draw_crystal_deposit(image, x, y, crystal_color)

func _add_ruins_details(image: Image, layer_index: int, base: Color, primary: Color, secondary: Color) -> void:
	var size := image.get_size()
	
	# Add ruined structures
	for i in range(12):
		var x := randi() % int(size.x)
		var y := randi() % int(size.y)
		var ruin_size := 60 + randi() % 80
		
		var ruin_color := base.darkened(0.3)
		_draw_ruin_structure(image, x, y, ruin_size, ruin_color)
	
	# Add vegetation overgrowth (closer layers)
	if layer_index >= 2:
		for i in range(30):
			var x := randi() % int(size.x)
			var y := randi() % int(size.y)
			var vine_color := primary
			vine_color.a = 0.4
			_draw_vine(image, x, y, vine_color)

func _add_underground_details(image: Image, layer_index: int, base: Color, primary: Color, secondary: Color) -> void:
	var size := image.get_size()
	
	# Add underground tunnels/structures
	for i in range(15):
		var x := randi() % int(size.x)
		var y := randi() % int(size.y)
		var tunnel_size := 40 + randi() % 60
		
		var tunnel_color := base.darkened(0.35)
		_draw_tunnel_structure(image, x, y, tunnel_size, tunnel_color)
	
	# Add bioluminescent fungi (closer layers)
	if layer_index >= 2:
		for i in range(20):
			var x := randi() % int(size.x)
			var y := randi() % int(size.y)
			var fungi_color := secondary
			fungi_color.a = 0.5
			_draw_fungi(image, x, y, fungi_color)

# Drawing helper functions
func _draw_tree_silhouette(image: Image, x: int, y: int, width: int, height: int, color: Color) -> void:
	for py in range(height):
		for px in range(width):
			var draw_x := x + px
			var draw_y := y + py
			if draw_x >= 0 and draw_x < image.get_width() and draw_y >= 0 and draw_y < image.get_height():
				# Simple tree shape
				var center_x := width / 2
				var tree_width := width * (1.0 - float(py) / float(height))
				if abs(px - center_x) < tree_width / 2:
					image.set_pixel(draw_x, draw_y, color)

func _draw_leaf(image: Image, x: int, y: int, color: Color) -> void:
	if x >= 0 and x < image.get_width() and y >= 0 and y < image.get_height():
		image.set_pixel(x, y, color)
		if x + 1 < image.get_width():
			image.set_pixel(x + 1, y, color)
		if y + 1 < image.get_height():
			image.set_pixel(x, y + 1, color)

func _draw_crystal(image: Image, x: int, y: int, size: int, color: Color) -> void:
	for i in range(size):
		var offset: float = i - size / 2
		var draw_x: int = x + int(offset)
		var draw_y: int = y + int(abs(offset))
		if draw_x >= 0 and draw_x < image.get_width() and draw_y >= 0 and draw_y < image.get_height():
			image.set_pixel(draw_x, draw_y, color)

func _draw_rock_formation(image: Image, x: int, y: int, size: int, color: Color) -> void:
	for py in range(size):
		for px in range(size):
			var draw_x := x + px
			var draw_y := y + py
			if draw_x >= 0 and draw_x < image.get_width() and draw_y >= 0 and draw_y < image.get_height():
				# Rough rock shape
				var noise := (px + py) % 3 == 0
				if noise:
					image.set_pixel(draw_x, draw_y, color)

func _draw_glow(image: Image, x: int, y: int, size: int, color: Color) -> void:
	for py in range(-size, size):
		for px in range(-size, size):
			var dist := sqrt(px * px + py * py)
			if dist < size:
				var draw_x := x + px
				var draw_y := y + py
				if draw_x >= 0 and draw_x < image.get_width() and draw_y >= 0 and draw_y < image.get_height():
					var alpha := 1.0 - (dist / size)
					var pixel_color := color
					pixel_color.a *= alpha
					image.set_pixel(draw_x, draw_y, pixel_color)

func _draw_tower_silhouette(image: Image, x: int, y: int, size: int, color: Color) -> void:
	for py in range(size):
		for px in range(size / 3):
			var draw_x := x + px
			var draw_y := y + py
			if draw_x >= 0 and draw_x < image.get_width() and draw_y >= 0 and draw_y < image.get_height():
				image.set_pixel(draw_x, draw_y, color)

func _draw_window(image: Image, x: int, y: int, color: Color) -> void:
	for py in range(8):
		for px in range(6):
			var draw_x := x + px
			var draw_y := y + py
			if draw_x >= 0 and draw_x < image.get_width() and draw_y >= 0 and draw_y < image.get_height():
				image.set_pixel(draw_x, draw_y, color)

func _draw_factory_structure(image: Image, x: int, y: int, size: int, color: Color) -> void:
	for py in range(size):
		for px in range(size / 2):
			var draw_x := x + px
			var draw_y := y + py
			if draw_x >= 0 and draw_x < image.get_width() and draw_y >= 0 and draw_y < image.get_height():
				if (px + py) % 4 == 0:
					image.set_pixel(draw_x, draw_y, color)

func _draw_pipe(image: Image, x: int, y: int, color: Color) -> void:
	for i in range(20):
		var draw_x := x + i
		var draw_y := y
		if draw_x >= 0 and draw_x < image.get_width() and draw_y >= 0 and draw_y < image.get_height():
			image.set_pixel(draw_x, draw_y, color)
		if draw_y + 1 < image.get_height():
			image.set_pixel(draw_x, draw_y + 1, color)

func _draw_cave_formation(image: Image, x: int, y: int, size: int, color: Color) -> void:
	for py in range(size):
		for px in range(size):
			var draw_x := x + px
			var draw_y := y + py
			if draw_x >= 0 and draw_x < image.get_width() and draw_y >= 0 and draw_y < image.get_height():
				var noise := sin(px * 0.3) * cos(py * 0.3) > 0
				if noise:
					image.set_pixel(draw_x, draw_y, color)

func _draw_crystal_deposit(image: Image, x: int, y: int, color: Color) -> void:
	for i in range(8):
		var offset: int = (i - 4) * 2
		var draw_x: int = x + offset
		var draw_y: int = y + abs(offset)
		if draw_x >= 0 and draw_x < image.get_width() and draw_y >= 0 and draw_y < image.get_height():
			image.set_pixel(draw_x, draw_y, color)

func _draw_ruin_structure(image: Image, x: int, y: int, size: int, color: Color) -> void:
	for py in range(size):
		for px in range(size):
			var draw_x := x + px
			var draw_y := y + py
			if draw_x >= 0 and draw_x < image.get_width() and draw_y >= 0 and draw_y < image.get_height():
				# Broken structure pattern
				if (px + py) % 5 != 0:
					image.set_pixel(draw_x, draw_y, color)

func _draw_vine(image: Image, x: int, y: int, color: Color) -> void:
	for i in range(15):
		var draw_x := x + (i % 3) - 1
		var draw_y := y + i
		if draw_x >= 0 and draw_x < image.get_width() and draw_y >= 0 and draw_y < image.get_height():
			image.set_pixel(draw_x, draw_y, color)

func _draw_tunnel_structure(image: Image, x: int, y: int, size: int, color: Color) -> void:
	for py in range(size):
		for px in range(size):
			var draw_x: int = x + px
			var draw_y: int = y + py
			if draw_x >= 0 and draw_x < image.get_width() and draw_y >= 0 and draw_y < image.get_height():
				# Tunnel arch shape
				var center_x: int = size / 2
				var arch_width: int = abs(px - center_x)
				if arch_width > size / 4 or py < size / 4:
					image.set_pixel(draw_x, draw_y, color)

func _draw_fungi(image: Image, x: int, y: int, color: Color) -> void:
	for py in range(6):
		for px in range(6):
			var draw_x: int = x + px - 3
			var draw_y: int = y + py
			if draw_x >= 0 and draw_x < image.get_width() and draw_y >= 0 and draw_y < image.get_height():
				if px * px + py * py < 9:
					image.set_pixel(draw_x, draw_y, color)

func _apply_theme() -> void:
	for i in range(_layers.size()):
		var layer := _layers[i]
		var texture_rect := layer.get_node_or_null("BackgroundTexture")
		if texture_rect:
			var texture: Texture2D = _generate_layer_texture(i)
			texture_rect.texture = texture

func _on_theme_changed(theme: ArtDirector.LevelTheme) -> void:
	_apply_theme()

func set_background_scroll(offset: Vector2) -> void:
	scroll_offset = offset
