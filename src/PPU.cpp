#include "PPU.hpp"
#include "Log.hpp"
#include "Bus.hpp"

#include "gfx/Screen.hpp"

const std::vector<Color> PPU::colorTable = {
	{84,	84,		84 },
	{0,		30,		116 },
	{8,		16,		144 },
	{48,	0,		136 },
	{68,	0,		100 },
	{92,	0,		48 },
	{84,	4,		0 },
	{60,	24,		0 },
	{32,	42,		0 },
	{8,		58,		0 },
	{0,		64,		0 },
	{0,		60,		0 },
	{0,		50,		60 },
	{0,		0,		0 },
	{0,		0,		0 },
	{0,		0,		0 },

	{152,	150,	152 },
	{8,		76,		196 },
	{48,	50,		236 },
	{92,	30,		228 },
	{136,	20,		176 },
	{160,	20,		100 },
	{152,	34,		32 },
	{120,	60,		0 },
	{84,	90,		0 },
	{40,	114,	0 },
	{8,		124,	0 },
	{0,		118,	40 },
	{0,		102,	120 },
	{0,		0,		0 },
	{0,		0,		0 },
	{0,		0,		0 },

	{236,	238,	236 },
	{76,	154,	236 },
	{120,	124,	236 },
	{176,	98,		236 },
	{228,	84,		236 },
	{236,	88,		180 },
	{236,	106,	100 },
	{212,	136,	32 },
	{160,	170,	0 },
	{116,	196,	0 },
	{76,	208,	32 },
	{56,	204,	108 },
	{56,	180,	204 },
	{60,	60,		60 },
	{0,		0,		0 },
	{0,		0,		0 },

	{236,	238,	236 },
	{168,	204,	236 },
	{188,	188,	236 },
	{212,	178,	236 },
	{236,	174,	236 },
	{236,	174,	212 },
	{236,	180,	176 },
	{228,	196,	144 },
	{204,	210,	120 },
	{180,	222,	120 },
	{168,	226,	144 },
	{152,	226,	180 },
	{160,	214,	228 },
	{160,	162,	160 },
	{0,		0,		0 },
	{0,		0,		0 }
};

PPU::PPU(Bus* bus, Screen* screen) :
	bus(bus), screen(screen), ppuctrl{ 0 }, ppustatus{ 0 }
{
	OAM = std::vector<Byte>(64 * 4, 0);
	secondaryOAM = std::vector<Byte>(8 * 4, 0);
	sprites = std::vector<Sprite>(8, { 0 });
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
	{
		ppustatus.Flag.VBlankStarted = 0;
		ppustatus.Flag.SpriteZeroHit = 0;
	}

	// Need to render
	if (scanlineType == ScanlineType::Visible || scanlineType == ScanlineType::PreRender)
	{
		EvaluateBackgroundTiles();
		EvaluateSprites();

		if (x == 257)
		{
			current.CoarseX = temporary.CoarseX;
			current.NametableSel &= 0x2;
			current.NametableSel |= temporary.NametableSel & 0x1;
		}

		if (scanlineType == ScanlineType::PreRender && ppumask.Flag.ShowBackground && x >= 280 && x <= 304)
		{
			current.FineY = temporary.FineY;
			current.CoarseY = temporary.CoarseY;
			current.NametableSel &= 0x1;
			current.NametableSel |= temporary.NametableSel & 0x2;
		}

	}
	
	if (x < 256 && y < 240)
	{
		Pixel bgPixel = GetBackgroundPixel();
		Pixel spritePixel = GetSpritePixel();

		Color pixel = MultiplexPixel(bgPixel, spritePixel);
		screen->SetPixel(x, y, pixel);
	}

	if (cycleType == CycleType::Fetching || cycleType == CycleType::PreFetching)
	{
		loTile.Raw <<= 1;
		hiTile.Raw <<= 1;
		loAttribute.Raw <<= 1;
		hiAttribute.Raw <<= 1;
	}
}

