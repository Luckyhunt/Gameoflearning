# ============================================================
# Enemy.gd  —  Platformer Slime Enemy AI
# ============================================================
# Supports Green & Purple Slimes with idle/patrol states,
# proximity-based aggro, and top-stomp melee kill mechanics.

extends CharacterBody2D
class_name Enemy

signal enemy_killed(enemy_ref: Node)

const TILE_SIZE: float = 32.0
const GRAVITY: float = 800.0

enum EnemyType {
	GREEN_SLIME = 0,
	PURPLE_SLIME = 1
}

@export var enemy_type: EnemyType = EnemyType.GREEN_SLIME

# Movement & Detection parameters
var patrol_speed: float = 45.0
var chase_speed: float = 80.0
var aggro_radius: float = 180.0
var patrol_range: float = 90.0

var _dir: float = 1.0
var _start_x: float = 0.0
var _player: CharacterBody2D = null
var _alive: bool = true
var _is_aggro: bool = false

# Sprite & Animation
var _sprite: Sprite2D = null
var _anim_timer: float = 0.0
var _anim_frame: int = 0

# Platform edge checking
var _tile_lookup: Dictionary = {}

func _ready() -> void:
	add_to_group("enemies")
	_start_x = global_position.x
	_player = get_tree().get_first_node_in_group("player") as CharacterBody2D
	
	_setup_visuals()
	_setup_collision()

func setup(p_type: EnemyType, tile_lookup: Dictionary = {}) -> void:
	enemy_type = p_type
	_tile_lookup = tile_lookup
	
	if enemy_type == EnemyType.GREEN_SLIME:
		patrol_speed = 45.0
		chase_speed = 80.0
		aggro_radius = 170.0
		patrol_range = 80.0
	else:
		patrol_speed = 65.0
		chase_speed = 115.0
		aggro_radius = 240.0
		patrol_range = 100.0
		
	if _sprite:
		var tex_path := "res://Assets/sprites/slime_green.png" if enemy_type == EnemyType.GREEN_SLIME else "res://Assets/sprites/slime_purple.png"
		if ResourceLoader.exists(tex_path):
			_sprite.texture = load(tex_path) as Texture2D

func _setup_visuals() -> void:
	_sprite = Sprite2D.new()
	_sprite.name = "Sprite2D"
	var tex_path := "res://Assets/sprites/slime_green.png" if enemy_type == EnemyType.GREEN_SLIME else "res://Assets/sprites/slime_purple.png"
	if ResourceLoader.exists(tex_path):
		_sprite.texture = load(tex_path) as Texture2D
	_sprite.hframes = 4
	_sprite.vframes = 3
	_sprite.frame = 0
	_sprite.texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST
	_sprite.position = Vector2(0, -12)
	add_child(_sprite)

func _setup_collision() -> void:
	# Hitbox shape for CharacterBody2D
	var cs := CollisionShape2D.new()
	cs.name = "CollisionShape2D"
	var box := RectangleShape2D.new()
	box.size = Vector2(18, 14)
	cs.shape = box
	cs.position = Vector2(0, -7)
	cs.debug_color = Color(0, 0, 0, 0)
	add_child(cs)
	
	# Touch detection area
	var touch_area := Area2D.new()
	touch_area.name = "TouchArea"
	var tcs := CollisionShape2D.new()
	var tbox := RectangleShape2D.new()
	tbox.size = Vector2(22, 16)
	tcs.shape = tbox
	tcs.position = Vector2(0, -8)
	tcs.debug_color = Color(0, 0, 0, 0)
	touch_area.add_child(tcs)
	add_child(touch_area)
	
	touch_area.body_entered.connect(_on_touch_body_entered)

