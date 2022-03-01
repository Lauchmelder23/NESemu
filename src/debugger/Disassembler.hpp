#pragma once

#include <set>
#include "DebugWindow.hpp"
#include "../Types.hpp"

class CPU;
struct Instruction;

struct Breakpoint
{
	Breakpoint(Word addr) :
		address(addr), active(true)
	{}

	Word address;
	mutable bool active;

	inline Word GetAddress() const { return address; }

	inline bool operator<(const Breakpoint& other) const { return (address < other.address); }
	inline bool operator<(Word other) const { return (address < other); }
};

class Disassembler :
	public DebugWindow
{
public:
	Disassembler(Debugger* debugger, CPU* cpu);
	
	virtual void OnRender() override;
	bool BreakpointHit();

private:
	void Disassemble(std::string& target, uint16_t& pc);
	void Disassemble(std::string& target, uint16_t pc, const Instruction* instr);

	void BreakpointWindow();

private:
	CPU* cpu;
	bool showBreakpoints = false;
	Word tempBreakpoint = 0x0000;
	std::set<Breakpoint> breakpoints;
};