Byte PPU::ReadRegister(Byte id)
{
	Byte data = 0;

	// Reading from a register fills the latch with the contents of the register
	// Write-only regs don't fill the latch
	// But in any case, the latch contents are returned
	switch (id)
	{
	case 0:
		data = ppuctrl.Raw;
		break;

	case 1:
		data = ppumask.Raw;
		break;

	case 2:
		data = ppustatus.Raw;
		ppustatus.Flag.VBlankStarted = 0;
		addressLatch = 0;
		break;

	case 3:
		break;

	case 4:
		data = ReadOAM(oamaddr++);
		break;

	case 5:
		break;

	case 6:
		break;

	case 7:
		data = latch;
		latch = bus->ReadPPU(ppuaddr.Raw);

		if (ppuaddr.Raw >= 0x3F00)
			data = latch;

		ppuaddr.Raw += (ppuctrl.Flag.VRAMAddrIncrement ? 32 : 1);
		break;
	
	default:
		LOG_CORE_WARN("Tried to read unimplemented PPU register $20{0:02X}", (Word)id);
		break;
	}

	return data;
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

	case 3:
		oamaddr = val;
		break;

	case 4:
		WriteOAM(oamaddr++, val);
		break;

		// PPUADDR and PPUSCROLL both take 2 accesses to fully set
		// When writing to them the address latch is switched. The latch
		// determines whether the hi or lo byte should be written next
	case 5:
		if (addressLatch == 0)
		{
			ppuscroll.x = val;
			temporary.CoarseX = (val >> 3);
			fineX = val & 0x7;
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

Byte PPU::ReadOAM(Byte offset)
{
	return OAM[offset] | OAMOverrideSignal;
}

void PPU::WriteOAM(Byte offset, Byte val)
{
	OAM[offset] = val;
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

void PPU::EvaluateBackgroundTiles()
{
	if (cycleType == CycleType::Idle)
	{
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
			attributeTableByte = Read(0x23C0 | (current.Raw & 0x0C00) | ((current.CoarseY >> 2) << 3) | (current.CoarseX >> 2));
			fetchPhase = FetchingPhase::PatternTableLo;
			break;

		case FetchingPhase::PatternTableLo:
			patternTableLo = Read(((Word)ppuctrl.Flag.BackgrPatternTableAddr << 12) | ((Word)nametableByte << 4) + current.FineY);
			fetchPhase = FetchingPhase::PatternTableHi;
			break;

		case FetchingPhase::PatternTableHi:
			patternTableHi = Read((((Word)ppuctrl.Flag.BackgrPatternTableAddr << 12) | ((Word)nametableByte << 4)) + 8 + current.FineY);

			loTile.Lo = patternTableLo;
			hiTile.Lo = patternTableHi;

			Byte attributeHalfNybble = attributeTableByte;
			attributeHalfNybble >>= (((current.CoarseX >> 1) % 2) ? 2 : 0);
			attributeHalfNybble >>= (((current.CoarseY >> 1) % 2) ? 4 : 0);

			loAttribute.Lo = ((attributeHalfNybble & 1) ? 0xFF : 0x00);
			hiAttribute.Lo = ((attributeHalfNybble & 2) ? 0xFF : 0x00);

			current.CoarseX++;
			if (x == 256)
			{
				current.FineY++;
				if (current.FineY == 0)
				{
					if (current.CoarseY == 29)
					{
						current.CoarseY = 0;
						current.NametableSel ^= 0x2;
					}
					else if (current.CoarseY == 31)
					{
						current.CoarseY = 0;
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

	memoryAccessLatch = 1 - memoryAccessLatch;
}

void PPU::EvaluateSprites()
{
	if (cycleType == CycleType::Idle)
	{
		currentlyEvaluatedSprite = 0x00;
		return;
	}

	if (scanlineType != ScanlineType::PreRender)
	{
		OAMOverrideSignal = (x <= 64);

		// Secondary OAM clear
		if (x < 64)
		{
			secondaryOAM[x >> 1] = ReadOAM(0x00);
			return;
		}

		// Sprite evaluation (just do it all at once)
		if (x == 65)
		{
			freeSecondaryOAMSlot = 0x00;

			Byte n;
			for (n = 0; n < 64; n++)
			{
				if (oamaddr + 4 * n >= 64 * 4)
					break;

				Byte spriteYCoord = ReadOAM(oamaddr + 4 * n);
				if (freeSecondaryOAMSlot >= 32)
					break;

				// Find free slot
				secondaryOAM[freeSecondaryOAMSlot] = spriteYCoord;
				if (y - spriteYCoord < 8 + ppuctrl.Flag.SpriteSize * 8)	// Choose between 8x8 and 8x16 mode
				{
					secondaryOAM[freeSecondaryOAMSlot + 1] = ReadOAM(4 * n + 1);
					secondaryOAM[freeSecondaryOAMSlot + 2] = ReadOAM(4 * n + 2);
					secondaryOAM[freeSecondaryOAMSlot + 3] = ReadOAM(4 * n + 3);

					freeSecondaryOAMSlot += 4;
				}

				
			}

			Byte m = 0;
			for (; n < 64; n++)
			{
				Byte spriteYCoord = ReadOAM(4 * n + m);
				if (spriteYCoord < 240)
					ppustatus.Flag.SpriteOverflow = 1;
				else
					m = (m + 1) % 4;	// Correctly implement sprite overflow bug
			}

			return;
		}
	}

	if (cycleType == CycleType::SpriteFetching)
	{
		oamaddr = 0;

		// Sprite tile fetches
		if (memoryAccessLatch == 1)
		{
			switch (fetchPhase)
			{
			case FetchingPhase::NametableByte:	// Fetch garbage
				nametableByte = Read(0x2000 | (current.Raw & 0x0FFF));
				sprites[currentlyEvaluatedSprite].Counter = secondaryOAM[4 * currentlyEvaluatedSprite + 3];
				sprites[currentlyEvaluatedSprite].FineX = 0;

				fetchPhase = FetchingPhase::AttributeTableByte;
				break;

			case FetchingPhase::AttributeTableByte:	// Fetch garbage
				attributeTableByte = Read(0x23C0 | (current.Raw & 0x0C00) | ((current.CoarseY >> 2) << 3) | (current.CoarseX >> 2));
				sprites[currentlyEvaluatedSprite].Latch = secondaryOAM[4 * currentlyEvaluatedSprite + 2];

				fetchPhase = FetchingPhase::PatternTableLo;
				break;

			case FetchingPhase::PatternTableLo:
			{
				Byte spriteFineY = y - secondaryOAM[4 * currentlyEvaluatedSprite];
				Byte tileNumber = secondaryOAM[4 * currentlyEvaluatedSprite + 1] + (spriteFineY >> 3);

				sprites[currentlyEvaluatedSprite].Lo = Read(((Word)ppuctrl.Flag.SpritePatternTableAddr << 12) | (tileNumber << 4) + spriteFineY);

				fetchPhase = FetchingPhase::PatternTableHi;
			} break;

			case FetchingPhase::PatternTableHi:
			{
				Byte spriteFineY = y - secondaryOAM[4 * currentlyEvaluatedSprite];
				Byte tileNumber = secondaryOAM[4 * currentlyEvaluatedSprite + 1] + (spriteFineY >> 3);

				sprites[currentlyEvaluatedSprite].Hi = Read(((Word)ppuctrl.Flag.SpritePatternTableAddr << 12) | (tileNumber << 4) + 8 + spriteFineY);
				
				if (currentlyEvaluatedSprite * 4 >= freeSecondaryOAMSlot)
				{
					sprites[currentlyEvaluatedSprite].Lo = 0x00;
					sprites[currentlyEvaluatedSprite].Hi = 0x00;
				}
				
				currentlyEvaluatedSprite++;

				fetchPhase = FetchingPhase::NametableByte;
			} break;
			}
		}

		memoryAccessLatch = 1 - memoryAccessLatch;
	}
}

Pixel PPU::GetBackgroundPixel()
{
	Pixel returnValue{ 0 };
	returnValue.color = 0x00;
	returnValue.palette = 0;

	if (!ppumask.Flag.ShowBackground)
		return returnValue;

	Byte shiftAmount = 7 - fineX;

	Byte loBit = (loTile.Hi >> shiftAmount) & 0x1;
	Byte hiBit = (hiTile.Hi >> shiftAmount) & 0x1;
	Byte loAttrBit = (loAttribute.Hi >> shiftAmount) & 0x1;
	Byte hiAttrBit = (hiAttribute.Hi >> shiftAmount) & 0x1;;

	returnValue.color = (hiBit << 1) | loBit;
	returnValue.palette = (hiAttrBit << 1) | loAttrBit;
	if (returnValue.color == 0x00)
		returnValue.palette = 0x00;

	return returnValue;
}

Pixel PPU::GetSpritePixel()
{
	for (Sprite& sprite : sprites)
	{
		if (sprite.Counter != 0)
		{
			sprite.Counter--;
			continue;
		}

		if (sprite.FineX != 8)
			sprite.FineX++;

	}

	Pixel returnValue{ 0 };
	returnValue.color = 0x00;
	returnValue.palette = 4;
	returnValue.priority = 1;

	if (!ppumask.Flag.ShowSprites)
		return returnValue;

	bool firstIteration = true;
	for (Sprite& sprite : sprites)
	{
		// Sprite is inacitve
		if (sprite.Counter != 0 || sprite.FineX >= 8)
			continue;
		
		// If sprite is active, determine the current pixel
		Byte loBit = (sprite.Hi & 0x80) >> 7;
		Byte hiBit = (sprite.Hi & 0x80) >> 7;

		uint8_t color = (hiBit << 1) | loBit;
		if (color == 0x00)
		{
			firstIteration = false;
			continue;
		}

		uint8_t palette = 4 + (sprite.Latch & 0x3);
		if (color == 0x00)
			palette = 0x00;

		returnValue.color = color;
		returnValue.palette = palette;
		returnValue.priority = (sprite.Latch >> 5) & 0x1;
		returnValue.isZeroSprite = firstIteration;

		break;
	}

	return returnValue;
}

Color PPU::MultiplexPixel(Pixel background, Pixel sprite)
{
	if (background.color == 0)
	{
		if (sprite.color == 0)
			return colorTable[Read(0x3F00)];

		else
			return colorTable[Read(0x3F00 | (sprite.palette << 2) | sprite.color)];
	}
	else
	{
		if (sprite.color == 0)
			return colorTable[Read(0x3F00 | (background.palette << 2) | background.color)];

		else
		{
			// Sprite Zero hit detection
			if (sprite.isZeroSprite)
			{
				// All of the conditions that make sprite zero hits not evaluate
				if (!(
					(!ppustatus.Flag.SpriteZeroHit) &&
					(x == 255) &&
					(!ppumask.Flag.ShowBackground || !ppumask.Flag.ShowSprites) &&
					((!ppumask.Flag.SpriteOnLeft || !ppumask.Flag.BackgroundOnLeft) && 0 <= x && x <= 7)
				)) ppustatus.Flag.SpriteZeroHit = 1;
			}

			if(sprite.priority == 0)
				return colorTable[Read(0x3F00 | (sprite.palette << 2) | sprite.color)];

			else
				return colorTable[Read(0x3F00 | (background.palette << 2) | background.color)];
		}
	}
}
