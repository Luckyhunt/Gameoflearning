# ============================================================
# PlayerMovement.gd  —  Movement physics component
# ============================================================
# Handles: Gravity, Movement, Jump, Coyote time, Jump buffer, Ice, Bounce

extends Node
class_name PlayerMovement

signal state_changed(new_state: PlayerEnums.MovementState)
signal ground_state_changed(new_state: PlayerEnums.GroundState)
signal jump_attempted()
signal jump_landed()

# Exported configuration for designers
@export var move_speed := 180.0
@export var jump_velocity := -420.0
@export var gravity := 800.0
@export var bounce_velocity := -600.0
@export var ice_friction := 0.92
@export var normal_friction := 0.0

@export var coyote_frames := 8
@export var jump_buffer_frames := 10

var _character: CharacterBody2D
var _tile_service: TileService

# State
var _coyote_count: int = 0
var _jump_buffer_count: int = 0
var _current_state: PlayerEnums.MovementState = PlayerEnums.MovementState.IDLE
var _ground_state: PlayerEnums.GroundState = PlayerEnums.GroundState.AIR
var _was_in_air: bool = false

# Stats
var _jumps_attempted: int = 0
var _jumps_landed: int = 0

func setup(character: CharacterBody2D, tile_service: TileService) -> void:
	_character = character
	_tile_service = tile_service

func update(delta: float) -> void:
	_apply_gravity(delta)
	_handle_horizontal_movement()
	_handle_jump_buffer()
	_execute_jump()
	_character.move_and_slide()
	_detect_floor_type()
	_check_fall_death()

func _apply_gravity(delta: float) -> void:
	if not _character.is_on_floor():
		_character.velocity.y += gravity * delta
	else:
		if _was_in_air:
			_jumps_landed += 1
			emit_signal("jump_landed")
		_was_in_air = false
		_coyote_count = coyote_frames

	if not _character.is_on_floor():
		_coyote_count -= 1
		_was_in_air = true

func _handle_horizontal_movement() -> void:
	var dir := Input.get_axis("move_left", "move_right")
	if dir == 0.0:
		dir = Input.get_axis("ui_left", "ui_right")

	if dir != 0.0 and _character != null:
		var sprite: Sprite2D = _character.get_node_or_null("Sprite2D") as Sprite2D
		if sprite != null:
			sprite.flip_h = (dir < 0.0)

	var target_vx := dir * move_speed

	if _ground_state == PlayerEnums.GroundState.ICE and _character.is_on_floor():
		_character.velocity.x = lerp(_character.velocity.x, target_vx, 1.0 - ice_friction)
	else:
		_character.velocity.x = target_vx

	_update_movement_state(dir)

var _was_jump_key_down: bool = false

func _handle_jump_buffer() -> void:
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
		_jump_buffer_count = jump_buffer_frames

	if _jump_buffer_count > 0:
		_jump_buffer_count -= 1

func _execute_jump() -> void:
	if _jump_buffer_count > 0 and _coyote_count > 0:
		_character.velocity.y = jump_velocity
		_coyote_count = 0
		_jump_buffer_count = 0
		_jumps_attempted += 1
		emit_signal("jump_attempted")
		_set_state(PlayerEnums.MovementState.JUMP)

func _detect_floor_type() -> void:
	var old_ground_state = _ground_state
	
	if _character.is_on_floor():
		var tile_pos := _tile_service.world_to_tile(_character.global_position + Vector2(0, 8))
		
		if _tile_service.is_ice(tile_pos):
			_ground_state = PlayerEnums.GroundState.ICE
		elif _tile_service.is_bounce(tile_pos):
			_ground_state = PlayerEnums.GroundState.BOUNCE
			_character.velocity.y = bounce_velocity
		else:
			_ground_state = PlayerEnums.GroundState.GROUND
	else:
		_ground_state = PlayerEnums.GroundState.AIR
	
	if old_ground_state != _ground_state:
		emit_signal("ground_state_changed", _ground_state)

func _check_fall_death() -> void:
	if _character.global_position.y > _character.get_viewport_rect().size.y + 200:
		emit_signal("state_changed", PlayerEnums.MovementState.DEAD)

func _update_movement_state(dir: float) -> void:
	var new_state := PlayerEnums.MovementState.IDLE
	
	if not _character.is_on_floor():
		new_state = PlayerEnums.MovementState.FALL
	elif dir != 0:
		new_state = PlayerEnums.MovementState.RUN
	
	if new_state != _current_state:
		_current_state = new_state
		emit_signal("state_changed", _current_state)

func _set_state(state: PlayerEnums.MovementState) -> void:
	_current_state = state
	emit_signal("state_changed", _current_state)

# Public getters for animation
func get_state() -> PlayerEnums.MovementState:
	return _current_state

func get_ground_state() -> PlayerEnums.GroundState:
	return _ground_state

func get_jumps_attempted() -> int:
	return _jumps_attempted

func get_jumps_landed() -> int:
	return _jumps_landed

func reset_stats() -> void:
	_jumps_attempted = 0
	_jumps_landed = 0
	_was_in_air = false
	_coyote_count = 0
	_jump_buffer_count = 0

func stop() -> void:
	_character.velocity = Vector2.ZERO
