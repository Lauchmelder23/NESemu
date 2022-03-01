#include "ControllerPortViewer.hpp"

#include "../ControllerPort.hpp"
#include <imgui/imgui.h>

ControllerPortViewer::ControllerPortViewer(Debugger* parent, ControllerPort* controllerPort) :
	DebugWindow("Controller Port Viewer", parent), controllerPort(controllerPort)
{
}

void ControllerPortViewer::OnRender()
{
	if (!ImGui::Begin(title.c_str(), &isOpen))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Latch          : %02X", controllerPort->latch.Raw);
	ImGui::Text("Controller port: %d", controllerPort->latch.Ports.Controller);
	ImGui::Text("Expansion port : %d", controllerPort->latch.Ports.Expansion);

	int counter = 1;
	for (Controller* controller : controllerPort->connectedDevices)
	{
		if (controller == nullptr)
			continue;

		std::string controllerName = "Controller " + std::to_string(counter);
		if (ImGui::CollapsingHeader(controllerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Output Line: D%d", controller->outPin);

			ImGui::Separator();

			ImGui::InputScalar("Shift Register", ImGuiDataType_U8, &controller->outRegister, (const void*)0, (const void*)0, "%02X", ImGuiInputTextFlags_CharsHexadecimal);
			
			if (ImGui::BeginTable(controllerName.c_str(), 8))
			{
				for(int i = 0; i < 8; i++)
				{
					ImGui::TableNextColumn();
					ImGui::Text("%d", i);
				}

				for (int i = 0; i < 8; i++)
				{
					ImGui::TableNextColumn();
					ImGui::Text("%d", (controller->outRegister >> i) & 0x1);
				}

				ImGui::EndTable();
			}
		}
	}

	ImGui::End();
}
