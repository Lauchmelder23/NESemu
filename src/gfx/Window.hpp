#pragma once

#include <cstdint>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window(uint16_t width, uint16_t height, const std::string& title);
	~Window();

	inline bool ShouldClose() { return glfwWindowShouldClose(handle); }
	inline GLFWwindow* GetNativeWindow() { return handle; }
	
	void SetScale(int scale);

	void Begin();
	void End();

private:
	GLFWwindow* handle;
};
