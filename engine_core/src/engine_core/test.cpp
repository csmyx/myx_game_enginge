#include "test.h"
#include "util.h"
#include <print>

namespace engine_core::test {
	void run_tests() {
		auto logger = spdlog::stdout_color_mt("console");
		spdlog::set_default_logger(logger);
		spdlog::set_level(spdlog::level::debug);

		spdlog::info("Hello spdlog from solution common vcpkg manifest");
		spdlog::debug("调试日志：数字={}", 123);
		std::println("Running tests...");
		// Add your test cases here
	}
}