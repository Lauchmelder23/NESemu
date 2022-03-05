#include "Cartridge.hpp"
#include "Bus.hpp"
#include "Log.hpp"

#include <fstream>

#include "Mapper.hpp"
#include "mappers/Mapper000.hpp"
#include "mappers/Mapper001.hpp"
#include "mappers/Mapper003.hpp"

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
	uint8_t mapperNumber = (header.Flag7.MapperHi << 4) | header.Flag6.MapperLo;
	LOG_CORE_INFO("Cartridge requires Mapper {0:d}", mapperNumber);
	switch (mapperNumber)
	{
	case 0:	mapper = new Mapper000(header, file);	break;
	case 1:	mapper = new Mapper001(header, file);	break;
	case 3:	mapper = new Mapper003(header, file);	break;

	default:
		throw std::runtime_error("Unsupported mapper ID " + std::to_string(mapperNumber));
	}
}
