#pragma once

#include "DebugWindow.hpp"
#include "../Types.hpp"

class Bus;

class MemoryViewer :
	public DebugWindow
{
public:
	MemoryViewer(Debugger* debugger, Bus* bus);

	virtual void OnRender() override;

private:
	void DrawPage(Byte page);

private:
	Bus* bus;
};
