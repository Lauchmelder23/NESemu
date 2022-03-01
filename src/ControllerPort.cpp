#include "ControllerPort.hpp"

ControllerPort::ControllerPort() :
	latch{0}
{
	for (auto it = connectedDevices.begin(); it != connectedDevices.end(); it++)
	{
		*it = nullptr;
	}
}

ControllerPort::~ControllerPort()
{
	for (Controller* controller : connectedDevices)
	{
		if (controller)
			delete controller;
	}
}

Byte ControllerPort::Write(Word addr, Byte val)
{
	if (addr != 0x4016)
		return 0x00;
	
	latch.Raw = val;
}

Byte ControllerPort::Read(Word addr)
{
	return connectedDevices[addr & 1]->CLK();
}

void ControllerPort::Tick()
{
	for (Controller* controller : connectedDevices)
	{
		if (controller)
			controller->OUT(latch);
	}
}
