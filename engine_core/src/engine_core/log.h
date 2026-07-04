#pragma once

#include "util.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace engine_core {

#pragma warning(push)
#pragma warning(disable: 4251)

class Log {
public:
	ENGINE_CORE_API static void init();

	ENGINE_CORE_API static std::shared_ptr<spdlog::logger>& get_core_logger();
	ENGINE_CORE_API static std::shared_ptr<spdlog::logger>& get_client_logger();

private:
	static std::shared_ptr<spdlog::logger> s_core_logger;
	static std::shared_ptr<spdlog::logger> s_client_logger;
};

#define ENGINE_CORE_TRACE(...)  ::engine_core::Log::get_core_logger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::trace, __VA_ARGS__)
#define ENGINE_CORE_INFO(...)   ::engine_core::Log::get_core_logger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::info, __VA_ARGS__)
#define ENGINE_CORE_WARN(...)   ::engine_core::Log::get_core_logger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::warn, __VA_ARGS__)
#define ENGINE_CORE_ERROR(...)  ::engine_core::Log::get_core_logger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::err, __VA_ARGS__)


#define ENGINE_CLIENT_TRACE(...)  ::engine_core::Log::get_client_logger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::trace, __VA_ARGS__)
#define ENGINE_CLIENT_INFO(...)   ::engine_core::Log::get_client_logger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::info, __VA_ARGS__)
#define ENGINE_CLIENT_WARN(...)   ::engine_core::Log::get_client_logger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::warn, __VA_ARGS__)
#define ENGINE_CLIENT_ERROR(...)  ::engine_core::Log::get_client_logger()->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::err, __VA_ARGS__)


#pragma warning(pop)

} // namespace engine_core

