#include "CPU.hpp"

#include <iomanip>

#include "Bus.hpp"
#include "Log.hpp"

#define BIND(x) (std::bind(&CPU::x, this))
#define NEW_INSTRUCTION(op, addr, size, cyc) Instruction{ BIND(op), BIND(addr), Addressing::addr, size, cyc, " " #op }
#define NEW_ILLGL_INSTR(op, addr, size, cyc) Instruction{ BIND(op), BIND(addr), Addressing::addr, size, cyc, "*" #op }

#define CHECK_NEGATIVE(x)	status.Flag.Negative = (((x) & 0x80) == 0x80)
#define CHECK_ZERO(x)		status.Flag.Zero = ((x) == 0x00)

CPU::CPU(Bus* bus) :
	bus(bus)
{
	LOG_CORE_INFO("Creating instruction lookup table");
	CreateInstructionTable();
}

Byte CPU::Read(Word addr)
{
	return bus->ReadCPU(addr);
}

void CPU::Write(Word addr, Byte val)
{
	bus->WriteCPU(addr, val);
}

void CPU::FetchValue()
{
	if (currentInstruction->AddrType != Addressing::ACC && currentInstruction->AddrType != Addressing::IMP)
		fetchedVal = Read(absoluteAddress.Raw);
}

uint8_t CPU::Tick()
{
	if (halted)
		return 0;

	// If there are still cycles left from last instruction, dont do anything
	totalCycles++;
	if (remainingCycles)
	{
		return remainingCycles--;
	}

	// Get current opcode and instruction
	Byte opcode = Read(pc.Raw++);
	currentInstruction = &(InstructionTable[opcode]);

	// Add this instruction to the past instruction list
	pastInstructions.push_back(std::make_pair(pc.Raw - 1, currentInstruction));
	if (pastInstructions.size() > 50)
		pastInstructions.pop_front();

	// If the instruction is not set in the lookup table, abort
	if (currentInstruction->Opcode == nullptr || currentInstruction->Mode == nullptr)
	{
		LOG_DEBUG_ERROR("Unknown instruction {0:02X} at ${1:04X}", opcode, pc.Raw);
		throw std::runtime_error("Encountered unknown opcode");
	}

	// Invoke addressing mode and instruction
	accumulatorAddressing = false;
	currentInstruction->Mode();
	currentInstruction->Opcode();

	// Set remaining cycles
	remainingCycles = currentInstruction->Cycles + additionalCycles;
	additionalCycles = 0;
	remainingCycles--;
	return 0;
}

