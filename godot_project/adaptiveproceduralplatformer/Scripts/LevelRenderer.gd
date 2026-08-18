## LevelRenderer.gd
## ─────────────────────────────────────────────────────────────────
## Owns the TileMapLayer and all runtime tracking of interactive
## tile positions (coins, checkpoints, hazards).
##
## Called by Main.gd after each generate_adaptive_level().
## ─────────────────────────────────────────────────────────────────

extends Node

# ── Public tile-size constant (shared with other modules) ─────────
const TILE_SIZE: int = 32

# ── Tile atlas: TileType int → atlas coords (4×4 grid) ────────────
const ATLAS: Dictionary = {
	 1: Vector2i(1, 0),   # Solid
	 2: Vector2i(2, 0),   # Platform
	 3: Vector2i(3, 0),   # Hazard
	 4: Vector2i(0, 1),   # Spawn
	 5: Vector2i(1, 1),   # Exit
	 6: Vector2i(2, 1),   # Checkpoint
	 7: Vector2i(3, 1),   # Coin
	 8: Vector2i(0, 2),   # Powerup
	 9: Vector2i(1, 2),   # Secret
	10: Vector2i(2, 2),   # MovingPlatform
	11: Vector2i(3, 2),   # FallingPlatform
	12: Vector2i(0, 3),   # BouncePad
	13: Vector2i(1, 3),   # IcePlatform
	14: Vector2i(2, 3),   # OneWayPlatform
}

# ── References ────────────────────────────────────────────────────
var layer: TileMapLayer = null  # set by Main.gd

# ── Level dimensions ──────────────────────────────────────────────
var level_width:  int = 22
var level_height: int = 10

# ── Tile data (read by Main.gd for collision checks) ──────────────
var tile_grid: Array = []               # 2D Array[Array[int]]
var live_coin_tiles: Array = []         # Array[Vector2i]
var live_checkpoint_tiles: Dictionary = {} # Vector2i → Vector2 (world pos)
var exit_tile_pos: Vector2i   = Vector2i(-1, -1)
var exit_world_pos: Vector2   = Vector2.ZERO
var spawn_world_pos: Vector2  = Vector2.ZERO

# ─────────────────────────────────────────────────────────────────
func render(ld: Dictionary) -> void:
	assert(layer != null, "LevelRenderer: layer not set")
	layer.clear()

	level_width  = int(ld.get("width",  22))
	level_height = int(ld.get("height", 10))
	var tiles_2d: Array = ld.get("tiles", []) as Array

	tile_grid = tiles_2d
	live_coin_tiles.clear()
	live_checkpoint_tiles.clear()

	for y in range(level_height):
		for x in range(level_width):
			var row: Array = tile_grid[y] as Array
			var tt: int = int(row[x]) if x < row.size() else 0
			if tt == 0:
				continue

			if ATLAS.has(tt):
				layer.set_cell(Vector2i(x, y), 0, ATLAS[tt] as Vector2i)

			# Track interactive tiles
			match tt:
				7:  # Coin
					live_coin_tiles.append(Vector2i(x, y))
				6:  # Checkpoint
					var wp := Vector2(
						x * TILE_SIZE + TILE_SIZE / 2.0,
						y * TILE_SIZE + TILE_SIZE / 2.0 - 4.0
					)
					live_checkpoint_tiles[Vector2i(x, y)] = wp

	# Spawn / Exit world positions
	var sx: int = int(ld.get("spawn_x", 1))
	var sy: int = int(ld.get("spawn_y", 1))
	spawn_world_pos = Vector2(sx * TILE_SIZE + TILE_SIZE / 2.0, sy * TILE_SIZE + TILE_SIZE / 2.0)

	var ex: int = int(ld.get("exit_x", 0))
	var ey: int = int(ld.get("exit_y", 0))
	exit_tile_pos  = Vector2i(ex, ey)
	exit_world_pos = Vector2(ex * TILE_SIZE + TILE_SIZE / 2.0,
	                          ey * TILE_SIZE + TILE_SIZE / 2.0)

	_print_stats(ld)

# ─────────────────────────────────────────────────────────────────
func collect_coin(tile_pos: Vector2i) -> void:
	var idx: int = live_coin_tiles.find(tile_pos)
	if idx != -1:
		live_coin_tiles.remove_at(idx)
		layer.erase_cell(tile_pos)

func consume_checkpoint(tile_pos: Vector2i) -> void:
	live_checkpoint_tiles.erase(tile_pos)

# ─────────────────────────────────────────────────────────────────
func get_tile_type(tile_pos: Vector2i) -> int:
	if tile_pos.y < 0 or tile_pos.y >= tile_grid.size():
		return 0
	var row: Array = tile_grid[tile_pos.y] as Array
	if tile_pos.x < 0 or tile_pos.x >= row.size():
		return 0
	return int(row[tile_pos.x])

