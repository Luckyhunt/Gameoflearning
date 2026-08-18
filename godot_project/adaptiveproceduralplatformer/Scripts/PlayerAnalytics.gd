# ============================================================
# PlayerAnalytics.gd  —  Research metrics component
# ============================================================
# Handles: Timer, Coins, Jump stats, Death tracking, Level stats

extends Node
class_name PlayerAnalytics

signal stats_updated(stats: Dictionary)

var _timer: float = 0.0
var _coins_collected: int = 0
var _deaths: int = 0
var _deaths_hazard: int = 0
var _deaths_fall: int = 0
var _deaths_enemy: int = 0
var _used_checkpoint: bool = false

var _jump_attempts: int = 0
var _jump_landed: int = 0

func update(delta: float) -> void:
	_timer += delta

func record_jump_attempt() -> void:
	_jump_attempts += 1

func record_jump_landed() -> void:
	_jump_landed += 1

func record_coin() -> void:
	_coins_collected += 1

func record_death(death_type: PlayerEnums.DeathType) -> void:
	_deaths += 1
	match death_type:
		PlayerEnums.DeathType.FALL:
			_deaths_fall += 1
		PlayerEnums.DeathType.HAZARD:
			_deaths_hazard += 1
		PlayerEnums.DeathType.ENEMY:
			_deaths_enemy += 1

func record_checkpoint_use() -> void:
	_used_checkpoint = true

func get_level_stats() -> Dictionary:
	var jump_accuracy := float(_jump_landed) / float(max(_jump_attempts, 1))
	return {
		"deaths": _deaths,
		"time_sec": _timer,
		"coins": _coins_collected,
		"jump_accuracy": jump_accuracy,
		"deaths_hazard": _deaths_hazard,
		"deaths_fall": _deaths_fall,
		"deaths_enemy": _deaths_enemy,
		"used_checkpoint": _used_checkpoint,
		"jump_attempts": _jump_attempts,
		"jump_landed": _jump_landed
	}

func get_timer() -> float:
	return _timer

func get_coins() -> int:
	return _coins_collected

func get_deaths() -> int:
	return _deaths

func reset() -> void:
	_timer = 0.0
	_deaths = 0
	_deaths_hazard = 0
	_deaths_fall = 0
	_deaths_enemy = 0
	_coins_collected = 0
	_jump_attempts = 0
	_jump_landed = 0
	_used_checkpoint = false
