#pragma once

#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#ifdef ENGINE_CORE_EXPORTS
#define ENGINE_CORE_API __declspec(dllexport)
#else
#define ENGINE_CORE_API __declspec(dllimport)
#endif
#else
#error "Unsupported platform"
#endif

#ifdef _MSC_VER
#define ENGINE_DEBUG_BREAK() __debugbreak()
#else
#define ENGINE_DEBUG_BREAK() __builtin_trap()
#endif

namespace engine_core {
[[noreturn]] inline void assert_fail(const char* expr, const char* msg,
    const char* file, int line)
{
    std::fprintf(stderr, "[ENGINE ASSERT] %s:%d: %s (%s)\n", file, line, msg,
        expr);
    ENGINE_DEBUG_BREAK();
    std::abort();
}
} // namespace engine_core

#ifdef NDEBUG
#define ENGINE_CORE_ASSERT(expr, msg) ((void)0)
#else
#define ENGINE_CORE_ASSERT(expr, msg) \
    ((expr) ? (void)0                 \
            : ::engine_core::assert_fail(#expr, msg, __FILE__, __LINE__))
#endif
