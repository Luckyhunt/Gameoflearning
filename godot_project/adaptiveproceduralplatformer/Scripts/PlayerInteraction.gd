# ============================================================
# PlayerInteraction.gd  —  World interaction component
# ============================================================
# Handles: Coin, Hazard, Enemy, Exit, Checkpoint interactions

extends Area2D
class_name PlayerInteraction

signal coin_collected(tile_pos: Vector2i)
signal checkpoint_reached(tile_pos: Vector2i, world_pos: Vector2)
signal exit_reached()
signal hazard_contact()
signal enemy_contact()

var _tile_service: TileService
var _character: Node2D

func setup(character: Node2D, tile_service: TileService) -> void:
	_character = character
	_tile_service = tile_service
	body_entered.connect(_on_body_entered)
	area_entered.connect(_on_area_entered)

func _on_body_entered(body: Node) -> void:
	if body.is_in_group("hazards"):
		emit_signal("hazard_contact")
	elif body.is_in_group("enemies"):
		emit_signal("enemy_contact")

func _on_area_entered(area: Area2D) -> void:
	if area.is_in_group("coins"):
		var tile_pos := _tile_service.world_to_tile(area.global_position)
		emit_signal("coin_collected", tile_pos)
		area.queue_free()
	elif area.is_in_group("checkpoints"):
		var tile_pos := _tile_service.world_to_tile(area.global_position)
		emit_signal("checkpoint_reached", tile_pos, area.global_position)
	elif area.is_in_group("exits"):
		emit_signal("exit_reached")

func check_tile_interactions() -> void:
	var tile_pos := _tile_service.world_to_tile(_character.global_position)
	var tile_type := _tile_service.get_tile_type(tile_pos)
	
	match tile_type:
		PlayerEnums.TileType.HAZARD:
			emit_signal("hazard_contact")
		PlayerEnums.TileType.COIN:
			emit_signal("coin_collected", tile_pos)
		PlayerEnums.TileType.CHECKPOINT:
			emit_signal("checkpoint_reached", tile_pos, _character.global_position)
		PlayerEnums.TileType.EXIT:
			emit_signal("exit_reached")
