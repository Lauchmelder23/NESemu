#include "Disassembler.hpp"

#include <iomanip>
#include <imgui/imgui.h>
#include "../Mapper.hpp"
#include "../CPU.hpp"

#define FORMAT std::setfill('0') << std::setw(4) << std::hex << std::uppercase

Disassembler::Disassembler(CPU* cpu) :
	DebugWindow("Disassembler"), cpu(cpu)
{
	
}

void Disassembler::OnRender()
{
	static bool scrollCenter = true;

	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin(title.c_str(), &isOpen))
	{
		ImGui::End();
		return;
	}

	std::string disassembly;

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.8f, 0.8f, 0.8f, 1.0f });
	if (cpu->pastPCs.size() < 50)
	{
		for (int i = 0; i < 50 - cpu->pastPCs.size(); i++)
		{
			ImGui::Text("-");
		}
	}

	for (Word pc : cpu->pastPCs)
	{
		Disassemble(disassembly, pc);
		ImGui::Text("- %s", disassembly.c_str());
	}
	ImGui::PopStyleColor();

	ImGui::Separator();

	uint16_t pc = cpu->pc.Raw;
	Disassemble(disassembly, pc);
	ImGui::Text("> %s", disassembly.c_str());

	if (scrollCenter)
	{
		ImGui::SetScrollHereY(0.5f);
		scrollCenter = false;
	}

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.8f, 0.8f, 0.8f, 1.0f });
	for (int i = 0; i < 50; i++)
	{
		Disassemble(disassembly, pc);
		ImGui::Text("+ %s", disassembly.c_str());
	}
	ImGui::PopStyleColor();

	ImGui::End();
}

void Disassembler::Disassemble(std::string& target, uint16_t& pc)
{
	std::stringstream ss;
	ss <<  FORMAT << pc << ": ";
	Instruction* currentInstr = &cpu->InstructionTable[cpu->Read(pc)];

	for (int i = 0; i < currentInstr->Size; i++)
	{
		ss << FORMAT << std::setw(2) << (Word)cpu->Read(pc + i) << " ";
	}
	ss << std::string(15 - ss.str().size(), ' ') << currentInstr->Mnemonic << " ";

	Address absoluteAddress;
	switch (currentInstr->AddrType)
	{
	case Addressing::ACC:
		ss << "A";
		break;

	case Addressing::ABS:
	{
		absoluteAddress.Bytes.lo = cpu->Read(pc + 1);
		absoluteAddress.Bytes.hi = cpu->Read(pc + 2);

		ss << "$" << FORMAT << absoluteAddress.Raw;
	} break;

	case Addressing::ABX:
	{
		absoluteAddress.Bytes.lo = cpu->Read(pc + 1);
		absoluteAddress.Bytes.hi = cpu->Read(pc + 2);

		ss << "$" << FORMAT << absoluteAddress.Raw << ",X";
	} break;

	case Addressing::ABY:
	{
		absoluteAddress.Bytes.lo = cpu->Read(pc + 1);
		absoluteAddress.Bytes.hi = cpu->Read(pc + 2);

		ss << "$" << FORMAT << absoluteAddress.Raw << ",Y";
	} break;

	case Addressing::IMM:
	{
		Word value = cpu->Read(pc + 1);

		ss << "#$" << FORMAT << std::setw(2) << value;
	} break;

	case Addressing::IMP: 
		break;

	case Addressing::IND:
	{
		absoluteAddress.Bytes.lo = cpu->Read(pc + 1);
		absoluteAddress.Bytes.hi = cpu->Read(pc + 2);

		ss << "($" << FORMAT << absoluteAddress.Raw << ")";
	} break;

	case Addressing::IDX:
	{
		absoluteAddress.Bytes.lo = cpu->Read(pc + 1);

		ss << "($" << FORMAT << std::setw(2) << (Word)absoluteAddress.Bytes.lo << ",X)";
	} break;

	case Addressing::IDY:
	{
		absoluteAddress.Bytes.lo = cpu->Read(pc + 1);

		ss << "($" << FORMAT << std::setw(2) << (Word)absoluteAddress.Bytes.lo << "),Y";
	} break;

	case Addressing::REL:
	{
		absoluteAddress.Bytes.lo = cpu->Read(pc + 1);

		ss << "$" << FORMAT << std::setw(2) << (Word)absoluteAddress.Bytes.lo;
	} break;

	case Addressing::ZPG:
	{
		absoluteAddress.Bytes.lo = cpu->Read(pc + 1);

		ss << "$" << FORMAT << std::setw(2) << (Word)absoluteAddress.Bytes.lo;
	} break;

	case Addressing::ZPX:
	{
		absoluteAddress.Bytes.lo = cpu->Read(pc + 1);

		ss << "$" << FORMAT << std::setw(2) << (Word)absoluteAddress.Bytes.lo << ",X";
	} break;

	case Addressing::ZPY:
	{
		absoluteAddress.Bytes.lo = cpu->Read(pc + 1);

		ss << "$" << FORMAT << std::setw(2) << (Word)absoluteAddress.Bytes.lo << ",Y";
	} break;
	}

	pc += currentInstr->Size;
	target = ss.str();
}
