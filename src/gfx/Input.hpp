#pragma once

#include <memory>
#include <GLFW/glfw3.h>

class Window;

class Input
{
public:
	static inline void SetWindow(Window* source) { window = source; }
	static bool IsKeyDown(int key);

private:
	static Window* window;
};
