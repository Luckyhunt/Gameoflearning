## LevelController.gd
## ─────────────────────────────────────────────────────────────────
## Owns one level's lifecycle: generation, rendering, entities,
## player spawn, and gameplay overlap checks (exit, coins,
## checkpoints, hazards).
##
## Main.gd wires session/UI modules and forwards high-level requests.
## ─────────────────────────────────────────────────────────────────

extends Node

# Reference to PlayerEnums for tile type constants
const PlayerEnums = preload("res://Scripts/PlayerEnums.gd")

signal player_died(death_type: String)
signal level_complete(stats: Dictionary)
signal checkpoint_reached
signal level_ready(level_num: int, width: int, height: int)

const TILE_SIZE: int = 32

var bridge: EngineBridge = null
var layer: TileMapLayer = null

var player_node: Node = null

var _renderer: Node = null   # LevelRenderer
var _spawner: Node = null    # EntitySpawner
var _game_mgr: Node = null   # GameManager reference for pause/game-over gating
var _visual_manager: Node = null  # VisualManager for theme and particles

# ─────────────────────────────────────────────────────────────────
func setup(p_bridge: EngineBridge, p_layer: TileMapLayer, p_game_mgr: Node) -> void:
	bridge = p_bridge
	layer = p_layer
	_game_mgr = p_game_mgr

	_renderer = _load_module("res://Scripts/LevelRenderer.gd")
	_spawner = _load_module("res://Scripts/EntitySpawner.gd")
	(_renderer as Node).set("layer", layer)

	layer.tile_set = (_renderer as Node).call("build_tileset") as TileSet
	
	# Initialize visual manager
	_visual_manager = get_node_or_null("/root/VisualManager")
	if not _visual_manager:
		_visual_manager = preload("res://Scenes/VisualManager.tscn").instantiate()
		_visual_manager.name = "VisualManager"
		get_tree().root.call_deferred("add_child", _visual_manager)
	
	# Connect visual manager signals
	_visual_manager.connect("visual_ready", _on_visual_ready)

# ─────────────────────────────────────────────────────────────────
func generate_level(level_num: int) -> bool:
	(_spawner as Node).call("clear_all")

	var ld: Dictionary
	if level_num <= 1:
		var static_script: Resource = load("res://Scripts/StaticLevel.gd")
		if static_script != null:
			ld = (static_script as GDScript).call("get_level_data") as Dictionary
		else:
			ld = bridge.generate_guaranteed_level()
	else:
		ld = bridge.generate_adaptive_level()

	if int(ld.get("width", 0)) <= 0:
		push_error("[LevelController] Level generation returned empty level!")
		return false

	(_renderer as Node).call("render", ld)
	(_spawner as Node).call("spawn_all", ld)
	
	if (_spawner as Node).has_signal("enemy_killed"):
		if not (_spawner as Node).is_connected("enemy_killed", Callable(self, "_on_spawner_enemy_killed")):
			(_spawner as Node).connect("enemy_killed", Callable(self, "_on_spawner_enemy_killed"))

	# Select theme based on level and difficulty
	if _visual_manager:
		var difficulty: int = bridge.get_difficulty_level()
		_visual_manager.call("select_theme_for_level", level_num, difficulty)

	var spawn_pos: Vector2 = _renderer.get("spawn_world_pos") as Vector2
	_spawn_or_reposition_player(spawn_pos)

	emit_signal(
		"level_ready",
		level_num,
		int(ld.get("width", 0)),
		int(ld.get("height", 0))
	)

	print("[LevelController] Level %d ready — %dx%d" % [
		level_num,
		int(ld.get("width", 0)),
		int(ld.get("height", 0))
	])
	return true

func restart_level(_lives: int = 3) -> void:
	if player_node == null:
		return
	var spawn_pos: Vector2 = _renderer.get("spawn_world_pos") as Vector2
	player_node.call("reset_level_stats")
	player_node.set("global_position", spawn_pos)
	player_node.set("velocity", Vector2.ZERO)

# ─────────────────────────────────────────────────────────────────
func process_frame() -> void:
	if _game_mgr == null or player_node == null:
		return
	var is_p = _game_mgr.get("is_paused")
	var is_go = _game_mgr.get("is_game_over")
	if (is_p != null and is_p == true) or (is_go != null and is_go == true):
		return

	_check_exit_overlap()
	_check_coin_overlaps()
	_check_checkpoint_overlaps()

