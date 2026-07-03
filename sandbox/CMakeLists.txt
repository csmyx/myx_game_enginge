# ── Executable ────────────────────────────────────────────────
add_executable(sandbox
    src/app.cpp
)

# ── Link against engine_core DLL ──────────────────────────────
target_link_libraries(sandbox PRIVATE engine_core)

# ── Post-build: copy engine_core.dll → sandbox output dir ─────
add_custom_command(TARGET sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:engine_core>"
        "$<TARGET_FILE_DIR:sandbox>"
    COMMENT "Deploying engine_core.dll → sandbox output directory"
)
