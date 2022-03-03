#pragma once

#include <array>
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

	std::array<uint32_t, 4> backgroundPalettes;
	std::array<uint32_t, 4> spritePalettes;
};
