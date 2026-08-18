# ============================================================
# PauseMenu.gd  —  Professional Pause Menu
# ============================================================
# Features: Blur background, keyboard/controller navigation,
# smooth transitions, professional styling

extends Control
class_name PauseMenu

signal resume_pressed
signal restart_pressed
signal settings_pressed
signal main_menu_pressed
signal exit_pressed

# Button nodes
@onready var btn_resume: Button = $CenterContainer/VBoxContainer/BtnResume
@onready var btn_restart: Button = $CenterContainer/VBoxContainer/BtnRestart
@onready var btn_settings: Button = $CenterContainer/VBoxContainer/BtnSettings
@onready var btn_main_menu: Button = $CenterContainer/VBoxContainer/BtnMainMenu
@onready var btn_exit: Button = $CenterContainer/VBoxContainer/BtnExit

# Background blur
@onready var background_panel: Panel = $Background

# Button list for navigation
var _buttons: Array[Button] = []
var _current_button_index: int = 0

func _ready() -> void:
	# Collect buttons
	_buttons = [btn_resume, btn_restart, btn_settings, btn_main_menu, btn_exit]
	
	# Connect button signals
	for i in range(_buttons.size()):
		_buttons[i].mouse_entered.connect(_on_button_hovered.bind(i))
		_buttons[i].pressed.connect(_on_button_pressed.bind(i))
	
	# Start animations
	_animate_menu_in()
	_animate_background()

func _input(event: InputEvent) -> void:
	if not visible:
		return
	
	# Keyboard navigation
	if event.is_action_pressed("ui_down"):
		_navigate_to_button(_current_button_index + 1)
	elif event.is_action_pressed("ui_up"):
		_navigate_to_button(_current_button_index - 1)
	elif event.is_action_pressed("ui_cancel"):
		emit_signal("resume_pressed")
		animate_out()
	elif event.is_action_pressed("ui_accept"):
		if _current_button_index >= 0 and _current_button_index < _buttons.size():
			_buttons[_current_button_index].pressed.emit()

func _navigate_to_button(index: int) -> void:
	index = wrapi(index, 0, _buttons.size())
	_current_button_index = index
	
	# Update button states
	for i in range(_buttons.size()):
		if i == index:
			_buttons[i].grab_focus()
		else:
			_buttons[i].release_focus()

func _on_button_hovered(index: int) -> void:
	_current_button_index = index
	_animate_button_hover(_buttons[index])

func _on_button_pressed(index: int) -> void:
	_animate_button_press(_buttons[index])
	
	match index:
		0: 
			emit_signal("resume_pressed")
			animate_out()
		1: 
			emit_signal("restart_pressed")
			animate_out()
		2: 
			emit_signal("settings_pressed")
		3: 
			emit_signal("main_menu_pressed")
			animate_out()
		4: 
			emit_signal("exit_pressed")
			animate_out()

func _animate_menu_in() -> void:
	for i in range(_buttons.size()):
		var button: Button = _buttons[i]
		button.modulate.a = 0.0
		button.position.x = -100
		
		var tween := create_tween()
		tween.set_parallel(false)
		tween.tween_interval(0.05 + i * 0.06)
		tween.tween_property(button, "modulate:a", 1.0, 0.3)
		var tween2 := create_tween()
		tween2.tween_property(button, "position:x", 0.0, 0.3)
		tween2.set_ease(Tween.EASE_OUT)
		tween2.set_trans(Tween.TRANS_BACK)

func _animate_background() -> void:
	if background_panel:
		background_panel.modulate.a = 0.0
		var tween := create_tween()
		tween.tween_property(background_panel, "modulate:a", 1.0, 0.2)

func _animate_button_hover(button: Button) -> void:
	var tween := create_tween()
	tween.tween_property(button, "scale", Vector2(1.05, 1.05), 0.12)
	tween.set_ease(Tween.EASE_OUT)

func _animate_button_press(button: Button) -> void:
	var tween := create_tween()
	tween.tween_property(button, "scale", Vector2(0.95, 0.95), 0.08)
	tween.set_ease(Tween.EASE_IN)
	var tween2 := create_tween()
	tween2.tween_property(button, "scale", Vector2(1.0, 1.0), 0.08)
	tween2.set_ease(Tween.EASE_OUT)

func animate_in() -> void:
	visible = true
	_animate_menu_in()
	_animate_background()

func animate_out() -> void:
	var tween := create_tween()
	tween.tween_property(self, "modulate:a", 0.0, 0.2)
	await tween.finished
	visible = false
