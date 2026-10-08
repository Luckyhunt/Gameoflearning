# ============================================================
# PlayerHealth.gd  —  Health and respawn component
# ============================================================
# Handles: Lives, Respawn, Death, Invincibility, Checkpoint respawn

extends Node
class_name PlayerHealth

signal health_changed(lives: int)
signal death_triggered(death_type: PlayerEnums.DeathType)
signal respawned(spawn_pos: Vector2)

var _character: Node2D
var _lives: int = 3
var _spawn_pos: Vector2 = Vector2.ZERO
var _checkpoint_pos: Vector2 = Vector2.ZERO
var _has_checkpoint: bool = false
var _invincible: bool = false
var _invincible_timer: float = 0.0
var _invincibility_duration: float = 1.5

func setup(character: Node2D, spawn_world: Vector2, lives: int) -> void:
	_character = character
	_spawn_pos = spawn_world
	_checkpoint_pos = spawn_world
	_lives = lives
	_character.global_position = spawn_world

func update(delta: float) -> void:
	if _invincible:
		_invincible_timer -= delta
		if _invincible_timer <= 0.0:
			_invincible = false

func take_damage(death_type: PlayerEnums.DeathType) -> void:
	if _invincible:
		return
	_lives -= 1
	_invincible = true
	_invincible_timer = _invincibility_duration
	
	emit_signal("health_changed", _lives)
	emit_signal("death_triggered", death_type)
	if _lives > 0:
		_respawn()

func _respawn() -> void:
	var target := _spawn_pos
	_character.global_position = target
	_invincible = true
	_invincible_timer = _invincibility_duration
	emit_signal("respawned", target)

func set_checkpoint(_world_pos: Vector2) -> void:
	pass

func get_lives() -> int:
	return _lives

func set_lives(v: int) -> void:
	_lives = v
	emit_signal("health_changed", _lives)

func is_invincible() -> bool:
	return _invincible

func reset_checkpoint() -> void:
	_has_checkpoint = false
	_checkpoint_pos = _spawn_pos

func reset_lives(initial_lives: int) -> void:
	_lives = initial_lives
	_invincible = false
	_invincible_timer = 0.0
	emit_signal("health_changed", _lives)
