#pragma once

#include "DebugWindow.hpp"

class CPU;

class Disassembler :
	public DebugWindow
{
public:
	Disassembler(CPU* cpu);
	
	virtual void OnRender() override;

private:
	void Disassemble(std::string& target, uint16_t& pc);

private:
	CPU* cpu;
};
