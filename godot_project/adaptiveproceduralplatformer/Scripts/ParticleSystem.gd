# ============================================================
# ParticleSystem.gd  —  Professional Particle Effects System
# ============================================================
# Creates polished particle effects for gameplay feedback
# Dust, landing, coins, checkpoints, hazards, etc.

extends Node2D
class_name ParticleSystem

# Particle types
enum ParticleType {
	DUST,
	LANDING,
	JUMP,
	COIN,
	CHECKPOINT,
	HAZARD,
	DEATH,
	POWERUP
}

# Art director reference
var _art_director: ArtDirector = null

# Particle pools for performance
var _particle_pools: Dictionary = {}

# Configuration
const MAX_PARTICLES: int = 100
const PARTICLE_LIFETIME: float = 1.0

func _ready() -> void:
	# Get art director
	_art_director = get_node_or_null("/root/ArtDirector")
	if not _art_director:
		_art_director = ArtDirector.new()
		get_tree().root.add_child(_art_director)
		_art_director.name = "ArtDirector"
	
	# Initialize particle pools
	_initialize_pools()

func _initialize_pools() -> void:
	for particle_type in ParticleType.values():
		_particle_pools[particle_type] = []

# Spawn particles at position
func spawn_particles(type: ParticleType, position: Vector2, count: int = 10) -> void:
	for i in range(count):
		var particle := _get_particle(type)
		if particle:
			_setup_particle(particle, type, position)
			add_child(particle)

func _get_particle(type: ParticleType) -> CPUParticles2D:
	var pool = _particle_pools[type]
	
	# Try to reuse existing particle
	if pool.size() > 0:
		var particle = pool.pop_back()
		particle.emitting = true
		return particle
	
	# Create new particle
	var particle := CPUParticles2D.new()
	_configure_particle(particle, type)
	return particle

func _configure_particle(particle: CPUParticles2D, type: ParticleType) -> void:
	match type:
		ParticleType.DUST:
			_configure_dust_particle(particle)
		ParticleType.LANDING:
			_configure_landing_particle(particle)
		ParticleType.JUMP:
			_configure_jump_particle(particle)
		ParticleType.COIN:
			_configure_coin_particle(particle)
		ParticleType.CHECKPOINT:
			_configure_checkpoint_particle(particle)
		ParticleType.HAZARD:
			_configure_hazard_particle(particle)
		ParticleType.DEATH:
			_configure_death_particle(particle)
		ParticleType.POWERUP:
			_configure_powerup_particle(particle)

func _configure_dust_particle(particle: CPUParticles2D) -> void:
	particle.amount = 15
	particle.lifetime = 0.8
	particle.emitting = false
	
	particle.texture = _create_dust_texture()
	particle.emission_shape = CPUParticles2D.EMISSION_SHAPE_SPHERE
	particle.emission_sphere_radius = 8.0
	
	particle.direction = Vector2(0, -1)
	particle.spread = 45.0
	particle.initial_velocity_min = 20.0
	particle.initial_velocity_max = 40.0
	
	particle.gravity = Vector2(0, 50)
	particle.scale_amount_min = 0.5
	particle.scale_amount_max = 1.5
	
	var dust_color := _art_director.get_color("primary")
	dust_color.a = 0.6
	particle.color = dust_color

func _configure_landing_particle(particle: CPUParticles2D) -> void:
	particle.amount = 20
	particle.lifetime = 1.0
	particle.emitting = false
	
	particle.texture = _create_dust_texture()
	particle.emission_shape = CPUParticles2D.EMISSION_SHAPE_SPHERE
	particle.emission_sphere_radius = 10.0
	
	particle.direction = Vector2(0, -1)
	particle.spread = 60.0
	particle.initial_velocity_min = 30.0
	particle.initial_velocity_max = 60.0
	
	particle.gravity = Vector2(0, 80)
	particle.scale_amount_min = 0.8
	particle.scale_amount_max = 2.0
	
	var dust_color := _art_director.get_color("primary")
	dust_color.a = 0.8
	particle.color = dust_color

func _configure_jump_particle(particle: CPUParticles2D) -> void:
	particle.amount = 12
	particle.lifetime = 0.6
	particle.emitting = false
	
	particle.texture = _create_dust_texture()
	particle.emission_shape = CPUParticles2D.EMISSION_SHAPE_SPHERE
	particle.emission_sphere_radius = 6.0
	
	particle.direction = Vector2(0, -1)
	particle.spread = 30.0
	particle.initial_velocity_min = 25.0
	particle.initial_velocity_max = 45.0
	
	particle.gravity = Vector2(0, 60)
	particle.scale_amount_min = 0.6
	particle.scale_amount_max = 1.2
	
	var dust_color := _art_director.get_color("primary")
	dust_color.a = 0.5
	particle.color = dust_color

