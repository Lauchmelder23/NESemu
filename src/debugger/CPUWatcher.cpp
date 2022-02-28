#include "CPUWatcher.hpp"

#include <iomanip>

#include <imgui/imgui.h>
#include "../CPU.hpp"

CPUWatcher::CPUWatcher(CPU* cpu) :
	DebugWindow("CPU Watch"), cpu(cpu)
{
}

void CPUWatcher::OnRender()
{
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(title.c_str(), &isOpen))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Registers");
	if (ImGui::BeginTable("Registers", 6))
	{
		ImGui::TableNextColumn();
		ImGui::Text("A");	
		ImGui::TableNextColumn();
		ImGui::Text("X");	
		ImGui::TableNextColumn();
		ImGui::Text("Y");	
		ImGui::TableNextColumn();
		ImGui::Text("PC");	
		ImGui::TableNextColumn();
		ImGui::Text("SP");
		ImGui::TableNextColumn();
		ImGui::Text("P");
		ImGui::TableNextColumn();
		ImGui::Text("%02X", cpu->acc);	
		ImGui::TableNextColumn();
		ImGui::Text("%02X", cpu->idx);	
		ImGui::TableNextColumn();
		ImGui::Text("%02X", cpu->idy);
		ImGui::TableNextColumn();
		ImGui::Text("%04X", cpu->pc);	
		ImGui::TableNextColumn();
		ImGui::Text("%02X", cpu->sp);
		ImGui::TableNextColumn();
		ImGui::Text("%02X", cpu->status.Raw);
	
		ImGui::EndTable();
	}

	ImGui::Separator();
	ImGui::Text("Status Flag Breakdown");
	if (ImGui::BeginTable("Status", 8))
	{
		ImGui::TableNextColumn();
		ImGui::Text("N");
		ImGui::TableNextColumn();
		ImGui::Text("V");
		ImGui::TableNextColumn();
		ImGui::Text("U");
		ImGui::TableNextColumn();
		ImGui::Text("B");
		ImGui::TableNextColumn();
		ImGui::Text("D");
		ImGui::TableNextColumn();
		ImGui::Text("I");
		ImGui::TableNextColumn();
		ImGui::Text("Z");
		ImGui::TableNextColumn();
		ImGui::Text("C");


		ImGui::TableNextColumn();
		ImGui::Text(cpu->status.Flag.Negative ? "1" : "-");
		ImGui::TableNextColumn();
		ImGui::Text(cpu->status.Flag.Overflow ? "1" : "-");
		ImGui::TableNextColumn();
		ImGui::Text(cpu->status.Flag.NoEffect ? "1" : "-");
		ImGui::TableNextColumn();
		ImGui::Text(cpu->status.Flag.Break ? "1" : "-");
		ImGui::TableNextColumn();
		ImGui::Text(cpu->status.Flag.Decimal ? "1" : "-");
		ImGui::TableNextColumn();
		ImGui::Text(cpu->status.Flag.InterruptDisable ? "1" : "-");
		ImGui::TableNextColumn();
		ImGui::Text(cpu->status.Flag.Zero ? "1" : "-");
		ImGui::TableNextColumn();
		ImGui::Text(cpu->status.Flag.Carry ? "1" : "-");
		ImGui::TableNextColumn();

		ImGui::EndTable();
	}

	ImGui::Separator();

	ImGui::Text("Halted: %s", cpu->halted ? "Yes" : "No");

	ImGui::End();
}