#pragma once

#include <vector>
#include <fstream>
#include "../Mapper.hpp"

struct Header;

class Mapper000 :
	public Mapper
{
public:
	Mapper000(const Header& header, std::ifstream& ifs);

	virtual Byte ReadCPU(Word addr) override;
	virtual Byte ReadPPU(Word addr) override;
	virtual void WriteCPU(Word addr, Byte val) override;
	virtual void WritePPU(Word addr, Byte val) override;
};
