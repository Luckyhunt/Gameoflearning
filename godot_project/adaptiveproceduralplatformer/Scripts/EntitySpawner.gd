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

func _spawn_enemies(_ld: Dictionary) -> void:
	# Enemies removed per design — pure platformer level traversal
	pass

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

	print("[EntitySpawner] Spawned %d moving platforms" % _active_platforms.size())
