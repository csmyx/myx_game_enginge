#pragma once

#include "util.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace engine_core {

#pragma warning(push)
#pragma warning(disable: 4251)

class ENGINE_CORE_API Log {
public:
	static void init();

	static std::shared_ptr<spdlog::logger>& get_core_logger();
	static std::shared_ptr<spdlog::logger>& get_client_logger();

private:
	static std::shared_ptr<spdlog::logger> s_core_logger;
	static std::shared_ptr<spdlog::logger> s_client_logger;
};

#pragma warning(pop)

} // namespace engine_core

