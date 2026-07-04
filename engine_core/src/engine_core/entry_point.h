#pragma once

#include "app.h"
#include "log.h"

extern engine_core::App* create_app();

#ifdef _WIN32
int main()
{
    engine_core::Log::init();
    ENGINE_CORE_WARN("Starting Game Engine...");
    auto* app = create_app();
    ENGINE_CORE_ASSERT(app, "create_app() returned nullptr");
    app->run();
    delete app;
    return 0;
}
#else
#error "Unsupported platform"
#endif