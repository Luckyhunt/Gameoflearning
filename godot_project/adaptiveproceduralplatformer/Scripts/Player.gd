extends CharacterBody2D
# ============================================================
# Player.gd  —  Full adaptive platformer player controller
#
# Features:
#   - Move / Jump / Coyote time / Jump buffer
#   - Ice platform friction
#   - Bounce pad launch
#   - Lives system (3 by default, set by Main.gd)
#   - Death categorisation (fall / hazard / enemy)
#   - Checkpoint respawn
#   - Restart (R key)
#   - ESC pause
#   - Analytics: jump attempts/successes, coins, timer
# ============================================================

signal player_died(death_type: String)        # "fall" | "hazard" | "enemy"
signal checkpoint_reached(tile_pos: Vector2i)
signal level_complete(stats: Dictionary)
signal coin_collected(tile_pos: Vector2i)
signal request_restart
signal request_pause

# ── Physics ─────────────────────────────────────────────────────
const GRAVITY        := 800.0
const MOVE_SPEED     := 180.0
const JUMP_VELOCITY  := -420.0
const BOUNCE_VELOCITY:= -600.0   # BouncePad launch
const ICE_FRICTION   := 0.92     # velocity multiplier per frame on ice
const NORMAL_FRICTION:= 0.0      # no sliding on normal ground

const COYOTE_FRAMES  := 8        # frames of grace after leaving edge
const JUMP_BUFFER    := 10       # frames of jump press buffer

# ── State ────────────────────────────────────────────────────────
var _lives: int = 3
var _spawn_pos: Vector2  = Vector2.ZERO
var _checkpoint_pos: Vector2 = Vector2.ZERO
var _has_checkpoint: bool = false

var _timer: float = 0.0
var _deaths: int = 0
var _deaths_hazard: int = 0
var _deaths_fall: int = 0
var _deaths_enemy: int = 0
var _damage_taken: int = 0
var _coins_collected: int = 0
var _jumps_attempted: int = 0
var _jumps_landed: int = 0
var _jump_pending_landing: bool = false
var _was_in_air: bool = false
var _used_checkpoint: bool = false

var _attacks_attempted: int = 0
var _attacks_landed: int = 0
var _enemies_killed: int = 0
var _combo_count: int = 0
var _highest_combo: int = 0
var _facing_dir: float = 1.0

var _coyote_count: int = 0
var _jump_buffer_count: int = 0
var _was_jump_key_down: bool = false
var _on_ice: bool = false
var _invincible: bool = false
var _invincible_timer: float = 0.0
var _camera_bounds: Rect2 = Rect2()

func set_camera_bounds(bounds: Rect2) -> void:
	_camera_bounds = bounds

@onready var _sprite: Node2D = $Sprite2D if has_node("Sprite2D") else null
@onready var _anim: AnimationPlayer = $AnimationPlayer if has_node("AnimationPlayer") else null

func _ready() -> void:
	_spawn_pos = global_position

func setup(spawn_world: Vector2, lives: int) -> void:
	_spawn_pos = spawn_world
	_checkpoint_pos = spawn_world
	_lives = lives
	global_position = spawn_world

# ── Input ────────────────────────────────────────────────────────
func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed("ui_cancel") or (event is InputEventKey and event.pressed and event.keycode == KEY_P):
		emit_signal("request_pause")
	if event is InputEventKey and event.pressed:
		if event.keycode == KEY_R:
			emit_signal("request_restart")
		elif event.keycode == KEY_J:
			_perform_attack("light")
		elif event.keycode == KEY_K:
			_perform_attack("heavy")
		elif event.keycode == KEY_H:
			_perform_dash()
		elif event.keycode == KEY_U:
			_perform_attack("special")
		elif event.keycode == KEY_I:
			_perform_interact()

