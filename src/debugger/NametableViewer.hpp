#pragma once

#include "DebugWindow.hpp"

class Bus;

class NametableViewer :
	public DebugWindow
{
public:
	NametableViewer(Debugger* debugger, Bus* bus);
	~NametableViewer();

	virtual void OnRender() override;
	
private:
	void DisplayNametable(uint8_t index);

private:
	Bus* bus;
	uint32_t texture;
	bool renderNametable = false;
};
