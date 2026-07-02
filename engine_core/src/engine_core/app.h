#pragma once

#include "util.h"

namespace engine_core {
	class ENGINE_CORE_API App {
	public:
		App() {};
		virtual ~App() {};
		void run();
	};
}