void CPU::CreateInstructionTable()
{
	InstructionTable[0x00] = NEW_INSTRUCTION(BRK, IMP, 1, 7);
	InstructionTable[0x01] = NEW_INSTRUCTION(ORA, IDX, 2, 6);
	InstructionTable[0x03] = NEW_ILLGL_INSTR(SLO, IDX, 2, 8);
	InstructionTable[0x04] = NEW_ILLGL_INSTR(NOP, ZPG, 2, 3);
	InstructionTable[0x05] = NEW_INSTRUCTION(ORA, ZPG, 2, 3);
	InstructionTable[0x06] = NEW_INSTRUCTION(ASL, ZPG, 2, 5);
	InstructionTable[0x07] = NEW_ILLGL_INSTR(SLO, ZPG, 2, 5);
	InstructionTable[0x08] = NEW_INSTRUCTION(PHP, IMP, 1, 3);
	InstructionTable[0x09] = NEW_INSTRUCTION(ORA, IMM, 2, 2);
	InstructionTable[0x0A] = NEW_INSTRUCTION(ASL, ACC, 1, 2);
	InstructionTable[0x0B] = NEW_ILLGL_INSTR(ANC, IMM, 2, 2);
	InstructionTable[0x0D] = NEW_INSTRUCTION(ORA, ABS, 3, 4);
	InstructionTable[0x0C] = NEW_ILLGL_INSTR(NOP, ABS, 3, 4);
	InstructionTable[0x0E] = NEW_INSTRUCTION(ASL, ABS, 3, 6);
	InstructionTable[0x0F] = NEW_ILLGL_INSTR(SLO, ABS, 3, 6);

	InstructionTable[0x10] = NEW_INSTRUCTION(BPL, REL, 2, 2);
	InstructionTable[0x11] = NEW_INSTRUCTION(ORA, IDY, 2, 5);
	InstructionTable[0x13] = NEW_ILLGL_INSTR(SLO, IDY, 2, 8);
	InstructionTable[0x14] = NEW_ILLGL_INSTR(NOP, ZPX, 2, 4);
	InstructionTable[0x15] = NEW_INSTRUCTION(ORA, ZPX, 2, 4);
	InstructionTable[0x16] = NEW_INSTRUCTION(ASL, ZPX, 2, 6);
	InstructionTable[0x17] = NEW_ILLGL_INSTR(SLO, ZPX, 2, 6);
	InstructionTable[0x18] = NEW_INSTRUCTION(CLC, IMP, 1, 2);
	InstructionTable[0x19] = NEW_INSTRUCTION(ORA, ABY, 3, 4);
	InstructionTable[0x1A] = NEW_ILLGL_INSTR(NOP, IMP, 1, 2);
	InstructionTable[0x1B] = NEW_ILLGL_INSTR(SLO, ABY, 3, 7);
	InstructionTable[0x1C] = NEW_ILLGL_INSTR(NOP, ABX, 3, 4);
	InstructionTable[0x1D] = NEW_INSTRUCTION(ORA, ABX, 3, 4);
	InstructionTable[0x1E] = NEW_INSTRUCTION(ASL, ABX, 3, 7);
	InstructionTable[0x1F] = NEW_ILLGL_INSTR(SLO, ABX, 3, 7);


	InstructionTable[0x20] = NEW_INSTRUCTION(JSR, ABS, 3, 6);
	InstructionTable[0x21] = NEW_INSTRUCTION(AND, IDX, 2, 6);
	InstructionTable[0x23] = NEW_ILLGL_INSTR(RLA, IDX, 2, 8);
	InstructionTable[0x24] = NEW_INSTRUCTION(BIT, ZPG, 2, 3);
	InstructionTable[0x25] = NEW_INSTRUCTION(AND, ZPG, 2, 3);
	InstructionTable[0x26] = NEW_INSTRUCTION(ROL, ZPG, 2, 5);
	InstructionTable[0x27] = NEW_ILLGL_INSTR(RLA, ZPG, 2, 5);
	InstructionTable[0x28] = NEW_INSTRUCTION(PLP, IMP, 1, 4);
	InstructionTable[0x29] = NEW_INSTRUCTION(AND, IMM, 2, 2);
	InstructionTable[0x2A] = NEW_INSTRUCTION(ROL, ACC, 1, 2);
	InstructionTable[0x2B] = NEW_ILLGL_INSTR(ANC, IMM, 2, 2);
	InstructionTable[0x2C] = NEW_INSTRUCTION(BIT, ABS, 3, 4);
	InstructionTable[0x2D] = NEW_INSTRUCTION(AND, ABS, 3, 4);
	InstructionTable[0x2E] = NEW_INSTRUCTION(ROL, ABS, 3, 6);
	InstructionTable[0x2F] = NEW_ILLGL_INSTR(RLA, ABS, 3, 6);

	InstructionTable[0x30] = NEW_INSTRUCTION(BMI, REL, 2, 2);
	InstructionTable[0x31] = NEW_INSTRUCTION(AND, IDY, 2, 5);
	InstructionTable[0x33] = NEW_ILLGL_INSTR(RLA, IDY, 2, 8);
	InstructionTable[0x34] = NEW_ILLGL_INSTR(NOP, ZPX, 2, 4);
	InstructionTable[0x35] = NEW_INSTRUCTION(AND, ZPX, 2, 4);
	InstructionTable[0x36] = NEW_INSTRUCTION(ROL, ZPX, 2, 6);
	InstructionTable[0x37] = NEW_ILLGL_INSTR(RLA, ZPX, 2, 6);
	InstructionTable[0x38] = NEW_INSTRUCTION(SEC, IMP, 1, 2);
	InstructionTable[0x39] = NEW_INSTRUCTION(AND, ABY, 3, 4);
	InstructionTable[0x3A] = NEW_ILLGL_INSTR(NOP, IMP, 1, 2);
	InstructionTable[0x3B] = NEW_ILLGL_INSTR(RLA, ABY, 3, 7);
	InstructionTable[0x3C] = NEW_ILLGL_INSTR(NOP, ABX, 3, 4);
	InstructionTable[0x3D] = NEW_INSTRUCTION(AND, ABX, 3, 4);
	InstructionTable[0x3E] = NEW_INSTRUCTION(ROL, ABX, 3, 7);
	InstructionTable[0x3F] = NEW_ILLGL_INSTR(RLA, ABX, 3, 7);

	InstructionTable[0x40] = NEW_INSTRUCTION(RTI, IMP, 1, 6);
	InstructionTable[0x41] = NEW_INSTRUCTION(EOR, IDX, 2, 6);
	InstructionTable[0x43] = NEW_ILLGL_INSTR(SRE, IDX, 2, 8);
	InstructionTable[0x44] = NEW_ILLGL_INSTR(NOP, ZPG, 2, 3);
	InstructionTable[0x45] = NEW_INSTRUCTION(EOR, ZPG, 2, 3);
	InstructionTable[0x46] = NEW_INSTRUCTION(LSR, ZPG, 2, 5);
	InstructionTable[0x47] = NEW_ILLGL_INSTR(SRE, ZPG, 2, 5);
	InstructionTable[0x48] = NEW_INSTRUCTION(PHA, IMP, 1, 3);
	InstructionTable[0x49] = NEW_INSTRUCTION(EOR, IMM, 2, 2);
	InstructionTable[0x4A] = NEW_INSTRUCTION(LSR, ACC, 1, 2);
	InstructionTable[0x4B] = NEW_ILLGL_INSTR(ALR, IMM, 2, 2);
	InstructionTable[0x4C] = NEW_INSTRUCTION(JMP, ABS, 3, 3);
	InstructionTable[0x4D] = NEW_INSTRUCTION(EOR, ABS, 3, 4);
	InstructionTable[0x4E] = NEW_INSTRUCTION(LSR, ABS, 3, 6);
	InstructionTable[0x4F] = NEW_ILLGL_INSTR(SRE, ABS, 3, 6);

	InstructionTable[0x50] = NEW_INSTRUCTION(BVC, REL, 2, 2);
	InstructionTable[0x51] = NEW_INSTRUCTION(EOR, IDY, 2, 5);
	InstructionTable[0x53] = NEW_ILLGL_INSTR(SRE, IDY, 2, 8);
	InstructionTable[0x54] = NEW_ILLGL_INSTR(NOP, ZPX, 2, 4);
	InstructionTable[0x55] = NEW_INSTRUCTION(EOR, ZPX, 2, 4);
	InstructionTable[0x56] = NEW_INSTRUCTION(LSR, ZPX, 2, 6);
	InstructionTable[0x57] = NEW_ILLGL_INSTR(SRE, ZPX, 2, 6);
	InstructionTable[0x58] = NEW_INSTRUCTION(CLI, IMP, 1, 2);
	InstructionTable[0x59] = NEW_INSTRUCTION(EOR, ABY, 3, 4);
	InstructionTable[0x5A] = NEW_ILLGL_INSTR(NOP, IMP, 1, 2);
	InstructionTable[0x5B] = NEW_ILLGL_INSTR(SRE, ABY, 3, 7);
	InstructionTable[0x5C] = NEW_ILLGL_INSTR(NOP, ABX, 3, 4);
	InstructionTable[0x5D] = NEW_INSTRUCTION(EOR, ABX, 3, 4);
	InstructionTable[0x5E] = NEW_INSTRUCTION(LSR, ABX, 3, 7);
	InstructionTable[0x5F] = NEW_ILLGL_INSTR(SRE, ABX, 3, 7);

	InstructionTable[0x60] = NEW_INSTRUCTION(RTS, IMP, 1, 6);
	InstructionTable[0x61] = NEW_INSTRUCTION(ADC, IDX, 2, 6);
	InstructionTable[0x63] = NEW_ILLGL_INSTR(RRA, IDX, 2, 8);
	InstructionTable[0x64] = NEW_ILLGL_INSTR(NOP, ZPG, 2, 3);
	InstructionTable[0x65] = NEW_INSTRUCTION(ADC, ZPG, 2, 3);
	InstructionTable[0x66] = NEW_INSTRUCTION(ROR, ZPG, 2, 5);
	InstructionTable[0x67] = NEW_ILLGL_INSTR(RRA, ZPG, 2, 5);
	InstructionTable[0x68] = NEW_INSTRUCTION(PLA, IMP, 1, 4);
	InstructionTable[0x69] = NEW_INSTRUCTION(ADC, IMM, 2, 2);
	InstructionTable[0x6A] = NEW_INSTRUCTION(ROR, ACC, 1, 2);
	InstructionTable[0x6B] = NEW_ILLGL_INSTR(ARR, IMM, 2, 2);
	InstructionTable[0x6C] = NEW_INSTRUCTION(JMP, IND, 3, 5);
	InstructionTable[0x6D] = NEW_INSTRUCTION(ADC, ABS, 3, 4);
	InstructionTable[0x6E] = NEW_INSTRUCTION(ROR, ABS, 3, 6);
	InstructionTable[0x6F] = NEW_ILLGL_INSTR(RRA, ABS, 3, 6);

	InstructionTable[0x70] = NEW_INSTRUCTION(BVS, REL, 2, 2);
	InstructionTable[0x71] = NEW_INSTRUCTION(ADC, IDY, 2, 5);
	InstructionTable[0x73] = NEW_ILLGL_INSTR(RRA, IDY, 2, 8);
	InstructionTable[0x74] = NEW_ILLGL_INSTR(NOP, ZPX, 2, 4);
	InstructionTable[0x75] = NEW_INSTRUCTION(ADC, ZPX, 2, 4);
	InstructionTable[0x76] = NEW_INSTRUCTION(ROR, ZPX, 2, 6);
	InstructionTable[0x77] = NEW_ILLGL_INSTR(RRA, ZPX, 2, 6);
	InstructionTable[0x78] = NEW_INSTRUCTION(SEI, IMP, 1, 2);
	InstructionTable[0x79] = NEW_INSTRUCTION(ADC, ABY, 3, 4);
	InstructionTable[0x7A] = NEW_ILLGL_INSTR(NOP, IMP, 1, 2);
	InstructionTable[0x7B] = NEW_ILLGL_INSTR(RRA, ABY, 3, 7);
	InstructionTable[0x7C] = NEW_ILLGL_INSTR(NOP, ABX, 3, 4);
	InstructionTable[0x7D] = NEW_INSTRUCTION(ADC, ABX, 3, 4);
	InstructionTable[0x7E] = NEW_INSTRUCTION(ROR, ABX, 3, 7);
	InstructionTable[0x7F] = NEW_ILLGL_INSTR(RRA, ABX, 3, 7);

	InstructionTable[0x80] = NEW_ILLGL_INSTR(NOP, IMM, 2, 2);
	InstructionTable[0x81] = NEW_INSTRUCTION(STA, IDX, 2, 6);
	InstructionTable[0x83] = NEW_ILLGL_INSTR(SAX, IDX, 2, 6);
	InstructionTable[0x82] = NEW_ILLGL_INSTR(NOP, IMM, 2, 2);
	InstructionTable[0x85] = NEW_INSTRUCTION(STA, ZPG, 2, 3);
	InstructionTable[0x84] = NEW_INSTRUCTION(STY, ZPG, 2, 3);
	InstructionTable[0x86] = NEW_INSTRUCTION(STX, ZPG, 2, 3);
	InstructionTable[0x87] = NEW_ILLGL_INSTR(SAX, ZPG, 2, 3);
	InstructionTable[0x88] = NEW_INSTRUCTION(DEY, IMP, 1, 2);
	InstructionTable[0x89] = NEW_ILLGL_INSTR(NOP, IMM, 2, 2);
	InstructionTable[0x8A] = NEW_INSTRUCTION(TXA, IMP, 1, 2);
	InstructionTable[0x8B] = NEW_ILLGL_INSTR(ANE, IMM, 2, 2);
	InstructionTable[0x8C] = NEW_INSTRUCTION(STY, ABS, 3, 4);
	InstructionTable[0x8D] = NEW_INSTRUCTION(STA, ABS, 3, 4);
	InstructionTable[0x8E] = NEW_INSTRUCTION(STX, ABS, 3, 4);
	InstructionTable[0x8F] = NEW_ILLGL_INSTR(SAX, ABS, 2, 4);

	InstructionTable[0x90] = NEW_INSTRUCTION(BCC, REL, 2, 2);
	InstructionTable[0x91] = NEW_INSTRUCTION(STA, IDY, 2, 6);
	InstructionTable[0x94] = NEW_INSTRUCTION(STY, ZPX, 2, 4);
	InstructionTable[0x95] = NEW_INSTRUCTION(STA, ZPX, 2, 4);
	InstructionTable[0x96] = NEW_INSTRUCTION(STX, ZPY, 2, 4);
	InstructionTable[0x97] = NEW_ILLGL_INSTR(SAX, ZPY, 2, 4);
	InstructionTable[0x98] = NEW_INSTRUCTION(TYA, IMP, 1, 2);
	InstructionTable[0x99] = NEW_INSTRUCTION(STA, ABY, 3, 5);
	InstructionTable[0x9A] = NEW_INSTRUCTION(TXS, IMP, 1, 2);
	InstructionTable[0x9C] = NEW_ILLGL_INSTR(SHY, ABX, 3, 5);
	InstructionTable[0x9D] = NEW_INSTRUCTION(STA, ABX, 3, 5);
	InstructionTable[0x9E] = NEW_ILLGL_INSTR(SHX, ABY, 3, 5);

	InstructionTable[0xA0] = NEW_INSTRUCTION(LDY, IMM, 2, 2);
	InstructionTable[0xA1] = NEW_INSTRUCTION(LDA, IDX, 2, 6);
	InstructionTable[0xA2] = NEW_INSTRUCTION(LDX, IMM, 2, 2);
	InstructionTable[0xA3] = NEW_ILLGL_INSTR(LAX, IDX, 2, 6);
	InstructionTable[0xA4] = NEW_INSTRUCTION(LDY, ZPG, 2, 3);
	InstructionTable[0xA5] = NEW_INSTRUCTION(LDA, ZPG, 2, 3);
	InstructionTable[0xA6] = NEW_INSTRUCTION(LDX, ZPG, 2, 3);
	InstructionTable[0xA7] = NEW_ILLGL_INSTR(LAX, ZPG, 2, 3);
	InstructionTable[0xA8] = NEW_INSTRUCTION(TAY, IMP, 1, 2);
	InstructionTable[0xA9] = NEW_INSTRUCTION(LDA, IMM, 2, 2);
	InstructionTable[0xAA] = NEW_INSTRUCTION(TAX, IMP, 1, 2);
	InstructionTable[0xAB] = NEW_ILLGL_INSTR(LXA, IMM, 2, 2);
	InstructionTable[0xAC] = NEW_INSTRUCTION(LDY, ABS, 3, 4);
	InstructionTable[0xAD] = NEW_INSTRUCTION(LDA, ABS, 3, 4);
	InstructionTable[0xAE] = NEW_INSTRUCTION(LDX, ABS, 3, 4);
	InstructionTable[0xAF] = NEW_ILLGL_INSTR(LAX, ABS, 3, 4);

	InstructionTable[0xB0] = NEW_INSTRUCTION(BCS, REL, 2, 2);
	InstructionTable[0xB1] = NEW_INSTRUCTION(LDA, IDY, 2, 5);
	InstructionTable[0xB3] = NEW_ILLGL_INSTR(LAX, IDY, 2, 5);
	InstructionTable[0xB4] = NEW_INSTRUCTION(LDY, ZPX, 2, 4);
	InstructionTable[0xB5] = NEW_INSTRUCTION(LDA, ZPX, 2, 4);
	InstructionTable[0xB6] = NEW_INSTRUCTION(LDX, ZPY, 2, 4);
	InstructionTable[0xB7] = NEW_ILLGL_INSTR(LAX, ZPY, 2, 4);
	InstructionTable[0xB8] = NEW_INSTRUCTION(CLV, IMP, 1, 2);
	InstructionTable[0xB9] = NEW_INSTRUCTION(LDA, ABY, 3, 4);
	InstructionTable[0xBA] = NEW_INSTRUCTION(TSX, IMP, 1, 2);
	InstructionTable[0xBC] = NEW_INSTRUCTION(LDY, ABX, 3, 4);
	InstructionTable[0xBD] = NEW_INSTRUCTION(LDA, ABX, 3, 4);
	InstructionTable[0xBE] = NEW_INSTRUCTION(LDX, ABY, 3, 4);
	InstructionTable[0xBF] = NEW_ILLGL_INSTR(LAX, ABY, 3, 4);

	InstructionTable[0xC0] = NEW_INSTRUCTION(CPY, IMM, 2, 2);
	InstructionTable[0xC1] = NEW_INSTRUCTION(CMP, IDX, 2, 6);
	InstructionTable[0xC2] = NEW_ILLGL_INSTR(NOP, IMM, 2, 2);
	InstructionTable[0xC3] = NEW_ILLGL_INSTR(DCP, IDX, 2, 8);
	InstructionTable[0xC4] = NEW_INSTRUCTION(CPY, ZPG, 2, 3);
	InstructionTable[0xC5] = NEW_INSTRUCTION(CMP, ZPG, 2, 3);
	InstructionTable[0xC6] = NEW_INSTRUCTION(DEC, ZPG, 2, 5);
	InstructionTable[0xC7] = NEW_ILLGL_INSTR(DCP, ZPG, 2, 5);
	InstructionTable[0xC8] = NEW_INSTRUCTION(INY, IMP, 1, 2);
	InstructionTable[0xC9] = NEW_INSTRUCTION(CMP, IMM, 2, 2);
	InstructionTable[0xCA] = NEW_INSTRUCTION(DEX, IMP, 1, 2);
	InstructionTable[0xCB] = NEW_ILLGL_INSTR(SBX, IMM, 2, 2);
	InstructionTable[0xCC] = NEW_INSTRUCTION(CPY, ABS, 3, 4);
	InstructionTable[0xCD] = NEW_INSTRUCTION(CMP, ABS, 3, 4);
	InstructionTable[0xCE] = NEW_INSTRUCTION(DEC, ABS, 3, 6);
	InstructionTable[0xCF] = NEW_ILLGL_INSTR(DCP, ABS, 3, 6);

	InstructionTable[0xD0] = NEW_INSTRUCTION(BNE, REL, 2, 2);
	InstructionTable[0xD1] = NEW_INSTRUCTION(CMP, IDY, 2, 5);
	InstructionTable[0xD3] = NEW_ILLGL_INSTR(DCP, IDY, 2, 8);
	InstructionTable[0xD4] = NEW_ILLGL_INSTR(NOP, ZPX, 2, 4);
	InstructionTable[0xD5] = NEW_INSTRUCTION(CMP, ZPX, 2, 4);
	InstructionTable[0xD6] = NEW_INSTRUCTION(DEC, ZPX, 2, 6);
	InstructionTable[0xD7] = NEW_ILLGL_INSTR(DCP, ZPX, 2, 6);
	InstructionTable[0xD8] = NEW_INSTRUCTION(CLD, IMP, 1, 2);
	InstructionTable[0xD9] = NEW_INSTRUCTION(CMP, ABY, 3, 4);
	InstructionTable[0xDA] = NEW_ILLGL_INSTR(NOP, IMP, 1, 2);
	InstructionTable[0xDB] = NEW_ILLGL_INSTR(DCP, ABY, 3, 7);
	InstructionTable[0xDC] = NEW_ILLGL_INSTR(NOP, ABX, 3, 4);
	InstructionTable[0xDD] = NEW_INSTRUCTION(CMP, ABX, 3, 4);
	InstructionTable[0xDE] = NEW_INSTRUCTION(DEC, ABX, 3, 7);
	InstructionTable[0xDF] = NEW_ILLGL_INSTR(DCP, ABX, 3, 7);

	InstructionTable[0xE0] = NEW_INSTRUCTION(CPX, IMM, 2, 2);
	InstructionTable[0xE1] = NEW_INSTRUCTION(SBC, IDX, 2, 6);
	InstructionTable[0xE2] = NEW_ILLGL_INSTR(NOP, IMM, 2, 2);
	InstructionTable[0xE3] = NEW_ILLGL_INSTR(ISC, IDX, 2, 8);
	InstructionTable[0xE4] = NEW_INSTRUCTION(CPX, ZPG, 2, 3);
	InstructionTable[0xE5] = NEW_INSTRUCTION(SBC, ZPG, 2, 3);
	InstructionTable[0xE6] = NEW_INSTRUCTION(INC, ZPG, 2, 5);
	InstructionTable[0xE7] = NEW_ILLGL_INSTR(ISC, ZPG, 2, 5);
	InstructionTable[0xE8] = NEW_INSTRUCTION(INX, IMP, 1, 2);
	InstructionTable[0xE9] = NEW_INSTRUCTION(SBC, IMM, 2, 2);
	InstructionTable[0xEA] = NEW_INSTRUCTION(NOP, IMP, 1, 2);
	InstructionTable[0xEB] = NEW_ILLGL_INSTR(SBC, IMM, 2, 2);
	InstructionTable[0xEC] = NEW_INSTRUCTION(CPX, ABS, 3, 4);
	InstructionTable[0xED] = NEW_INSTRUCTION(SBC, ABS, 3, 4);
	InstructionTable[0xEE] = NEW_INSTRUCTION(INC, ABS, 3, 6);
	InstructionTable[0xEF] = NEW_ILLGL_INSTR(ISC, ABS, 3, 6);

	InstructionTable[0xF0] = NEW_INSTRUCTION(BEQ, REL, 2, 2);
	InstructionTable[0xF1] = NEW_INSTRUCTION(SBC, IDY, 2, 5);
	InstructionTable[0xF3] = NEW_ILLGL_INSTR(ISC, IDY, 2, 8);
	InstructionTable[0xF4] = NEW_ILLGL_INSTR(NOP, ZPX, 2, 4);
	InstructionTable[0xF5] = NEW_INSTRUCTION(SBC, ZPX, 2, 4);
	InstructionTable[0xF6] = NEW_INSTRUCTION(INC, ZPX, 2, 6);
	InstructionTable[0xF7] = NEW_ILLGL_INSTR(ISC, ZPX, 2, 6);
	InstructionTable[0xF8] = NEW_INSTRUCTION(SED, IMP, 1, 2);
	InstructionTable[0xF9] = NEW_INSTRUCTION(SBC, ABY, 3, 4);
	InstructionTable[0xFA] = NEW_ILLGL_INSTR(NOP, IMP, 1, 2);
	InstructionTable[0xFB] = NEW_ILLGL_INSTR(ISC, ABY, 3, 7);
	InstructionTable[0xFC] = NEW_ILLGL_INSTR(NOP, ABX, 3, 4);
	InstructionTable[0xFD] = NEW_INSTRUCTION(SBC, ABX, 3, 4);
	InstructionTable[0xFE] = NEW_INSTRUCTION(INC, ABX, 3, 7);
	InstructionTable[0xFF] = NEW_ILLGL_INSTR(ISC, ABX, 3, 7);
}

