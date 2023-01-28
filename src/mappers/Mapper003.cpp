#include "Mapper003.hpp"

#include <fstream>
#include "../Log.hpp"

Mapper003::Mapper003(const Header& header, std::ifstream& ifs) :
	Mapper(header)
{
	LOG_CORE_INFO("Allocating PRG ROM");
	PRG_ROM = std::vector<Byte>(0x4000 * prgBanks);
	ifs.read((char*)PRG_ROM.data(), 0x4000 * prgBanks);

	LOG_CORE_INFO("Allocating CHR ROM");
	CHR_ROM = std::vector<Byte>(0x2000 * chrBanks);
	ifs.read((char*)CHR_ROM.data(), 0x2000 * chrBanks);
}

Byte Mapper003::ReadCPU(Word addr)
{
	if (0x8000 <= addr && addr <= 0xFFFF)
	{
		return PRG_ROM[addr & (0x4000 * prgBanks - 1)];
	}

	return 0x00;
}

Byte Mapper003::ReadPPU(Word addr)
{
	if (0x0000 <= addr && addr <= 0x1FFF)
	{
		return CHR_ROM[0x2000 * selectedChrBank + addr];
	}

	return 0x00;
}

void Mapper003::WriteCPU(Word addr, Byte val)
{
	if (0x8000 <= addr && addr <= 0xFFFF)
	{
		selectedChrBank = val & 0x3;
	}
}

void Mapper003::WritePPU(Word, Byte)
{
}
