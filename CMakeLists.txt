cmake_minimum_required(VERSION 3.20)
project(AdaptiveProceduralLevelGeneration VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /permissive- /MT)
    # TYPED_METHOD_BIND: required for MSVC.
    # Without it, godot-cpp uses reinterpret_cast on method pointers
    # via a stub class, which MSVC correctly rejects. This define
    # switches godot-cpp to use proper typed templates instead.
    add_compile_definitions(TYPED_METHOD_BIND)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()


# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Engine)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Player)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Physics)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Generation)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/AI)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Difficulty)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Gameplay)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Rendering)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Audio)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/UI)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Utilities)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Validation)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/LevelEngine)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Decoration)

# Godot GDExtension setup
set(GODOT_CPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/godot-cpp")

include_directories(${GODOT_CPP_DIR}/include)
include_directories(${GODOT_CPP_DIR}/gen/include)
include_directories(${GODOT_CPP_DIR}/gdextension)

# Godot C++ library
set(GODOT_CPP_LIB "${GODOT_CPP_DIR}/bin/libgodot-cpp.windows.template_debug.x86_64.lib")

if(NOT EXISTS ${GODOT_CPP_LIB})
    message(FATAL_ERROR "Godot C++ library not found at ${GODOT_CPP_LIB}. Please build godot-cpp first.")
endif()

# Source files
file(GLOB_RECURSE ENGINE_SOURCES "Engine/*.cpp")
file(GLOB_RECURSE PLAYER_SOURCES "Player/*.cpp")
file(GLOB_RECURSE PHYSICS_SOURCES "Physics/*.cpp")
file(GLOB_RECURSE GENERATION_SOURCES "Generation/*.cpp")
file(GLOB_RECURSE AI_SOURCES "AI/*.cpp")
file(GLOB_RECURSE DIFFICULTY_SOURCES "Difficulty/*.cpp")
file(GLOB_RECURSE GAMEPLAY_SOURCES "Gameplay/*.cpp")
file(GLOB_RECURSE RENDERING_SOURCES "Rendering/*.cpp")
file(GLOB_RECURSE AUDIO_SOURCES "Audio/*.cpp")
file(GLOB_RECURSE UI_SOURCES "UI/*.cpp")
file(GLOB_RECURSE UTILITIES_SOURCES "Utilities/*.cpp")
file(GLOB_RECURSE SRC_SOURCES "Src/*.cpp")
file(GLOB_RECURSE VALIDATION_SOURCES "Validation/*.cpp")
file(GLOB_RECURSE LEVELENGINE_SOURCES "LevelEngine/*.cpp")
file(GLOB_RECURSE DECORATION_SOURCES "Decoration/*.cpp")

# Include GDExtension.cpp for GDExtension mode
# list(FILTER SRC_SOURCES EXCLUDE REGEX ".*GDExtension\\.cpp")

# Exclude test files from main library
list(FILTER UTILITIES_SOURCES EXCLUDE REGEX ".*Test\\.cpp")
list(FILTER PHYSICS_SOURCES EXCLUDE REGEX ".*Test\\.cpp")
list(FILTER PLAYER_SOURCES EXCLUDE REGEX ".*Test\\.cpp")
list(FILTER GENERATION_SOURCES EXCLUDE REGEX ".*Test\\.cpp")
list(FILTER GENERATION_SOURCES EXCLUDE REGEX ".*Validation.*\\.cpp")
list(FILTER DIFFICULTY_SOURCES EXCLUDE REGEX ".*Test\\.cpp")
list(FILTER AI_SOURCES EXCLUDE REGEX ".*Test\\.cpp")
list(FILTER GAMEPLAY_SOURCES EXCLUDE REGEX ".*Test\\.cpp")
list(FILTER UTILITIES_SOURCES EXCLUDE REGEX "SaveSystem.*Test\\.cpp")
list(FILTER UI_SOURCES EXCLUDE REGEX ".*Test\\.cpp")
list(FILTER AUDIO_SOURCES EXCLUDE REGEX ".*Test\\.cpp")

# Create static library for tests and standalone
add_library(${PROJECT_NAME} STATIC
    ${ENGINE_SOURCES}
    ${PLAYER_SOURCES}
    ${PHYSICS_SOURCES}
    ${GENERATION_SOURCES}
    ${VALIDATION_SOURCES}
    ${LEVELENGINE_SOURCES}
    ${DECORATION_SOURCES}
    ${AI_SOURCES}
    ${DIFFICULTY_SOURCES}
    ${GAMEPLAY_SOURCES}
    ${RENDERING_SOURCES}
    ${AUDIO_SOURCES}
    ${UI_SOURCES}
    ${UTILITIES_SOURCES}
    ${SRC_SOURCES}
)

# Link Godot C++ library
target_link_libraries(${PROJECT_NAME}
    ${GODOT_CPP_LIB}
)

# Add Godot include directories to main library
target_include_directories(${PROJECT_NAME} PRIVATE
    ${GODOT_CPP_DIR}/include
    ${GODOT_CPP_DIR}/gen/include
    ${GODOT_CPP_DIR}/gdextension
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Engine
    ${CMAKE_CURRENT_SOURCE_DIR}/Player
    ${CMAKE_CURRENT_SOURCE_DIR}/Physics
    ${CMAKE_CURRENT_SOURCE_DIR}/Generation
    ${CMAKE_CURRENT_SOURCE_DIR}/AI
    ${CMAKE_CURRENT_SOURCE_DIR}/Difficulty
    ${CMAKE_CURRENT_SOURCE_DIR}/Gameplay
    ${CMAKE_CURRENT_SOURCE_DIR}/Rendering
    ${CMAKE_CURRENT_SOURCE_DIR}/Audio
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
    ${CMAKE_CURRENT_SOURCE_DIR}/Utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/Validation
    ${CMAKE_CURRENT_SOURCE_DIR}/LevelEngine
    ${CMAKE_CURRENT_SOURCE_DIR}/Decoration
)

# Windows specific
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded"
    )
