#pragma once

#include "../Types.hpp"

union PortLatch
{
	struct
	{
		Byte Controller : 1;
		Byte Expansion : 2;
		Byte Padding : 5;
	} Ports;

	Byte Raw;
};

class Controller
{
	friend class ControllerPortViewer;

public:
	Controller(int outPin) : outPin(outPin) {}

	virtual void OUT(PortLatch latch) = 0;

	inline Byte CLK()
	{
		Byte output = (outRegister & 0x80) >> 7;
		outRegister <<= 1;

		return 0x40 | (output << outPin);
	}

protected:
	Byte outPin;
	Byte outRegister{0};
};
