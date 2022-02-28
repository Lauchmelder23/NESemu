#pragma once

#include "DebugWindow.hpp"

class PPU;

class PPUWatcher :
	public DebugWindow
{
public:
	PPUWatcher(PPU* ppu);

	virtual void OnRender() override;

private:
	PPU* ppu;
};
