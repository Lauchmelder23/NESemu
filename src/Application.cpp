#include "Application.hpp"

#include <stdexcept>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>

#include "Log.hpp"
#include "Bus.hpp"
#include "Debugger.hpp"
#include "gfx/Window.hpp"

void Application::Launch()
{
	glfwInit();

	Application* app = nullptr;
	try
	{
		app = new Application;
	}
	catch (const std::runtime_error& err)
	{
		LOG_CORE_FATAL(err.what());
		delete app;
		glfwTerminate();
		return;
	}

	if (app == nullptr)
	{
		LOG_CORE_ERROR("Application object is nullptr");
		glfwTerminate();
		return;
	}

	while (app->Update());
	delete app;
	glfwTerminate();
}

Application::Application() :
	bus(nullptr), window(nullptr)
{
	LOG_CORE_INFO("Creating window");
	try
	{
		window = new Window(1280, 720, "NES Emulator");
	}
	catch (const std::runtime_error& err)
	{
		LOG_CORE_ERROR("Window creation failed");
		throw err;
	}

	LOG_CORE_INFO("Loading OpenGL API");
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::runtime_error("Failed to set up OpenGL loader");
	
	LOG_CORE_INFO("Setting up ImGui");
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplGlfw_InitForOpenGL(window->GetNativeWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460 core");

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	bus = new Bus;
	debugger = new Debugger(bus);
}

Application::~Application()
{
	delete debugger;

	if(bus)
		delete bus;

	delete window;
}

bool Application::Update()
{
	glfwPollEvents();

	if (!debugger->Update())
		return false;

	window->Begin();
	debugger->Render();
	window->End();

	return !window->ShouldClose();
}
