## EntitySpawner.gd
## ─────────────────────────────────────────────────────────────────
## Spawns and manages enemy entities on procedural platforms.
## Enforces minimum safe distance from player spawn, mixes Green
## and Purple slimes, and tracks level completion via enemy elimination.
## ─────────────────────────────────────────────────────────────────

extends Node

const TILE_SIZE: int = 32

signal enemy_killed(remaining_count: int)
signal all_enemies_defeated

# Active entities
var _active_enemies: Array[Node] = []
var _active_platforms: Array[Node] = []

func spawn_all(ld: Dictionary) -> void:
	clear_all()
	_spawn_enemies(ld)
	_spawn_platforms(ld)

func clear_all() -> void:
	for e in _active_enemies:
		if is_instance_valid(e):
			e.queue_free()
	_active_enemies.clear()

	for p in _active_platforms:
		if is_instance_valid(p):
			p.queue_free()
	_active_platforms.clear()

func get_active_enemy_count() -> int:
	var count := 0
	for e in _active_enemies:
		if is_instance_valid(e):
			count += 1
	return count

func get_active_enemies() -> Array[Node]:
	var valid_list: Array[Node] = []
	for e in _active_enemies:
		if is_instance_valid(e):
			valid_list.append(e)
	return valid_list

func _spawn_enemies(ld: Dictionary) -> void:
	var enemy_script: Resource = load("res://Scripts/Enemy.gd")
	if enemy_script == null:
		push_error("[EntitySpawner] Enemy.gd not found!")
		return

	var platforms: Array = ld.get("platforms", []) as Array
	var spawn_x: int = int(ld.get("spawn_x", 2))
	var spawn_y: int = int(ld.get("spawn_y", 16))
	var spawn_world := Vector2(spawn_x * TILE_SIZE, spawn_y * TILE_SIZE)

	# Determine difficulty
	var diff_str := "MODERATE"
	var gm := get_node_or_null("/root/GameManager")
	if gm and "current_difficulty" in gm and str(gm.get("current_difficulty")) != "":
		diff_str = str(gm.get("current_difficulty")).to_upper()
	elif ld.has("stats"):
		var stats: Dictionary = ld.get("stats") as Dictionary
		var diff_score: float = float(stats.get("difficulty_score", 0.4))
		if diff_score < 0.4: diff_str = "BEGINNER"
		elif diff_score < 0.65: diff_str = "MODERATE"
		elif diff_score < 0.85: diff_str = "ADVANCED"
		else: diff_str = "EXPERT"

	# Calculate enemy quota based on the 4 difficulty classifications
	var quota: int = 3
	var purple_ratio: float = 0.33
	if diff_str == "BEGINNER":
		quota = 2
		purple_ratio = 0.0 # Pure green slimes
	elif diff_str == "MODERATE":
		quota = 3
		purple_ratio = 0.34 # 1 purple, 2 green
	elif diff_str == "ADVANCED":
		quota = 4
		purple_ratio = 0.50 # 2 purple, 2 green
	elif diff_str == "EXPERT":
		quota = 5
		purple_ratio = 0.70 # Mostly purple slimes

	# Filter viable platforms: must be at least 200px away from spawn point
	# and not directly above player spawn column
	var viable_platforms: Array = []
	for p_raw in platforms:
		var p: Dictionary = p_raw as Dictionary
		var px: int = int(p.get("x", 0))
		var py: int = int(p.get("y", 0))
		var plen: int = int(p.get("length", 1))
		var is_start: bool = bool(p.get("is_start", false))
		
		if is_start or plen < 2:
			continue
			
		var center_world := Vector2((px + plen / 2.0) * TILE_SIZE, py * TILE_SIZE)
		var dx := absf(center_world.x - spawn_world.x)
		if center_world.distance_to(spawn_world) >= 200.0 and dx >= 80.0:
			viable_platforms.append(p)

	if viable_platforms.is_empty():
		# Fallback: all non-start platforms with distance check
		for p_raw in platforms:
			var p: Dictionary = p_raw as Dictionary
			if not bool(p.get("is_start", false)):
				viable_platforms.append(p)

	# Shuffle viable platforms for variety
	viable_platforms.shuffle()

	var to_spawn: int = mini(quota, viable_platforms.size())
	if to_spawn < 1 and not platforms.is_empty():
		to_spawn = 1
		viable_platforms = [platforms.back()]

	# Build tile lookup for platform edge checking
	var tiles_2d: Array = ld.get("tiles", []) as Array
	var tile_lookup: Dictionary = {}
	for y in range(tiles_2d.size()):
		var row: Array = tiles_2d[y] as Array
		for x in range(row.size()):
			tile_lookup[Vector2i(x, y)] = int(row[x])

	var purple_count: int = int(round(to_spawn * purple_ratio))

	for i in range(to_spawn):
		var plat: Dictionary = viable_platforms[i] as Dictionary
		var px: int = int(plat.get("x", 0))
		var py: int = int(plat.get("y", 0))
		var plen: int = int(plat.get("length", 1))

		var enemy_node := CharacterBody2D.new()
		enemy_node.set_script(enemy_script)
		
		# Position standing cleanly on the platform surface
		var world_x := (px + plen / 2.0) * TILE_SIZE
		var world_y := py * TILE_SIZE - 2.0
		enemy_node.global_position = Vector2(world_x, world_y)

		# Explicit platform left and right corner bounds
		var min_patrol_x: float = px * TILE_SIZE + 10.0
		var max_patrol_x: float = (px + plen) * TILE_SIZE - 10.0

		var etype = 1 if i < purple_count else 0
		enemy_node.call("setup", etype, min_patrol_x, max_patrol_x, tile_lookup)
		enemy_node.connect("enemy_killed", Callable(self, "_on_enemy_killed"))

		get_parent().add_child(enemy_node)
		_active_enemies.append(enemy_node)

	print("[EntitySpawner] Spawned %d platform-bounded enemies (Difficulty: %s)" % [_active_enemies.size(), diff_str])

func _on_enemy_killed(enemy_node: Node) -> void:
	if _active_enemies.has(enemy_node):
		_active_enemies.erase(enemy_node)
	var remaining := get_active_enemy_count()
	emit_signal("enemy_killed", remaining)
	print("[EntitySpawner] Enemy killed! %d remaining" % remaining)

	if remaining <= 0:
		print("[EntitySpawner] All enemies defeated! Emitting all_enemies_defeated")
		emit_signal("all_enemies_defeated")

func _spawn_platforms(ld: Dictionary) -> void:
	var moving: Array = ld.get("moving_platforms", []) as Array
	if moving.is_empty():
		return

	var plat_script: Resource = load("res://Scripts/MovingPlatform.gd")
	if plat_script == null:
		return

	for raw in moving:
		var pd: Dictionary = raw as Dictionary
		var node := Node2D.new()
		node.set_script(plat_script)
		node.global_position = Vector2(
			int(pd.get("x", 0)) * TILE_SIZE + TILE_SIZE / 2.0,
			int(pd.get("y", 0)) * TILE_SIZE + TILE_SIZE / 2.0
		)
		var spr := Sprite2D.new()
		spr.name = "Sprite2D"
		var ptex_path: String = "res://Assets/sprites/platforms.png"
		if ResourceLoader.exists(ptex_path):
			spr.texture = load(ptex_path) as Texture2D
			spr.region_enabled = true
			spr.region_rect = Rect2(16, 0, 32, 9)
		node.add_child(spr)

		get_parent().add_child(node)
		_active_platforms.append(node)
