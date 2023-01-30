#pragma once

#include <vector>
#include <imgui/imgui.h>
#include "DebugWindow.hpp"

class Logger final: 
    public DebugWindow 
{
public:
    static void Init(Debugger* debugger);

    static Logger* GetInstance() {
        return instance;
    }

    void Log(const char* module, const char* fmt, ...);
    virtual void OnRender() override;

private:
    Logger(Debugger* debugger);

private:
    inline static Logger* instance = nullptr;

    ImGuiTextBuffer buffer;
    std::vector<int> offsets;
    bool autoScroll;
};