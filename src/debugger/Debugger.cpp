#include "Debugger.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "../Bus.hpp"
#include "CPUWatcher.hpp"
#include "PPUWatcher.hpp"
#include "Disassembler.hpp"
#include "MemoryViewer.hpp"
#include "NametableViewer.hpp"

Debugger::Debugger(Bus* bus) :
	bus(bus)
{
	windows.push_back(new CPUWatcher(&bus->cpu));
	windows.push_back(new PPUWatcher(&bus->ppu));
	windows.push_back(new Disassembler(&bus->cpu));
	windows.push_back(new MemoryViewer(bus));
	windows.push_back(new NametableViewer(bus));
}

Debugger::~Debugger()
{
	for (DebugWindow* window : windows)
		delete window;
}

bool Debugger::Update()
{
	if (running)
		return bus->Frame();

	return true;
}

void Debugger::Render()
{
	ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Debugger", (bool*)0, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Tools"))
		{
			for (DebugWindow* window : windows)
			{
				ImGui::MenuItem(window->title.c_str(), NULL, &window->isOpen);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Controls", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, running);
		if (running)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.7f, 0.7f, 0.7f, 0.7f });

		if (ImGui::Button("Single Step"))
			bus->Instruction();

		ImGui::SameLine();

		if (ImGui::Button("Single Frame"))
			bus->Frame();

		if (running)
			ImGui::PopStyleColor();
		ImGui::PopItemFlag();

		ImGui::SameLine();

		if (ImGui::Button(running ? "Pause" : "Run"))
			running = !running;

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, running);
		if (running)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.7f, 0.7f, 0.7f, 0.7f });

		if (ImGui::Button("Reset"))
		{
			bus->Reset();
			if (overrideResetVector)
				bus->cpu.pc.Raw = resetVector;
		}

		ImGui::SameLine();

		if (ImGui::Button("Reboot"))
		{
			bus->Reboot();
			if (overrideResetVector)
				bus->cpu.pc.Raw = resetVector;
		}

		if (running)
			ImGui::PopStyleColor();
		ImGui::PopItemFlag();


		ImGui::Separator();

		ImGui::Text("Override Reset Vector: ");
		ImGui::SameLine();
		if (ImGui::Button(overrideResetVector ? "Yes" : "No"))
			overrideResetVector = !overrideResetVector;

		ImGui::InputScalar("Reset Vector", ImGuiDataType_U16, &resetVector, (const void*)0, (const void*)0, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
	}

	for (DebugWindow* window : windows)
	{
		if (window->isOpen) window->OnRender();
	}

	ImGui::End();
}
