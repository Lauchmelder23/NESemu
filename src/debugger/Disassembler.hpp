#pragma once

#include "DebugWindow.hpp"

class CPU;
struct Instruction;

class Disassembler :
	public DebugWindow
{
public:
	Disassembler(CPU* cpu);
	
	virtual void OnRender() override;

private:
	void Disassemble(std::string& target, uint16_t& pc);
	void Disassemble(std::string& target, uint16_t pc, const Instruction* instr);

private:
	CPU* cpu;
};
