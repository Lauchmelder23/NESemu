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

void ControllerPort::Write(Byte val)
{
	latch.Raw = val;
}

Byte ControllerPort::Read(Word addr)
{
	if (connectedDevices[addr & 1] == nullptr)
		return 0x00;

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
