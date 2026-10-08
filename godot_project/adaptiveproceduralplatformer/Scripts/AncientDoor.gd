## ============================================================
## AncientDoor.gd — Mystical Ancient Stone Portal / Goal Node
## ============================================================
## Features:
## - Majestic ancient stone archway with runic carvings
## - Animated ethereal portal rift with pulsing magical glow
## - Floating mystic energy particles
## - Collision trigger that completes the level on player entry
## ============================================================

extends Area2D
class_name AncientDoor

signal door_entered

var _portal_glow: Sprite2D
var _door_sprite: Sprite2D
var _particles: CPUParticles2D
var _active: bool = true
var _anim_time: float = 0.0
var _label: Label

func _ready() -> void:
	name = "AncientDoor"
	add_to_group("exits")
	monitoring = true
	monitorable = true
	
	# Set collision layer/mask
	collision_layer = 0
	collision_mask = 1 # Player layer
	
	_create_visuals()
	_create_particles()
	_create_collision()
	
	body_entered.connect(_on_body_entered)
	area_entered.connect(_on_area_entered)

func _create_visuals() -> void:
	# Generate ancient stone door texture
	var door_tex := _generate_ancient_door_texture()
	_door_sprite = Sprite2D.new()
	_door_sprite.name = "DoorFrame"
	_door_sprite.texture = door_tex
	_door_sprite.position = Vector2(0, -32)
	_door_sprite.z_index = 2
	add_child(_door_sprite)
	
	# Create inner portal glow
	var glow_tex := _generate_portal_glow_texture()
	_portal_glow = Sprite2D.new()
	_portal_glow.name = "PortalGlow"
	_portal_glow.texture = glow_tex
	_portal_glow.position = Vector2(0, -28)
	_portal_glow.z_index = 1
	add_child(_portal_glow)
	
	# Floating ancient runic title indicator
	_label = Label.new()
	_label.text = "ANCIENT GATE"
	_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	_label.add_theme_font_size_override("font_size", 10)
	_label.add_theme_color_override("font_color", Color(0.4, 0.9, 1.0, 0.9))
	_label.position = Vector2(-50, -82)
	_label.size = Vector2(100, 20)
	_label.z_index = 5
	add_child(_label)

func _create_particles() -> void:
	_particles = CPUParticles2D.new()
	_particles.name = "RunicParticles"
	_particles.position = Vector2(0, -8)
	_particles.amount = 18
	_particles.lifetime = 1.6
	_particles.emission_shape = CPUParticles2D.EMISSION_SHAPE_RECTANGLE
	_particles.emission_rect_extents = Vector2(14, 4)
	_particles.direction = Vector2(0, -1)
	_particles.spread = 15.0
	_particles.gravity = Vector2(0, -20)
	_particles.initial_velocity_min = 25.0
	_particles.initial_velocity_max = 55.0
	_particles.scale_amount_min = 2.0
	_particles.scale_amount_max = 4.0
	_particles.color = Color(0.3, 0.85, 1.0, 0.8)
	_particles.z_index = 3
	add_child(_particles)

func _create_collision() -> void:
	var shape := CollisionShape2D.new()
	var rect := RectangleShape2D.new()
	rect.size = Vector2(36, 60)
	shape.shape = rect
	shape.position = Vector2(0, -30)
	add_child(shape)

func _process(delta: float) -> void:
	_anim_time += delta
	if _portal_glow:
		# Pulsing ethereal portal breath
		var pulse := 0.75 + 0.25 * sin(_anim_time * 3.5)
		_portal_glow.modulate = Color(0.4, 0.85, 1.0, pulse)
		_portal_glow.scale = Vector2(1.0 + 0.05 * sin(_anim_time * 2.0), 1.0 + 0.04 * cos(_anim_time * 2.5))
	
	if _label:
		# Floating indicator bounce
		_label.position.y = -82 + sin(_anim_time * 2.5) * 3.0

func _on_body_entered(body: Node2D) -> void:
	if not _active: return
	if body.is_in_group("player") or body.name == "Player":
		_trigger_portal(body)

func _on_area_entered(area: Area2D) -> void:
	if not _active: return
	if area.is_in_group("player") or area.owner and area.owner.is_in_group("player"):
		_trigger_portal(area.owner if area.owner else area)

