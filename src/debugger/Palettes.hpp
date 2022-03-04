#pragma once

#include "DebugWindow.hpp"

class Bus;

class Palettes :
	public DebugWindow
{
public:
	Palettes(Debugger* debugger, Bus* bus);
	~Palettes();

	virtual void OnRender() override;

private:
	Bus* bus;

	uint32_t backgroundPalettes;
	uint32_t spritePalettes;
};
