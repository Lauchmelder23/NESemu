#pragma once

#include <array>
#include <functional>
#include <sstream>
#include <deque>
#include "Types.hpp"

class Bus;

using Operation = std::function<void(void)>;
using AddressingMode = std::function<void(void)>;

/**
 * @brief Addressing modes of the CPU.
 */
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

/**
 * @brief Stores data of an instruction.
 */
struct Instruction
{
	Operation Opcode = nullptr;
	AddressingMode Mode = nullptr;		
	Addressing AddrType = Addressing::IMP;	
	uint8_t Size = 0;
	uint8_t Cycles = 0;
	char Mnemonic[5] = " ???";
};

/**
 * @brief Represents the CPU.
 */
class CPU
{
	// Give certain debugger components direct access
	friend class Debugger;
	friend class CPUWatcher;
	friend class Disassembler;

public:
	CPU(Bus* bus);

	/**
	 * @brief Do one CPU cycle.
	 */
	uint8_t Tick();

	/**
	 * @brief Powerup the CPU.
	 * Internal state is equal to the powerup state
	 */
	void Powerup();

	/**
	 * @brief Reset the CPU.
	 * Internal state is equal to the reset state
	 */
	void Reset();

	/**
	 * @brief Halt the CPU (stops it from executing anything).
	 */
	inline void Halt() { halted = true; }

	/**
	 * @brief Request an interrupt.
	 * Can be blocked if the IRQ disable flag is set in the status register
	 */
	void IRQ();

	/**
	 * @brief Trigger a non-maskable interrupt.
	 */
	void NMI();

private:
	/**
	 * @brief Create a lookup table of instructions.
	 */
	void CreateInstructionTable();

	/**
	 * @brief Push a byte to the stack
	 */
	inline void Push(Byte val) { Write(0x0100 | (sp--), val); }

	/**
	 * @brief Pop a byte from the stack.
	 */
	inline Byte Pop() { return Read(0x0100 | (++sp)); }


	/**
	 * @brief Wraps Bus::ReadCPU.
	 */
	Byte Read(Word addr);

	/**
	 * @brief Wraps Bus::WriteCPU.
	 */
	void Write(Word addr, Byte val);

	void FetchValue();
	
private:	// Stuff regarding addressing modes
	Address rawAddress;			//< Temporary storage while decoding addresses
	Address absoluteAddress;	//< Address the current instruction operates on
	Byte relativeAddress;		//< (Relative) address the current instruction operates on
	Byte fetchedVal;			//< The value needed for the current instruction
	bool accumulatorAddressing = false;

	// The following functions all perform the same steps:
	//  1. Fetch bytes needed for opcode (if any) and advance PC
	//  2. Construct the address the instruction operates on (addressing mode specific)
	//  3. Fetch the value in RAM at that address
	void ABS();		// Absolute
	void ABX();		// Absolute X-indexed
	void ABY();		// Absolute Y-indexed
	void ACC();		// Accumulator
	void IDX();		// X-indexed indirect
	void IDY();		// Indirect Y-indexed
	void IMM();		// Immediate
	void IMP();		// Implied
	void IND();		// Indirect
	void REL();		// Relative
	void ZPG();		// Zeropage
	void ZPX();		// Zeropage X-indexed
	void ZPY();		// Zeropage Y-indexed

private:	// Stuff regarding instructions
	// Instructions that the NES can perform
	// They simply perform the operations needed
	void ADC();
	void ALR();
	void AND();
	void ANE();
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
	void LXA();
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

private:	// CPU internals
	// Registers
	Byte acc;
	Byte idx, idy;
	Address pc;
	Word sp;
	StatusFlag status;

	Instruction* currentInstruction = nullptr;
	uint8_t remainingCycles = 0;
	uint8_t additionalCycles = 0;	//< E.g. when a page boundary was crossed
	uint64_t totalCycles = 0;
	std::deque<std::pair<Word, Instruction*>> pastInstructions;	//< For debugging, saves the past 50 instructions
	bool halted = false;

#ifndef NDEBUG
	std::stringstream debugString;
#endif

private:
	std::array<Instruction, 256> InstructionTable;
	Bus* bus;
};