func _trigger_portal(target: Node) -> void:
	_active = false
	print("[AncientDoor] Player stepped into the Ancient Door!")
	
	if has_node("/root/AudioManager"):
		get_node("/root/AudioManager").play_sound("power_up")
		
	# Portal burst effect
	if _particles:
		_particles.amount = 40
		_particles.initial_velocity_max = 120.0
		
	# Door flash tween
	var tween := create_tween()
	tween.tween_property(_door_sprite, "modulate", Color(2.5, 2.5, 3.0, 1.0), 0.15)
	tween.tween_property(_door_sprite, "modulate", Color(1.0, 1.0, 1.0, 1.0), 0.3)
	
	emit_signal("door_entered")
	if target.has_method("on_exit_reached"):
		target.call("on_exit_reached")
	elif target.has_method("_on_exit_reached"):
		target.call("_on_exit_reached")

## Procedurally crafts high-detail Ancient Stone Arch texture (48 x 64 px)
func _generate_ancient_door_texture() -> ImageTexture:
	var width := 48
	var height := 64
	var img := Image.create(width, height, false, Image.FORMAT_RGBA8)
	img.fill(Color(0, 0, 0, 0))
	
	var stone_dark := Color(0.18, 0.20, 0.24, 1.0)
	var stone_mid := Color(0.28, 0.31, 0.36, 1.0)
	var stone_light := Color(0.42, 0.46, 0.52, 1.0)
	var stone_rune := Color(0.30, 0.85, 0.95, 0.9)
	var wood_dark := Color(0.12, 0.08, 0.06, 1.0)
	var iron_band := Color(0.35, 0.35, 0.38, 1.0)
	
	# Base pedestal (bottom 6 px)
	for y in range(58, 64):
		for x in range(2, 46):
			var c = stone_dark if y == 58 or x == 2 or x == 45 else stone_mid
			img.set_pixel(x, y, c)

	# Left pillar (cols 4..13) and Right pillar (cols 34..43)
	for y in range(16, 58):
		# Left pillar
		for x in range(4, 14):
			var col = stone_mid
			if x == 4 or y % 10 == 0: col = stone_dark
			elif x == 13: col = stone_light
			elif (y % 10 == 4 or y % 10 == 6) and (x >= 7 and x <= 10):
				col = stone_rune # Engraved glowing rune mark
			img.set_pixel(x, y, col)
			
		# Right pillar
		for x in range(34, 44):
			var col = stone_mid
			if x == 34: col = stone_light
			elif x == 43 or y % 10 == 0: col = stone_dark
			elif (y % 10 == 3 or y % 10 == 5) and (x >= 37 and x <= 40):
				col = stone_rune # Engraved glowing rune mark
			img.set_pixel(x, y, col)

	# Top Arch curved lintel (y=4..16)
	var center_x := 24.0
	for y in range(4, 18):
		for x in range(4, 44):
			var dist := Vector2(x - center_x, y - 18).length()
			if dist <= 20.0 and dist >= 10.0:
				var col = stone_mid
				if dist > 18.5: col = stone_dark
				elif dist < 12.0: col = stone_light
				# Keystone at center
				if abs(x - 24) <= 3 and y <= 10:
					col = stone_rune if abs(x - 24) <= 1 else stone_light
				img.set_pixel(x, y, col)

	# Inner door opening (x=14..33, y=18..57)
	for y in range(18, 58):
		for x in range(14, 34):
			var col = wood_dark
			# Iron bands across door
			if y == 26 or y == 44:
				col = iron_band
			# Vertical door planks
			elif x == 23 or x == 24:
				col = Color(0.08, 0.05, 0.04, 1.0)
			# Mystic portal energy vortex crack in the center seam
			if (x == 23 or x == 24) and y >= 20 and y <= 54:
				col = stone_rune
			img.set_pixel(x, y, col)
			
	return ImageTexture.create_from_image(img)

## Procedurally crafts inner glowing portal texture (32 x 48 px)
func _generate_portal_glow_texture() -> ImageTexture:
	var width := 32
	var height := 48
	var img := Image.create(width, height, false, Image.FORMAT_RGBA8)
	img.fill(Color(0, 0, 0, 0))
	
	var center := Vector2(16.0, 24.0)
	for y in range(height):
		for x in range(width):
			var dist := Vector2((x - center.x) / 14.0, (y - center.y) / 22.0).length()
			if dist < 1.0:
				var alpha := pow(1.0 - dist, 1.8) * 0.75
				var color := Color(0.2, 0.7, 1.0, alpha)
				# Inner bright core
				if dist < 0.4:
					color = Color(0.7, 0.95, 1.0, alpha * 1.2)
				img.set_pixel(x, y, color)
				
	return ImageTexture.create_from_image(img)
