#pragma once

#include <string>
#include <memory>

#include "Types.hpp"
#include "Mapper.hpp"

class Bus;

struct Header
{
	Byte Signature[4];
	Byte PrgROM;
	Byte ChrROM;
	Byte MapperLo;
	Byte MapperHi;
	Byte PrgRAM;
	Byte TV1;
	Byte TV2;
	Byte Padding[5];
};

class Cartridge
{
public:
	Cartridge(Bus* bus);
	~Cartridge();

	Byte ReadCPU(Word addr);
	Byte ReadPPU(Word addr);
	void WriteCPU(Word addr, Byte val);
	void WritePPU(Word addr, Byte val);

	void Load(std::string path);

	inline Mapper* GetMapper() { return mapper; }

private:
	Mapper* mapper;
	Bus* bus;
};
