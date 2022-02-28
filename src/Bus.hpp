#pragma once

#include <vector>

#include "Types.hpp"
#include "CPU.hpp"
#include "PPU.hpp"
#include "Cartridge.hpp"

class Bus
{
	friend class Debugger;
	friend class MemoryViewer;
	friend class NametableViewer;

public:
	Bus();

	void Reboot();
	void Reset();

	uint8_t Tick();
	bool Instruction();
	bool Frame();

	Byte ReadCPU(Word addr);
	Byte ReadPPU(Word addr);
	void WriteCPU(Word addr, Byte val);
	void WritePPU(Word addr, Byte val);

	inline void NMI() { cpu.NMI(); }

private:
	std::vector<Byte> RAM, VRAM;
	CPU cpu;
	PPU ppu;
	Cartridge cartridge;
};
