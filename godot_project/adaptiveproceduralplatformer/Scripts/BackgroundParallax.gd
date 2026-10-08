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
		_art_director.name = "ArtDirector"
		get_tree().root.call_deferred("add_child", _art_director)
	
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
	
	# Layer 0: Sky & Clouds (ultra-distant, 0.03 speed)
	var sky_layer := ParallaxLayer.new()
	sky_layer.motion_scale = Vector2(0.03, 0.01)
	sky_layer.motion_mirroring = Vector2(1024, 0)
	add_child(sky_layer)
	_layers.append(sky_layer)
	_layer_speeds.append(0.03)
	_create_layer_texture(sky_layer, 0)

	# Layer 1: Distant Mountain Ridges (0.10 speed)
	var mtn_layer := ParallaxLayer.new()
	mtn_layer.motion_scale = Vector2(0.10, 0.04)
	mtn_layer.motion_mirroring = Vector2(1024, 0)
	add_child(mtn_layer)
	_layers.append(mtn_layer)
	_layer_speeds.append(0.10)
	_create_layer_texture(mtn_layer, 1)

	# Layer 2: Mid-ground Canopy / Ancient Ruins (0.24 speed)
	var mid_layer := ParallaxLayer.new()
	mid_layer.motion_scale = Vector2(0.24, 0.10)
	mid_layer.motion_mirroring = Vector2(1024, 0)
	add_child(mid_layer)
	_layers.append(mid_layer)
	_layer_speeds.append(0.24)
	_create_layer_texture(mid_layer, 2)

	# Layer 3: Foreground Atmospheric Details (0.40 speed)
	var fg_layer := ParallaxLayer.new()
	fg_layer.motion_scale = Vector2(0.40, 0.18)
	fg_layer.motion_mirroring = Vector2(1024, 0)
	add_child(fg_layer)
	_layers.append(fg_layer)
	_layer_speeds.append(0.40)
	_create_layer_texture(fg_layer, 3)

func _create_layer_texture(layer: ParallaxLayer, layer_index: int) -> void:
	var texture_rect := TextureRect.new()
	texture_rect.name = "BackgroundTexture"
	texture_rect.size = Vector2(1024, 720)
	texture_rect.expand_mode = TextureRect.EXPAND_IGNORE_SIZE
	
	var texture: Texture2D = _generate_layer_texture(layer_index)
	texture_rect.texture = texture
	layer.add_child(texture_rect)

func _generate_layer_texture(layer_index: int) -> Texture2D:
	var width := 1024
	var height := 720
	var image := Image.create(width, height, false, Image.FORMAT_RGBA8)

	if layer_index == 0:
		# Layer 0: Sky Gradient with Fluffy Pixel Clouds
		var top_sky := Color(0.12, 0.24, 0.48, 1.0)
		var mid_sky := Color(0.26, 0.48, 0.74, 1.0)
		var horizon := Color(0.60, 0.76, 0.90, 1.0)

		for y in range(height):
			var t := float(y) / float(height)
			var c: Color = top_sky.lerp(mid_sky, t / 0.6) if t < 0.6 else mid_sky.lerp(horizon, (t - 0.6) / 0.4)
			for x in range(width):
				image.set_pixel(x, y, c)

		# Add floating clouds in upper sky
		var cloud_pts: Array[Vector2i] = [
			Vector2i(100, 110), Vector2i(260, 150), Vector2i(440, 95),
			Vector2i(620, 135), Vector2i(800, 105), Vector2i(960, 160)
		]
		for pt in cloud_pts:
			_draw_cloud(image, pt.x, pt.y, 48, 18)

	elif layer_index == 1:
		# Layer 1: Distant Mountain Silhouettes against Sky
		image.fill(Color(0, 0, 0, 0)) # Transparent
		var mtn_col := Color(0.20, 0.32, 0.50, 0.85)
		_draw_mountain_silhouette(image, 520, mtn_col)

	else:
		# Layer 2 & 3: Theme-specific silhouettes and atmospheric accents
		image.fill(Color(0, 0, 0, 0))
		var bg_color := _art_director.get_background_color()
		var primary_color := _art_director.get_color("primary")
		var secondary_color := _art_director.get_color("secondary")
		_add_theme_details(image, layer_index, bg_color, primary_color, secondary_color)

	return ImageTexture.create_from_image(image)

func _draw_cloud(image: Image, cx: int, cy: int, w: int, h: int) -> void:
	var col_white := Color(0.96, 0.98, 1.0, 0.78)
	var col_shade := Color(0.70, 0.82, 0.92, 0.65)
	for dy in range(-h, h + 1):
		for dx in range(-w, w + 1):
			var dist_sq: float = (float(dx) / float(w)) * (float(dx) / float(w)) + (float(dy) / float(h)) * (float(dy) / float(h))
			if dist_sq <= 1.0:
				var px: int = cx + dx
				var py: int = cy + dy
				if px >= 0 and px < image.get_width() and py >= 0 and py < image.get_height():
					var c: Color = col_shade if dy > int(h * 0.2) else col_white
					image.set_pixel(px, py, c)

func _draw_mountain_silhouette(image: Image, base_y: int, mtn_color: Color) -> void:
	var w: int = image.get_width()
	for x in range(w):
		var h: float = sin(x * 0.008) * 90.0 + sin(x * 0.022) * 45.0 + cos(x * 0.05) * 20.0
		var top_y: int = int(base_y - 120 + h)
		for y in range(clampi(top_y, 0, image.get_height() - 1), image.get_height()):
			image.set_pixel(x, y, mtn_color)

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
