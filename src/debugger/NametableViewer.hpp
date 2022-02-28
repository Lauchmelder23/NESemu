#pragma once

#include "DebugWindow.hpp"

class Bus;

class NametableViewer :
	public DebugWindow
{
public:
	NametableViewer(Bus* bus);

	virtual void OnRender() override;
	
private:
	void DisplayNametable(uint8_t index);

private:
	Bus* bus;
};
