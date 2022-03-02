#pragma once

#include <vector>
#include "DebugWindow.hpp"
#include "../PPU.hpp"

struct FrameStateBreakpoint
{
	FrameStateBreakpoint(ScanlineType location, const std::string& name) :
		location(location), name(name), enabled(false)
	{ }

	ScanlineType location;
	std::string name;
	bool enabled;
};

class PPUWatcher :
	public DebugWindow
{
public:
	PPUWatcher(Debugger* debugger, PPU* ppu);

	virtual void OnRender() override;
	bool BreakpointHit();

private:
	PPU* ppu;
	std::vector<FrameStateBreakpoint> breakpoints;
};
