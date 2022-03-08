#include "Bus.hpp"
#include "Log.hpp"

#include <stdexcept>

#include "controllers/StandardController.hpp"

Bus::Bus(Screen* screen) :
	cpu(this), ppu(this, screen), apu(this), cartridge(this)
{
	LOG_CORE_INFO("Allocating RAM");
	RAM = std::vector<Byte>(0x800);

	LOG_CORE_INFO("Allocating VRAM");
	VRAM = std::vector<Byte>(0x800);
	palettes = std::vector<Byte>(0x20, 0);

	LOG_CORE_INFO("Inserting cartridge");
	cartridge.Load("roms/mario.nes");

	LOG_CORE_INFO("Powering up CPU");
	cpu.Powerup();

	LOG_CORE_INFO("Powering up PPU");
	ppu.Powerup();

	LOG_CORE_INFO("Powering up APU");
	apu.Powerup();

	controllerPort.PlugInController<StandardController>(0);
}

void Bus::Reboot()
{
	cpu.Powerup();
	ppu.Powerup();
	apu.Powerup();
}

void Bus::Reset()
{
	cpu.Reset();
	ppu.Reset();
	apu.Reset();
}

uint8_t Bus::Tick()
{
	controllerPort.Tick();

	uint8_t result = 0x00;
	if (DMACyclesLeft == 0)
	{
		result = cpu.Tick();
	}
	else
	{
		DMATick();
		result = DMACyclesLeft;
	}


	// 3 ppu ticks per cpu tick
	ppu.Tick();
	ppu.Tick();
	ppu.Tick();

	// APU is only ticked every 2 cycles, but that logic
	// is handled inside the APU class
	apu.Tick();

	return result;
}

void Bus::DMATick()
{
	if (preDMACycles > 0)
	{
		preDMACycles--;
		return;
	}

	if (DMALatch != 0)
	{
		Byte data = ReadCPU(((Word)DMAPage << 8) | (0x100 - DMACyclesLeft));
		ppu.WriteRegister(0x2004, data);

		DMACyclesLeft--;
	}

	DMALatch = 1 - DMALatch;
}

void Bus::PPUTick()
{
	if (ppuClock == 0)
	{
		cpu.Tick();
		apu.Tick();
	}

	ppu.Tick();
	ppuClock = (ppuClock + 1) % 3;
}

bool Bus::Instruction()
{
	try
	{
		while (Tick());
	}
	catch (const std::runtime_error& err)
	{
		LOG_CORE_FATAL("Fatal Bus error: {0}", err.what());
		cpu.Halt();
		return true;
	}

	return true;
}

bool Bus::Frame()
{
	try
	{
		while (!ppu.IsFrameDone())
			Tick();
	}
	catch (const std::runtime_error& err)
	{
		LOG_CORE_FATAL("Fatal Bus error: {0}", err.what());
		cpu.Halt();
		return true;
	}

	return true;
}

Byte Bus::ReadCPU(Word addr)
{
	if (0x0000 <= addr && addr < 0x2000)
	{
		return RAM[addr & 0x7FF];
	}
	else if (0x2000 <= addr && addr < 0x4000)
	{
		return ppu.ReadRegister(addr & 0x7);
	}
	else if (0x8000 <= addr && addr <= 0xFFFF)
	{
		return cartridge.ReadCPU(addr);
	}
	else if (0x4000 <= addr && addr <= 0x4017)
	{
		switch (addr)
		{
		case 0x4014:
			return 0x00;

		case 0x4016:
		case 0x4017:
			return controllerPort.Read(addr);
		}
	}

	return 0x00;
}

Byte Bus::ReadPPU(Word addr)
{
	addr &= 0x3FFF;

	if (0x0000 <= addr && addr < 0x2000)
	{
		return cartridge.ReadPPU(addr);
	}
	else if(0x2000 <= addr && addr < 0x3F00)
	{
		if (cartridge.MapCIRAM(addr))
			return cartridge.ReadVRAM(addr);

		return VRAM[addr & 0xFFF];
	}
	else if (0x3F00 <= addr && addr < 0x4000)
	{
		if ((addr & 0x3) == 0x00)
			addr &= 0xF;

		return palettes[addr & 0x1F];
	}
	
	return 0x00;
}

void Bus::WriteCPU(Word addr, Byte val)
{
	if (0x0000 <= addr && addr < 0x2000)
	{
		if (addr == 0x0348)
			volatile int jdfkdf = 3;
		RAM[addr & 0x7FF] = val;
	}
	else if (0x2000 <= addr && addr < 0x4000)
	{
		ppu.WriteRegister(addr & 0x7, val);
	}
	else if (0x8000 <= addr && addr <= 0xFFFF)
	{
		cartridge.WriteCPU(addr, val);
	}
	else if (0x4000 <= addr && addr <= 0x4017)
	{
		switch (addr)
		{
		case 0x4014:
			DMAPage = val;
			DMACyclesLeft = 0x100;
			preDMACycles = 1 + (cpu.GetTotalCycles() % 2);
			return;

		case 0x4016:
			controllerPort.Write(addr, val);
			break;

		case 0x4017:
			apu.WriteRegister(addr, val);
			break;
		}
	}
}

void Bus::WritePPU(Word addr, Byte val)
{
	addr &= 0x3FFF;

	if (0x0000 <= addr && addr < 0x2000)
	{
		cartridge.WritePPU(addr, val);
	}
	else if (0x2000 <= addr && addr < 0x3F00)
	{
		if (cartridge.MapCIRAM(addr))
			cartridge.WriteVRAM(addr, val);

		VRAM[addr & 0xFFF] = val;
	}
	else if (0x3F00 <= addr && addr < 0x4000)
	{
		if ((addr & 0x3) == 0x00)
			addr &= 0xF;

		palettes[addr & 0x1F] = val;
	}
}
