#include "Application.hpp"
#include "Log.hpp"

int main(int argc, char** argv)
{
	Log::Init();
	Application::Launch();

	return 0;
}
