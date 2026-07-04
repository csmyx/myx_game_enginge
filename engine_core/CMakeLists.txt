# ── Dependencies ──────────────────────────────────────────────
find_package(spdlog CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

# ── Sources ───────────────────────────────────────────────────
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_library(engine_core SHARED ${ENGINE_SOURCES})

# ── Include paths ─────────────────────────────────────────────
target_include_directories(engine_core
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
)

# ── Defines ───────────────────────────────────────────────────
target_compile_definitions(engine_core
    PRIVATE ENGINE_CORE_EXPORTS
    PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD
    PRIVATE "IMGUI_API=__declspec(dllexport)"
)

# ── Link libraries ────────────────────────────────────────────
target_link_libraries(engine_core
    PUBLIC
        spdlog::spdlog
        glfw
        glad::glad
)

# ── Disable auto-export (we use explicit ENGINE_CORE_API) ─────
set_target_properties(engine_core PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS OFF
)

# ── IDE source groups ─────────────────────────────────────────
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${ENGINE_SOURCES})
