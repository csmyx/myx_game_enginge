#include "app.h"
#include "log.h"

void engine_core::App::init()
{
	Log::init();
}



void engine_core::App::run() {
	init();
	Log::get_core_logger()->info("Running the application...");
}