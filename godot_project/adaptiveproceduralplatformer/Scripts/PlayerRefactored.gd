extends CharacterBody2D
# ============================================================
# PlayerRefactored.gd  —  Orchestrator for component-based player
# ============================================================
# Coordinates movement, health, interaction, and analytics components

signal player_died(death_type: PlayerEnums.DeathType)
signal checkpoint_reached(tile_pos: Vector2i)
signal level_complete(stats: Dictionary)
signal coin_collected(tile_pos: Vector2i)
signal request_restart
signal request_pause

# Components
@onready var _movement: PlayerMovement = $PlayerMovement
@onready var _health: PlayerHealth = $PlayerHealth
@onready var _interaction: PlayerInteraction = $PlayerInteraction
@onready var _analytics: PlayerAnalytics = $PlayerAnalytics
@onready var _tile_service: TileService = $TileService

@onready var _sprite: Node2D = $Sprite2D if has_node("Sprite2D") else null
@onready var _anim: AnimationPlayer = $AnimationPlayer if has_node("AnimationPlayer") else null
@onready var _camera: CameraController = $Camera2D if has_node("Camera2D") else null

# Knight Sprite Sheet Animation
var _anim_timer: float = 0.0
var _anim_frame_idx: int = 0
const IDLE_FRAMES: Array[int] = [0, 1, 2, 3]
const RUN_FRAMES: Array[int] = [16, 17, 18, 19, 20, 21, 22, 23]
const JUMP_FRAME: int = 24
const FALL_FRAME: int = 25
const HIT_FRAMES: Array[int] = [48, 49, 50, 51]

# Visual manager reference
var _visual_manager: Node = null

func _ready() -> void:
	# Hide any debug collision overlays
	var cs := get_node_or_null("CollisionShape2D") as CollisionShape2D
	if cs: cs.debug_color = Color(0, 0, 0, 0)
	var acs := get_node_or_null("PlayerInteraction/PlayerInteractionCollision") as CollisionShape2D
	if acs: acs.debug_color = Color(0, 0, 0, 0)

	# Initialize components
	_movement.setup(self, _tile_service)
	_interaction.setup(self, _tile_service)
	
	# Setup camera
	if _camera:
		_camera.set_target(self)
	
	# Get visual manager
	_visual_manager = get_node_or_null("/root/VisualManager")
	
	# Connect component signals
	_movement.state_changed.connect(_on_movement_state_changed)
	_movement.ground_state_changed.connect(_on_ground_state_changed)
	_movement.jump_attempted.connect(_on_jump_attempted)
	_movement.jump_landed.connect(_on_jump_landed)
	_health.health_changed.connect(_on_health_changed)
	_health.death_triggered.connect(_on_death_triggered)
	_health.respawned.connect(_on_respawned)
	_interaction.coin_collected.connect(_on_coin_collected)
	_interaction.checkpoint_reached.connect(_on_checkpoint_reached)
	_interaction.exit_reached.connect(_on_exit_reached)
	_interaction.hazard_contact.connect(_on_hazard_contact)
	_interaction.enemy_contact.connect(_on_enemy_contact)

func setup(spawn_world: Vector2, lives: int) -> void:
	global_position = spawn_world
	_health.setup(self, spawn_world, lives)
	_tile_service.set_tile_size(32)
	if _camera:
		_camera.set_target(self)
		_camera.reset_camera_to_target()

func _physics_process(delta: float) -> void:
	_movement.update(delta)
	_health.update(delta)
	_analytics.update(delta)
	_update_knight_animation(delta)

func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed("ui_cancel") or event.is_action_pressed("pause"):
		emit_signal("request_pause")
	if event is InputEventKey and event.pressed:
		if event.keycode == KEY_R:
			emit_signal("request_restart")

# ── Component signal handlers ──────────────────────────────────────
func _on_movement_state_changed(state: PlayerEnums.MovementState) -> void:
	# Update animation based on movement state
	_update_animation(state)

func _on_ground_state_changed(_state: PlayerEnums.GroundState) -> void:
	pass

func _on_health_changed(_lives: int) -> void:
	pass

func _on_death_triggered(death_type: PlayerEnums.DeathType) -> void:
	_analytics.record_death(death_type)
	var type_str := "hazard"
	match death_type:
		PlayerEnums.DeathType.FALL: type_str = "fall"
		PlayerEnums.DeathType.HAZARD: type_str = "hazard"
		PlayerEnums.DeathType.ENEMY: type_str = "enemy"
	if has_node("/root/AudioManager"):
		var audio = get_node("/root/AudioManager")
		if _health and _health.get_lives() <= 0:
			audio.play_sound("explosion")
		else:
			audio.play_sound("hurt")
	emit_signal("player_died", type_str)
	_movement.stop()

func _on_respawned(spawn_pos: Vector2) -> void:
	global_position = spawn_pos
	_movement.stop()
	if _camera:
		_camera.reset_camera_to_target()

