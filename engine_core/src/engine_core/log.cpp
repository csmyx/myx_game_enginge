#include "log.h"

namespace engine_core {

std::shared_ptr<spdlog::logger> Log::s_core_logger;
std::shared_ptr<spdlog::logger> Log::s_client_logger;

void Log::init() {
	s_core_logger = spdlog::stdout_color_mt("ENGINE");
	s_core_logger->set_level(spdlog::level::trace);
	s_core_logger->set_pattern("%^[%T] %n: %v%$");

	s_client_logger = spdlog::stdout_color_mt("APP");
	s_client_logger->set_level(spdlog::level::trace);
	s_client_logger->set_pattern("%^[%T] %n: %v%$");

	ENGINE_CORE_INFO("Log initialized.");
}

std::shared_ptr<spdlog::logger>& Log::get_core_logger() {
	return s_core_logger;
}

std::shared_ptr<spdlog::logger>& Log::get_client_logger() {
	return s_client_logger;
}

} // namespace engine_core
