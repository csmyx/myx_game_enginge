#pragma once

#include "util.h"

namespace engine_core {
	class ENGINE_CORE_API App {
	public:
		static void init();
		static App* create_app();
	public:
		App() {};

		virtual ~App() {};
		void run();
	};
	engine_core::App* create_app();
}
