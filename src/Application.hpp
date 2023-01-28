#pragma once

class Bus;
class Window;
class Debugger;
class Screen;

/**
 * @brief Contains the program loop and invokes other objects update functions.
 */
class Application
{
public:
	/**
	 * @brief Create and launch a new application.
	 */
	static void Launch(const char* rom);

private:
	Application(const char* rom);
	~Application();

	/**
	 * @brief Update the application.
	 * This includes polling events, ticking the emulator and rendering
	 */
	bool Update();

private:
	int scale = 3;

	Window* window;
	Bus* bus;
	Screen* screen;
	Debugger* debugger;
};
