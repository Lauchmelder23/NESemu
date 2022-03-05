#pragma once

#include <fstream>
#include "../Mapper.hpp"

class Mapper001 :
	public Mapper
{
public:
	Mapper001(const Header& header, std::ifstream& ifs);

	virtual Byte ReadCPU(Word addr) override;
	virtual Byte ReadPPU(Word addr) override;
	virtual void WriteCPU(Word addr, Byte val) override;
	virtual void WritePPU(Word addr, Byte val) override;
	
	virtual bool MapCIRAM(Word& addr) override;

private:
	Byte latch = 0;

	Byte shiftRegister = 0x00;
	Byte control = 0x00;
	Byte chrBank0 = 0x00;
	Byte chrBank1 = 0x00;
	Byte prgBank = 0x00;
};
