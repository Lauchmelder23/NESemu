#include "Mapper001.hpp"

Mapper001::Mapper001(const Header& header, std::ifstream& ifs) :
	Mapper(header)
{
	LOG_CORE_INFO("Allocating PRG ROM");
	PRG_ROM = std::vector<Byte>(0x4000 * prgBanks);
	ifs.read((char*)PRG_ROM.data(), 0x4000 * prgBanks);

	LOG_CORE_INFO("Allocating CHR ROM");
	CHR_ROM = std::vector<Byte>(0x2000 * chrBanks);
	ifs.read((char*)CHR_ROM.data(), 0x2000 * chrBanks);
}

Byte Mapper001::ReadCPU(Word addr)
{
	if (0x8000 <= addr && addr <= 0xFFFF)
	{
		Byte selectedBank = prgBank;
		Byte prgControl = (control >> 2) & 0x3;

		switch (prgControl)
		{
		case 0:
			selectedBank &= ~0x1;
			break;

		case 1:
			if (addr < 0xC000)
				selectedBank = 1;
			break;

		case 2:
			if (addr >= 0xC000)
				selectedBank = prgBanks;
			break;
		}

		return PRG_ROM[addr & (0x4000 * selectedBank - 1)];
	}

	return 0x00;
}

Byte Mapper001::ReadPPU(Word addr)
{
	if (0x0000 <= addr && addr <= 0x1FFF)
	{
		Byte selectedBank = 0x00;
		Byte chrControl = (control >> 4) & 0x1;

		if (chrControl)
		{
			if (addr < 0x1000)
				selectedBank = chrBank0;
			else
				selectedBank = chrBank1;
		}
		else
		{
			selectedBank = chrBank0 & ~0x1;
		}

		return CHR_ROM[addr & (0x1000 * selectedBank - 1)];
	}

	return 0x00;
}

void Mapper001::WriteCPU(Word addr, Byte val)
{
	if (addr <= 0x8000 && addr <= 0xFFFF)
	{
		if ((val & 0x80) == 0x80)
		{
			shiftRegister = 0x00;
			control |= 0x0C;
			return;
		}

		latch++;
		shiftRegister >>= 1;
		shiftRegister |= (val & 0x1) << 4;

		if (latch == 5)
		{
			Byte registerSelect = (addr & 0x6000) >> 13;
			switch (registerSelect)
			{
			case 0:	control		= shiftRegister; break;
			case 1:	chrBank0	= shiftRegister; break;
			case 2:	chrBank1	= shiftRegister; break;
			case 3:	prgBank		= shiftRegister; break;
			}

			shiftRegister = 0x00;
			latch = 0;
		}
	}
}

void Mapper001::WritePPU(Word, Byte)
{
}

bool Mapper001::MapCIRAM(Word& addr)
{
	if ((control & 0x3) < 2)
	{
		addr = 0x2000 + (control & 0x3) * 0x400;
		return false;
	}

	if ((control & 0x3) == 0x3)
	{
		// Shift Bit 11 into Bit 10
		addr &= ~(1 << 10);
		addr |= ((addr & (1 << 11)) >> 1);
	}

	// Unset bit 11
	addr &= ~(1 << 11);

	return false;
}