func build_tile_lookup() -> Dictionary:
	var lookup: Dictionary = {}
	for y in range(tile_grid.size()):
		var row: Array = tile_grid[y] as Array
		for x in range(row.size()):
			lookup[Vector2i(x, y)] = int(row[x])
	return lookup

# ─────────────────────────────────────────────────────────────────
func build_tileset() -> TileSet:
	var ts := TileSet.new()
	ts.tile_size = Vector2i(TILE_SIZE, TILE_SIZE)

	var src := TileSetAtlasSource.new()
	src.texture = _create_debug_atlas()
	src.texture_region_size = Vector2i(TILE_SIZE, TILE_SIZE)
	ts.add_source(src, 0)

	for tile_type: int in ATLAS.keys():
		var coords: Vector2i = ATLAS[tile_type] as Vector2i
		if not src.has_tile(coords):
			src.create_tile(coords)

	var solid_tiles: Array = [1, 2, 3, 10, 11, 12, 13, 14]
	ts.add_physics_layer(0)

	var half_size: float = TILE_SIZE / 2.0
	for tile_type: int in solid_tiles:
		if not ATLAS.has(tile_type):
			continue
		var coords: Vector2i = ATLAS[tile_type] as Vector2i
		var td: TileData = src.get_tile_data(coords, 0)
		if td == null:
			continue
		var poly := PackedVector2Array([
			Vector2(-half_size, -half_size),
			Vector2(half_size, -half_size),
			Vector2(half_size, half_size),
			Vector2(-half_size, half_size),
		])
		td.add_collision_polygon(0)
		td.set_collision_polygon_points(0, 0, poly)

	return ts

# ─────────────────────────────────────────────────────────────────
func _print_stats(ld: Dictionary) -> void:
	var stats: Dictionary = ld.get("stats", {}) as Dictionary
	if stats.is_empty():
		return
	var gen_name: String = str(stats.get("generator", "PLE"))
	print("[LevelRenderer] Seed:%d Jumps:%d DiffScore:%.2f Valid:%s Gen:%s" % [
		int(stats.get("seed", 0)),
		int(stats.get("jump_count", 0)),
		float(stats.get("difficulty_score", 0.0)),
		"YES" if bool(stats.get("valid", false)) else "NO",
		gen_name
	])

# ─────────────────────────────────────────────────────────────────
func _create_debug_atlas() -> ImageTexture:
	var GRID: int = 4
	var img := Image.create(TILE_SIZE * GRID, TILE_SIZE * GRID, false, Image.FORMAT_RGBA8)

	var tile_colors: Dictionary = {
		Vector2i(0, 0): Color(0.0,  0.0,  0.0,  0.0),
		Vector2i(1, 0): Color(0.35, 0.35, 0.35, 1.0),
		Vector2i(2, 0): Color(0.55, 0.38, 0.18, 1.0),
		Vector2i(3, 0): Color(0.90, 0.15, 0.10, 1.0),
		Vector2i(0, 1): Color(0.10, 0.80, 0.20, 1.0),
		Vector2i(1, 1): Color(0.90, 0.70, 0.10, 1.0),
		Vector2i(2, 1): Color(0.20, 0.60, 1.00, 1.0),
		Vector2i(3, 1): Color(1.00, 0.90, 0.10, 1.0),
		Vector2i(0, 2): Color(0.80, 0.20, 0.90, 1.0),
		Vector2i(1, 2): Color(0.50, 0.50, 0.50, 1.0),
		Vector2i(2, 2): Color(0.30, 0.50, 0.90, 1.0),
		Vector2i(3, 2): Color(0.80, 0.45, 0.10, 1.0),
		Vector2i(0, 3): Color(0.40, 0.90, 0.30, 1.0),
		Vector2i(1, 3): Color(0.65, 0.85, 1.00, 1.0),
		Vector2i(2, 3): Color(0.70, 0.70, 0.50, 1.0),
	}

	for coords: Vector2i in tile_colors.keys():
		var col: Color = tile_colors[coords] as Color
		var px: int = coords.x * TILE_SIZE
		var py: int = coords.y * TILE_SIZE
		for dy in range(TILE_SIZE):
			for dx in range(TILE_SIZE):
				var border: bool = (dx == 0 or dy == 0 or dx == TILE_SIZE - 1 or dy == TILE_SIZE - 1)
				var draw_col: Color = col.darkened(0.35) if border else col
				img.set_pixel(px + dx, py + dy, draw_col)

	return ImageTexture.create_from_image(img)

func get_level_dimensions() -> Array:
	return [level_width, level_height]
