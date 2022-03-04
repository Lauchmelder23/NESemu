#include "Palettes.hpp"

#include <glad/glad.h>

#include "../Bus.hpp"
#include "../PPU.hpp"
#include <imgui/imgui.h>


Palettes::Palettes(Debugger* debugger, Bus* bus) :
	DebugWindow("Palettes", debugger), bus(bus)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &backgroundPalettes);
	glCreateTextures(GL_TEXTURE_2D, 1, &spritePalettes);

	glTextureStorage2D(backgroundPalettes, 1, GL_RGB8, 4, 4);
	glTextureParameteri(backgroundPalettes, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(backgroundPalettes, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureStorage2D(spritePalettes, 1, GL_RGB8, 4, 4);
	glTextureParameteri(spritePalettes, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(spritePalettes, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Palettes::~Palettes()
{
	glDeleteTextures(1, &spritePalettes);
	glDeleteTextures(1, &backgroundPalettes);
}

void Palettes::OnRender()
{
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(title.c_str(), &isOpen))
	{
		ImGui::End();
		return;
	}

	Color palette[4 * 4];
	

	ImVec2 size = ImGui::GetWindowSize();
	size.x /= 2;
	size.y = size.x / 4;
	if (ImGui::CollapsingHeader("Background"))
	{
		for (int i = 0; i < 4; i++)
		{
			palette[4 * i + 0] = PPU::colorTable[bus->palettes[0]];
			palette[4 * i + 1] = PPU::colorTable[bus->palettes[i * 4 + 1]];
			palette[4 * i + 2] = PPU::colorTable[bus->palettes[i * 4 + 2]];
			palette[4 * i + 3] = PPU::colorTable[bus->palettes[i * 4 + 3]];	
		}

		glTextureSubImage2D(backgroundPalettes, 0, 0, 0, 4, 4, GL_RGB, GL_UNSIGNED_BYTE, (const void*)palette);

		for(float i = 0; i < 1.0f; i += 0.25f)
			ImGui::Image((ImTextureID)backgroundPalettes, size, ImVec2(0, i), ImVec2(1, i + 0.25f));
	}

	if (ImGui::CollapsingHeader("Sprites"))
	{
		for (int i = 0; i < 4; i++)
		{
			palette[4 * i + 0] = PPU::colorTable[bus->palettes[0]];
			palette[4 * i + 1] = PPU::colorTable[bus->palettes[0x10 + i * 4 + 1]];
			palette[4 * i + 2] = PPU::colorTable[bus->palettes[0x10 + i * 4 + 2]];
			palette[4 * i + 3] = PPU::colorTable[bus->palettes[0x10 + i * 4 + 3]];
		}

		glTextureSubImage2D(spritePalettes, 0, 0, 0, 4, 4, GL_RGB, GL_UNSIGNED_BYTE, (const void*)palette);

		for (float i = 0; i < 1.0f; i += 0.25f)
			ImGui::Image((ImTextureID)spritePalettes, size, ImVec2(0, i), ImVec2(1, i + 0.25f));
	}

	ImGui::End();
}
