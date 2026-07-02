#pragma once

#include "app.h"
#include <print>

extern engine_core::App* create_app();

int main() {
	//engine_core::test::run_tests();
	auto *app = create_app();
	app->run();
	delete app;
	return 0;
}
