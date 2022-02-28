#include "Cartridge.hpp"
#include "Bus.hpp"
#include "Log.hpp"

#include <fstream>

#include "Mapper.hpp"
#include "mappers/Mapper000.hpp"

Cartridge::Cartridge(Bus* bus) :
	bus(bus)
{
}

Cartridge::~Cartridge()
{
	delete mapper;
}

Byte Cartridge::ReadCPU(Word addr)
{
	return mapper->ReadCPU(addr);
}

Byte Cartridge::ReadPPU(Word addr)
{
	return mapper->ReadPPU(addr);
}

void Cartridge::WriteCPU(Word addr, Byte val)
{
	mapper->WriteCPU(addr, val);
}

void Cartridge::WritePPU(Word addr, Byte val)
{
	mapper->WritePPU(addr, val);
}

void Cartridge::Load(std::string path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file)
		throw std::runtime_error("Failed to open file " + path);

	LOG_CORE_INFO("Extracting header");
	Header header;
	file.read((char*)&header, sizeof(Header));

	uint8_t mapperNumber = (header.MapperHi & 0xF0) | (header.MapperLo >> 4);
	LOG_CORE_INFO("Cartridge requires Mapper {0:d}", mapperNumber);
	switch (mapperNumber)
	{
	case 0:	mapper = new Mapper000(header, file);	break;

	default:
		throw std::runtime_error("Unsupported mapper ID " + std::to_string(mapperNumber));
	}
}