void CPU::Powerup()
{
	status.Raw = 0x24;
	acc = 0;
	idx = 0;
	idy = 0;
	sp = 0xFD;

	Write(0x4017, 0x00);
	Write(0x4015, 0x00);

	pc.Bytes.lo = Read(0xFFFC);
	pc.Bytes.hi = Read(0xFFFD);
	remainingCycles = 6;
	halted = false;
}

void CPU::Reset()
{
	sp -= 3;
	status.Flag.InterruptDisable = 1;

	pc.Bytes.lo = Read(0xFFFC);
	pc.Bytes.hi = Read(0xFFFD);
	halted = false;
}

void CPU::IRQ()
{
	if (status.Flag.InterruptDisable)
		return;

	Push(pc.Bytes.hi);
	Push(pc.Bytes.lo);
	Push(status.Raw | (0x1 << 5));

	status.Flag.InterruptDisable = 1;
	pc.Bytes.lo = Read(0xFFFE);
	pc.Bytes.hi = Read(0xFFFF);
}

void CPU::NMI()
{
	Push(pc.Bytes.hi);
	Push(pc.Bytes.lo);
	Push(status.Raw | (0x1 << 5));

	status.Flag.InterruptDisable = 1;
	pc.Bytes.lo = Read(0xFFFA);
	pc.Bytes.hi = Read(0xFFFB);
}