func _physics_process(delta: float) -> void:
	if not _alive:
		return
		
	if _player == null or not is_instance_valid(_player):
		_player = get_tree().get_first_node_in_group("player") as CharacterBody2D
		
	# Apply gravity
	if not is_on_floor():
		velocity.y += GRAVITY * delta
	else:
		velocity.y = 0.0

	# Aggro check: only attack when player gets near
	var dist_to_player := 9999.0
	if _player and is_instance_valid(_player):
		dist_to_player = global_position.distance_to(_player.global_position)
		
	if dist_to_player <= aggro_radius:
		_is_aggro = true
	elif dist_to_player > aggro_radius * 1.5:
		_is_aggro = false
		
	if _is_aggro and _player:
		# Chase player
		var target_dir := signf(_player.global_position.x - global_position.x)
		if target_dir != 0.0:
			_dir = target_dir
		velocity.x = _dir * chase_speed
	else:
		# Calm platform patrol
		if not _can_move_ahead(_dir):
			_dir *= -1.0
		elif global_position.x > _start_x + patrol_range:
			_dir = -1.0
		elif global_position.x < _start_x - patrol_range:
			_dir = 1.0
		velocity.x = _dir * patrol_speed

	move_and_slide()
	_update_animation(delta)

func _can_move_ahead(dir_x: float) -> bool:
	if _tile_lookup.is_empty():
		return true
	var check_x := global_position.x + dir_x * 16.0
	var foot_y := global_position.y + 6.0
	var wall_y := global_position.y - 8.0

	var tile_ahead := Vector2i(int(check_x / TILE_SIZE), int(wall_y / TILE_SIZE))
	var tile_foot := Vector2i(int(check_x / TILE_SIZE), int(foot_y / TILE_SIZE))

	var wall_type: int = _tile_lookup.get(tile_ahead, 0)
	if wall_type == 1: 
		return false # Wall ahead

	var ground_type: int = _tile_lookup.get(tile_foot, 0)
	if ground_type == 0: 
		return false # Platform drop-off (don't fall into void during patrol)

	return true

func _update_animation(delta: float) -> void:
	if not _sprite:
		return
		
	# Facing direction
	_sprite.flip_h = (_dir > 0.0)
	
	# Frame cycling
	_anim_timer += delta
	var frame_speed := 0.12 if _is_aggro else 0.18
	if _anim_timer >= frame_speed:
		_anim_timer = 0.0
		_anim_frame = (_anim_frame + 1) % 4
		# Row 0 = idle/patrol (frames 0..3), Row 1 = aggro hop (frames 4..7)
		var base_frame := 4 if _is_aggro else 0
		_sprite.frame = base_frame + _anim_frame

func _on_touch_body_entered(body: Node2D) -> void:
	if not _alive:
		return
		
	if body.is_in_group("player"):
		_handle_player_collision(body)

func _handle_player_collision(player: Node2D) -> void:
	var p_vel: Vector2 = player.velocity if "velocity" in player else Vector2.ZERO
	# Melee jump stomp check: player is moving downward and their feet/center is above the slime
	var is_falling: bool = p_vel.y > -50.0 # falling or at apex
	var is_above: bool = player.global_position.y < (global_position.y - 4.0)

	if is_falling and is_above:
		# Successful Melee Stomp Attack!
		_stomp_kill(player)
	else:
		# Player collided from the side or below without jumping on enemy
		if player.has_method("is_invincible") and player.call("is_invincible"):
			return
		if player.has_method("on_enemy_contact"):
			player.call("on_enemy_contact", global_position)

func _stomp_kill(player: Node2D) -> void:
	_alive = false
	
	# Bounce the player upward in satisfying platformer style
	if "velocity" in player:
		player.velocity.y = -350.0
		
	if has_node("/root/AudioManager"):
		get_node("/root/AudioManager").play_sound("power_up")
		
	# Squash & vanish tween
	if _sprite:
		var tw := create_tween()
		tw.set_parallel(true)
		tw.tween_property(_sprite, "scale:y", 0.15, 0.12)
		tw.tween_property(_sprite, "modulate:a", 0.0, 0.15)
		tw.chain().tween_callback(Callable(self, "_finish_kill"))
	else:
		_finish_kill()

func _finish_kill() -> void:
	emit_signal("enemy_killed", self)
	queue_free()

func kill() -> void:
	if not _alive:
		return
	_alive = false
	_finish_kill()
