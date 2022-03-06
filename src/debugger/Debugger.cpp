#include "Debugger.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "../Bus.hpp"
#include "../Log.hpp"
#include "CPUWatcher.hpp"
#include "PPUWatcher.hpp"
#include "Disassembler.hpp"
#include "MemoryViewer.hpp"
#include "NametableViewer.hpp"
#include "OAMViewer.hpp"
#include "PatternTableViewer.hpp"
#include "ControllerPortViewer.hpp"
#include "Palettes.hpp"

Debugger::Debugger(Bus* bus) :
	bus(bus)
{
	windows.push_back(new CPUWatcher(this, &bus->cpu));

	ppuWatcher = new PPUWatcher(this, &bus->ppu);
	windows.push_back(ppuWatcher);

	disassembler = new Disassembler(this, &bus->cpu);
	windows.push_back(disassembler);

	windows.push_back(new MemoryViewer(this, bus));
	windows.push_back(new NametableViewer(this, bus));
	windows.push_back(new OAMViewer(this, &bus->ppu));
	windows.push_back(new PatternTableViewer(this, bus->cartridge.GetMapper()));
	windows.push_back(new ControllerPortViewer(this, &bus->controllerPort));
	windows.push_back(new Palettes(this, bus));
}

Debugger::~Debugger()
{
	for (DebugWindow* window : windows)
		delete window;
}

bool Debugger::Frame()
{
	try
	{
		while (!bus->ppu.IsFrameDone())
		{
			bus->Tick();
			if (disassembler->BreakpointHit() || ppuWatcher->BreakpointHit())
			{
				running = false;
				break;
			}
		}
	}
	catch (const std::runtime_error& err)
	{
		LOG_CORE_FATAL("Fatal Bus error: {0}", err.what());
		bus->cpu.Halt();
		return true;
	}
}

bool Debugger::Update()
{
	if (running)
		return Frame();

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

		if (ImGui::Button("PPU Tick"))
			bus->PPUTick();

		ImGui::SameLine();

		if (ImGui::Button("Single Step"))
			bus->Instruction();

		ImGui::SameLine();

		if (ImGui::Button("Single Frame"))
			Frame();

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