#pragma region Addressing Modes

void CPU::ABS()
{
	absoluteAddress.Bytes.lo = Read(pc.Raw++);
	absoluteAddress.Bytes.hi = Read(pc.Raw++);
}

void CPU::ABX()
{
	rawAddress.Bytes.lo = Read(pc.Raw++);
	rawAddress.Bytes.hi = Read(pc.Raw++);

	absoluteAddress.Raw = rawAddress.Raw + idx;

	if (rawAddress.Bytes.hi != absoluteAddress.Bytes.hi)
		additionalCycles = 1;
}

void CPU::ABY()
{
	rawAddress.Bytes.lo = Read(pc.Raw++);
	rawAddress.Bytes.hi = Read(pc.Raw++);

	absoluteAddress.Raw = rawAddress.Raw + idy;

	if (rawAddress.Bytes.hi != absoluteAddress.Bytes.hi)
		additionalCycles = 1;
}

void CPU::ACC()
{
	fetchedVal = acc;
	accumulatorAddressing = true;
}

void CPU::IDX()
{
	Byte index = Read(pc.Raw++);
	Byte indirectAddress = index + idx;

	absoluteAddress.Bytes.lo = Read(indirectAddress);
	absoluteAddress.Bytes.hi = Read((indirectAddress + 1) & 0xFF);
}

