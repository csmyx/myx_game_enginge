# Copilot Instructions

## Build & Run

- Open `myx_game_engine.slnx` in Visual Studio 2026+ (requires v145/MSVC toolset).
- Build with **Debug|x64** (C++23) or **Release|x64** (C++20).
- Dependencies are managed via vcpkg manifests (`vcpkg.json`); VS restores them automatically on build.
- Output binaries land in `bin/$(Configuration)-$(Platform)/$(ProjectName)/`. There is no CLI build script.

## Architecture

This is an early-stage C++ Windows game engine with two projects:

- **`engine_core`** — a DLL (DynamicLibrary) that exports the engine API.
- **`sandbox`** — an executable (Application) that links against `engine_core` and serves as the test/demo harness.

### DLL Export Pattern

`engine_core/src/engine_core/util.h` defines the `ENGINE_CORE_API` macro:
- Defined as `__declspec(dllexport)` when `ENGINE_CORE_EXPORTS` is set (building the DLL).
- Defined as `__declspec(dllimport)` otherwise (consuming the DLL).

Every public class or function in `engine_core` must be annotated with `ENGINE_CORE_API`.

### Public Header Pattern

`engine_core/src/engine_core.h` is the **single public include** for consumers. It aggregates all sub-headers:
```cpp
#include "engine_core/app.h"
#include "engine_core/test.h"
```

Consumers (like `sandbox`) add `$(SolutionDir)engine_core\src` to their include path and write `#include <engine_core.h>`.

## Key Conventions

- **Header guards**: Always use `#pragma once` (no traditional include guards).
- **Namespaces**: Use nested namespace syntax. Core engine types go in `namespace engine_core { }`; test helpers in `namespace engine_core::test { }`.
- **Logging & output**: Use `spdlog` for structured logging (set up in `engine_core::test::run_tests()`). Use `std::println` (C++23) for simple console output.
- **File layout**: Implementation files live alongside headers in `src/engine_core/`. Each logical module gets its own `.h`/`.cpp` pair.
- **Platform**: Windows-only. The `util.h` header emits `#error "Unsupported platform"` on non-Windows; do not add cross-platform abstractions without updating this guard.
- **Dependencies**: All third-party libraries must be added via `vcpkg.json` (manifest mode), never vendored or committed.