func _configure_coin_particle(particle: CPUParticles2D) -> void:
	particle.amount = 25
	particle.lifetime = 1.2
	particle.emitting = false
	
	particle.texture = _create_coin_texture()
	particle.emission_shape = CPUParticles2D.EMISSION_SHAPE_SPHERE
	particle.emission_sphere_radius = 10.0
	
	particle.direction = Vector2(0, -1)
	particle.spread = 360.0
	particle.initial_velocity_min = 50.0
	particle.initial_velocity_max = 100.0
	
	particle.gravity = Vector2(0, 20)
	particle.scale_amount_min = 0.3
	particle.scale_amount_max = 1.0
	
	var coin_color := _art_director.get_coin_color()
	coin_color.a = 1.0
	particle.color = coin_color

func _configure_checkpoint_particle(particle: CPUParticles2D) -> void:
	particle.amount = 40
	particle.lifetime = 2.0
	particle.emitting = false
	
	particle.texture = _create_star_texture()
	particle.emission_shape = CPUParticles2D.EMISSION_SHAPE_SPHERE
	particle.emission_sphere_radius = 15.0
	
	particle.direction = Vector2(0, -1)
	particle.spread = 360.0
	particle.initial_velocity_min = 30.0
	particle.initial_velocity_max = 80.0
	
	particle.gravity = Vector2(0, -10)
	particle.scale_amount_min = 0.5
	particle.scale_amount_max = 1.5
	
	var primary_color := _art_director.get_color("primary")
	primary_color.a = 1.0
	particle.color = primary_color

func _configure_hazard_particle(particle: CPUParticles2D) -> void:
	particle.amount = 30
	particle.lifetime = 0.8
	particle.emitting = false
	
	particle.texture = _create_spark_texture()
	particle.emission_shape = CPUParticles2D.EMISSION_SHAPE_SPHERE
	particle.emission_sphere_radius = 12.0
	
	particle.direction = Vector2(0, -1)
	particle.spread = 360.0
	particle.initial_velocity_min = 60.0
	particle.initial_velocity_max = 120.0
	
	particle.gravity = Vector2(0, 100)
	particle.scale_amount_min = 0.4
	particle.scale_amount_max = 1.0
	
	var hazard_color := _art_director.get_hazard_color()
	hazard_color.a = 1.0
	particle.color = hazard_color

func _configure_death_particle(particle: CPUParticles2D) -> void:
	particle.amount = 50
	particle.lifetime = 1.5
	particle.emitting = false
	
	particle.texture = _create_explosion_texture()
	particle.emission_shape = CPUParticles2D.EMISSION_SHAPE_SPHERE
	particle.emission_sphere_radius = 20.0
	
	particle.direction = Vector2(0, -1)
	particle.spread = 360.0
	particle.initial_velocity_min = 80.0
	particle.initial_velocity_max = 150.0
	
	particle.gravity = Vector2(0, 50)
	particle.scale_amount_min = 0.5
	particle.scale_amount_max = 2.0
	
	var hazard_color := _art_director.get_hazard_color()
	hazard_color.a = 1.0
	particle.color = hazard_color

func _configure_powerup_particle(particle: CPUParticles2D) -> void:
	particle.amount = 35
	particle.lifetime = 1.5
	particle.emitting = false
	
	particle.texture = _create_star_texture()
	particle.emission_shape = CPUParticles2D.EMISSION_SHAPE_SPHERE
	particle.emission_sphere_radius = 18.0
	
	particle.direction = Vector2(0, -1)
	particle.spread = 360.0
	particle.initial_velocity_min = 40.0
	particle.initial_velocity_max = 90.0
	
	particle.gravity = Vector2(0, -20)
	particle.scale_amount_min = 0.6
	particle.scale_amount_max = 1.8
	
	var accent_color := _art_director.get_color("accent")
	accent_color.a = 1.0
	particle.color = accent_color

func _setup_particle(particle: CPUParticles2D, type: ParticleType, position: Vector2) -> void:
	particle.global_position = position
	particle.one_shot = true
	particle.emitting = true
	
	# Auto-return to pool after lifetime
	await get_tree().create_timer(particle.lifetime + 0.1).timeout
	if particle.get_parent() == self:
		remove_child(particle)
		_particle_pools[type].append(particle)

