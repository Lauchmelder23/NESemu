#include "Logger.hpp"

#include <imgui/imgui.h>

Logger::Logger(Debugger* debugger) :
    DebugWindow("Log", debugger), autoScroll(true)
{
    buffer.clear();

    offsets.clear();
    offsets.push_back(0);
}

void Logger::Init(Debugger* debugger) 
{
    if (Logger::instance != nullptr) {
        return;
    }

    Logger::instance = new Logger(debugger);
}

void Logger::Log(const char* module, const char* fmt, ...) 
{
    int old_size = buffer.size();
    va_list args;
    va_start(args, fmt);

    buffer.appendf("[%s] ", module);
    buffer.appendfv(fmt, args);
    va_end(args);

    for (int new_size = buffer.size(); old_size < new_size; old_size++) {
        if (buffer[old_size] == '\n') {
            offsets.push_back(old_size + 1);
        }
    }
}

void Logger::OnRender() 
{
    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin(title.c_str(), &isOpen)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginPopup("Options")) {
        ImGui::Checkbox("Auto scroll", &autoScroll);
        ImGui::EndPopup();
    }

    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            const char* buf = buffer.begin();
            const char* buf_end = buffer.end();

            ImGuiListClipper clipper;
            clipper.Begin((int)offsets.size());
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + offsets[line_no];
                    const char* line_end = (line_no + 1 < offsets.size()) ? (buf + offsets[line_no + 1] - 1) : buf_end;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
            ImGui::PopStyleVar();

            if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();

    ImGui::End();
}