#include <engine_core.h>
#include <print>

class MyApp : public engine_core::App {
public:
	MyApp() {
		std::println("MyApp constructor");
	}
	~MyApp() {
		std::println("MyApp destructor");
	}
};

engine_core::App* create_app()
{
	return new MyApp();
}

