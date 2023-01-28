#include "PPUWatcher.hpp"

#include <map>
#include <imgui/imgui.h>
#include "../PPU.hpp"

static const std::map<ScanlineType, std::string> scanlineTypeNames = {
	{ScanlineType::PreRender,	"Pre-render"},
	{ScanlineType::Visible,		"Visible"},
	{ScanlineType::PostRender,	"Post-render"},
	{ScanlineType::VBlank,		"VBlank"}
};

static const std::map<CycleType, std::string> cycleTypeNames = {
	{CycleType::Idle,				"Idle"},
	{CycleType::Fetching,			"Fetching BG Tiles"},
	{CycleType::SpriteFetching,		"Sprite Fetching"},
	{CycleType::PreFetching,		"Pre-Fetching"},
	{CycleType::UnknownFetching,	"Random Fetches"}
};

static const std::map<FetchingPhase, std::string> fetchingPhaseNames = {
	{FetchingPhase::NametableByte,		"Nametable"},
	{FetchingPhase::AttributeTableByte,	"Attribute Table"},
	{FetchingPhase::PatternTableLo,		"Pattern Table (lo)"},
	{FetchingPhase::PatternTableHi,		"Pattern Table (hi)"},
};

PPUWatcher::PPUWatcher(Debugger* debugger, PPU* ppu) :
	DebugWindow("PPU Watch", debugger), ppu(ppu)
{
	breakpoints.emplace_back(ScanlineType::PreRender,	"Pre-Render");
	breakpoints.emplace_back(ScanlineType::Visible,		"Visible");
	breakpoints.emplace_back(ScanlineType::PostRender,	"Post-Render");
	breakpoints.emplace_back(ScanlineType::VBlank,		"VBlank");
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
	ImGui::Text("Scanline      : %s", scanlineTypeNames.find(ppu->scanlineType)->second.c_str());
	ImGui::Text("Cycle         : %s", cycleTypeNames.find(ppu->cycleType)->second.c_str());
	ImGui::Text("Fetching Phase: %s", fetchingPhaseNames.find(ppu->fetchPhase)->second.c_str());

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Internal Registers"))
	{
		ImGui::Text("Current VRAM Address (v)  : %02X ($%04X)", ppu->current.Raw, 0x2000 | (ppu->current.Raw & 0x0FFF));
		if (ImGui::TreeNode("Breakdown (v)"))
		{
			ImGui::Text("Coarse X : %02X", ppu->current.Data.CoarseX);
			ImGui::Text("Coarse Y : %02X", ppu->current.Data.CoarseY);
			ImGui::Text("Nametable: %02X", ppu->current.Data.NametableSel);
			ImGui::Text("Fine Y   : %02X", ppu->current.Data.FineY);

			ImGui::TreePop();
		}

		ImGui::Text("Temporary VRAM Address (t): %02X ($%04X)", ppu->temporary.Raw, 0x2000 | (ppu->current.Raw & 0x0FFF));
		if (ImGui::TreeNode("Breakdown (t)"))
		{
			ImGui::Text("Coarse X : %02X", ppu->temporary.Data.CoarseX);
			ImGui::Text("Coarse Y : %02X", ppu->temporary.Data.CoarseY);
			ImGui::Text("Nametable: %02X", ppu->temporary.Data.NametableSel);
			ImGui::Text("Fine Y   : %02X", ppu->temporary.Data.FineY);

			ImGui::TreePop();
		}

		ImGui::Text("Fine X Scroll             : %02X", ppu->fineX);
		ImGui::Text("Write Toggle Bit          : %02X", ppu->addressLatch);
	}

	if (ImGui::CollapsingHeader("Latches"))
	{
		ImGui::Text("Nametable Byte : %02X", ppu->nametableByte);
		ImGui::Text("Attribute Byte : %02X", ppu->attributeTableByte);
		ImGui::Text("Pattern Tile Lo: %02X", ppu->patternTableLo);
		ImGui::Text("Pattern Tile Hi: %02X", ppu->patternTableHi);
	}

	if (ImGui::CollapsingHeader("Shift Registers"))
	{
		ImGui::Text("Pattern Tile Lo: %04X", ppu->loTile.Raw);
		ImGui::Text("Pattern Tile Hi: %02X", ppu->hiTile.Raw);
		ImGui::Text("Attribute Lo   : %02X", ppu->loAttribute.Raw);
		ImGui::Text("Attribute Hi   : %02X", ppu->hiAttribute.Raw);
	}

	if (ImGui::CollapsingHeader("Registers"))
	{
		if (ImGui::TreeNode("PPUCTRL"))
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
				ImGui::Text("Sprite Size");
				ImGui::TableNextColumn();
				ImGui::Text(ppu->ppuctrl.Flag.SpriteSize ? "8x16" : "8x8");

				ImGui::TableNextColumn();
				ImGui::Text("Master/Slave");
				ImGui::TableNextColumn();
				ImGui::Text(ppu->ppuctrl.Flag.MasterSlaveSelect ? "Output to EXT" : "Read from EXT");

				ImGui::TableNextColumn();
				ImGui::Text("VBlank NMI Generation");
				ImGui::TableNextColumn();
				ImGui::Text(ppu->ppuctrl.Flag.VBlankNMI ? "On" : "Off");

				ImGui::EndTable();
				ImGui::TreePop();
			}
		}

		if (ImGui::TreeNode("PPUMASK"))
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
				ImGui::TreePop();
			}
		}

		if (ImGui::TreeNode("PPUSTATUS"))
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
				ImGui::TreePop();
			}
		}

		if (ImGui::TreeNode("PPUSCROLL & PPUADDR"))
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
				ImGui::TreePop();
			}
		}
	}

	if (ImGui::CollapsingHeader("Breakpoints"))
	{
		ImGui::Text("Break at beginning of");

		for (FrameStateBreakpoint& breakpoint : breakpoints)
		{
			ImGui::Checkbox(breakpoint.name.c_str(), &breakpoint.enabled);
		}
	}

	ImGui::End();
}

bool PPUWatcher::BreakpointHit()
{
	if (ppu->x > 2)
		return false;

	for (const FrameStateBreakpoint& breakpoint : breakpoints)
	{
		if (breakpoint.enabled && breakpoint.location == ppu->scanlineType)
			return true;
	}

	return false;
}
