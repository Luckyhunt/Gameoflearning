# Godot GDExtension Headers

This directory should contain the Godot GDExtension API headers.

## How to Obtain

1. Download Godot 4.x from https://godotengine.org/download
2. Extract the Godot editor
3. Copy the headers from `godot/include/` to this directory

Alternatively, you can download the headers separately from:
https://github.com/godotengine/godot/tree/master/modules/gdextension/include

## Required Files

- `gdextension_interface.h`
- `gdextension.gen.h`

## Note

For the purpose of this project, the core C++ implementation is independent of Godot.
The Godot integration layer will be added in the Rendering module.
