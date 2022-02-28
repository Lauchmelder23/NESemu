#pragma once

class Bus;
class Window;
class Debugger;

class Application
{
public:
	static void Launch();

private:
	Application();
	~Application();

	bool Update();

private:
	Window* window;
	Bus* bus;
	Debugger* debugger;
};
