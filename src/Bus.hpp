#pragma once

#include <vector>

#include "Types.hpp"
#include "CPU.hpp"
#include "PPU.hpp"
#include "APU.hpp"
#include "Cartridge.hpp"
#include "ControllerPort.hpp"

/**
 * @brief The main bus for hardware to communicate.
 * 
 * This is not a realistic representation of the NES's bus, as the
 * CPU and PPU don't share a bus in the real world. However this
 * bus class still doesn't give the CPU/PPU direct access to the other
 * components address space.
 */
class Bus
{
	friend class Debugger;
	friend class MemoryViewer;
	friend class NametableViewer;
	friend class Palettes;

public:
	Bus(const char* rom, Screen* screen);

	/**
	 * @brief Reboot the NES.
	 * 
	 * This equates to turning the NES off and on again.
	 * Internal state will be equal to the powerup state
	 */
	void Reboot();

	/**
	 * @brief Resets the NES.
	 *
	 * Internal state will be equal to the reset state
	 */
	void Reset();

	/**
	 * @brief Advance the emulator by one CPU cycle (and 3 PPU cycles).
	 */
	uint8_t Tick();

	void DMATick();

	void PPUTick();

	/**
	 * @brief Advance the emulator by one CPU instruction.
	 */
	bool Instruction();

	/**
	 * @brief Advance the emulator by one Frame.
	 * The emulator runs until the PPU triggers VBlankStart
	 */
	bool Frame();

	/**
	 * @brief Read call from the CPU
	 */
	Byte ReadCPU(Word addr);

	/**
	 * @brief Read call from the PPU
	 */
	Byte ReadPPU(Word addr);

	/**
	 * @brief Write call from the CPU
	 */
	void WriteCPU(Word addr, Byte val);

	/**
	 * @brief Write call from the PPU
	 */
	void WritePPU(Word addr, Byte val);

	/**
	 * @brief Lets the PPU trigger NMIs.
	 */
	inline void NMI() { cpu.NMI(); }
	inline void IRQ() { cpu.IRQ(); }

private:
	std::vector<Byte> RAM, VRAM;
	std::vector<Byte> palettes;
	CPU cpu;
	PPU ppu;
	APU apu;
	Cartridge cartridge;
	ControllerPort controllerPort;

	Byte preDMACycles = 0;
	Word DMACyclesLeft = 0;
	Byte DMAPage = 0;
	Byte DMALatch = 0;

	uint8_t ppuClock = 0;
};
