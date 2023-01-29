#include "Application.hpp"

#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>

#include "Log.hpp"
#include "Bus.hpp"
#include "Screen.hpp"
#include "Debugger.hpp"
#include "gfx/Window.hpp"

void Application::Launch(const char* rom)
{
	glfwInit();

	Application* app = nullptr;
	try
	{
		app = new Application(rom);
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

Application::Application(const char* rom) :
	bus(nullptr), window(nullptr)
{
	LOG_CORE_INFO("Creating window");
	try
	{
		window = new Window(256, 240, "NES Emulator");
	}
	catch (const std::runtime_error& err)
	{
		LOG_CORE_ERROR("Window creation failed");
		throw err;
	}

	LOG_CORE_INFO("Loading OpenGL API");
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::runtime_error("Failed to set up OpenGL loader");
	
	window->SetScale(scale);

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

	try
	{
		screen = new Screen;
	}
	catch(const std::runtime_error& err)
	{
		LOG_CORE_ERROR("Screen creation failed");
		throw err;
	}

	bus = new Bus(rom, screen);
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
	window->SetScale(scale);

	glfwPollEvents();

	if (!debugger->Update())
		return false;

	window->Begin();
	screen->Render();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Screen"))
		{
			if (ImGui::BeginMenu("Scale"))
			{
				ImGui::RadioButton("x1", &scale, 1);
				ImGui::RadioButton("x2", &scale, 2);
				ImGui::RadioButton("x3", &scale, 3);
				ImGui::RadioButton("x4", &scale, 4);
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	debugger->Render();
	window->End();

	std::chrono::microseconds frametime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - lastFrameTime);
	lastFrameTime = std::chrono::steady_clock::now();

	if (frametime < std::chrono::microseconds(1000000 / 60)) {
		std::this_thread::sleep_for(std::chrono::microseconds(1000000 / 60) - frametime);
	}


	return !window->ShouldClose();
}
