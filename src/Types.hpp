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
