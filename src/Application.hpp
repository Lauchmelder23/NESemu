#pragma once

class Bus;
class Window;
class Debugger;

/**
 * @brief Contains the program loop and invokes other objects update functions.
 */
class Application
{
public:
	/**
	 * @brief Create and launch a new application.
	 */
	static void Launch();

private:
	Application();
	~Application();

	/**
	 * @brief Update the application.
	 * This includes polling events, ticking the emulator and rendering
	 */
	bool Update();

private:
	Window* window;
	Bus* bus;
	Debugger* debugger;
};
