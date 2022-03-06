#pragma once

#include "DebugWindow.hpp"

class PPU;

class OAMViewer :
	public DebugWindow
{
public:
	OAMViewer(Debugger* debugger, PPU* ppu);

	virtual void OnRender() override;

private:
	PPU* ppu;
};
