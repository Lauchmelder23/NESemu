#include "Cartridge.hpp"
#include "Bus.hpp"
#include "Log.hpp"

#include <fstream>

#include "Mapper.hpp"
#include "mappers/Mapper000.hpp"

Cartridge::Cartridge(Bus* bus) :
	bus(bus), mapper(nullptr)
{
}

Cartridge::~Cartridge()
{
	delete mapper;
}

void Cartridge::Load(std::string path)
{
	// Try to load file from disk
	std::ifstream file(path, std::ios::binary);
	if (!file)
		throw std::runtime_error("Failed to open file " + path);

	// Read header into (temporary) structure
	LOG_CORE_INFO("Extracting header");
	Header header;
	file.read((char*)&header, sizeof(Header));

	// Figure out which mapper the cartridge uses and create a mapper object
	uint8_t mapperNumber = (header.MapperHi & 0xF0) | (header.MapperLo >> 4);
	LOG_CORE_INFO("Cartridge requires Mapper {0:d}", mapperNumber);
	switch (mapperNumber)
	{
	case 0:	mapper = new Mapper000(header, file);	break;

	default:
		throw std::runtime_error("Unsupported mapper ID " + std::to_string(mapperNumber));
	}
}
