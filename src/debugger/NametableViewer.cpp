#include "NametableViewer.hpp"

#include <glad/glad.h>
#include <imgui/imgui.h>
#include "../Bus.hpp"

NametableViewer::NametableViewer(Debugger* debugger, Bus* bus) :
	DebugWindow("Nametable Viewer", debugger), bus(bus), texture(0)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, 1, GL_R8, 32, 32);

	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

NametableViewer::~NametableViewer()
{
	glDeleteTextures(1, &texture);
}

void NametableViewer::OnRender()
{
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(title.c_str(), &isOpen, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Views"))
		{
			ImGui::MenuItem("Rendered View", NULL, &renderNametable);

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::BeginTabBar("Nametables");
	for (uint8_t index = 0; index < 2; index++)
	{
		char baseAddress[12];
		std::sprintf(baseAddress, "Nametable %c", 'A' + index);
		if (ImGui::BeginTabItem(baseAddress))
		{
			DisplayNametable(index);
			if (renderNametable)
			{
				glTextureSubImage2D(texture, 0, 0, 0, 32, 32, GL_RED, GL_UNSIGNED_BYTE, &bus->VRAM[0x400 * index]);
			}

			ImGui::EndTabItem();
		}

	}
	ImGui::EndTabBar();

	if (renderNametable)
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Rendered Nametable", &renderNametable))
		{
			ImGui::End();
			return;
		}

		

		float smallerSize = std::min(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()) - 20.0f;
		if (smallerSize < 40.0f)
			smallerSize = 40.0f;

		ImGui::Image((ImTextureID)texture, ImVec2{smallerSize, smallerSize - 20.0f});
		ImGui::End();
	}

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
