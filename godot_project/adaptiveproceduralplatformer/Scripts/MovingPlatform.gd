extends Node2D
# ============================================================
# MovingPlatform.gd
# Horizontal oscillation, speed + range set by Main.gd.
# The StaticBody2D child provides Godot physics collision.
# ============================================================

@export var move_range: float = 64.0   # px
@export var move_speed: float = 60.0   # px/sec

var _start_pos: Vector2
var _dir: float = 1.0

@onready var _body: StaticBody2D = $StaticBody2D

func _ready() -> void:
	_start_pos = global_position

func _physics_process(delta: float) -> void:
	var new_x := global_position.x + _dir * move_speed * delta
	if new_x > _start_pos.x + move_range:
		_dir = -1.0
	elif new_x < _start_pos.x - move_range:
		_dir = 1.0
	var dx := _dir * move_speed * delta
	global_position.x += dx
	# Move any player standing on it
	if _body:
		_body.move_and_collide(Vector2(dx, 0))
