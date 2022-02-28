#include "PPUWatcher.hpp"

#include <imgui/imgui.h>
#include "../PPU.hpp"

PPUWatcher::PPUWatcher(PPU* ppu) :
	DebugWindow("PPU Watch"), ppu(ppu)
{
}

void PPUWatcher::OnRender()
{
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin(title.c_str(), &isOpen))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("On Pixel (%d, %d)", ppu->x, ppu->y);

	ImGui::Separator();

	if (ImGui::CollapsingHeader("PPUCTRL"))
	{
		if (ImGui::BeginTable("ppuctrl", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Base Nametable Addr");
			ImGui::TableNextColumn();
			ImGui::Text("$%04X", 0x2000 + 0x400 * ppu->ppuctrl.Flag.BaseNametableAddr);

			ImGui::TableNextColumn();
			ImGui::Text("VRAM Addr Increment");
			ImGui::TableNextColumn();
			ImGui::Text("%d", ppu->ppuctrl.Flag.VRAMAddrIncrement ? 32 : 1);

			ImGui::TableNextColumn();
			ImGui::Text("Sprite Pattern Table Addr");
			ImGui::TableNextColumn();
			ImGui::Text("$%04X", ppu->ppuctrl.Flag.SpritePatternTableAddr ? 0x1000 : 0x0000);

			ImGui::TableNextColumn();
			ImGui::Text("Backgr Pattern Table Addr");
			ImGui::TableNextColumn();
			ImGui::Text("$%04X", ppu->ppuctrl.Flag.BackgrPatternTableAddr ? 0x1000 : 0x0000);

			ImGui::TableNextColumn();
			ImGui::Text("Master/Slave");
			ImGui::TableNextColumn();
			ImGui::Text(ppu->ppuctrl.Flag.MasterSlaveSelect ? "Output to EXT" : "Read from EXT");

			ImGui::TableNextColumn();
			ImGui::Text("VBlank NMI Generation");
			ImGui::TableNextColumn();
			ImGui::Text(ppu->ppuctrl.Flag.VBlankNMI ? "On" : "Off");

			ImGui::EndTable();
		}
	}

	if (ImGui::CollapsingHeader("PPUMASK"))
	{
		if (ImGui::BeginTable("ppumask", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Greyscale");
			ImGui::TableNextColumn();
			ImGui::Text(ppu->ppumask.Flag.Greyscale ? "On" : "Off");

			ImGui::TableNextColumn();
			ImGui::Text("Left Col Background");
			ImGui::TableNextColumn();
			ImGui::Text(ppu->ppumask.Flag.BackgroundOnLeft ? "Show" : "Hide");

			ImGui::TableNextColumn();
			ImGui::Text("Left Col Sprites");
			ImGui::TableNextColumn();
			ImGui::Text(ppu->ppumask.Flag.SpriteOnLeft ? "Show" : "Hide");

			ImGui::TableNextColumn();
			ImGui::Text("Show Background");
			ImGui::TableNextColumn();
			ImGui::Text(ppu->ppumask.Flag.ShowBackground ? "On" : "Off");

			ImGui::TableNextColumn();
			ImGui::Text("Show Sprites");
			ImGui::TableNextColumn();
			ImGui::Text(ppu->ppumask.Flag.ShowSprites ? "On" : "Off");

			ImGui::TableNextColumn();
			ImGui::Text("Emphasized Colors");
			ImGui::TableNextColumn();
			ImGui::Text("%s%s%s", ppu->ppumask.Flag.EmphasizeRed ? "R" : "-", ppu->ppumask.Flag.EmphasizeGreen ? "G" : "-", ppu->ppumask.Flag.EmphasizeBlue ? "B" : "-");

			ImGui::EndTable();
		}
	}

	if (ImGui::CollapsingHeader("PPUSTATUS"))
	{
		if (ImGui::BeginTable("ppustatus", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Lower 5 Bits");
			ImGui::TableNextColumn();
			ImGui::Text("%02X", ppu->ppustatus.Flag.Unused);

			ImGui::TableNextColumn();
			ImGui::Text("Sprite Overflow");
			ImGui::TableNextColumn();
			ImGui::Text(ppu->ppustatus.Flag.SpriteOverflow ? "Yes" : "No");

			ImGui::TableNextColumn();
			ImGui::Text("Sprite 0 Hit");
			ImGui::TableNextColumn();
			ImGui::Text(ppu->ppustatus.Flag.SpriteZeroHit ? "Yes" : "No");

			ImGui::TableNextColumn();
			ImGui::Text("VBlank Started");
			ImGui::TableNextColumn();
			ImGui::Text(ppu->ppustatus.Flag.VBlankStarted ? "Yes" : "No");

			ImGui::EndTable();
		}
	}

	if (ImGui::CollapsingHeader("PPUSCROLL & PPUADDR"))
	{
		if (ImGui::BeginTable("ppuscrolladdr", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Scroll X");
			ImGui::TableNextColumn();
			ImGui::Text("%d", ppu->ppuscroll.x);

			ImGui::TableNextColumn();
			ImGui::Text("Scroll Y");
			ImGui::TableNextColumn();
			ImGui::Text("%d", ppu->ppuscroll.x);

			ImGui::TableNextColumn();
			ImGui::Text("Address");
			ImGui::TableNextColumn();
			ImGui::Text("$%04X", ppu->ppuaddr.Raw);

			ImGui::EndTable();
		}
	}

	ImGui::End();
}
