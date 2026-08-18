# ============================================================
# ArtDirector.gd  —  Art Direction and Visual Style Management
# ============================================================
# Manages color palettes, themes, and visual consistency
# across all game elements following professional indie game standards

extends Node
class_name ArtDirector

# Theme definitions
enum LevelTheme {
	FOREST,
	ICE,
	LAVA,
	CASTLE,
	FACTORY,
	CAVE,
	RUINS,
	UNDERGROUND
}

# Color palettes for each theme
var _theme_palettes: Dictionary = {
	LevelTheme.FOREST: {
		"primary": Color(0.2, 0.6, 0.3),
		"secondary": Color(0.4, 0.8, 0.5),
		"accent": Color(0.9, 0.7, 0.2),
		"background": Color(0.08, 0.12, 0.08),
		"platform": Color(0.3, 0.5, 0.3),
		"platform_dark": Color(0.2, 0.35, 0.2),
		"platform_light": Color(0.4, 0.65, 0.4),
		"hazard": Color(0.8, 0.3, 0.2),
		"coin": Color(0.94, 0.76, 0.13),
		"particle": Color(0.5, 0.8, 0.6)
	},
	LevelTheme.ICE: {
		"primary": Color(0.4, 0.7, 0.9),
		"secondary": Color(0.6, 0.85, 1.0),
		"accent": Color(0.2, 0.4, 0.8),
		"background": Color(0.08, 0.1, 0.15),
		"platform": Color(0.5, 0.7, 0.85),
		"platform_dark": Color(0.35, 0.5, 0.65),
		"platform_light": Color(0.65, 0.85, 0.95),
		"hazard": Color(0.2, 0.3, 0.6),
		"coin": Color(0.94, 0.76, 0.13),
		"particle": Color(0.7, 0.9, 1.0)
	},
	LevelTheme.LAVA: {
		"primary": Color(0.9, 0.3, 0.2),
		"secondary": Color(1.0, 0.5, 0.3),
		"accent": Color(1.0, 0.8, 0.2),
		"background": Color(0.12, 0.05, 0.03),
		"platform": Color(0.6, 0.35, 0.25),
		"platform_dark": Color(0.45, 0.25, 0.18),
		"platform_light": Color(0.75, 0.45, 0.32),
		"hazard": Color(1.0, 0.2, 0.1),
		"coin": Color(0.94, 0.76, 0.13),
		"particle": Color(1.0, 0.6, 0.3)
	},
	LevelTheme.CASTLE: {
		"primary": Color(0.5, 0.45, 0.6),
		"secondary": Color(0.7, 0.65, 0.8),
		"accent": Color(0.9, 0.85, 0.95),
		"background": Color(0.1, 0.08, 0.12),
		"platform": Color(0.4, 0.35, 0.45),
		"platform_dark": Color(0.3, 0.25, 0.32),
		"platform_light": Color(0.5, 0.45, 0.55),
		"hazard": Color(0.6, 0.3, 0.5),
		"coin": Color(0.94, 0.76, 0.13),
		"particle": Color(0.8, 0.7, 0.9)
	},
	LevelTheme.FACTORY: {
		"primary": Color(0.6, 0.55, 0.5),
		"secondary": Color(0.8, 0.75, 0.7),
		"accent": Color(0.9, 0.4, 0.3),
		"background": Color(0.08, 0.07, 0.06),
		"platform": Color(0.45, 0.4, 0.35),
		"platform_dark": Color(0.32, 0.28, 0.25),
		"platform_light": Color(0.55, 0.5, 0.45),
		"hazard": Color(0.8, 0.2, 0.15),
		"coin": Color(0.94, 0.76, 0.13),
		"particle": Color(0.7, 0.65, 0.6)
	},
	LevelTheme.CAVE: {
		"primary": Color(0.4, 0.35, 0.3),
		"secondary": Color(0.6, 0.55, 0.5),
		"accent": Color(0.3, 0.7, 0.4),
		"background": Color(0.05, 0.04, 0.03),
		"platform": Color(0.35, 0.3, 0.25),
		"platform_dark": Color(0.25, 0.22, 0.18),
		"platform_light": Color(0.45, 0.4, 0.35),
		"hazard": Color(0.5, 0.2, 0.15),
		"coin": Color(0.94, 0.76, 0.13),
		"particle": Color(0.5, 0.45, 0.4)
	},
	LevelTheme.RUINS: {
		"primary": Color(0.7, 0.65, 0.5),
		"secondary": Color(0.85, 0.8, 0.65),
		"accent": Color(0.4, 0.7, 0.3),
		"background": Color(0.1, 0.09, 0.07),
		"platform": Color(0.55, 0.5, 0.4),
		"platform_dark": Color(0.4, 0.37, 0.3),
		"platform_light": Color(0.65, 0.6, 0.5),
		"hazard": Color(0.6, 0.4, 0.2),
		"coin": Color(0.94, 0.76, 0.13),
		"particle": Color(0.75, 0.7, 0.55)
	},
	LevelTheme.UNDERGROUND: {
		"primary": Color(0.3, 0.25, 0.4),
		"secondary": Color(0.5, 0.45, 0.6),
		"accent": Color(0.7, 0.3, 0.8),
		"background": Color(0.04, 0.03, 0.06),
		"platform": Color(0.25, 0.22, 0.32),
		"platform_dark": Color(0.18, 0.16, 0.22),
		"platform_light": Color(0.32, 0.28, 0.4),
		"hazard": Color(0.6, 0.2, 0.5),
		"coin": Color(0.94, 0.76, 0.13),
		"particle": Color(0.5, 0.4, 0.6)
	}
}

# Current theme
var _current_theme: LevelTheme = LevelTheme.FOREST

func _ready() -> void:
	# Set default theme
	set_theme(LevelTheme.FOREST)

func set_theme(theme: LevelTheme) -> void:
	_current_theme = theme
	emit_signal("theme_changed", theme)

func get_current_theme() -> LevelTheme:
	return _current_theme

func get_color(color_name: String) -> Color:
	var palette = _theme_palettes[_current_theme]
	if palette.has(color_name):
		return palette[color_name]
	return Color.WHITE

func get_palette() -> Dictionary:
	return _theme_palettes[_current_theme]

func get_theme_name() -> String:
	match _current_theme:
		LevelTheme.FOREST: return "FOREST"
		LevelTheme.ICE: return "ICE"
		LevelTheme.LAVA: return "LAVA"
		LevelTheme.CASTLE: return "CASTLE"
		LevelTheme.FACTORY: return "FACTORY"
		LevelTheme.CAVE: return "CAVE"
		LevelTheme.RUINS: return "RUINS"
		LevelTheme.UNDERGROUND: return "UNDERGROUND"
		_: return "UNKNOWN"

signal theme_changed(theme: LevelTheme)

# Platform visual styles
func get_platform_color(variant: int = 0) -> Color:
	match variant:
		0: return get_color("platform")
		1: return get_color("platform_dark")
		2: return get_color("platform_light")
		_: return get_color("platform")

# Hazard visual styles
func get_hazard_color() -> Color:
	return get_color("hazard")

# Coin visual styles
func get_coin_color() -> Color:
	return get_color("coin")

# Particle colors
func get_particle_color() -> Color:
	return get_color("particle")

# Background color
func get_background_color() -> Color:
	return get_color("background")

# UI accent color
func get_ui_accent_color() -> Color:
	return get_color("primary")
