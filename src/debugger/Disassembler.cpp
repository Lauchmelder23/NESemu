#include "Disassembler.hpp"

#include <iomanip>
#include <imgui/imgui.h>
#include "../Mapper.hpp"
#include "../CPU.hpp"
#include "Debugger.hpp"

#define FORMAT std::setfill('0') << std::setw(4) << std::hex << std::uppercase

Disassembler::Disassembler(Debugger* debugger, CPU* cpu) :
	DebugWindow("Disassembler", debugger), cpu(cpu)
{
	
}

void Disassembler::OnRender()
{
	static bool scrollCenter = true;

	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin(title.c_str(), &isOpen, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::MenuItem("Breakpoints", NULL, &showBreakpoints);
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	std::string disassembly;

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0.8f, 0.8f, 0.8f, 1.0f });
	if (cpu->pastInstructions.size() < 50)
	{
		for (int i = 0; i < 50 - cpu->pastInstructions.size(); i++)
		{
			ImGui::Text("-");
		}
	}

	for (auto pair : cpu->pastInstructions)
	{
		Disassemble(disassembly, pair.first, pair.second);
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

	if (showBreakpoints)
	{
		BreakpointWindow();
	}

	ImGui::End();
}

bool Disassembler::BreakpointHit()
{
	auto bpFound = breakpoints.find(cpu->pc.Raw);
	if (bpFound != breakpoints.end() && bpFound->active)
		return true;

	return false;
}

void Disassembler::Disassemble(std::string& target, uint16_t& pc)
{
	Instruction* currentInstr = &cpu->InstructionTable[cpu->Read(pc)];
	Disassemble(target, pc, currentInstr);
	pc += currentInstr->Size;
}

void Disassembler::Disassemble(std::string& target, uint16_t pc, const Instruction* instr)
{
	std::stringstream ss;
	ss << FORMAT << pc << ": ";

	for (int i = 0; i < instr->Size; i++)
	{
		ss << FORMAT << std::setw(2) << (Word)cpu->Read(pc + i) << " ";
	}
	ss << std::string(15 - ss.str().size(), ' ') << instr->Mnemonic << " ";

	Address absoluteAddress;
	switch (instr->AddrType)
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

	target = ss.str();
}

void Disassembler::BreakpointWindow()
{
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
	
	if (!ImGui::Begin("Breakpoints", &showBreakpoints))
	{
		ImGui::End();
		return;
	}

	ImGui::InputScalar("##", ImGuiDataType_U16, &tempBreakpoint, (const void*)0, (const void*)0, "%04X", ImGuiInputTextFlags_CharsHexadecimal);
	ImGui::SameLine();
	if (ImGui::Button("Add Breakpoint"))
	{
		breakpoints.insert(Breakpoint(tempBreakpoint));
	}

	ImGui::Separator();

	char label[6];
	for (std::set<Breakpoint>::const_iterator it = breakpoints.begin(); it != breakpoints.end(); )
	{
		if (ImGui::Button("X"))
		{ 
			it = breakpoints.erase(it);
			continue;
		}

		ImGui::SameLine();

		std::sprintf(label, "$%04X", it->GetAddress());
		ImGui::Checkbox(label, &it->active);
		
		it++;
	}

	ImGui::End();
}
