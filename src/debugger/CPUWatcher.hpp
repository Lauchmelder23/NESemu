#pragma once

#include "DebugWindow.hpp"

class CPU;

class CPUWatcher :
	public DebugWindow
{
public:
	CPUWatcher(CPU* cpu);

	virtual void OnRender() override;

private:
	CPU* cpu;
};