void CPU::IDY()
{
	Byte index = Read(pc.Raw++);
	rawAddress.Bytes.lo = Read(index);
	rawAddress.Bytes.hi = Read((index + 1) & 0xFF);

	absoluteAddress.Raw = rawAddress.Raw + idy;

	if (absoluteAddress.Bytes.hi != rawAddress.Bytes.hi)
		additionalCycles = 1;
}

void CPU::IMM()
{
	absoluteAddress.Raw = pc.Raw++;
}

void CPU::IMP()
{
	// Nothing to do
}

void CPU::IND()
{
	rawAddress.Bytes.lo = Read(pc.Raw++);
	rawAddress.Bytes.hi = Read(pc.Raw++);

	absoluteAddress.Bytes.lo = Read(rawAddress.Raw);
	rawAddress.Bytes.lo++;
	absoluteAddress.Bytes.hi = Read(rawAddress.Raw);
}

void CPU::REL()
{
	relativeAddress = Read(pc.Raw++);
}

void CPU::ZPG()
{
	absoluteAddress.Bytes.hi = 0x00;
	absoluteAddress.Bytes.lo = Read(pc.Raw++);
}

void CPU::ZPX()
{
	rawAddress.Bytes.lo = Read(pc.Raw++);

	absoluteAddress.Bytes.hi = 0x00;
	absoluteAddress.Bytes.lo = rawAddress.Bytes.lo + idx;
}

