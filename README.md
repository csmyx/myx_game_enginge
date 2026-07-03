# Myx Game Engine

Early-stage C++23 Windows game engine, built with **MSVC** (v145 toolset) and **vcpkg** for dependency management.

## Project Structure

```
myx_game_engine/
├── CMakeLists.txt                 # CMake build entry point
├── myx_game_engine.slnx           # Visual Studio solution
├── vcpkg.json                     # vcpkg manifest (spdlog, glfw3)
├── docs/                          # Design documentation
│   ├── event_system.md
│   └── window_system.md
├── engine_core/                   # Engine DLL
│   ├── CMakeLists.txt
│   ├── engine_core.vcxproj
│   └── src/
│       ├── engine_core.h          # Public aggregate header
│       └── engine_core/
│           ├── app.h / app.cpp    # Application base class
│           ├── log.h / log.cpp    # Logging (wraps spdlog)
│           ├── window.h           # Abstract Window interface
│           ├── entry_point.h      # main() entry point
│           ├── util.h             # DLL export macros + assertions
│           ├── event/             # Event system (header-only)
│           │   ├── event.h
│           │   ├── event_dispatcher.h
│           │   ├── key_event.h
│           │   ├── mouse_event.h
│           │   └── window_event.h
│           └── platform/
│               └── windows/       # GLFW-based Windows window
└── sandbox/                       # Test/demo executable
    ├── CMakeLists.txt
    ├── sandbox.vcxproj
    └── src/
        └── app.cpp                # create_app() implementation
```

## Prerequisites

- **Visual Studio 2026+** (v145 MSVC toolset)
- **vcpkg** (installed at a known path, e.g. `E:\download\others\vcpkg`)
- **CMake 3.28+** (bundled with Visual Studio)

### Nushell users

If you use [Nushell](https://www.nushell.sh/) as your default shell, VS dev tools (`cl.exe`, `link.exe`) won't be in PATH by default. Load them with:

```nu
source scripts/setup-vs-env.nu
setup-vs-env          # loads x64 MSVC environment
```

Add this to your `env.nu` to load automatically on shell startup. After that, `cmake`, `cl`, and `link` will work normally.

## Build

### Option A: Visual Studio (MSBuild)

1. Open `myx_game_engine.slnx` in Visual Studio.
2. Select **Debug|x64** or **Release|x64**.
3. Build → Build Solution (Ctrl+Shift+B).

vcpkg restores dependencies automatically on first build.

Output lands in:
```
bin/Debug-x64/engine_core/engine_core.dll
bin/Debug-x64/sandbox/sandbox.exe
```

### Option B: CMake (command line)

```powershell
# Set your vcpkg path (once per session, or set it as a system environment variable)
$env:VCPKG_ROOT = "E:\download\others\vcpkg"

# Configure & build Debug (Visual Studio generator)
cmake --preset vs2026-x64
cmake --build --preset debug

# Build Release
cmake --build --preset release
```

Output lands in:
```
build/cmake-vs/bin/Debug-x64/   (engine_core.dll, sandbox.exe, dependency DLLs)
build/cmake-vs/bin/Release-x64/
```

### Option C: Ninja (for clangd / compile_commands.json)

The Ninja generator produces `compile_commands.json` for clangd/LSP support. **Must run from a VS Developer Command Prompt** — otherwise CMake may pick a non-MSVC linker (e.g., MSYS2 `ld`) which cannot resolve `__declspec(dllimport)` symbols.

```powershell
# Open "Developer Command Prompt for VS 2026" from Start Menu, then:
$env:VCPKG_ROOT = "E:\download\others\vcpkg"
cmake --preset ninja-x64-debug    # Generates compile_commands.json
cmake --build --preset ninja-debug
```

**For VS Code:** launch from the developer prompt (`code .`), or select **Visual Studio Community 2026 Release - amd64** as the CMake kit (`Ctrl+Shift+P` → `CMake: Select a Kit`).

Copy the generated file to the project root for clangd:

```powershell
copy build\cmake-ninja-debug\compile_commands.json .
```

## Architecture

- **`engine_core`** — Shared library (DLL) exporting the engine API via `ENGINE_CORE_API`.
- **`sandbox`** — Executable that links `engine_core`, defines `create_app()`, and serves as the test harness.
- **`entry_point.h`** — Provides `main()`; included by the consumer via `<engine_core.h>`.
- **`util.h`** — Defines `ENGINE_CORE_API` (dllimport/dllexport), `ENGINE_CORE_ASSERT` (debug-only), and `ENGINE_DEBUG_BREAK`.

## Key Features

| Feature | Description |
|---|---|
| **Logging** | Wraps spdlog with `ENGINE` (core) and `APP` (client) loggers |
| **Event System** | Header-only, macro-driven (`EVENT_CLASS_TYPE`/`EVENT_CLASS_CATEGORY`), with `EventDispatcher` for type-safe routing |
| **Window** | Abstract `Window` interface with GLFW-backed `WindowsWindow`; cross-platform ready via `platform/<os>/` subdirectories |
| **Assertions** | `ENGINE_CORE_ASSERT(expr, msg)` — triggers debug break in Debug, stripped in Release (`NDEBUG`) |

## Dependencies

Managed via `vcpkg.json` (manifest mode):

| Package | Version | Usage |
|---|---|---|
| `spdlog` | 1.17 | Structured logging |
| `glfw3` | 3.4 | Window creation & input |
| `fmt` | 12.2 | (transitive, via spdlog) |

## Documentation

- [Event System](docs/event_system.md)
- [Window System](docs/window_system.md)