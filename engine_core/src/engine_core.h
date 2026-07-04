#pragma once

///
/// For use by engine_core applications
///

#include <GLFW/glfw3.h>

// imgui static lib inside DLL — symbols must be exported/imported
#ifdef ENGINE_CORE_EXPORTS
  #define IMGUI_API __declspec(dllexport)
#else
  #define IMGUI_API __declspec(dllimport)
#endif
#include "engine_core/imgui/imgui.h"

#include "engine_core/app.h"
#include "engine_core/entry_point.h"
#include "engine_core/layer.h"
#include "engine_core/log.h"
#include "engine_core/test.h"
#include "engine_core/window.h"


#include "engine_core/event/event.h"
#include "engine_core/event/event_dispatcher.h"
#include "engine_core/event/key_event.h"
#include "engine_core/event/mouse_event.h"
#include "engine_core/event/window_event.h"
#include "engine_core/imgui_layer.h"
