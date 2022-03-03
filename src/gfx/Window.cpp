#include "Window.hpp"

#include <stdexcept>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>

#include "Input.hpp"

Window::Window(uint16_t width, uint16_t height, const std::string& title) :
	handle(nullptr)
{
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (handle == nullptr)
	{
		const char* err;
		int code = glfwGetError(&err);
		throw std::runtime_error(std::string(err) + " (" + std::to_string(code) + ")");
	}

	glfwMakeContextCurrent(handle);
	Input::SetWindow(this);
}

Window::~Window()
{
	if(handle)
		glfwDestroyWindow(handle);
}

void Window::SetScale(int scale)
{
	glfwSetWindowSize(handle, 256 * scale, 240 * scale + 20);
	glViewport(0, 0, 256 * scale, 240 * scale);
}

void Window::Begin()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::End()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}

	glfwSwapBuffers(handle);
}
