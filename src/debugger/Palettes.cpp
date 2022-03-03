#include "Palettes.hpp"

#include <glad/glad.h>

#include "../Bus.hpp"
#include "../PPU.hpp"
#include <imgui/imgui.h>


Palettes::Palettes(Debugger* debugger, Bus* bus) :
	DebugWindow("Palettes", debugger), bus(bus)
{
	glCreateTextures(GL_TEXTURE_2D, 4, backgroundPalettes.data());
	glCreateTextures(GL_TEXTURE_2D, 4, spritePalettes.data());

	for (GLuint texture : backgroundPalettes)
	{
		glTextureStorage2D(texture, 1, GL_RGB8, 4, 1);
		glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	for (GLuint texture : spritePalettes)
	{
		glTextureStorage2D(texture, 1, GL_RGB8, 4, 1);
		glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}

Palettes::~Palettes()
{
	glDeleteTextures(4, spritePalettes.data());
	glDeleteTextures(4, backgroundPalettes.data());
}

void Palettes::OnRender()
{
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(title.c_str(), &isOpen))
	{
		ImGui::End();
		return;
	}

	Color palette[4];
	palette[0] = PPU::colorTable[bus->palettes[0]];
	ImVec2 size = ImGui::GetWindowSize();
	size.x /= 2;
	size.y = size.x / 4;
	if (ImGui::CollapsingHeader("Background"))
	{
		for (int i = 0; i < 4; i++)
		{
			palette[1] = PPU::colorTable[bus->palettes[i * 4 + 1]];
			palette[2] = PPU::colorTable[bus->palettes[i * 4 + 2]];
			palette[3] = PPU::colorTable[bus->palettes[i * 4 + 3]];

			glTextureSubImage2D(backgroundPalettes[i], 0, 0, 0, 4, 1, GL_RGB, GL_UNSIGNED_BYTE, (const void*)palette);
			ImGui::Image((ImTextureID)backgroundPalettes[i], size);
		}
	}

	if (ImGui::CollapsingHeader("Sprites"))
	{
		for (int i = 0; i < 4; i++)
		{
			palette[1] = PPU::colorTable[bus->palettes[0x10 + i * 4 + 1]];
			palette[2] = PPU::colorTable[bus->palettes[0x10 + i * 4 + 2]];
			palette[3] = PPU::colorTable[bus->palettes[0x10 + i * 4 + 3]];

			glTextureSubImage2D(spritePalettes[i], 0, 0, 0, 4, 1, GL_RGB, GL_UNSIGNED_BYTE, (const void*)palette);
			ImGui::Image((ImTextureID)spritePalettes[i], size);
		}
	}

	ImGui::End();
}
