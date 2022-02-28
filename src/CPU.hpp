#pragma once

#include <array>
#include <functional>
#include <sstream>
#include <deque>
#include "Types.hpp"

class Bus;

using Operation = std::function<void(void)>;
using AddressingMode = std::function<void(void)>;

enum class Addressing
{
	ABS,
	ABX,
	ABY,
	ACC,
	IDX,
	IDY,
	IMM,
	IMP,
	IND,
	REL,
	ZPG,
	ZPX,
	ZPY
};

union StatusFlag
{
	struct
	{
		Byte Carry : 1;
		Byte Zero : 1;
		Byte InterruptDisable : 1;
		Byte Decimal : 1;
		Byte Break : 1;
		Byte NoEffect : 1;
		Byte Overflow : 1;
		Byte Negative : 1;
	} Flag;

	Word Raw;
};

struct Instruction
{
	Operation Operation = nullptr;
	AddressingMode Mode = nullptr;
	Addressing AddrType = Addressing::IMP;
	uint8_t Size = 0;
	uint8_t Cycles = 0;
	char Mnemonic[5] = " XXX";
};

class CPU
{
	friend class Debugger;
	friend class CPUWatcher;
	friend class Disassembler;

public:
	CPU(Bus* bus);

	uint8_t Tick();
	void Powerup();
	void Reset();
	inline void Halt() { halted = true; }

	void IRQ();
	void NMI();

private:
	void CreateInstructionTable();

	inline void Push(Byte val) { Write(0x0100 | (sp--), val); }
	inline Byte Pop() { return Read(0x0100 | (++sp)); }

	Byte Read(Word addr);
	void Write(Word addr, Byte val);

private:
	Address rawAddress;
	Address absoluteAddress;
	Byte relativeAddress;
	Byte fetchedVal;
	bool accumulatorAddressing = false;
	void ABS();
	void ABX();
	void ABY();
	void ACC();
	void IDX();
	void IDY();
	void IMM();
	void IMP();
	void IND();
	void REL();
	void ZPG();
	void ZPX();
	void ZPY();

private:
	void ADC();
	void AND();
	void ASL();
	void BCC();
	void BCS();
	void BEQ();
	void BIT();
	void BMI();
	void BNE();
	void BPL();
	void BRK();
	void BVC();
	void BVS();
	void CLC();
	void CLD();
	void CLI();
	void CLV();
	void CMP();
	void CPX();
	void CPY();
	void DCP();
	void DEC();
	void DEX();
	void DEY();
	void EOR();
	void INC();
	void INX();
	void INY();
	void ISC();
	void JMP();
	void JSR();
	void LAX();
	void LDA();
	void LDX();
	void LDY();
	void LSR();
	void NOP();
	void ORA();
	void PHA();
	void PHP();
	void PLA();
	void PLP();
	void RLA();
	void ROL();
	void ROR();
	void RRA();
	void RTI();
	void RTS();
	void SAX();
	void SBC();
	void SEC();
	void SED();
	void SEI();
	void SLO();
	void SRE();
	void STA();
	void STX();
	void STY();
	void TAX();
	void TAY();
	void TSX();
	void TXA();
	void TXS();
	void TYA();

private:
	Byte acc;
	Byte idx, idy;
	Address pc;
	Word sp;

	StatusFlag status;

	Instruction* currentInstruction = nullptr;
	uint8_t remainingCycles = 0;
	uint8_t additionalCycles = 0;
	uint64_t totalCycles = 0;
	std::deque<Word> pastPCs;
	bool halted = false;
#ifndef NDEBUG
	std::stringstream debugString;
#endif

private:
	std::array<Instruction, 256> InstructionTable;
	Bus* bus;
};
