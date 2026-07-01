#include "test.h"
#include <print>

namespace engine_core::test {
	__declspec(dllexport) void run_tests() {
		std::println("Running tests...");
		// Add your test cases here
	}
}