cmake_minimum_required(VERSION 3.28)

project(myx_game_engine
    VERSION 0.1.0
    DESCRIPTION "Myx Game Engine"
    LANGUAGES CXX
)

# ── clangd / LSP support ─────────────────────────────────────
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── C++ Standard ──────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── MSVC Options ──────────────────────────────────────────────
if(MSVC)
    add_compile_options(/utf-8 /W3)
endif()

# ── Architecture detection for output naming ──────────────────
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(MYX_ARCH "x64")
else()
    set(MYX_ARCH "x86")
endif()

# ── Output directories (match MSBuild convention) ─────────────
set(MYX_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin/$<CONFIG>-${MYX_ARCH}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${MYX_OUTPUT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${MYX_OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${MYX_OUTPUT_DIR}")

add_subdirectory(engine_core)
add_subdirectory(sandbox)
