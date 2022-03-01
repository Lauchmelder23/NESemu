#pragma once

#include "DebugWindow.hpp"

class CPU;

class CPUWatcher :
	public DebugWindow
{
public:
	CPUWatcher(Debugger* debugger, CPU* cpu);

	virtual void OnRender() override;

private:
	CPU* cpu;
};
