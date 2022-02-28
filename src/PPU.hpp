#pragma once

#include "Types.hpp"

class Bus;

class PPU
{
	friend class PPUWatcher;

public:
	PPU(Bus* bus);

	void Powerup();
	void Reset();

	void Tick();

	Byte ReadRegister(Byte id);
	void WriteRegister(Byte id, Byte val);

	inline bool IsFrameDone() { bool returnVal = isFrameDone; isFrameDone = false; return returnVal; }

private:
	Byte Read(Word addr);
	void Write(Word addr, Byte val);

private: // Registers

	union
	{
		struct
		{
			Byte BaseNametableAddr : 2;
			Byte VRAMAddrIncrement : 1;
			Byte SpritePatternTableAddr : 1;
			Byte BackgrPatternTableAddr : 1;
			Byte SpriteSize : 1;
			Byte MasterSlaveSelect : 1;
			Byte VBlankNMI : 1;
		} Flag;

		Byte Raw;
	} ppuctrl;

	union
	{
		struct
		{
			Byte Greyscale : 1;
			Byte BackgroundOnLeft : 1;
			Byte SpriteOnLeft : 1;
			Byte ShowBackground : 1;
			Byte ShowSprites : 1;
			Byte EmphasizeRed : 1;
			Byte EmphasizeGreen : 1;
			Byte EmphasizeBlue : 1;
		} Flag;

		Byte Raw;
	} ppumask;

	union
	{
		struct
		{
			Byte Unused : 5;
			Byte SpriteOverflow : 1;
			Byte SpriteZeroHit : 1;
			Byte VBlankStarted : 1;
		} Flag;

		Byte Raw;
	} ppustatus;

	struct
	{
		Byte x;
		Byte y;
	} ppuscroll;

	Address ppuaddr;

	uint16_t x, y;
	Byte addressLatch = 0;

private:
	bool isFrameDone = false;
	Bus* bus;
};
