#include "Window.hpp"
#include "Input.hpp"

Window* Input::window = nullptr;

bool Input::IsKeyDown(int key)
{
	return (glfwGetKey(window->GetNativeWindow(), key) == GLFW_PRESS);
}
