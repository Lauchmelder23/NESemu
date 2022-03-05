#pragma once

#include <fstream>
#include "../Mapper.hpp"

struct Header;

class Mapper003 :
	public Mapper
{
public:
	Mapper003(const Header& header, std::ifstream& ifs);

	virtual Byte ReadCPU(Word addr) override;
	virtual Byte ReadPPU(Word addr) override;
	virtual void WriteCPU(Word addr, Byte val) override;
	virtual void WritePPU(Word addr, Byte val) override;

private:
	Byte selectedChrBank = 0;
};