# ── Combat & Abilities ──────────────────────────────────────────
func _perform_attack(type: String) -> void:
	_attacks_attempted += 1
	var attack_range: float = 50.0
	if type == "heavy": attack_range = 80.0
	elif type == "special": attack_range = 120.0

	var hit_any := false
	var enemies := get_tree().get_nodes_in_group("enemies")
	for e in enemies:
		if is_instance_valid(e):
			var dist := global_position.distance_to(e.global_position)
			if dist <= attack_range:
				if e.has_method("kill"):
					e.call("kill")
					hit_any = true
					_enemies_killed += 1

	if hit_any:
		_attacks_landed += 1
		_combo_count += 1
		_highest_combo = max(_highest_combo, _combo_count)
	else:
		_combo_count = 0

func _perform_dash() -> void:
	velocity.x = _facing_dir * 500.0
	_invincible = true
	_invincible_timer = 0.4

func _perform_interact() -> void:
	print("[Player] Interact action pressed")

# ── Physics process ──────────────────────────────────────────────
func _physics_process(delta: float) -> void:
	_timer += delta

	# Invincibility frames
	if _invincible:
		_invincible_timer -= delta
		if _invincible_timer <= 0.0:
			_invincible = false

	# Gravity
	if not is_on_floor():
		velocity.y += GRAVITY * delta
	else:
		if _jump_pending_landing:
			_jumps_landed += 1
			_jump_pending_landing = false
		_was_in_air = false
		_coyote_count = COYOTE_FRAMES

	if not is_on_floor():
		_coyote_count -= 1
		_was_in_air = true

	# Horizontal movement (A / D or Left / Right)
	var dir := Input.get_axis("move_left", "move_right")
	if dir == 0.0:
		dir = Input.get_axis("ui_left", "ui_right")

	if dir != 0.0:
		_facing_dir = sign(dir)
		if _sprite != null and _sprite is Sprite2D:
			(_sprite as Sprite2D).flip_h = (_facing_dir < 0.0)

	var target_vx := dir * MOVE_SPEED

	if _on_ice and is_on_floor():
		velocity.x = lerp(velocity.x, target_vx, 1.0 - ICE_FRICTION)
	else:
		velocity.x = target_vx

	# Jump buffer (Space / W / Up)
	var is_jump_key_down := (
		(InputMap.has_action("jump") and Input.is_action_pressed("jump")) or
		(InputMap.has_action("move_up") and Input.is_action_pressed("move_up")) or
		Input.is_action_pressed("ui_accept") or
		Input.is_action_pressed("ui_up") or
		Input.is_key_pressed(KEY_W) or
		Input.is_key_pressed(KEY_UP) or
		Input.is_key_pressed(KEY_SPACE)
	)

	var jump_just_pressed := is_jump_key_down and not _was_jump_key_down
	_was_jump_key_down = is_jump_key_down

	if jump_just_pressed:
		_jump_buffer_count = JUMP_BUFFER

	if _jump_buffer_count > 0:
		_jump_buffer_count -= 1

	# Jump execute
	if _jump_buffer_count > 0 and _coyote_count > 0:
		velocity.y = JUMP_VELOCITY
		_coyote_count = 0
		_jump_buffer_count = 0
		_jumps_attempted += 1
		_jump_pending_landing = true

	move_and_slide()

	# Enforce strict map boundary clamping
	if _camera_bounds.size != Vector2.ZERO:
		global_position.x = clampf(global_position.x, _camera_bounds.position.x + 16.0, _camera_bounds.position.x + _camera_bounds.size.x - 16.0)
		global_position.y = clampf(global_position.y, _camera_bounds.position.y + 16.0, _camera_bounds.position.y + _camera_bounds.size.y - 16.0)

	# Floor type detection (ice / bounce)
	_on_ice = false
	if is_on_floor():
		var tile_pos := _world_to_tile(global_position + Vector2(0, 8))
		var tile_type := _get_tile_type(tile_pos)
		if tile_type == 13:  # IcePlatform
			_on_ice = true
		elif tile_type == 12:  # BouncePad
			velocity.y = BOUNCE_VELOCITY

	# Death: fell out of world (bottom of level)
	var fall_limit_y: float = _camera_bounds.position.y + _camera_bounds.size.y + 100.0 if _camera_bounds.size != Vector2.ZERO else 2000.0
	if global_position.y > fall_limit_y:
		_trigger_death("fall")

