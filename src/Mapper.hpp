#pragma once

#include <vector>
#include "Types.hpp"

class Mapper
{
	friend class Disassembler;

public:
	virtual Byte ReadCPU(Word addr) = 0;
	virtual Byte ReadPPU(Word addr) = 0;
	virtual void WriteCPU(Word addr, Byte val) = 0;
	virtual void WritePPU(Word addr, Byte val) = 0;

protected:
	Mapper() = default;

protected:
	std::vector<Byte> PRG_ROM;
	std::vector<Byte> CHR_ROM;
};
