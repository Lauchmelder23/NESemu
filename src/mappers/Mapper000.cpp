#include "Mapper000.hpp"
#include "../Log.hpp"

#include <fstream>
#include "../Cartridge.hpp"

Mapper000::Mapper000(const Header& header, std::ifstream& ifs) :
	Mapper(header)
{
	LOG_CORE_INFO("Allocating PRG ROM");
	PRG_ROM = std::vector<Byte>(0x4000);
	ifs.read((char*)PRG_ROM.data(), 0x4000);

	LOG_CORE_INFO("Allocating CHR ROM");
	CHR_ROM = std::vector<Byte>(0x2000);
	ifs.read((char*)CHR_ROM.data(), 0x2000);
}

Byte Mapper000::ReadCPU(Word addr)
{
	if (0x8000 <= addr && addr <= 0xFFFF)
	{
		return PRG_ROM[addr & 0x03FFF];
	}

	return 0x00;
}

Byte Mapper000::ReadPPU(Word addr)
{
	if (0x0000 <= addr && addr <= 0x1FFF)
	{
		return CHR_ROM[addr];
	}

	return 0x00;
}

void Mapper000::WriteCPU(Word addr, Byte val)
{
}

void Mapper000::WritePPU(Word addr, Byte val)
{
}