# ── External tile checks (called by Main.gd) ──────────────────────
func _world_to_tile(world_pos: Vector2) -> Vector2i:
	return Vector2i(int(world_pos.x / 32), int(world_pos.y / 32))

func _get_tile_type(_tile_pos: Vector2i) -> int:
	return _tile_lookup.get(_tile_pos, 0)

var _tile_lookup: Dictionary = {}

func set_tile_lookup(lookup: Dictionary) -> void:
	_tile_lookup = lookup

# ── Called by Main.gd when player overlaps hazard ────────────────
func on_hazard_contact() -> void:
	_damage_taken += 10

func on_enemy_contact() -> void:
	_damage_taken += 15

func on_coin_collected(tile_pos: Vector2i) -> void:
	_coins_collected += 1
	emit_signal("coin_collected", tile_pos)

func on_checkpoint_hit(tile_pos: Vector2i, world_pos: Vector2) -> void:
	_checkpoint_pos = world_pos
	_has_checkpoint = true
	_used_checkpoint = true
	emit_signal("checkpoint_reached", tile_pos)

# ── Death / Respawn ───────────────────────────────────────────────
func _trigger_death(death_type: String) -> void:
	if _invincible:
		return
	_invincible = true
	_invincible_timer = 2.0
	_deaths += 1
	_damage_taken += 25
	emit_signal("player_died", death_type)
	_respawn()

func _respawn() -> void:
	var target := _checkpoint_pos if _has_checkpoint else _spawn_pos
	global_position = target
	velocity = Vector2.ZERO
	_invincible = true
	_invincible_timer = 1.5

# ── Level complete ────────────────────────────────────────────────
func on_all_enemies_cleared() -> void:
	on_exit_reached()

func on_exit_reached() -> void:
	var jump_accuracy := clampf(float(_jumps_landed) / float(max(_jumps_attempted, 1)), 0.0, 1.0)
	var attack_accuracy := clampf(float(_attacks_landed) / float(max(_attacks_attempted, 1)), 0.0, 1.0)
	var stats := {
		"deaths":            _deaths,
		"damage_taken":      _damage_taken,
		"time_sec":          _timer,
		"coins":             _coins_collected,
		"jump_accuracy":     jump_accuracy,
		"attack_accuracy":   attack_accuracy,
		"attacks_attempted": _attacks_attempted,
		"attacks_landed":    _attacks_landed,
		"jumps_attempted":   _jumps_attempted,
		"jumps_landed":      _jumps_landed,
		"enemies_killed":    _enemies_killed,
		"highest_combo":     _highest_combo,
		"used_checkpoint":   _used_checkpoint,
	}
	emit_signal("level_complete", stats)

# ── Public accessors ──────────────────────────────────────────────
func get_lives() -> int: return _lives
func set_lives(v: int) -> void: _lives = v
func get_deaths() -> int: return _deaths
func get_coins() -> int: return _coins_collected
func get_timer() -> float: return _timer
func is_invincible() -> bool: return _invincible
func reset_level_stats() -> void:
	_timer = 0.0
	_deaths = 0
	_deaths_hazard = 0
	_deaths_fall = 0
	_deaths_enemy = 0
	_damage_taken = 0
	_coins_collected = 0
	_jumps_attempted = 0
	_jumps_landed = 0
	_attacks_attempted = 0
	_attacks_landed = 0
	_enemies_killed = 0
	_combo_count = 0
	_highest_combo = 0
	_was_in_air = false
	_used_checkpoint = false
	_has_checkpoint = false
	_checkpoint_pos = _spawn_pos
	velocity = Vector2.ZERO
