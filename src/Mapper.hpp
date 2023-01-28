#pragma once

#include <vector>
#include "../Log.hpp"
#include "Types.hpp"

class Mapper
{
	friend class Disassembler;
	friend class PatternTableViewer;

public:
	virtual ~Mapper() {}

	virtual Byte ReadCPU(Word addr) = 0;
	virtual Byte ReadPPU(Word addr) = 0;
	virtual void WriteCPU(Word addr, Byte val) = 0;
	virtual void WritePPU(Word addr, Byte val) = 0;

	/**
	 * The cartridge actually controls the PPUs access to VRAM.
	 * It can modify the effective address the PPU accesses in order to
	 * enforce nametable mirroring, or even completely remap the address
	 * to internal VRAM
	 */
	virtual bool MapCIRAM(Word& addr)
	{
		if (header.Flag6.IgnoreMirroringBit)
			return true;

		if (header.Flag6.Mirroring == 0x0)
		{
			// Shift Bit 11 into Bit 10
			addr &= ~(1 << 10);
			addr |= ((addr & (1 << 11)) >> 1);
		}

		// Unset bit 11
		addr &= ~(1 << 11);

		return false;
	}

	virtual Byte ReadVRAM(Word) { return 0x00; }
	virtual void WriteVRAM(Word, Byte) {}

protected:
	Mapper(const Header& header) : header(header), prgBanks(header.PrgROM), chrBanks(header.ChrROM)
	{
	}

protected:
	std::vector<Byte> PRG_ROM;
	std::vector<Byte> CHR_ROM;
	Byte prgBanks = 0;
	Byte chrBanks = 0;
	Header header;
};
