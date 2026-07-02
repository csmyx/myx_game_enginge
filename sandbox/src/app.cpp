#include <engine_core.h>

class MyApp : public engine_core::App {
};

int main() {
	engine_core::test::run_tests();
	MyApp app;
	app.run();
	return 0;
}