# Texture generation functions
func _create_dust_texture() -> Texture2D:
	var image := Image.create(16, 16, false, Image.FORMAT_RGBA8)
	image.fill(Color.TRANSPARENT)
	
	var center := Vector2(8, 8)
	for y in range(16):
		for x in range(16):
			var dist := center.distance_to(Vector2(x, y))
			if dist < 6:
				var alpha := 1.0 - (dist / 6.0)
				image.set_pixel(x, y, Color(1, 1, 1, alpha))
	
	return ImageTexture.create_from_image(image)

func _create_coin_texture() -> Texture2D:
	var image := Image.create(16, 16, false, Image.FORMAT_RGBA8)
	image.fill(Color.TRANSPARENT)
	
	var center := Vector2(8, 8)
	for y in range(16):
		for x in range(16):
			var dist := center.distance_to(Vector2(x, y))
			if dist < 7:
				var alpha := 1.0 - (dist / 7.0)
				image.set_pixel(x, y, Color(1, 1, 1, alpha))
	
	return ImageTexture.create_from_image(image)

func _create_star_texture() -> Texture2D:
	var image := Image.create(16, 16, false, Image.FORMAT_RGBA8)
	image.fill(Color.TRANSPARENT)
	
	var center := Vector2(8, 8)
	var points: PackedVector2Array = []
	for i in range(5):
		var angle := deg_to_rad(i * 72 - 90)
		var outer_radius := 7.0
		var inner_radius := 3.0
		points.append(center + Vector2(cos(angle), sin(angle)) * outer_radius)
		angle += deg_to_rad(36)
		points.append(center + Vector2(cos(angle), sin(angle)) * inner_radius)
	
	# Draw star
	for i in range(points.size()):
		var next_i := (i + 1) % points.size()
		_draw_line_on_image(image, points[i], points[next_i], Color(1, 1, 1, 1))
	
	return ImageTexture.create_from_image(image)

func _create_spark_texture() -> Texture2D:
	var image := Image.create(16, 16, false, Image.FORMAT_RGBA8)
	image.fill(Color.TRANSPARENT)
	
	var center := Vector2(8, 8)
	for y in range(16):
		for x in range(16):
			var dist := center.distance_to(Vector2(x, y))
			if dist < 5:
				var alpha := 1.0 - (dist / 5.0)
				image.set_pixel(x, y, Color(1, 1, 1, alpha))
	
	return ImageTexture.create_from_image(image)

func _create_explosion_texture() -> Texture2D:
	var image := Image.create(16, 16, false, Image.FORMAT_RGBA8)
	image.fill(Color.TRANSPARENT)
	
	var center := Vector2(8, 8)
	for y in range(16):
		for x in range(16):
			var dist := center.distance_to(Vector2(x, y))
			if dist < 8:
				var alpha := 1.0 - (dist / 8.0)
				image.set_pixel(x, y, Color(1, 1, 1, alpha))
	
	return ImageTexture.create_from_image(image)

func _draw_line_on_image(image: Image, start: Vector2, end: Vector2, color: Color) -> void:
	var steps: int = max(abs(end.x - start.x), abs(end.y - start.y)) + 1
	for i in range(steps):
		var t: float = float(i) / float(steps)
		var point: Vector2 = start.lerp(end, t)
		var px: int = int(point.x)
		var py: int = int(point.y)
		if px >= 0 and px < 16 and py >= 0 and py < 16:
			image.set_pixel(px, py, color)

# Public convenience methods
func spawn_dust(position: Vector2) -> void:
	spawn_particles(ParticleType.DUST, position, 8)

func spawn_landing(_position: Vector2) -> void:
	pass

func spawn_jump(_position: Vector2) -> void:
	pass

func spawn_coin(position: Vector2) -> void:
	spawn_particles(ParticleType.COIN, position, 20)

func spawn_checkpoint(position: Vector2) -> void:
	spawn_particles(ParticleType.CHECKPOINT, position, 30)

func spawn_hazard(position: Vector2) -> void:
	spawn_particles(ParticleType.HAZARD, position, 25)

func spawn_death(position: Vector2) -> void:
	spawn_particles(ParticleType.DEATH, position, 40)

func spawn_powerup(position: Vector2) -> void:
	spawn_particles(ParticleType.POWERUP, position, 30)
