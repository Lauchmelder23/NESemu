#pragma once

#include "Controller.hpp"

union StandardButtons
{
	struct
	{
		Byte Right : 1;
		Byte Left : 1;
		Byte Down : 1;
		Byte Up : 1;
		Byte Start : 1;
		Byte Select : 1;
		Byte B : 1;
		Byte A : 1;
	} Buttons;

	Byte Raw;
};

class StandardController :
	public Controller
{
public:
	StandardController();

	virtual void OUT(PortLatch latch);

private:
};
