// ============================================================
// GDExtension.cpp
//
// Entry point for the APLG GDExtension DLL.
//
// Godot calls aplg_library_init() when loading the DLL.
// We use GDExtensionBinding::InitObject to:
//   1. Set the minimum init level (SCENE — we need Node)
//   2. Register a callback that runs ClassDB::register_class<>
//      for every C++ class Godot needs to know about.
//
// RULE: Only class registration happens here.
//       No game logic. No engine initialization.
//       That happens inside EngineBridge::_ready().
// ============================================================

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>

#include "EngineBridge.h"  // Our bridge class

using namespace godot;

// ──────────────────────────────────────────────────────────────
// Registration callbacks
// Called by Godot at the appropriate initialization level.
// ──────────────────────────────────────────────────────────────

static void initialize_aplg_module(ModuleInitializationLevel p_level)
{
    // Only register scene-level classes at the SCENE level.
    // Registering at the wrong level causes a crash.
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    // Register EngineBridge so Godot knows it exists.
    // After this call:
    //   - GDScript can do: var bridge = EngineBridge.new()
    //   - The editor shows EngineBridge in the Add Node dialog
    //   - All methods bound in _bind_methods() are callable
    ClassDB::register_class<EngineBridge>();
}

static void uninitialize_aplg_module(ModuleInitializationLevel p_level)
{
    // Nothing to manually unregister in Godot 4 —
    // ClassDB handles cleanup automatically.
    // This callback exists for future use (e.g., freeing singletons).
    (void)p_level;
}

// ──────────────────────────────────────────────────────────────
// DLL entry point — exported as "aplg_library_init"
// Must match entry_symbol in aplg.gdextension exactly.
// ──────────────────────────────────────────────────────────────

extern "C" {

GDExtensionBool GDE_EXPORT aplg_library_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    GDExtensionClassLibraryPtr         p_library,
    GDExtensionInitialization*         r_initialization)
{
    GDExtensionBinding::InitObject init_obj(
        p_get_proc_address, p_library, r_initialization);

    // Set minimum level: NODE lives at SCENE level.
    init_obj.set_minimum_library_initialization_level(
        MODULE_INITIALIZATION_LEVEL_SCENE);

    // Register our callbacks with the binding system.
    init_obj.register_initializer(initialize_aplg_module);
    init_obj.register_terminator(uninitialize_aplg_module);

    return init_obj.init();
}

} // extern "C"
