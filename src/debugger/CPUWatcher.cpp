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

	if (ImGui::CollapsingHeader("Registers", ImGuiTreeNodeFlags_DefaultOpen))
	{

		ImGui::InputScalar("A", ImGuiDataType_U8, &cpu->acc, (const void*)0, (const void*)0, "%02X", ImGuiInputTextFlags_CharsHexadecimal);
		ImGui::InputScalar("X", ImGuiDataType_U8, &cpu->idx, (const void*)0, (const void*)0, "%02X", ImGuiInputTextFlags_CharsHexadecimal);
		ImGui::InputScalar("Y", ImGuiDataType_U8, &cpu->idy, (const void*)0, (const void*)0, "%02X", ImGuiInputTextFlags_CharsHexadecimal);
		ImGui::InputScalar("PC", ImGuiDataType_U16, &cpu->pc, (const void*)0, (const void*)0, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
		ImGui::InputScalar("SP", ImGuiDataType_U8, &cpu->sp, (const void*)0, (const void*)0, "%04X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
		ImGui::InputScalar("P", ImGuiDataType_U8, &cpu->status.Raw, (const void*)0, (const void*)0, "%04X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_ReadOnly);
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