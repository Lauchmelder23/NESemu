#pragma once

#include <string>

class DebugWindow
{
public:
	DebugWindow(const std::string& title) :
		title(title)
	{
	}

	virtual ~DebugWindow() = default;
	virtual void OnRender() = 0;

public:
	const std::string title;
	bool isOpen = false;
};
