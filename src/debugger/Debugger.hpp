#pragma once

#include <vector>
#include "DebugWindow.hpp"

class Bus;
class Disassembler;
class PPUWatcher;

class Debugger
{
public:
	Debugger(Bus* bus);
	~Debugger();

	bool Frame();
	bool Update();
	void Render();

public:
	bool isOpen = true;

private:
	Bus* bus;
	bool running = false;
	bool overrideResetVector = false;
	uint16_t resetVector = 0x0000;

	std::vector<DebugWindow*> windows;
	Disassembler* disassembler;
	PPUWatcher* ppuWatcher;
};
