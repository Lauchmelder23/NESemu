#include "APU.hpp"

#include "Bus.hpp"

APU::APU(Bus* bus) :
	bus(bus)
{
}

void APU::Powerup()
{
	mode = false;
	disableInterrupt = false;
}

void APU::Reset()
{
	// Nothing for now
}

void APU::Tick()
{
	if (cycles == 0)
		sequencer = 0;
	else
		cycles--;

	if (!APUActionLatch)
	{
		APUActionLatch = !APUActionLatch;
		return;
	}

	if (sequencer == 14914 || sequencer == 14915)
	{
		if(!mode && !disableInterrupt)
			bus->IRQ();

		if (sequencer == 14915)
			sequencer = 0;
	}
	sequencer++;

	APUActionLatch = !APUActionLatch;
}

void APU::WriteRegister(Word addr, Byte val)
{
	switch (addr)
	{
	case 0x4017:
	{
		mode = ((val & 0x80) == 0x80);
		disableInterrupt = ((val & 0x40) == 0x40);

		cycles = 3;
		if (!APUActionLatch)
			cycles++;
	} break;
	}
}
