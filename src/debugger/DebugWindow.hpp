#pragma once

#include <string>

class Debugger;

class DebugWindow
{
public:
	DebugWindow(const std::string& title, Debugger* parent) :
		title(title), parent(parent)
	{
	}

	virtual ~DebugWindow() = default;
	virtual void OnRender() = 0;

public:
	const std::string title;
	bool isOpen = false;

protected:
	Debugger* parent;
};
