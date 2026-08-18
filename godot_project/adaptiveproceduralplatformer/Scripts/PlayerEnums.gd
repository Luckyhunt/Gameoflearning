# ============================================================
# PlayerEnums.gd  —  Shared enums and constants for player system
# ============================================================
# IMPORTANT: These enums must match C++ definitions in Common.h
# C++ is the source of truth - update both together

extends RefCounted
class_name PlayerEnums

enum TileType {
	EMPTY = 0,
	SOLID = 1,
	PLATFORM = 2,
	HAZARD = 3,
	SPAWN = 4,
	EXIT = 5,
	CHECKPOINT = 6,
	COIN = 7,
	POWERUP = 8,
	SECRET = 9,
	MOVING_PLATFORM = 10,
	FALLING_PLATFORM = 11,
	BOUNCE_PAD = 12,
	ICE_PLATFORM = 13,
	ONE_WAY_PLATFORM = 14
}

enum MovementState {
	IDLE,
	RUN,
	JUMP,
	FALL,
	WALL_SLIDE,
	DASH,
	DEAD
}

enum GroundState {
	AIR,
	GROUND,
	ICE,
	BOUNCE
}

enum LifeState {
	ALIVE,
	INVINCIBLE,
	DEAD
}

enum DeathType {
	FALL,
	HAZARD,
	ENEMY
}
