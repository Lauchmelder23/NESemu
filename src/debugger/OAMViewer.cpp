#include "OAMViewer.hpp"

#include "../PPU.hpp"
#include <imgui/imgui.h>

OAMViewer::OAMViewer(Debugger* debugger, PPU* ppu) :
	DebugWindow("OAM Viewer", debugger), ppu(ppu)
{
}

void OAMViewer::OnRender()
{
	if (!ImGui::Begin("OAM Viewer", &isOpen))
	{
		ImGui::End();
		return;
	}

	char label[sizeof("Sprite 00")];
	for (int i = 0; i < 64; i++)
	{
		std::sprintf(label, "Sprite %02d", i);
		if (ImGui::CollapsingHeader(label))
		{
			ImGui::Text("Y pos    : %02X", ppu->OAM[4 * i + 0]);
			ImGui::Text("Tile     : %02X", ppu->OAM[4 * i + 1]);
			ImGui::Text("Attribute: %02X", ppu->OAM[4 * i + 2]);
			ImGui::Text("X pos    : %02X", ppu->OAM[4 * i + 3]);
		}
	}

	ImGui::End();
}