func _on_coin_collected(tile_pos: Vector2i) -> void:
	if has_node("/root/AudioManager"):
		get_node("/root/AudioManager").play_sound("coin")
	_analytics.record_coin()
	emit_signal("coin_collected", tile_pos)

func _on_checkpoint_reached(_tile_pos: Vector2i, _world_pos: Vector2) -> void:
	pass

func _on_exit_reached() -> void:
	if has_node("/root/AudioManager"):
		get_node("/root/AudioManager").play_sound("power_up")
	var stats: Dictionary = _analytics.get_level_stats()
	emit_signal("level_complete", stats)

func _on_hazard_contact() -> void:
	if _health:
		_health.take_damage(PlayerEnums.DeathType.FALL)

func _on_enemy_contact() -> void:
	if _health:
		_health.take_damage(PlayerEnums.DeathType.ENEMY)

func _on_jump_attempted() -> void:
	if has_node("/root/AudioManager"):
		get_node("/root/AudioManager").play_sound("jump")
	_analytics.record_jump_attempt()
	# Spawn jump particles
	if _visual_manager:
		_visual_manager.call("spawn_jump", global_position)

func _on_jump_landed() -> void:
	_analytics.record_jump_landed()
	# Spawn landing particles
	if _visual_manager:
		_visual_manager.call("spawn_landing", global_position)
	if _camera:
		_camera.trigger_shake(2.0, 0.1)

# ── Animation ───────────────────────────────────────────────────────
func _update_animation(state: PlayerEnums.MovementState) -> void:
	if _anim:
		match state:
			PlayerEnums.MovementState.IDLE:
				_anim.play("idle")
			PlayerEnums.MovementState.RUN:
				_anim.play("run")
			PlayerEnums.MovementState.JUMP:
				_anim.play("jump")
			PlayerEnums.MovementState.FALL:
				_anim.play("fall")
			PlayerEnums.MovementState.DEAD:
				_anim.play("death")

func _update_knight_animation(delta: float) -> void:
	if not _sprite or not (_sprite is Sprite2D):
		return
	var spr := _sprite as Sprite2D
	
	# Invincibility flicker
	if _health and _health.is_invincible():
		spr.modulate.a = 0.5 + 0.5 * sin(Time.get_ticks_msec() * 0.02)
	else:
		spr.modulate.a = 1.0

	var m_state: PlayerEnums.MovementState = _movement.get_state() if _movement else PlayerEnums.MovementState.IDLE
	_anim_timer += delta

	match m_state:
		PlayerEnums.MovementState.IDLE:
			if _anim_timer >= 0.15:
				_anim_timer = 0.0
				_anim_frame_idx = (_anim_frame_idx + 1) % IDLE_FRAMES.size()
			spr.frame = IDLE_FRAMES[_anim_frame_idx % IDLE_FRAMES.size()]
		PlayerEnums.MovementState.RUN:
			if _anim_timer >= 0.08:
				_anim_timer = 0.0
				_anim_frame_idx = (_anim_frame_idx + 1) % RUN_FRAMES.size()
			spr.frame = RUN_FRAMES[_anim_frame_idx % RUN_FRAMES.size()]
		PlayerEnums.MovementState.JUMP:
			spr.frame = JUMP_FRAME
		PlayerEnums.MovementState.FALL:
			spr.frame = FALL_FRAME
		PlayerEnums.MovementState.DEAD:
			spr.frame = 48

# ── Public accessors (for Main.gd compatibility) ─────────────────────
func get_lives() -> int:
	return _health.get_lives()

func set_lives(v: int) -> void:
	_health.set_lives(v)

func is_invincible() -> bool:
	return _health != null and _health.is_invincible()

func get_deaths() -> int:
	return _analytics.get_deaths()

func get_coins() -> int:
	return _analytics.get_coins()

func get_timer() -> float:
	return _analytics.get_timer()

func reset_level_stats() -> void:
	_analytics.reset()
	_movement.reset_stats()
	_health.reset_checkpoint()
	_health.reset_lives(3)

func set_tile_lookup(lookup: Dictionary) -> void:
	_tile_service.set_tile_lookup(lookup)

func set_camera_bounds(bounds: Rect2) -> void:
	if _camera:
		_camera.set_level_bounds(bounds)
		_camera.reset_camera_to_target()

# ── Legacy compatibility methods (called by Main.gd) ───────────────
func on_hazard_contact() -> void:
	_on_hazard_contact()

func on_enemy_contact(enemy_pos: Variant = null) -> void:
	_on_enemy_contact()

func on_coin_collected(tile_pos: Vector2i) -> void:
	_on_coin_collected(tile_pos)

func on_checkpoint_hit(tile_pos: Vector2i, world_pos: Vector2) -> void:
	_on_checkpoint_reached(tile_pos, world_pos)

func on_exit_reached() -> void:
	_on_exit_reached()
