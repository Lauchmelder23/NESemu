#include "Application.hpp"
#include "Log.hpp"

int main(int argc, char** argv)
{
	Log::Init();
	
	if (argc != 2) {
		LOG_CORE_FATAL("Usage: {0} <rom>", argv[0]);
		return -1;
	}

	Application::Launch(argv[1]);

	return 0;
}