void CPU::ZPY()
{
	rawAddress.Bytes.lo = Read(pc.Raw++);

	absoluteAddress.Bytes.hi = 0x00;
	absoluteAddress.Bytes.lo = rawAddress.Bytes.lo + idy;
}

#pragma endregion

#pragma region Instructions

void CPU::ADC()
{
	FetchValue();
	Word result = (Word)acc + fetchedVal + status.Flag.Carry;

	status.Flag.Overflow = ((((~acc & ~fetchedVal & result) | (acc & fetchedVal & ~result)) & 0x80) == 0x80);

	acc = result & 0xFF;
	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);
	status.Flag.Carry = ((result & 0x100) == 0x100);
}

void CPU::ALR()
{
	FetchValue();
	acc &= fetchedVal;

	status.Flag.Carry = ((acc & 0x01) == 0x01);
	acc >>= 1;

	status.Flag.Negative = 0;
	CHECK_ZERO(acc);

	additionalCycles = 0;
}

void CPU::ANC()
{
	FetchValue();
	acc &= fetchedVal;

	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);
	status.Flag.Carry = ((acc & 0x80) == 0x80);
}

void CPU::AND()
{
	FetchValue();
	acc &= fetchedVal;

	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);
}

void CPU::ANE()
{
	FetchValue();
	Byte result = (acc | 0xFF) & idx & fetchedVal;

	acc = result;

	CHECK_NEGATIVE(result);
	CHECK_ZERO(result);

	additionalCycles = 0;
}

void CPU::ARR()
{
	FetchValue();
	acc &= fetchedVal;

	acc >>= 1;
	acc |= (status.Flag.Carry << 7);

	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);
	status.Flag.Overflow = ((acc >> 6) & 0x1) ^ ((acc >> 5) & 0x1);
	status.Flag.Carry = ((acc >> 6) & 0x1);
}

void CPU::ASL()
{
	FetchValue();
	status.Flag.Carry = ((fetchedVal & 0x80) == 0x80);
	fetchedVal <<= 1;

	CHECK_NEGATIVE(fetchedVal);
	CHECK_ZERO(fetchedVal);

	if (accumulatorAddressing)
		acc = fetchedVal;
	else
		Write(absoluteAddress.Raw, fetchedVal);
}

void CPU::BCC()
{
	if (!status.Flag.Carry)
	{
		absoluteAddress.Raw = pc.Raw + (int8_t)relativeAddress;

		if (pc.Bytes.hi != absoluteAddress.Bytes.hi)
			additionalCycles++;

		pc.Raw = absoluteAddress.Raw;
		additionalCycles++;
	}
}

void CPU::BCS()
{
	if (status.Flag.Carry)
	{
		absoluteAddress.Raw = pc.Raw + (int8_t)relativeAddress;

		if (pc.Bytes.hi != absoluteAddress.Bytes.hi)
			additionalCycles++;

		pc.Raw = absoluteAddress.Raw;
		additionalCycles++;
	}
}

void CPU::BEQ()
{
	if (status.Flag.Zero)
	{
		absoluteAddress.Raw = pc.Raw + (int8_t)relativeAddress;

		if (pc.Bytes.hi != absoluteAddress.Bytes.hi)
			additionalCycles++;

		pc.Raw = absoluteAddress.Raw;
		additionalCycles++;
	}
}

void CPU::BIT()
{
	FetchValue();
	status.Flag.Negative = ((fetchedVal & 0x80) == 0x80);
	status.Flag.Overflow = ((fetchedVal & 0x40) == 0x40);

	CHECK_ZERO(acc & fetchedVal);
}

void CPU::BMI()
{
	if (status.Flag.Negative)
	{
		absoluteAddress.Raw = pc.Raw + (int8_t)relativeAddress;

		if (pc.Bytes.hi != absoluteAddress.Bytes.hi)
			additionalCycles++;

		pc.Raw = absoluteAddress.Raw;
		additionalCycles++;
	}
}

void CPU::BNE()
{
	if (!status.Flag.Zero)
	{
		absoluteAddress.Raw = pc.Raw + (int8_t)relativeAddress;

		if (pc.Bytes.hi != absoluteAddress.Bytes.hi)
			additionalCycles++;

		pc.Raw = absoluteAddress.Raw;
		additionalCycles++;
	}
}

