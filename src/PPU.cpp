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
	// On this cycle the VBlankStarted bit is set in the ppustatus
	if (y == 241 && x == 1)
	{
		// Set flag and send NMI if necessary
		ppustatus.Flag.VBlankStarted = 1;
		if (ppuctrl.Flag.VBlankNMI)
			bus->NMI();

		isFrameDone = true;
	}

	// This cycle resets the VBlankStarted flag
	if (y == 261 && x == 1)
		ppustatus.Flag.VBlankStarted = 0;

	// Advance pixel counters
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
	// Reading from a register fills the latch with the contents of the register
	// Write-only regs don't fill the latch
	// But in any case, the latch contents are returned
	switch (id)
	{
	case 0:
		latch = ppuctrl.Raw;
		break;

	case 1:
		latch = ppumask.Raw;
		break;

	case 2:
		latch =  ppustatus.Raw;
		ppustatus.Flag.VBlankStarted = 0;
		addressLatch = 0;
		break;

	case 5:
		break;

	case 6:
		break;

	case 7:
		latch = bus->ReadPPU(ppuaddr.Raw);
		ppuaddr.Raw += (ppuctrl.Flag.VRAMAddrIncrement ? 32 : 1);
		break;
	
	default:
		LOG_CORE_WARN("Tried to read unimplemented PPU register $20{0:02X}", (Word)id);
		break;
	}

	return latch;
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

		// PPUADDR and PPUSCROLL both take 2 accesses to fully set
		// When writing to them the address latch is switched. The latch
		// determines whether the hi or lo byte should be written next
	case 5:
		if (addressLatch == 0)
			ppuscroll.x = val;
		else
			ppuscroll.y = val;

		addressLatch = !addressLatch;
		break;

	case 6:
		if (addressLatch == 0)
			ppuaddr.Bytes.hi = val;
		else
			ppuaddr.Bytes.lo = val;

		addressLatch = !addressLatch;
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
