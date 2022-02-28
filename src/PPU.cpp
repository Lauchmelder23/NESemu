#include "PPU.hpp"
#include "Log.hpp"
#include "Bus.hpp"

PPU::PPU(Bus* bus) :
	bus(bus), ppuctrl{0}, ppustatus{0}
{
}

void PPU::Powerup()
{
	ppuctrl.Raw		= 0b00000000;
	ppumask.Raw		= 0b00000000;
	ppustatus.Raw	= 0b10100000;
	ppuscroll.x		= 0x00;
	ppuscroll.y		= 0x00;
	ppuaddr.Raw		= 0x0000;

	x = 0;
	y = 0;
	addressLatch = 0;
}

void PPU::Reset()
{
	ppuctrl.Raw = 0b00000000;
	ppumask.Raw = 0b00000000;
	ppuscroll.x = 0x00;
	ppuscroll.y = 0x00;

	x = 0;
	y = 0;
	addressLatch = 1;
}

void PPU::Tick()
{
	if (y == 241 && x == 1)
	{
		ppustatus.Flag.VBlankStarted = 1;
		if (ppuctrl.Flag.VBlankNMI)
			bus->NMI();

		isFrameDone = true;
	}

	if (y == 261 && x == 1)
		ppustatus.Flag.VBlankStarted = 0;

	x++;
	if (x > 340)
	{
		x = 0;
		y++;
		if (y > 261)
			y = 0;
	}
}

Byte PPU::ReadRegister(Byte id)
{
	Byte returnVal = 0x00;

	switch (id)
	{
	case 0:
		returnVal = ppuctrl.Raw;
		break;

	case 1:
		returnVal = ppumask.Raw;
		break;

	case 2:
		returnVal =  ppustatus.Raw;
		ppustatus.Flag.VBlankStarted = 0;
		addressLatch = 0;
		break;

	case 5:
		returnVal = 0x00;
		break;

	case 6:
		returnVal = 0x00;
		break;

	case 7:
		returnVal = bus->ReadPPU(ppuaddr.Raw);
		ppuaddr.Raw += (ppuctrl.Flag.VRAMAddrIncrement ? 32 : 1);
		break;
	
	default:
		LOG_CORE_WARN("Tried to read unimplemented PPU register $20{0:02X}", (Word)id);
		break;
	}

	return returnVal;
}

void PPU::WriteRegister(Byte id, Byte val)
{
	switch (id)
	{
	case 0:
		ppuctrl.Raw = val;
		break;

	case 1:
		ppumask.Raw = val;
		break;

	case 2:
		ppustatus.Raw = val;
		break;

	case 5:
		if (addressLatch == 0)
			ppuscroll.x = val;
		else
			ppuscroll.y = val;

		addressLatch = 1 - addressLatch;
		break;

	case 6:
		if (addressLatch == 0)
			ppuaddr.Bytes.hi = val;
		else
			ppuaddr.Bytes.lo = val;

		addressLatch = 1 - addressLatch;
		break;

	case 7:
		bus->WritePPU(ppuaddr.Raw, val);
		ppuaddr.Raw += (ppuctrl.Flag.VRAMAddrIncrement ? 32 : 1);
		break;

	default:
		LOG_CORE_WARN("Tried to write unimplemented PPU register $20{0:02X}", (Word)id);
		return;
	}

	ppustatus.Flag.Unused = val & 0x1F;
}

Byte PPU::Read(Word addr)
{
	return bus->ReadPPU(addr);
}

void PPU::Write(Word addr, Byte val)
{
	bus->WritePPU(addr, val);
}