void CPU::BPL()
{
	if (!status.Flag.Negative)
	{
		absoluteAddress.Raw = pc.Raw + (int8_t)relativeAddress;

		if (pc.Bytes.hi != absoluteAddress.Bytes.hi)
			additionalCycles++;

		pc.Raw = absoluteAddress.Raw;
		additionalCycles++;
	}
}

void CPU::BRK()
{
	pc.Raw++;
	Push(pc.Bytes.hi);
	Push(pc.Bytes.lo);
	Push(status.Raw | (0x3 << 4));

	status.Flag.InterruptDisable = 1;
	pc.Bytes.lo = Read(0xFFFE);
	pc.Bytes.hi = Read(0xFFFF);
}

void CPU::BVC()
{
	if (!status.Flag.Overflow)
	{
		absoluteAddress.Raw = pc.Raw + (int8_t)relativeAddress;

		if (pc.Bytes.hi != absoluteAddress.Bytes.hi)
			additionalCycles++;

		pc.Raw = absoluteAddress.Raw;
		additionalCycles++;
	}
}

void CPU::BVS()
{
	if (status.Flag.Overflow)
	{
		absoluteAddress.Raw = pc.Raw + (int8_t)relativeAddress;

		if (pc.Bytes.hi != absoluteAddress.Bytes.hi)
			additionalCycles++;

		pc.Raw = absoluteAddress.Raw;
		additionalCycles++;
	}
}


void CPU::CLC()
{
	status.Flag.Carry = 0;
}

void CPU::CLD()
{
	status.Flag.Decimal = 0;
}

void CPU::CLI()
{
	status.Flag.InterruptDisable = 0;
}

void CPU::CLV()
{
	status.Flag.Overflow = 0;
}

void CPU::CMP()
{
	FetchValue();
	Word result = acc - fetchedVal;

	CHECK_NEGATIVE(result);
	CHECK_ZERO(result);
	status.Flag.Carry = (acc >= fetchedVal);
}

void CPU::CPX()
{
	FetchValue();
	Word result = idx - fetchedVal;

	CHECK_NEGATIVE(result);
	CHECK_ZERO(result);
	status.Flag.Carry = (idx >= fetchedVal);
}

void CPU::CPY()
{
	FetchValue();
	Word result = idy - fetchedVal;

	CHECK_NEGATIVE(result);
	CHECK_ZERO(result);
	status.Flag.Carry = (idy >= fetchedVal);
}

void CPU::DCP()
{
	FetchValue();
	fetchedVal--;
	Write(absoluteAddress.Raw, fetchedVal);

	Word result = acc - fetchedVal;

	CHECK_NEGATIVE(result);
	CHECK_ZERO(result);
	status.Flag.Carry = (acc >= fetchedVal);

	additionalCycles = 0;
}

void CPU::DEC()
{
	FetchValue();
	fetchedVal--;
	CHECK_NEGATIVE(fetchedVal);
	CHECK_ZERO(fetchedVal);

	Write(absoluteAddress.Raw, fetchedVal);
}

void CPU::DEX()
{
	idx--;
	CHECK_NEGATIVE(idx);
	CHECK_ZERO(idx);
}

void CPU::DEY()
{
	idy--;
	CHECK_NEGATIVE(idy);
	CHECK_ZERO(idy);
}

void CPU::EOR()
{
	FetchValue();
	acc ^= fetchedVal;

	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);
}

void CPU::INC()
{
	FetchValue();
	fetchedVal++;
	CHECK_NEGATIVE(fetchedVal);
	CHECK_ZERO(fetchedVal);

	Write(absoluteAddress.Raw, fetchedVal);
}

void CPU::INX()
{
	idx++;
	CHECK_NEGATIVE(idx);
	CHECK_ZERO(idx);
}

void CPU::INY()
{
	idy++;
	CHECK_NEGATIVE(idy);
	CHECK_ZERO(idy);
}

void CPU::ISC()
{
	FetchValue();
	fetchedVal++;
	Write(absoluteAddress.Raw, fetchedVal);

	Word result = (Word)acc - fetchedVal - (1 - status.Flag.Carry);

	status.Flag.Overflow = ((((~acc & fetchedVal & result) | (acc & ~fetchedVal & ~result)) & 0x80) == 0x80);

	acc = result & 0xFF;
	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);
	status.Flag.Carry = ((result & 0x100) != 0x100);

	additionalCycles = 0;
}

void CPU::JMP()
{
	pc.Raw = absoluteAddress.Raw;
}

void CPU::JSR()
{
	pc.Raw--;
	Push(pc.Bytes.hi);
	Push(pc.Bytes.lo);
	pc.Raw = absoluteAddress.Raw;
}

void CPU::LAX()
{
	FetchValue();
	CHECK_NEGATIVE(fetchedVal);
	CHECK_ZERO(fetchedVal);

	acc = fetchedVal;
	idx = acc;
}

void CPU::LDA()
{
	FetchValue();
	CHECK_NEGATIVE(fetchedVal);
	CHECK_ZERO(fetchedVal);

	acc = fetchedVal;
}

void CPU::LDX()
{
	FetchValue();
	CHECK_NEGATIVE(fetchedVal);
	CHECK_ZERO(fetchedVal);

	idx = fetchedVal;
}

void CPU::LDY()
{
	FetchValue();
	CHECK_NEGATIVE(fetchedVal);
	CHECK_ZERO(fetchedVal);

	idy = fetchedVal;
}

void CPU::LSR()
{
	FetchValue();
	status.Flag.Carry = ((fetchedVal & 0x01) == 0x01);
	fetchedVal >>= 1;

	status.Flag.Negative = 0;
	CHECK_ZERO(fetchedVal);

	if (accumulatorAddressing)
		acc = fetchedVal;
	else
		Write(absoluteAddress.Raw, fetchedVal);
}

void CPU::LXA()
{
	FetchValue();
	acc = fetchedVal;
	idx = acc;

	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);

	additionalCycles = 0;
}

