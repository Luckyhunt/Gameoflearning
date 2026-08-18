## EntitySpawner.gd
## ─────────────────────────────────────────────────────────────────
## Spawns and despawns enemy and platform entities from a
## level dictionary produced by generate_adaptive_level().
##
## Main.gd calls spawn_all(ld) after rendering the tile layer,
## and clear_all() before generating the next level.
## ─────────────────────────────────────────────────────────────────

extends Node

const TILE_SIZE: int = 32

# ── Active entities (kept for cleanup) ───────────────────────────
var _active_enemies: Array = []    # Array[Node]
var _active_platforms: Array = []  # Array[Node]

# ─────────────────────────────────────────────────────────────────
func spawn_all(ld: Dictionary) -> void:
	_spawn_enemies(ld)
	_spawn_platforms(ld)

func clear_all() -> void:
	for e: Node in _active_enemies:
		if is_instance_valid(e):
			e.queue_free()
	_active_enemies.clear()

	for p: Node in _active_platforms:
		if is_instance_valid(p):
			p.queue_free()
	_active_platforms.clear()

signal enemy_killed(remaining_count: int)

func get_active_enemy_count() -> int:
	var count := 0
	for e in _active_enemies:
		if is_instance_valid(e):
			count += 1
	return count

func _spawn_enemies(ld: Dictionary) -> void:
	var enemies_raw: Array = ld.get("enemies", []) as Array
	if enemies_raw.is_empty():
		print("[EntitySpawner] No enemies in level dictionary")
		return

	var enemy_script: Resource = load("res://Scripts/Enemy.gd")
	if enemy_script == null:
		push_error("[EntitySpawner] Enemy.gd not found")
		return

	var level_w := int(ld.get("width", 45))
	var level_h := int(ld.get("height", 16))
	var bounds := Rect2(0, 0, level_w * TILE_SIZE, level_h * TILE_SIZE)

	var tiles_raw: Array = ld.get("tiles", []) as Array
	var tile_lookup := {}
	for y in range(tiles_raw.size()):
		var row: Array = tiles_raw[y] as Array
		for x in range(row.size()):
			tile_lookup[Vector2i(x, y)] = int(row[x])

	var player_node = get_tree().get_first_node_in_group("player")
	var player_pos: Vector2 = player_node.global_position if player_node != null else Vector2.ZERO

	for raw in enemies_raw:
		var ed: Dictionary = raw as Dictionary
		var epos := Vector2(
			int(ed.get("x", 0)) * TILE_SIZE + TILE_SIZE / 2.0,
			int(ed.get("y", 0)) * TILE_SIZE + TILE_SIZE / 2.0
		)

		if player_pos != Vector2.ZERO and epos.distance_to(player_pos) < 180.0:
			continue

		var node := Node2D.new()
		node.set_script(enemy_script)
		node.set("speed", float(ed.get("speed", 60.0)))
		node.set("enemy_type", int(ed.get("type", 0)))
		node.global_position = epos
		node.add_to_group("enemies")

		if node.has_method("set_tile_lookup"):
			node.call("set_tile_lookup", tile_lookup, bounds)

		get_parent().add_child(node)
		_active_enemies.append(node)

		node.connect("enemy_killed", Callable(self, "_on_enemy_killed").bind(node))

	print("[EntitySpawner] Spawned %d enemies" % _active_enemies.size())

func _on_enemy_killed(enemy_node: Node) -> void:
	if _active_enemies.has(enemy_node):
		_active_enemies.erase(enemy_node)
	var remaining := get_active_enemy_count()
	emit_signal("enemy_killed", remaining)

# ─────────────────────────────────────────────────────────────────
# Moving Platforms
# ─────────────────────────────────────────────────────────────────

func _spawn_platforms(ld: Dictionary) -> void:
	var moving: Array = ld.get("moving_platforms", []) as Array
	if moving.is_empty():
		return

	var plat_script: Resource = load("res://Scripts/MovingPlatform.gd")
	if plat_script == null:
		push_error("[EntitySpawner] MovingPlatform.gd not found")
		return

	for raw in moving:
		var pd: Dictionary = raw as Dictionary
		var node := Node2D.new()
		node.set_script(plat_script)
		node.global_position = Vector2(
			int(pd.get("x", 0)) * TILE_SIZE + TILE_SIZE / 2.0,
			int(pd.get("y", 0)) * TILE_SIZE + TILE_SIZE / 2.0
		)
		get_parent().add_child(node)
		_active_platforms.append(node)

	print("[EntitySpawner] Spawned %d moving platforms" % _active_platforms.size())
