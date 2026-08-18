extends Node2D
# ============================================================
# Enemy.gd  —  Platformer enemy AI (Teachers / Aliens)
#
# Modes:
#   0 = Patrol  — Math Teacher: oscillates horizontally within platform edges
#   1 = Chase   — Principal / Alien Brute: chases player with LOS & edge guards
#   2 = Flying  — Alien Drone: floating patrol pattern
#   3 = Ranged  — Science Teacher / Sentry: shoots chalk/lasers with LOS & cooldown
# ============================================================

signal enemy_killed

const TILE_SIZE := 32

@export var speed: float = 60.0
@export var enemy_type: int = 0     # 0=Patrol, 1=Chase, 2=Flying, 3=Ranged
@export var patrol_range: float = 80.0

var _dir: float = 1.0
var _start_pos: Vector2
var _player: CharacterBody2D = null
var _alive: bool = true
var _patrol_timer: float = 0.0
var _attack_cooldown: float = 0.0
var _tile_lookup: Dictionary = {}
var _level_bounds := Rect2(32, 32, 1400, 700)

@onready var _sprite := $Sprite2D if has_node("Sprite2D") else null

func _ready() -> void:
	_start_pos = global_position
	_player = get_tree().get_first_node_in_group("player") as CharacterBody2D

func set_tile_lookup(lookup: Dictionary, bounds: Rect2 = Rect2()) -> void:
	_tile_lookup = lookup
	if bounds.size != Vector2.ZERO:
		_level_bounds = bounds

func _physics_process(delta: float) -> void:
	if not _alive: return
	if _player == null or not is_instance_valid(_player):
		_player = get_tree().get_first_node_in_group("player") as CharacterBody2D

	if _attack_cooldown > 0.0:
		_attack_cooldown -= delta

	match enemy_type:
		0: _patrol(delta)
		1: _chase(delta)
		2: _fly(delta)
		3: _ranged(delta)

	# Strict boundary enforcement
	global_position.x = clampf(global_position.x, _level_bounds.position.x + 16.0, _level_bounds.position.x + _level_bounds.size.x - 16.0)
	global_position.y = clampf(global_position.y, _level_bounds.position.y + 16.0, _level_bounds.position.y + _level_bounds.size.y - 16.0)

	if _sprite:
		_sprite.scale.x = _dir

func _patrol(delta: float) -> void:
	if not _can_move_ahead(_dir):
		_dir *= -1.0

	global_position.x += _dir * speed * delta

	if global_position.x > _start_pos.x + patrol_range:
		_dir = -1.0
	elif global_position.x < _start_pos.x - patrol_range:
		_dir = 1.0

func _chase(delta: float) -> void:
	if not _player:
		_patrol(delta)
		return

	var dist := global_position.distance_to(_player.global_position)
	if dist < 220.0 and _has_line_of_sight(_player.global_position):
		var target_dir := signf(_player.global_position.x - global_position.x)
		if target_dir != 0.0 and _can_move_ahead(target_dir):
			_dir = target_dir
			global_position.x += _dir * speed * delta
		else:
			_patrol(delta)
	else:
		_patrol(delta)

func _fly(delta: float) -> void:
	_patrol_timer += delta
	global_position.x += _dir * speed * delta * 0.8
	global_position.y += sin(_patrol_timer * 3.0) * speed * delta * 0.5
	if global_position.x > _start_pos.x + patrol_range:
		_dir = -1.0
	elif global_position.x < _start_pos.x - patrol_range:
		_dir = 1.0

func _ranged(delta: float) -> void:
	if not _player: return
	var dist := global_position.distance_to(_player.global_position)
	_dir = signf(_player.global_position.x - global_position.x)
	if _dir == 0.0: _dir = 1.0

	if dist > 80.0 and dist < 300.0:
		if _attack_cooldown <= 0.0 and _has_line_of_sight(_player.global_position):
			_shoot_projectile()
			_attack_cooldown = 2.0
	elif dist <= 80.0:
		var back_dir := -_dir
		if _can_move_ahead(back_dir):
			global_position.x += back_dir * speed * 0.6 * delta

func _can_move_ahead(dir_x: float) -> bool:
	if _tile_lookup.is_empty(): return true
	var check_x := global_position.x + dir_x * 18.0
	var foot_y := global_position.y + 20.0
	var wall_y := global_position.y

	var tile_ahead := Vector2i(int(check_x / TILE_SIZE), int(wall_y / TILE_SIZE))
	var tile_foot := Vector2i(int(check_x / TILE_SIZE), int(foot_y / TILE_SIZE))

	var wall_type: int = _tile_lookup.get(tile_ahead, 0)
	if wall_type == 1: return false # Solid block ahead

	var ground_type: int = _tile_lookup.get(tile_foot, 0)
	if ground_type == 0: return false # Platform drop-off

	return true

func _has_line_of_sight(target_pos: Vector2) -> bool:
	if _tile_lookup.is_empty(): return true
	var steps := int(global_position.distance_to(target_pos) / 16.0)
	if steps <= 0: steps = 1
	var step_vec := (target_pos - global_position) / float(steps)
	var curr := global_position
	for i in range(steps):
		curr += step_vec
		var tpos := Vector2i(int(curr.x / TILE_SIZE), int(curr.y / TILE_SIZE))
		if _tile_lookup.get(tpos, 0) == 1:
			return false
	return true

func _shoot_projectile() -> void:
	var proj := Area2D.new()
	proj.global_position = global_position + Vector2(_dir * 16.0, -4.0)

	var cs := CollisionShape2D.new()
	var circle := CircleShape2D.new()
	circle.radius = 6.0
	cs.shape = circle
	proj.add_child(cs)

	var icon := ColorRect.new()
	icon.size = Vector2(10, 10)
	icon.position = Vector2(-5, -5)
	icon.color = Color(1.0, 0.9, 0.3)
	proj.add_child(icon)

	var pdir := ( _player.global_position - proj.global_position ).normalized()
	get_parent().add_child(proj)

	var timer := get_tree().create_timer(3.0)
	timer.timeout.connect(func(): if is_instance_valid(proj): proj.queue_free())

	proj.connect("body_entered", func(body: Node2D):
		if body.is_in_group("player") and body.has_method("on_enemy_contact"):
			body.call("on_enemy_contact")
			if is_instance_valid(proj): proj.queue_free()
		elif body is TileMapLayer or body is StaticBody2D:
			if is_instance_valid(proj): proj.queue_free()
	)

	var tween := proj.create_tween()
	tween.tween_property(proj, "global_position", proj.global_position + pdir * 280.0, 1.2)

func _process(_delta: float) -> void:
	if not _alive or not _player: return
	var dist := global_position.distance_to(_player.global_position)
	if dist < 22.0:
		_player.on_enemy_contact()

func kill() -> void:
	_alive = false
	emit_signal("enemy_killed")
	queue_free()
