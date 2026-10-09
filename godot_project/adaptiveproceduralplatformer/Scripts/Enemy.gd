# ============================================================
# Enemy.gd  —  Platformer Slime Enemy AI
# ============================================================
# STOMP ARCHITECTURE (same as Mario/Celeste):
#   - Stomp is detected on the PLAYER'S side by checking slide
#     collisions after move_and_slide(). If the player hits an
#     enemy with an upward normal while falling -> stomp.
#   - This enemy exposes: stomp_kill()  -> called by player
#                         side_damage() -> called by Area2D side-touch
#   - The enemy's Area2D only handles SIDE damage to player.

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

var patrol_speed: float = 38.0
var platform_min_x: float = -99999.0
var platform_max_x: float = 99999.0

var _dir: float = 1.0
var _alive: bool = true

var _sprite: Sprite2D = null
var _anim_timer: float = 0.0
var _anim_frame: int = 0

var _tile_lookup: Dictionary = {}
var _overlapping_players: Array = []   # bodies currently touching SideDamageArea
var _damage_cooldown: float = 0.0      # seconds until next side-damage tick
const DAMAGE_INTERVAL: float = 0.8    # damage player every 0.8s of contact

func _ready() -> void:
	add_to_group("enemies")
	_setup_visuals()
	_setup_collision()

func setup(p_type: EnemyType, min_x: float = -99999.0, max_x: float = 99999.0, tile_lookup: Dictionary = {}) -> void:
	enemy_type = p_type
	platform_min_x = min_x
	platform_max_x = max_x
	_tile_lookup = tile_lookup
	if enemy_type == EnemyType.GREEN_SLIME:
		patrol_speed = 36.0
	else:
		patrol_speed = 50.0
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
	# CharacterBody2D collision setup
	# Layer 2 = enemies. Mask includes 1 (tiles) so gravity works,
	# and also includes 1 (player layer) so the player's move_and_slide
	# generates a slide collision that _check_stomp_kills() can read.
	collision_layer = 2
	collision_mask  = 1  # tiles only — player resolves FROM player's side

	var cs := CollisionShape2D.new()
	cs.name = "CollisionShape2D"
	var box := RectangleShape2D.new()
	box.size = Vector2(18, 14)
	cs.shape = box
	cs.position = Vector2(0, -7)
	cs.debug_color = Color(0, 0, 0, 0)
	add_child(cs)

	# Side-damage Area2D — detects player walking INTO the enemy from the side.
	# collision_mask = 1 because the player CharacterBody2D is on collision_layer 1.
	# Stomps are handled exclusively by PlayerMovement._check_stomp_kills().
	var side_area := Area2D.new()
	side_area.name = "SideDamageArea"
	side_area.collision_layer = 0  # Area doesn't need to be seen by others
	side_area.collision_mask  = 1  # MUST match player's collision_layer (= 1)

	var scs := CollisionShape2D.new()
	var sbox := RectangleShape2D.new()
	sbox.size = Vector2(16, 10)
	scs.shape = sbox
	scs.position = Vector2(0, -5)
	scs.debug_color = Color(0, 0, 0, 0)
	side_area.add_child(scs)
	add_child(side_area)
	side_area.body_entered.connect(_on_side_body_entered)
	side_area.body_exited.connect(_on_side_body_exited)

func _physics_process(delta: float) -> void:
	if not _alive:
		return

	if not is_on_floor():
		velocity.y += GRAVITY * delta
	else:
		velocity.y = 0.0

	var cur_x := global_position.x
	if cur_x <= platform_min_x:
		global_position.x = platform_min_x
		_dir = 1.0
	elif cur_x >= platform_max_x:
		global_position.x = platform_max_x
		_dir = -1.0
	elif not _can_move_ahead(_dir):
		_dir *= -1.0

	velocity.x = _dir * patrol_speed
	move_and_slide()

	if platform_min_x > -90000.0 and platform_max_x < 90000.0:
		global_position.x = clampf(global_position.x, platform_min_x, platform_max_x)

	_update_animation(delta)

	# Continuous side-damage tick while player overlaps the SideDamageArea
	if _damage_cooldown > 0.0:
		_damage_cooldown -= delta
	if not _overlapping_players.is_empty() and _damage_cooldown <= 0.0:
		for p in _overlapping_players:
			if is_instance_valid(p) and p.is_in_group("player"):
				if p.has_method("is_invincible") and p.call("is_invincible"):
					continue
				if p.has_method("on_enemy_contact"):
					p.call("on_enemy_contact", global_position)
					_damage_cooldown = DAMAGE_INTERVAL
					break

func _can_move_ahead(dir_x: float) -> bool:
	if _tile_lookup.is_empty():
		return true
	var check_x := global_position.x + dir_x * 16.0
	var foot_y  := global_position.y + 6.0
	var wall_y  := global_position.y - 8.0
	var tile_ahead := Vector2i(int(check_x / TILE_SIZE), int(wall_y / TILE_SIZE))
	var tile_foot  := Vector2i(int(check_x / TILE_SIZE), int(foot_y / TILE_SIZE))
	if _tile_lookup.get(tile_ahead, 0) == 1:
		return false
	if _tile_lookup.get(tile_foot, 0) == 0:
		return false
	return true

func _update_animation(delta: float) -> void:
	if not _sprite:
		return
	_sprite.flip_h = (_dir > 0.0)
	_anim_timer += delta
	if _anim_timer >= 0.16:
		_anim_timer = 0.0
		_anim_frame = (_anim_frame + 1) % 4
		_sprite.frame = _anim_frame

func _on_side_body_entered(body: Node2D) -> void:
	if not _alive:
		return
	if body.is_in_group("player"):
		if not _overlapping_players.has(body):
			_overlapping_players.append(body)


func _on_side_body_exited(body: Node2D) -> void:
	_overlapping_players.erase(body)


func _on_side_damage_body_entered(body: Node2D) -> void:
	# Legacy alias kept for safety
	_on_side_body_entered(body)

# Called by PlayerMovement when it detects stomp via its own slide-collisions
func stomp_kill(player: Node2D) -> void:
	if not _alive:
		return
	_alive = false
	_overlapping_players.clear()  # stop side-damage ticks
	if "velocity" in player:
		player.velocity.y = -380.0
	if has_node("/root/AudioManager"):
		get_node("/root/AudioManager").play_sound("power_up")
	if _sprite:
		var tw := create_tween()
		tw.set_parallel(true)
		tw.tween_property(_sprite, "scale:y", 0.1, 0.1)
		tw.tween_property(_sprite, "scale:x", 1.4, 0.1)
		tw.tween_property(_sprite, "modulate:a", 0.0, 0.18)
		tw.chain().tween_callback(Callable(self, "_finish_kill"))
	else:
		_finish_kill()

func kill() -> void:
	if not _alive:
		return
	_alive = false
	_finish_kill()

func _finish_kill() -> void:
	emit_signal("enemy_killed", self)
	queue_free()