func get_hud_snapshot() -> Dictionary:
	if player_node == null or _game_mgr == null:
		return {}
	return {
		"level_number": int(_game_mgr.get("level_number")),
		"elapsed_time": float(player_node.call("get_timer")),
		"coins_collected": int(player_node.call("get_coins")),
	}

# ─────────────────────────────────────────────────────────────────
func _spawn_or_reposition_player(spawn_pos: Vector2, lives: int = 3) -> void:
	if player_node == null:
		var pscene_res: Resource = load("res://Scenes/Player.tscn")
		if pscene_res:
			player_node = (pscene_res as PackedScene).instantiate()
		else:
			var pscript_res: Resource = load("res://Scripts/Player.gd")
			if pscript_res == null:
				push_error("[LevelController] Player script not found")
				return
			var cb := CharacterBody2D.new()
			cb.set_script(pscript_res)
			player_node = cb

		add_child(player_node)
		player_node.add_to_group("player")

		player_node.connect("player_died", _on_player_died)
		player_node.connect("level_complete", _on_level_complete)
		player_node.connect("request_restart", Callable(_game_mgr, "restart_level"))
		player_node.connect("request_pause", Callable(_game_mgr, "toggle_pause"))
		player_node.connect("coin_collected", _on_coin_collected)
		player_node.connect("checkpoint_reached", _on_checkpoint_reached)

	player_node.call("setup", spawn_pos, lives)
	player_node.call("set_tile_lookup", (_renderer as Node).call("build_tile_lookup"))
	
	# Set camera bounds based on actual level dimensions
	var level_width: int = int(_renderer.get("level_width"))
	var level_height: int = int(_renderer.get("level_height"))
	if level_width <= 0: level_width = 45
	if level_height <= 0: level_height = 16
	var bounds := Rect2(0, 0, level_width * TILE_SIZE, level_height * TILE_SIZE)
	player_node.call("set_camera_bounds", bounds)

# ─────────────────────────────────────────────────────────────────
func _tile_of_player() -> Vector2i:
	var pos: Vector2 = player_node.get("global_position") as Vector2
	return Vector2i(int(pos.x / TILE_SIZE), int(pos.y / TILE_SIZE))

func _check_exit_overlap() -> void:
	var exit_pos: Vector2i = _renderer.get("exit_tile_pos") as Vector2i
	if _tile_of_player() == exit_pos:
		player_node.call("on_exit_reached")

func _check_coin_overlaps() -> void:
	var pt := _tile_of_player()
	var coins: Array = _renderer.get("live_coin_tiles") as Array
	if coins.find(pt) != -1:
		(_renderer as Node).call("collect_coin", pt)
		player_node.call("on_coin_collected", pt)
		# Spawn coin particles
		if _visual_manager:
			var player_pos := player_node.get("global_position") as Vector2
			_visual_manager.call("spawn_coin", player_pos)

func _check_checkpoint_overlaps() -> void:
	var pt := _tile_of_player()
	var cps: Dictionary = _renderer.get("live_checkpoint_tiles") as Dictionary
	if cps.has(pt):
		var wp: Vector2 = cps[pt] as Vector2
		player_node.call("on_checkpoint_hit", pt, wp)
		(_renderer as Node).call("consume_checkpoint", pt)
		# Spawn checkpoint particles
		if _visual_manager:
			var player_pos := player_node.get("global_position") as Vector2
			_visual_manager.call("spawn_checkpoint", player_pos)
		emit_signal("checkpoint_reached")

func _check_hazard_overlaps() -> void:
	pass

# ─────────────────────────────────────────────────────────────────
func _on_spawner_enemy_killed(remaining_count: int) -> void:
	print("[LevelController] Enemy killed. Remaining: %d" % remaining_count)
	if remaining_count <= 0 and player_node != null:
		print("[LevelController] ALL ENEMIES CLEARED! Level Complete.")
		player_node.call("on_all_enemies_cleared")

func _on_player_died(death_type: String) -> void:
	emit_signal("player_died", death_type)

func _on_level_complete(stats: Dictionary) -> void:
	emit_signal("level_complete", stats)

func _on_visual_ready() -> void:
	# Visual system is ready, can now apply themes
	pass

func _on_coin_collected(_tile_pos: Vector2i) -> void:
	pass

func _on_checkpoint_reached(_tile_pos: Vector2i) -> void:
	pass

# ─────────────────────────────────────────────────────────────────
func _load_module(path: String) -> Node:
	var res: Resource = load(path)
	if res == null:
		push_error("[LevelController] Module not found: " + path)
		return Node.new()
	var node := Node.new()
	node.set_script(res)
	node.name = path.get_file().get_basename()
	add_child(node)
	return node
