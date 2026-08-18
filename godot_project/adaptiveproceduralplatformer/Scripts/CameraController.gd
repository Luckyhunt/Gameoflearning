# ============================================================
# CameraController.gd  —  Professional platformer camera
# ============================================================
# Features: Smooth follow, look-ahead, zoom control, boundaries,
# camera shake, landing effects, checkpoint zoom

extends Camera2D
class_name CameraController

# Camera settings
@export var follow_speed: float = 8.0
@export var look_ahead_amount: float = 100.0
@export var look_ahead_speed: float = 5.0
@export var zoom_level: float = 1.0
@export var enable_smoothing: bool = true

# Camera boundaries
var level_bounds: Rect2 = Rect2()
var has_bounds: bool = false

# Look-ahead
var target_offset: Vector2 = Vector2.ZERO
var current_offset: Vector2 = Vector2.ZERO

# Camera shake
var shake_intensity: float = 0.0
var shake_duration: float = 0.0
var shake_timer: float = 0.0

# Target to follow
var target: Node2D = null

func _ready() -> void:
	zoom = Vector2(zoom_level, zoom_level)
	position_smoothing_enabled = false
	offset = Vector2.ZERO

func _process(_delta: float) -> void:
	if target:
		global_position = target.global_position
		offset = Vector2.ZERO

func set_target(new_target: Node2D) -> void:
	target = new_target
	if target:
		global_position = target.global_position
		offset = Vector2.ZERO

func reset_camera_to_target() -> void:
	if target:
		global_position = target.global_position
		offset = Vector2.ZERO

func set_level_bounds(_bounds: Rect2) -> void:
	# Ignore bounds to keep camera freely locked on player everywhere
	has_bounds = false
	offset = Vector2.ZERO

func _update_look_ahead(_delta: float) -> void:
	offset = Vector2.ZERO

func _update_camera_shake(_delta: float) -> void:
	offset = Vector2.ZERO

func trigger_shake(_intensity: float, _duration: float) -> void:
	offset = Vector2.ZERO

func set_zoom_for_tile_view(tile_size: int, visible_tiles: int) -> void:
	var desired_view_size := tile_size * visible_tiles
	var viewport_height := get_viewport_rect().size.y
	var new_zoom := viewport_height / desired_view_size
	zoom = Vector2(new_zoom, new_zoom)

func zoom_to_checkpoint(duration: float = 0.5) -> void:
	var tween := create_tween()
	var original_zoom := zoom
	tween.tween_property(self, "zoom", original_zoom * 1.5, duration * 0.5)
	tween.tween_property(self, "zoom", original_zoom, duration * 0.5)

func zoom_on_level_complete(duration: float = 1.0) -> void:
	var tween := create_tween()
	var original_zoom := zoom
	tween.tween_property(self, "zoom", original_zoom * 0.5, duration * 0.5)
	tween.tween_delay(duration * 0.5)
	tween.tween_property(self, "zoom", original_zoom, duration * 0.5)