void CPU::NOP()
{
	// Nothing to do
}

void CPU::ORA()
{
	FetchValue();
	acc |= fetchedVal;

	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);
}

void CPU::PHA()
{
	Push(acc);
}

void CPU::PHP()
{
	Push(status.Raw | (0x3 << 4));
}

void CPU::PLA()
{
	acc = Pop();

	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);
}

void CPU::PLP()
{
	static Byte mask = 0x3 << 4;

	status.Raw = (status.Raw & mask) | (Pop() & ~mask);
}

void CPU::RLA()
{
	FetchValue();

	Byte oldCarry = status.Flag.Carry;
	status.Flag.Carry = ((fetchedVal & 0x80) == 0x80);
	fetchedVal <<= 1;
	fetchedVal |= oldCarry;

	Write(absoluteAddress.Raw, fetchedVal);

	acc &= fetchedVal;

	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);

	additionalCycles = 0;
}

void CPU::ROL()
{
	FetchValue();

	Byte oldCarry = status.Flag.Carry;
	status.Flag.Carry = ((fetchedVal & 0x80) == 0x80);
	fetchedVal <<= 1;
	fetchedVal |= oldCarry;

	CHECK_NEGATIVE(fetchedVal);
	CHECK_ZERO(fetchedVal);

	if (accumulatorAddressing)
		acc = fetchedVal;
	else
		Write(absoluteAddress.Raw, fetchedVal);
}

void CPU::ROR()
{
	FetchValue();

	Byte oldCarry = status.Flag.Carry;
	status.Flag.Carry = ((fetchedVal & 0x01) == 0x01);
	fetchedVal >>= 1;
	fetchedVal |= (oldCarry << 7);

	CHECK_NEGATIVE(fetchedVal);
	CHECK_ZERO(fetchedVal);

	if (accumulatorAddressing)
		acc = fetchedVal;
	else
		Write(absoluteAddress.Raw, fetchedVal);
}

void CPU::RRA()
{
	FetchValue();

	Byte oldCarry = status.Flag.Carry;
	status.Flag.Carry = ((fetchedVal & 0x01) == 0x01);
	fetchedVal >>= 1;
	fetchedVal |= (oldCarry << 7);
	
	Write(absoluteAddress.Raw, fetchedVal);

	Word result = (Word)acc + fetchedVal + status.Flag.Carry;

	status.Flag.Overflow = ((((~acc & ~fetchedVal & result) | (acc & fetchedVal & ~result)) & 0x80) == 0x80);

	acc = result & 0xFF;
	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);
	status.Flag.Carry = ((result & 0x100) == 0x100);

	additionalCycles = 0;
}

void CPU::RTI()
{
	static Byte mask = 0x3 << 4;

	status.Raw = (status.Raw & mask) | (Pop() & ~mask);
	pc.Bytes.lo = Pop();
	pc.Bytes.hi = Pop();
}

void CPU::RTS()
{
	pc.Bytes.lo = Pop();
	pc.Bytes.hi = Pop();
	pc.Raw++;
}

void CPU::SAX()
{
	Write(absoluteAddress.Raw, acc & idx);
}

void CPU::SBX()
{
	FetchValue();
	Word result = (acc & idx) - fetchedVal;

	CHECK_NEGATIVE(result);
	CHECK_ZERO(result);
	status.Flag.Carry = ((acc & idx) >= fetchedVal);

	idx = result;
}

void CPU::SBC()
{
	FetchValue();
	Word result = (Word)acc - fetchedVal - (1 - status.Flag.Carry);

	status.Flag.Overflow = ((((~acc & fetchedVal & result) | (acc & ~fetchedVal & ~result)) & 0x80) == 0x80);

	acc = result & 0xFF;
	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);
	status.Flag.Carry = ((result & 0x100) != 0x100);
}

void CPU::SEC()
{
	status.Flag.Carry = 1;
}

void CPU::SED()
{
	status.Flag.Decimal = 1;
}

void CPU::SEI()
{
	status.Flag.InterruptDisable = 1;
}

void CPU::SHX()
{
	Byte result = idx & (rawAddress.Bytes.hi + 1);
	absoluteAddress.Bytes.hi = result;
	Write(absoluteAddress.Raw, result);

	additionalCycles = 0;
}

void CPU::SHY()
{
	Byte result = idy & (rawAddress.Bytes.hi + 1);
	absoluteAddress.Bytes.hi = result;
	Write(absoluteAddress.Raw, result);

	additionalCycles = 0;
}

void CPU::SLO()
{
	FetchValue();
	status.Flag.Carry = ((fetchedVal & 0x80) == 0x80);
	fetchedVal <<= 1;

	Write(absoluteAddress.Raw, fetchedVal);

	acc |= fetchedVal;

	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);

	additionalCycles = 0;
}

void CPU::SRE()
{
	FetchValue();
	status.Flag.Carry = ((fetchedVal & 0x01) == 0x01);
	fetchedVal >>= 1;

	Write(absoluteAddress.Raw, fetchedVal);

	acc ^= fetchedVal;

	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);

	additionalCycles = 0;
}

void CPU::STA()
{
	Write(absoluteAddress.Raw, acc);
	additionalCycles = 0;
}

void CPU::STX()
{
	Write(absoluteAddress.Raw, idx);
}

void CPU::STY()
{
	Write(absoluteAddress.Raw, idy);
}

void CPU::TAX()
{
	idx = acc;

	CHECK_NEGATIVE(idx);
	CHECK_ZERO(idx);
}

void CPU::TAY()
{
	idy = acc;

	CHECK_NEGATIVE(idy);
	CHECK_ZERO(idy);
}

void CPU::TSX()
{
	idx = sp;

	CHECK_NEGATIVE(idx);
	CHECK_ZERO(idx);
}

void CPU::TXA()
{
	acc = idx;

	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);
}

void CPU::TXS()
{
	sp = idx;
}

void CPU::TYA()
{
	acc = idy;

	CHECK_NEGATIVE(acc);
	CHECK_ZERO(acc);
}

#pragma endregion

