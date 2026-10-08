# ============================================================
# CameraController.gd  —  Smooth Action Follow Camera
# ============================================================
# Smoothly follows the player character at a crisp, clear 2.2x zoom
# with room boundary clamping so the knight is large and detailed.
# ============================================================

extends Camera2D
class_name CameraController

@export var follow_speed: float = 8.0
@export var zoom_level: float = 2.2
@export var enable_smoothing: bool = true

var target: Node2D = null
var _bounds: Rect2 = Rect2()

func _ready() -> void:
	top_level = false
	position_smoothing_enabled = true
	position_smoothing_speed = follow_speed
	offset = Vector2.ZERO
	zoom = Vector2(zoom_level, zoom_level)

func set_target(new_target: Node2D) -> void:
	target = new_target

func reset_camera_to_target() -> void:
	if target:
		global_position = target.global_position
	reset_smoothing()

func set_level_bounds(bounds: Rect2) -> void:
	_bounds = bounds
	if bounds.size.x > 0 and bounds.size.y > 0:
		limit_left = int(bounds.position.x)
		limit_top = int(bounds.position.y)
		limit_right = int(bounds.position.x + bounds.size.x)
		limit_bottom = int(bounds.position.y + bounds.size.y)
		limit_smoothed = true

func fit_to_container(_level_width: int, _level_height: int, _tile_size: int = 32) -> void:
	# Keep player follow zoom
	zoom = Vector2(zoom_level, zoom_level)

func trigger_shake(_intensity: float, _duration: float) -> void:
	pass

func get_visible_world_rect() -> Rect2:
	var vp_size := get_viewport_rect().size
	var cam_size := vp_size / zoom
	var cam_pos := get_screen_center_position() - cam_size / 2.0
	return Rect2(cam_pos, cam_size)

