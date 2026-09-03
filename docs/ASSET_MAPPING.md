# Asset Mapping Document — Brackeys Platformer Assets

This document maps all assets located in `brackeys_platformer_assets/` to their corresponding usage in the **Standalone C++ Executable** and the **Godot 4 Engine Integration**.

---

## 1. Asset Inventory & Categorization

| Category | Filename | Type | Resolution / Spec | Purpose / Usage |
| :--- | :--- | :--- | :--- | :--- |
| **Sprites** | `sprites/knight.png` | PNG | Sprite Sheet (16x16 frames) | Player animations (Idle, Run, Roll/Dash, Hit) |
| **Sprites** | `sprites/world_tileset.png` | PNG | Tileset (16x16 tiles) | Terrain, ground, walls, platforms, foliage, hazards |
| **Sprites** | `sprites/platforms.png` | PNG | Sprite Sheet (16x16 tiles) | Moving platforms, one-way wooden/stone platforms |
| **Sprites** | `sprites/slime_green.png` | PNG | Sprite Sheet (16x16 frames) | Basic enemy walk/idle animations |
| **Sprites** | `sprites/slime_purple.png` | PNG | Sprite Sheet (16x16 frames) | Advanced / ranged enemy animations |
| **Sprites** | `sprites/coin.png` | PNG | Sprite Sheet (16x16 frames) | Collectible currency animation |
| **Sprites** | `sprites/fruit.png` | PNG | Sprite Sheet (16x16 frames) | Health / bonus pickup animation |
| **Audio** | `sounds/jump.wav` | WAV | PCM Audio | Jump sound effect |
| **Audio** | `sounds/coin.wav` | WAV | PCM Audio | Coin collection sound effect |
| **Audio** | `sounds/hurt.wav` | WAV | PCM Audio | Player damage sound effect |
| **Audio** | `sounds/explosion.wav` | WAV | PCM Audio | Enemy defeat / explosion sound effect |
| **Audio** | `sounds/power_up.wav` | WAV | PCM Audio | Level completion / power up sound effect |
| **Audio** | `sounds/tap.wav` | WAV | PCM Audio | UI button click / menu navigation sound |
| **Music** | `music/time_for_adventure.mp3` | MP3 | Stereo MP3 | Background music track |
| **Fonts** | `fonts/PixelOperator8.ttf` | TTF | TrueType Font | Retro 8-bit UI text font |
| **Fonts** | `fonts/PixelOperator8-Bold.ttf` | TTF | TrueType Font | Retro 8-bit UI headers and title font |

---

## 2. Standalone C++ Asset Integration Mapping

In the C++ Standalone executable (`StandaloneApp.exe`), assets are managed via `APLG::AssetManager` using Windows GDI+ (`Gdiplus::Bitmap`) and WinMM audio (`PlaySoundA` / `mciSendStringA`):

### Sprites & Tiles Slicing Guide (16x16 Grid)
- **Ground / Walls (`world_tileset.png`)**:
  - Solid Block: `(0, 0, 16, 16)`
  - Platform Top: `(16, 0, 16, 16)`
  - Wall Left/Right: `(0, 16, 16, 16)`
- **Player (`knight.png`)**:
  - Idle Frame 0: `(0, 0, 32, 32)`
  - Run Animation: `(0, 64, 32, 32)` to `(224, 64, 32, 32)` (8 frames)
  - Hit Frame: `(0, 192, 32, 32)`
- **Enemies (`slime_green.png` & `slime_purple.png`)**:
  - Idle/Walk: `(0, 24, 24, 24)` to `(72, 24, 24, 24)` (4 frames)
- **Collectibles (`coin.png`)**:
  - Spinning Coin: `(0, 0, 16, 16)` to `(176, 0, 16, 16)` (12 frames)

### Audio Triggers
- `AssetManager::playSound(SoundEffect::Jump)` → Triggers on player jump.
- `AssetManager::playSound(SoundEffect::Coin)` → Triggers when picking up a coin/fruit.
- `AssetManager::playSound(SoundEffect::Hurt)` → Triggers when player receives damage.
- `AssetManager::playSound(SoundEffect::Explosion)` → Triggers when an enemy is defeated.
- `AssetManager::playSound(SoundEffect::PowerUp)` → Triggers on level clear.
- `AssetManager::playSound(SoundEffect::Tap)` → Triggers on menu button selection.
- `AssetManager::playMusic("music/time_for_adventure.mp3")` → Loops background music during gameplay.

---

## 3. Future Godot Engine Integration Mapping

For future Godot 4 scenes (`godot_project/adaptiveproceduralplatformer/`), assets in `brackeys_platformer_assets` map to Godot resources as follows:

| Godot Resource Path | Source Asset File | Godot Resource Type |
| :--- | :--- | :--- |
| `res://Assets/Sprites/knight.png` | `brackeys_platformer_assets/sprites/knight.png` | `CompressedTexture2D` / `AnimatedSprite2D` |
| `res://Assets/Sprites/world_tileset.png` | `brackeys_platformer_assets/sprites/world_tileset.png` | `TileSet` atlas source |
| `res://Assets/Sprites/platforms.png` | `brackeys_platformer_assets/sprites/platforms.png` | `TileSet` / `Sprite2D` |
| `res://Assets/Sprites/slime_green.png` | `brackeys_platformer_assets/sprites/slime_green.png` | `AnimatedSprite2D` |
| `res://Assets/Audio/jump.wav` | `brackeys_platformer_assets/sounds/jump.wav` | `AudioStreamWAV` |
| `res://Assets/Audio/coin.wav` | `brackeys_platformer_assets/sounds/coin.wav` | `AudioStreamWAV` |
| `res://Assets/Audio/time_for_adventure.mp3` | `brackeys_platformer_assets/music/time_for_adventure.mp3` | `AudioStreamMP3` |
| `res://Assets/Fonts/PixelOperator8.ttf` | `brackeys_platformer_assets/fonts/PixelOperator8.ttf` | `FontFile` |

---

## 4. Licensing & Attribution

All assets are licensed under the original Brackeys asset package terms (`brackeys_platformer_assets/LICENSE & CREDITS.txt`).
- Created by Brackeys (https://brackeys.com).
- Free for personal and commercial game development.
