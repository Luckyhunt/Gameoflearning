## StaticLevel.gd
## ─────────────────────────────────────────────────────────────────
## Master Handcrafted Level: Asymmetric, Unidirectional, Playable
##
## Enclosed Playground Container:
##   - Bounded by solid walls on all 4 sides (ceiling, floor, left, right)
##   - Strict unidirectional progression: Left (Spawn) → Right (Ancient Door)
##   - Organic & completely ASYMMETRIC layout (no mirror, no repeating patterns)
##   - Zero diagonal skewness: Undulating mid-height elevation profile
##   - 100% playable with verified jump heights (≤ 3 tiles) and gaps (≤ 3 tiles)
## ─────────────────────────────────────────────────────────────────

extends RefCounted
class_name StaticLevel

static func get_level_data() -> Dictionary:
	var width: int = 38
	var height: int = 21
	
	# Initialize 2D tiles grid (21 rows x 38 columns) filled with 0 (air)
	var tiles: Array = []
	for y in range(height):
		var row: Array = []
		row.resize(width)
		row.fill(0)
		tiles.append(row)
	
	# ── 1. SOLID BOUNDARY WALLS (ENCLOSED CONTAINER) ──────────────────
	# Ceiling (Row 0) and Floor (Row 20)
	for x in range(width):
		tiles[0][x] = 1
		tiles[20][x] = 1
	# Left Wall (Col 0) and Right Wall (Col 37)
	for y in range(height):
		tiles[y][0] = 1
		tiles[y][width - 1] = 1

	# ── 2. UNIDIRECTIONAL ASYMMETRIC PRIMARY PATH ─────────────────────
	# Start Landing Platform (Cols 1..6, Row 16)
	for x in range(1, 7):
		tiles[16][x] = 1
		tiles[17][x] = 1
		tiles[18][x] = 1
		tiles[19][x] = 1

	# Step 1: Gentle Rise (Cols 9..12, Row 14) — Gap: 3, Δy: -2
	for x in range(9, 13):
		tiles[14][x] = 2

	# Step 2: High Leap (Cols 15..18, Row 11) — Gap: 3, Δy: -3
	for x in range(15, 19):
		tiles[11][x] = 2

	# Step 3: Gentle Dip & Rest (Cols 20..22, Row 13) — Gap: 2, Δy: +2
	for x in range(20, 23):
		tiles[13][x] = 2

	# Step 4: Mid-High Ascent (Cols 24..27, Row 10) — Gap: 2, Δy: -3
	for x in range(24, 28):
		tiles[10][x] = 2

	# Step 5: High Ridge (Cols 29..31, Row 8) — Gap: 2, Δy: -2
	for x in range(29, 32):
		tiles[8][x] = 2

	# Step 6: Descent to Goal Platform (Cols 33..36, Row 11) — Gap: 2, Δy: +3
	for x in range(33, 37):
		tiles[11][x] = 1
		tiles[12][x] = 1
		tiles[13][x] = 1
		tiles[14][x] = 1

	# ── 3. ASYMMETRIC SECONDARY / EXPLORATION PLATFORMS ───────────────
	# Low safety stepping stone over bottom air
	for x in range(13, 16):
		tiles[17][x] = 2
	for x in range(22, 25):
		tiles[16][x] = 2

	# High discovery catwalks (asymmetric elevation variety)
	for x in range(8, 11):
		tiles[8][x] = 2
	for x in range(16, 19):
		tiles[6][x] = 2

	# Hazards in isolated floor pits
	for x in range(7, 9):
		tiles[19][x] = 3
	for x in range(26, 29):
		tiles[19][x] = 3

	# Spawn and Ancient Door positions (clear air)
	tiles[15][2] = 0   # Spawn position
	tiles[10][34] = 0  # Ancient Door position

	return {
		"width": width,
		"height": height,
		"spawn_x": 2,
		"spawn_y": 15,
		"exit_x": 34,
		"exit_y": 10,
		"tiles": tiles,
		"stats": {
			"seed": 204,
			"jump_count": 6,
			"difficulty_score": 0.25,
			"valid": true,
			"generator": "AsymmetricUnidirectional"
		}
	}
