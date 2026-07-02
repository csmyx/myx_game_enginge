#include <engine_core.h>
#include <print>

class MyApp : public engine_core::App {
public:
	MyApp() {
		ENGINE_CLIENT_INFO("MyApp constructed.");
	}
	~MyApp() {
		ENGINE_CLIENT_INFO("MyApp destructed.");
	}
};

engine_core::App* create_app()
{
	return new MyApp();
}

