## Main.gd
## ─────────────────────────────────────────────────────────────────
## Scene bootstrap — wires session/UI modules to LevelController.
## ─────────────────────────────────────────────────────────────────

extends Node

var bridge: EngineBridge = null
var layer: TileMapLayer = null
var hud_node: Node = null

var _level_controller: Node = null   # LevelController
var _game_mgr: Node = null           # GameManager
var _analytics: Node = null          # Analytics

var _pause_overlay: CanvasLayer = null
var _fade_rect: ColorRect = null

# ─────────────────────────────────────────────────────────────────
func _ready() -> void:
	print("=" .repeat(44))
	print("[APLG] Adaptive Platform Generation Framework")
	print("=".repeat(44))

	bridge = $EngineBridge as EngineBridge
	layer = $TileMapLayer as TileMapLayer

	if bridge == null:
		push_error("[Main] EngineBridge node not found!")
		return
	if layer == null:
		push_error("[Main] TileMapLayer node not found!")
		return

	_level_controller = _load_module("res://Scripts/LevelController.gd")
	_game_mgr = _load_module("res://Scripts/GameManager.gd")
	_analytics = _load_module("res://Scripts/Analytics.gd")

	var bg_script: Resource = load("res://Scripts/BackgroundParallax.gd")
	if bg_script:
		var bg_parallax := ParallaxBackground.new()
		bg_parallax.set_script(bg_script)
		add_child(bg_parallax)
		move_child(bg_parallax, 0)

	(_analytics as Node).set("bridge", bridge)

	var hud_res: Resource = load("res://Scenes/HUD.tscn")
	if hud_res:
		hud_node = (hud_res as PackedScene).instantiate()
		add_child(hud_node)
		hud_node.set("bridge", bridge)

	_fade_rect = ColorRect.new()
	_fade_rect.color = Color(0, 0, 0, 0)
	_fade_rect.z_index = 100
	_fade_rect.mouse_filter = Control.MOUSE_FILTER_IGNORE
	_fade_rect.size = get_viewport().get_visible_rect().size
	var fade_canvas := CanvasLayer.new()
	fade_canvas.layer = 99
	fade_canvas.add_child(_fade_rect)
	add_child(fade_canvas)

	_game_mgr.set("hud_node", hud_node)
	_game_mgr.set("fade_rect", _fade_rect)
	_pause_overlay = (_game_mgr as Node).call("build_pause_overlay") as CanvasLayer
	add_child(_pause_overlay)

	(_game_mgr as Node).request_next_level.connect(_start_new_level)
	(_game_mgr as Node).request_restart.connect(_restart_level)
	(_game_mgr as Node).request_pause_toggle.connect(_on_pause_toggled)

	(_level_controller as Node).call("setup", bridge, layer, _game_mgr)
	(_level_controller as Node).player_died.connect(_on_player_died)
	(_level_controller as Node).level_complete.connect(_on_level_complete)
	(_level_controller as Node).checkpoint_reached.connect(_on_checkpoint_reached)
	(_level_controller as Node).level_ready.connect(_on_level_ready)

	_start_new_level()

func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventKey and event.pressed and not event.echo:
		if event.keycode == KEY_ESCAPE:
			if _game_mgr:
				(_game_mgr as Node).call("toggle_pause")
		elif event.keycode == KEY_R:
			if _game_mgr:
				(_game_mgr as Node).call("restart_level")
		elif event.keycode == KEY_N:
			if _game_mgr:
				var is_p = _game_mgr.get("is_paused")
				if is_p != null and is_p == true:
					(_game_mgr as Node).call("toggle_pause")
				var lvl = int(_game_mgr.get("level_number"))
				_game_mgr.set("level_number", lvl + 1)
				_start_new_level()

# ─────────────────────────────────────────────────────────────────
func _process(_delta: float) -> void:
	(_level_controller as Node).call("process_frame")
	_update_hud()

# ─────────────────────────────────────────────────────────────────
func _start_new_level() -> void:
	var level_num: int = int(_game_mgr.get("level_number"))
	if not (_level_controller as Node).call("generate_level", level_num):
		return

func _restart_level() -> void:
	(_level_controller as Node).call("restart_level")

func _on_level_ready(_level_num: int, _width: int, _height: int, _lives: int = 3) -> void:
	(_game_mgr as Node).call("reset_for_new_level")

func _update_hud() -> void:
	if hud_node == null:
		return
	var snapshot: Dictionary = (_level_controller as Node).call("get_hud_snapshot") as Dictionary
	if snapshot.is_empty():
		return
	hud_node.set("level_number", snapshot.get("level_number", 1))
	hud_node.set("elapsed_time", snapshot.get("elapsed_time", 0.0))
	hud_node.set("coins_collected", snapshot.get("coins_collected", 0))

# ─────────────────────────────────────────────────────────────────
func _on_player_died(_death_type: String) -> void:
	if hud_node and hud_node.has_method("flash_death"):
		hud_node.flash_death()

func _on_level_complete(stats: Dictionary) -> void:
	var lvl: int = int(_game_mgr.get("level_number")) if _game_mgr else 1
	(_analytics as Node).call("report_level_complete", stats, lvl)
	(_game_mgr as Node).call("on_level_complete")

func _on_checkpoint_reached() -> void:
	if hud_node and hud_node.has_method("flash_checkpoint"):
		hud_node.flash_checkpoint()

func _on_pause_toggled() -> void:
	if _pause_overlay:
		_pause_overlay.visible = bool(_game_mgr.get("is_paused"))

# ─────────────────────────────────────────────────────────────────
func _load_module(path: String) -> Node:
	var res: Resource = load(path)
	if res == null:
		push_error("[Main] Module not found: " + path)
		return Node.new()
	var node := Node.new()
	node.set_script(res)
	node.name = path.get_file().get_basename()
	add_child(node)
	return node
