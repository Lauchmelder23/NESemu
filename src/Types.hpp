#pragma once

#include <cstdint>

using Byte = uint8_t;
using Word = uint16_t;

union Address
{
	struct
	{
		Byte lo;
		Byte hi;
	} Bytes;

	Word Raw;
};

/** 
 * @brief iNES ROM header.
 */
struct Header
{
	Byte Signature[4];
	Byte PrgROM;
	Byte ChrROM;

	struct
	{
		Byte Mirroring : 1;
		Byte BatterBackedPRGRAM : 1;
		Byte TrainerPresent : 1;
		Byte IgnoreMirroringBit : 1;
		Byte MapperLo : 4;
	} Flag6;

	struct
	{
		Byte VSUnisystem : 1;
		Byte PlayChoice10 : 1;
		Byte iNES2Format : 1;
		Byte MapperHi : 4;
	} Flag7;

	Byte PrgRAM;
	Byte TV1;
	Byte TV2;
	Byte Padding[5];
};
