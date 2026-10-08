# ============================================================
# BackgroundParallax.gd  —  Parallax Background System
# ============================================================
# Creates layered parallax backgrounds with rich sky gradients,
# distant mountain ridges, and atmospheric depth without any noisy artifacts.

extends ParallaxBackground
class_name BackgroundParallax

# Parallax layers
var _layers: Array[ParallaxLayer] = []
var _layer_speeds: Array[float] = []

# Art director reference
var _art_director: ArtDirector = null

func _ready() -> void:
	_art_director = get_node_or_null("/root/ArtDirector")
	if not _art_director:
		_art_director = ArtDirector.new()
		_art_director.name = "ArtDirector"
		get_tree().root.call_deferred("add_child", _art_director)
	
	_create_parallax_layers()
	_apply_theme()
	_art_director.theme_changed.connect(_on_theme_changed)

func _create_parallax_layers() -> void:
	for layer in _layers:
		layer.queue_free()
	_layers.clear()
	_layer_speeds.clear()
	
	# Layer 0: Sky Gradient & Distant Clouds (0.02 motion)
	var sky_layer := ParallaxLayer.new()
	sky_layer.motion_scale = Vector2(0.02, 0.01)
	sky_layer.motion_mirroring = Vector2(1280, 0)
	add_child(sky_layer)
	_layers.append(sky_layer)
	_layer_speeds.append(0.02)
	_create_layer_texture(sky_layer, 0)

	# Layer 1: Distant Majestic Mountains (0.08 motion)
	var mtn_layer := ParallaxLayer.new()
	mtn_layer.motion_scale = Vector2(0.08, 0.03)
	mtn_layer.motion_mirroring = Vector2(1280, 0)
	add_child(mtn_layer)
	_layers.append(mtn_layer)
	_layer_speeds.append(0.08)
	_create_layer_texture(mtn_layer, 1)

	# Layer 2: Mid-ground Rolling Foothills (0.18 motion)
	var hill_layer := ParallaxLayer.new()
	hill_layer.motion_scale = Vector2(0.18, 0.08)
	hill_layer.motion_mirroring = Vector2(1280, 0)
	add_child(hill_layer)
	_layers.append(hill_layer)
	_layer_speeds.append(0.18)
	_create_layer_texture(hill_layer, 2)

func _create_layer_texture(layer: ParallaxLayer, layer_index: int) -> void:
	var texture_rect := TextureRect.new()
	texture_rect.name = "BackgroundTexture"
	texture_rect.size = Vector2(1280, 720)
	texture_rect.expand_mode = TextureRect.EXPAND_IGNORE_SIZE
	
	var texture: Texture2D = _generate_layer_texture(layer_index)
	texture_rect.texture = texture
	layer.add_child(texture_rect)

func _generate_layer_texture(layer_index: int) -> Texture2D:
	var width := 1280
	var height := 720
	var image := Image.create(width, height, false, Image.FORMAT_RGBA8)

	if layer_index == 0:
		# Layer 0: Sky Gradient with Fluffy Pixel Clouds
		var top_sky := Color(0.12, 0.22, 0.44, 1.0)
		var mid_sky := Color(0.24, 0.44, 0.70, 1.0)
		var horizon := Color(0.56, 0.72, 0.88, 1.0)

		for y in range(height):
			var t := float(y) / float(height)
			var c: Color = top_sky.lerp(mid_sky, t / 0.55) if t < 0.55 else mid_sky.lerp(horizon, (t - 0.55) / 0.45)
			for x in range(width):
				image.set_pixel(x, y, c)

		# Soft drifting pixel clouds
		var cloud_pts: Array[Vector2i] = [
			Vector2i(140, 110), Vector2i(380, 150), Vector2i(640, 95),
			Vector2i(890, 140), Vector2i(1120, 110)
		]
		for pt in cloud_pts:
			_draw_cloud(image, pt.x, pt.y, 44, 16)

	elif layer_index == 1:
		# Layer 1: Distant Mountain Silhouettes
		image.fill(Color(0, 0, 0, 0)) # Transparent
		var mtn_col := Color(0.18, 0.28, 0.46, 0.85)
		_draw_mountain_silhouette(image, 530, mtn_col, 0.007, 75.0, 0.018, 35.0)

	elif layer_index == 2:
		# Layer 2: Closer Rolling Foothills
		image.fill(Color(0, 0, 0, 0)) # Transparent
		var hill_col := Color(0.13, 0.20, 0.33, 0.90)
		_draw_mountain_silhouette(image, 600, hill_col, 0.012, 50.0, 0.028, 22.0)

	return ImageTexture.create_from_image(image)

func _draw_cloud(image: Image, cx: int, cy: int, w: int, h: int) -> void:
	var col_white := Color(0.96, 0.98, 1.0, 0.75)
	var col_shade := Color(0.68, 0.80, 0.90, 0.60)
	for dy in range(-h, h + 1):
		for dx in range(-w, w + 1):
			var dist_sq: float = (float(dx) / float(w)) ** 2.0 + (float(dy) / float(h)) ** 2.0
			if dist_sq <= 1.0:
				var px: int = cx + dx
				var py: int = cy + dy
				if px >= 0 and px < image.get_width() and py >= 0 and py < image.get_height():
					var c: Color = col_shade if dy > int(h * 0.25) else col_white
					image.set_pixel(px, py, c)

func _draw_mountain_silhouette(image: Image, base_y: int, mtn_color: Color, freq1: float, amp1: float, freq2: float, amp2: float) -> void:
	var w: int = image.get_width()
	for x in range(w):
		var h: float = sin(x * freq1) * amp1 + sin(x * freq2) * amp2 + cos(x * 0.04) * 15.0
		var top_y: int = int(base_y - 100 + h)
		for y in range(clampi(top_y, 0, image.get_height() - 1), image.get_height()):
			image.set_pixel(x, y, mtn_color)

func _apply_theme() -> void:
	for i in range(_layers.size()):
		var layer := _layers[i]
		var texture_rect := layer.get_node_or_null("BackgroundTexture")
		if texture_rect:
			var texture: Texture2D = _generate_layer_texture(i)
			texture_rect.texture = texture

func _on_theme_changed(_theme: ArtDirector.LevelTheme) -> void:
	_apply_theme()

func set_background_scroll(offset: Vector2) -> void:
	scroll_offset = offset
