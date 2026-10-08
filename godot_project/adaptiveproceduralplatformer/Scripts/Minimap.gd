## Minimap.gd  —  Retro Radar Minimap
## ============================================================
## Displays the complete container layout in miniature:
##   - Bounded container walls and platform geometry
##   - Ancient Door goal marker (golden/cyan glow)
##   - Live player position blip (pulsing white/green dot)
##   - Camera viewport field-of-view rectangle
## ============================================================

extends Control
class_name Minimap

var level_width: int = 38
var level_height: int = 21
var tile_size: int = 32

var _tiles_grid: Array = []
var _exit_tile: Vector2i = Vector2i.ZERO
var _spawn_tile: Vector2i = Vector2i.ZERO

var _player_ref: Node2D = null
var _camera_ref: CameraController = null

var _pulse_timer: float = 0.0

func _ready() -> void:
	custom_minimum_size = Vector2(180, 88)
	texture_filter = CanvasItem.TEXTURE_FILTER_NEAREST

func setup_level(ld: Dictionary, player: Node2D, camera: CameraController = null) -> void:
	level_width = int(ld.get("width", 38))
	level_height = int(ld.get("height", 21))
	if level_width <= 0: level_width = 38
	if level_height <= 0: level_height = 21
	
	_tiles_grid = ld.get("tiles", [])
	_spawn_tile = Vector2i(int(ld.get("spawn_x", 2)), int(ld.get("spawn_y", 15)))
	_exit_tile = Vector2i(int(ld.get("exit_x", level_width - 4)), int(ld.get("exit_y", 10)))
	
	_player_ref = player
	_camera_ref = camera
	queue_redraw()

func _process(delta: float) -> void:
	_pulse_timer += delta * 4.0
	queue_redraw()

func _draw() -> void:
	var w := size.x
	var h := size.y
	
	# Background
	draw_rect(Rect2(0, 0, w, h), Color(0.04, 0.06, 0.09, 0.92), true)
	# Pixel Border
	draw_rect(Rect2(0, 0, w, h), Color(0.30, 0.42, 0.58, 0.95), false, 2.0)
	
	if level_width <= 0 or level_height <= 0:
		return
		
	var pad := 6.0
	var draw_w := w - pad * 2.0
	var draw_h := h - pad * 2.0
	
	var scale_x := draw_w / float(level_width)
	var scale_y := draw_h / float(level_height)
	var scale := minf(scale_x, scale_y)
	
	var offset_x := pad + (draw_w - level_width * scale) / 2.0
	var offset_y := pad + (draw_h - level_height * scale) / 2.0
	
	# Draw Level Geometry
	var solid_color := Color(0.22, 0.28, 0.38, 0.9)
	var plat_color  := Color(0.85, 0.75, 0.40, 0.95)
	
	if not _tiles_grid.is_empty():
		for y in range(min(_tiles_grid.size(), level_height)):
			var row = _tiles_grid[y]
			if row is Array:
				for x in range(min(row.size(), level_width)):
					var t = int(row[x])
					if t == 1: # Solid Wall / Floor
						var rx := offset_x + x * scale
						var ry := offset_y + y * scale
						draw_rect(Rect2(rx, ry, maxf(scale, 1.0), maxf(scale, 1.0)), solid_color, true)
					elif t == 2: # Platform
						var rx := offset_x + x * scale
						var ry := offset_y + y * scale
						draw_rect(Rect2(rx, ry, maxf(scale, 1.0), maxf(scale, 1.0)), plat_color, true)
	
	# Draw Enemy Radar Blips
	var enemies := get_tree().get_nodes_in_group("enemies")
	for e in enemies:
		if is_instance_valid(e) and e is Node2D:
			var e_pos: Vector2 = e.global_position
			var ex_tile := e_pos.x / float(tile_size)
			var ey_tile := e_pos.y / float(tile_size)
			var ex := offset_x + ex_tile * scale
			var ey := offset_y + ey_tile * scale
			var pulse_glow := (sin(_pulse_timer * 1.5) + 1.0) * 0.5
			var enemy_color := Color(1.0, 0.25, 0.35, 0.8 + pulse_glow * 0.2)
			draw_circle(Vector2(ex, ey), maxf(scale * 0.85, 3.2), enemy_color)
			draw_circle(Vector2(ex, ey), maxf(scale * 0.35, 1.5), Color(1.0, 0.9, 0.9))
	
	# Draw Player Blip
	if _player_ref and is_instance_valid(_player_ref):
		var p_world: Vector2 = _player_ref.global_position
		var px_tile := p_world.x / float(tile_size)
		var py_tile := p_world.y / float(tile_size)
		
		var px := offset_x + px_tile * scale
		var py := offset_y + py_tile * scale
		
		var p_color := Color(0.2, 1.0, 0.4, 0.85 + pulse_glow * 0.15)
		draw_circle(Vector2(px, py), maxf(scale * 1.0, 4.0), p_color)
		draw_circle(Vector2(px, py), maxf(scale * 0.5, 2.0), Color.WHITE)
		
	# Draw Camera Viewport Frustum Box
	if _camera_ref and is_instance_valid(_camera_ref):
		var cam_rect := _camera_ref.get_visible_world_rect()
		var cx_tile := cam_rect.position.x / float(tile_size)
		var cy_tile := cam_rect.position.y / float(tile_size)
		var cw_tile := cam_rect.size.x / float(tile_size)
		var ch_tile := cam_rect.size.y / float(tile_size)
		
		var vx := offset_x + cx_tile * scale
		var vy := offset_y + cy_tile * scale
		var vw := cw_tile * scale
		var vh := ch_tile * scale
		
		draw_rect(Rect2(vx, vy, vw, vh), Color(1.0, 1.0, 1.0, 0.35), false, 1.0)
