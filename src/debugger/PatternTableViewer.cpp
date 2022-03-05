#include "PatternTableViewer.hpp"

#include <glad/glad.h>
#include <imgui/imgui.h>
#include "Mapper000.hpp"

PatternTableViewer::PatternTableViewer(Debugger* debugger, Mapper* mapper) :
	DebugWindow("Pattern Table Viewer", debugger), mapper(mapper)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, 1, GL_RGB8, 128 * 2, 128);

	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	std::vector<Color> pixels(128 * 128);
	for (int i = 0; i < 2; i++)
	{
		DecodePatternTable(i, pixels);
		glTextureSubImage2D(texture, 0, 128 * i, 0, 128, 128, GL_RGB, GL_UNSIGNED_BYTE, (const void*)pixels.data());
	}
}

PatternTableViewer::~PatternTableViewer()
{
	glDeleteTextures(1, &texture);
}

void PatternTableViewer::OnRender()
{
	if (!ImGui::Begin("Pattern Table Viewer", &isOpen))
	{
		ImGui::End();
		return;
	}

	float smallerSize = std::min(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()) - 20.0f;
	if (smallerSize < 40.0f)
		smallerSize = 40.0f;

	if (ImGui::BeginTabBar("pattern_tables"))
	{
		if (ImGui::BeginTabItem("Table 1"))
		{
			ImGui::Image((ImTextureID)texture, ImVec2(smallerSize, smallerSize - 40.0f), ImVec2(0.0f, 0.0f), ImVec2(0.5f, 1.0f));
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Table 2"))
		{
			ImGui::Image((ImTextureID)texture, ImVec2(smallerSize, smallerSize - 40.0f), ImVec2(0.5f, 0.0f), ImVec2(1.0f, 1.0f));
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	

	ImGui::End();
}

void PatternTableViewer::DecodePatternTable(int index, std::vector<Color>& buffer)
{
	// uint8_t stride = 128;
	Word baseAddr = 0x1000 * index;
	if (baseAddr >= mapper->CHR_ROM.size())
		return;

	for (int y = 0; y < 16; y++)
	{
		for (int x = 0; x < 16; x++)
		{
			Word tileAddress = baseAddr + (16 * 16 * y) + (16 * x);
			
			for(int l = 0; l < 8; l++)
			{
				uint8_t loColor = mapper->CHR_ROM[tileAddress + l];
				uint8_t hiColor = mapper->CHR_ROM[tileAddress + 8 + l];

				for (int k = 0; k < 8; k++)
				{
					uint8_t color = ((loColor & 0x80) >> 7) | ((hiColor & 0x80) >> 6);
					color *= 80;
					buffer[(y * 8 + l) * 128 + (x * 8 + k)] = Color{ color, color, color };

					loColor <<= 1;
					hiColor <<= 1;
				}
			}
		}
	}
}
