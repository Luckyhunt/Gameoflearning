## StaticLevel.gd
## ─────────────────────────────────────────────────────────────────
## Handcrafted, 100% playable static level design.
## Guaranteed reachable jumps, safe falls, headroom, and clear start/finish.
##
## Capability Rules Enforced:
##   - Max jump height: 3 tiles
##   - Max horizontal gap: 4 tiles
##   - Min headroom: 2 tiles
##   - Max safe fall: 6 tiles
## ─────────────────────────────────────────────────────────────────

extends RefCounted
class_name StaticLevel

static func get_level_data() -> Dictionary:
	var width: int = 50
	var height: int = 16
	
	# Initialize 2D tiles grid (16 rows x 50 columns) filled with 0 (air)
	var tiles: Array = []
	for y in range(height):
		var row: Array = []
		row.resize(width)
		row.fill(0)
		tiles.append(row)
	
	# Build boundary walls
	for y in range(height):
		tiles[y][0] = 1
		tiles[y][width - 1] = 1

	# --- START PLATFORM (Cols 1..6, Row 13) ---
	for x in range(1, 7):
		tiles[13][x] = 1
		tiles[14][x] = 1
		tiles[15][x] = 1
	tiles[12][2] = 4  # Spawn marker
	
	# --- PLATFORM 1 (Cols 10..14, Row 12) ---
	# Gap from Start: 3 tiles (Cols 7..9 open air). Height: +1 tile
	for x in range(10, 15):
		tiles[12][x] = 2
	tiles[11][12] = 7  # Coin

	# --- PLATFORM 2 (Cols 18..22, Row 10) ---
	# Gap from Plat 1: 3 tiles (Cols 15..17 open air). Height: +2 tiles
	for x in range(18, 23):
		tiles[10][x] = 2
	tiles[9][20] = 6   # Checkpoint

	# --- PLATFORM 3 (Cols 27..31, Row 11) ---
	# Gap from Plat 2: 4 tiles (Cols 23..26 open air). Height: -1 tile (drop)
	for x in range(27, 32):
		tiles[11][x] = 2
	tiles[10][29] = 7  # Coin

	# --- HIGHER PLATFORM (Cols 35..39, Row 8) ---
	# Gap from Plat 3: 3 tiles (Cols 32..34 open air). Height: +3 tiles (Max jump height!)
	for x in range(35, 40):
		tiles[8][x] = 2
	tiles[7][37] = 7   # Coin

	# --- FINAL PLATFORM & EXIT (Cols 43..48, Row 10) ---
	# Gap from Higher Plat: 3 tiles (Cols 40..42 open air). Height: -2 tiles (Safe fall)
	for x in range(43, 49):
		tiles[10][x] = 1
		tiles[11][x] = 1
		tiles[12][x] = 1
		tiles[13][x] = 1
		tiles[14][x] = 1
		tiles[15][x] = 1
	tiles[9][46] = 5   # Exit marker

	return {
		"width": width,
		"height": height,
		"spawn_x": 2,
		"spawn_y": 12,
		"exit_x": 46,
		"exit_y": 9,
		"tiles": tiles,
		"stats": {
			"seed": 42,
			"jump_count": 5,
			"difficulty_score": 0.20,
			"valid": true,
			"generator": "HandcraftedStaticLevel"
		}
	}
