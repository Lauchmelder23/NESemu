#include "NametableViewer.hpp"

#include <imgui/imgui.h>
#include "../Bus.hpp"

NametableViewer::NametableViewer(Bus* bus) :
	DebugWindow("Nametable Viewer"), bus(bus)
{
}

void NametableViewer::OnRender()
{
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(title.c_str(), &isOpen))
	{
		ImGui::End();
		return;
	}

	ImGui::BeginTabBar("Nametables");
	for (uint8_t index = 0; index < 4; index++)
	{
		char baseAddress[6];
		std::sprintf(baseAddress, "$2%X00", index * 4);
		if (ImGui::BeginTabItem(baseAddress))
		{
			DisplayNametable(index);
			ImGui::EndTabItem();
		}

	}
	ImGui::EndTabBar();

	ImGui::End();
}

void NametableViewer::DisplayNametable(uint8_t index)
{
	Word baseAddr = 0x400 * index;
	Word displayBaseAddr = 0x2000 + baseAddr;
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

		for (Word hi = 0x0; hi <= 0x3F; hi++)
		{
			Byte hiOffset = hi << 4;
			ImGui::TableNextColumn();

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
			ImGui::Text("%04X", displayBaseAddr | hiOffset);
			ImGui::PopStyleColor();

			for (Byte lo = 0x0; lo <= 0xF; lo++)
			{
				ImGui::TableNextColumn();

				Byte entry = bus->VRAM[baseAddr | hiOffset | lo];

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
