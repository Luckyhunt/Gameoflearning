# ============================================================
# TileService.gd  —  World data queries for player
# ============================================================
# Provides tile information without storing world data in player
# Only answers questions about tiles - no gameplay logic

extends Node
class_name TileService

var _tile_lookup: Dictionary = {}
var _tile_size: int = 32

func set_tile_lookup(lookup: Dictionary) -> void:
	_tile_lookup = lookup

func set_tile_size(size: int) -> void:
	_tile_size = size

func world_to_tile(world_pos: Vector2) -> Vector2i:
	return Vector2i(int(world_pos.x / _tile_size), int(world_pos.y / _tile_size))

func get_tile_type(tile_pos: Vector2i) -> int:
	return _tile_lookup.get(tile_pos, PlayerEnums.TileType.EMPTY)

func is_ice(tile_pos: Vector2i) -> bool:
	return get_tile_type(tile_pos) == PlayerEnums.TileType.ICE_PLATFORM

func is_bounce(tile_pos: Vector2i) -> bool:
	return get_tile_type(tile_pos) == PlayerEnums.TileType.BOUNCE_PAD

func is_hazard(tile_pos: Vector2i) -> bool:
	return get_tile_type(tile_pos) == PlayerEnums.TileType.HAZARD

func is_solid(tile_pos: Vector2i) -> bool:
	var type = get_tile_type(tile_pos)
	return type == PlayerEnums.TileType.SOLID or type == PlayerEnums.TileType.PLATFORM
