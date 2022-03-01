#include "MemoryViewer.hpp"

#include <imgui/imgui.h>
#include "../Bus.hpp"

MemoryViewer::MemoryViewer(Debugger* debugger, Bus* bus) :
	DebugWindow("Memory Viewer", debugger), bus(bus)
{
}

void MemoryViewer::OnRender()
{
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin(title.c_str(), &isOpen))
	{
		ImGui::End();
		return;
	}

	ImGui::BeginTabBar("Pages");

	for (Byte page = 0; page < 8; page++)
	{
		char title[7];
		std::sprintf(title, "Page %d", page);
		if (ImGui::BeginTabItem(title))
		{
			DrawPage(page);
			ImGui::EndTabItem();
		}
	}

	ImGui::EndTabBar();
	ImGui::End();
}

void MemoryViewer::DrawPage(Byte page)
{
	Word baseAddr = ((Word)page << 8);
	if (ImGui::BeginTable("memorymap", 17))
	{
		ImGui::TableNextColumn();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
		for (Byte header = 0x0; header < 0x10; header++)
		{
			ImGui::TableNextColumn();
			ImGui::Text("%X", header);
		}
		ImGui::PopStyleColor();

		for (Byte hi = 0x0; hi <= 0xF; hi++)
		{
			Byte hiOffset = hi << 4;
			ImGui::TableNextColumn();

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
			ImGui::Text("%04X", baseAddr | hiOffset);
			ImGui::PopStyleColor();

			for (Byte lo = 0x0; lo <= 0xF; lo++)
			{
				ImGui::TableNextColumn();

				Byte entry = bus->RAM[baseAddr | hiOffset | lo];

				if (entry == 0x00)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));

				ImGui::Text("%02X", entry);

				if (entry == 0x00)
					ImGui::PopStyleColor();
			}
		}
		ImGui::EndTable();
	}
}
