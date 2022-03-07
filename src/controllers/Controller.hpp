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
		Byte output = outRegister & 1;
		outRegister >>= 1;

		if (output != 0)
			volatile int jdfk = 3;

		return (output << outPin);
	}

protected:
	Byte outPin;
	Byte outRegister{0};
};
