#include "Application.hpp"
#include "Log.hpp"

int main()
{
	Log::Init();
	Application::Launch();

	return 0;
}
