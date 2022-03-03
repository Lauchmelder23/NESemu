#include "PPU.hpp"
#include "Log.hpp"
#include "Bus.hpp"

#include "gfx/Screen.hpp"

PPU::PPU(Bus* bus, Screen* screen) :
	bus(bus), screen(screen), ppuctrl{ 0 }, ppustatus{ 0 }
{
	LOG_CORE_INFO("{0}", sizeof(VRAMAddress));
}

void PPU::Powerup()
{
	ppuctrl.Raw		= 0b00000000;
	ppumask.Raw		= 0b00000000;
	ppustatus.Raw	= 0b10100000;
	ppuscroll.x		= 0x00;
	ppuscroll.y		= 0x00;
	ppuaddr.Raw		= 0x0000;

	// This will cause the Tick() functioon to set both to 0 on the first tick
	x = 400;
	y = 400;
	fineX = 0;
	addressLatch = 0;

	scanlineType = ScanlineType::Visible;
	cycleType = CycleType::Idle;
	fetchPhase = FetchingPhase::NametableByte;
}

void PPU::Reset()
{
	ppuctrl.Raw = 0b00000000;
	ppumask.Raw = 0b00000000;
	ppuscroll.x = 0x00;
	ppuscroll.y = 0x00;

	// This will cause the Tick() functioon to set both to 0 on the first tick
	x = 400;
	y = 400;
	fineX = 0;
	addressLatch = 0;

	scanlineType = ScanlineType::Visible;
	cycleType = CycleType::Idle;
	fetchPhase = FetchingPhase::NametableByte;
}

void PPU::Tick()
{
	// Advance pixel counters
	x++;
	if (x > 340)
	{
		x = 0;
		y++;
		if (y > 261)
			y = 0;
	}

	UpdateState();

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


	// Need to render
	if (scanlineType == ScanlineType::Visible || scanlineType == ScanlineType::PreRender)
	{
		PerformRenderAction();
	}
	
	if (x < 256 && y < 240)
	{
		uint8_t xOffset = 7 - fineX;

		uint8_t color = (((patternTableHi >> xOffset) & 0x1) << 1) | ((patternTableLo >> xOffset) & 0x1);
		color *= 80;
		screen->SetPixel(x, y, { color, color, color});
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
		temporary.NametableSel = ppuctrl.Flag.BaseNametableAddr;
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
		{
			ppuscroll.x = val;
			temporary.CoarseX = (val >> 3);
			fineX = val & 0x3;
		}
		else
		{
			ppuscroll.y = val;
			temporary.CoarseY = (val >> 3);
			temporary.FineY = val & 0x3;
		}

		addressLatch = !addressLatch;
		break;

	case 6:
		if (addressLatch == 0)
		{
			ppuaddr.Bytes.hi = val;
			temporary.Raw &= 0xFF;
			temporary.Raw |= ((Word)(val & 0x3F) << 8);
			temporary.FineY &= 0x3;
		}
		else
		{
			ppuaddr.Bytes.lo = val;
			temporary.Raw &= ((Word)0xFF << 8);
			temporary.Raw |= val;

			current.Raw = temporary.Raw;
		}

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

void PPU::UpdateState()
{
	switch (y)
	{
	case 0:
		scanlineType = ScanlineType::Visible;
		break;

	case 240:
		scanlineType = ScanlineType::PostRender;
		break;

	case 241:
		scanlineType = ScanlineType::VBlank;
		break;

	case 261:
		scanlineType = ScanlineType::PreRender;
		break;
	}

	switch (x)
	{
	case 0:
		cycleType = CycleType::Idle;
		break;

	case 1:
		cycleType = CycleType::Fetching;
		break;

	case 257:
		cycleType = CycleType::SpriteFetching;
		break;

	case 321:
		cycleType = CycleType::PreFetching;
		break;

	case 337:
		cycleType = CycleType::UnknownFetching;
		break;
	}
}

void PPU::PerformRenderAction()
{
	if (cycleType == CycleType::Idle)
	{
		fineX = 0;
		return;
	}

	if (cycleType == CycleType::SpriteFetching)
		return;

	if (memoryAccessLatch == 1)
	{
		switch (fetchPhase)
		{
		case FetchingPhase::NametableByte:
			nametableByte = Read(0x2000 | (current.Raw & 0x0FFF));

			if(cycleType != CycleType::UnknownFetching)
				fetchPhase = FetchingPhase::AttributeTableByte;
			break;

		case FetchingPhase::AttributeTableByte:
			attributeTableByte = Read(0x23C0 | (current.Raw & 0x0FFF) | ((current.Raw >> 4) & 0x38) | ((current.Raw >> 2) & 0x07));
			fetchPhase = FetchingPhase::PatternTableLo;
			break;

		case FetchingPhase::PatternTableLo:
			patternTableLo = Read(((Word)ppuctrl.Flag.BackgrPatternTableAddr << 12) | ((Word)nametableByte << 4) + current.FineY);
			fetchPhase = FetchingPhase::PatternTableHi;
			break;

		case FetchingPhase::PatternTableHi:
			patternTableLo = Read((((Word)ppuctrl.Flag.BackgrPatternTableAddr << 12) | ((Word)nametableByte << 4)) + 8 + current.FineY);

			current.CoarseX++;
			if (x == 256)
			{
				current.CoarseX = temporary.CoarseX;
				current.NametableSel ^= 0x1;

				if (current.FineY < 7)
				{
					current.FineY++;
				}
				else
				{
					current.FineY = temporary.FineY;
					if (current.CoarseY == 29)
					{
						current.CoarseY = temporary.CoarseY;
						current.NametableSel ^= 0x2;
					}
					else if (current.CoarseY == 31)
					{
						current.CoarseY = temporary.CoarseY;
					}
					else
					{
						current.CoarseY++;
					}
				}
			}

			fetchPhase = FetchingPhase::NametableByte;
			break;
		}
	}

	fineX++;
	if (fineX >= 8)
		fineX = 0;
	memoryAccessLatch = 1 - memoryAccessLatch;
}