endif()

# Create GDExtension DLL (separate from static library)
add_library(${PROJECT_NAME}_gdextension SHARED
    Src/GDExtension.cpp
)

target_link_libraries(${PROJECT_NAME}_gdextension
    ${PROJECT_NAME}
    ${GODOT_CPP_LIB}
)

target_include_directories(${PROJECT_NAME}_gdextension PRIVATE
    ${GODOT_CPP_DIR}/include
    ${GODOT_CPP_DIR}/gen/include
    ${GODOT_CPP_DIR}/gdextension
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Engine
    ${CMAKE_CURRENT_SOURCE_DIR}/Player
    ${CMAKE_CURRENT_SOURCE_DIR}/Physics
    ${CMAKE_CURRENT_SOURCE_DIR}/Generation
    ${CMAKE_CURRENT_SOURCE_DIR}/AI
    ${CMAKE_CURRENT_SOURCE_DIR}/Difficulty
    ${CMAKE_CURRENT_SOURCE_DIR}/Gameplay
    ${CMAKE_CURRENT_SOURCE_DIR}/Rendering
    ${CMAKE_CURRENT_SOURCE_DIR}/Audio
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
    ${CMAKE_CURRENT_SOURCE_DIR}/Utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/Validation
    ${CMAKE_CURRENT_SOURCE_DIR}/LevelEngine
    ${CMAKE_CURRENT_SOURCE_DIR}/Decoration
)

if(WIN32)
    set_target_properties(${PROJECT_NAME}_gdextension PROPERTIES
        PREFIX ""
        OUTPUT_NAME "AdaptiveProceduralLevelGeneration_gdextension"
        SUFFIX ".dll"
        MSVC_RUNTIME_LIBRARY "MultiThreaded"
    )
endif()

# ── Post-build: copy DLL into the Godot project directory ──────
# This keeps the Godot project automatically in sync after every build.
# The .gdextension manifest points to this exact filename.
set(GODOT_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/godot_project/adaptiveproceduralplatformer")

add_custom_command(TARGET ${PROJECT_NAME}_gdextension POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${PROJECT_NAME}_gdextension>
        "${GODOT_PROJECT_DIR}/AdaptiveProceduralLevelGeneration_gdextension.dll"
    COMMENT "Copying GDExtension DLL to Godot project directory..."
)

# Testing
enable_testing()
add_subdirectory(Tests)

# Standalone application
add_executable(StandaloneApp
    standalone/main.cpp
)

target_include_directories(StandaloneApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Engine
    ${CMAKE_CURRENT_SOURCE_DIR}/Player
    ${CMAKE_CURRENT_SOURCE_DIR}/Physics
    ${CMAKE_CURRENT_SOURCE_DIR}/Generation
    ${CMAKE_CURRENT_SOURCE_DIR}/AI
    ${CMAKE_CURRENT_SOURCE_DIR}/Difficulty
    ${CMAKE_CURRENT_SOURCE_DIR}/Gameplay
    ${CMAKE_CURRENT_SOURCE_DIR}/Rendering
    ${CMAKE_CURRENT_SOURCE_DIR}/Audio
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
    ${CMAKE_CURRENT_SOURCE_DIR}/Utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/Validation
    ${CMAKE_CURRENT_SOURCE_DIR}/LevelEngine
)

target_link_libraries(StandaloneApp
    ${PROJECT_NAME}
)

if(WIN32)
    target_link_libraries(StandaloneApp user32 gdi32 winmm msimg32)
    set_target_properties(StandaloneApp PROPERTIES
        WIN32_EXECUTABLE TRUE
        MSVC_RUNTIME_LIBRARY "MultiThreaded"
    )
endif()
