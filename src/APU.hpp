#pragma once

#include "Types.hpp"

class Bus;

class APU
{
public:
	APU(Bus* bus);

	void Powerup();
	void Reset();

	void Tick();

	void WriteRegister(Word addr, Byte val);

private:
	uint64_t sequencer = 0;
	bool mode = false;
	bool disableInterrupt = false;

	Byte cycles = 0;
	bool APUActionLatch = true;

private:
	Bus* bus;
};
