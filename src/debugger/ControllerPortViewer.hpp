#pragma once

#include "DebugWindow.hpp"

class ControllerPort;

class ControllerPortViewer :
	public DebugWindow
{
public:
	ControllerPortViewer(Debugger* parent, ControllerPort* controllerPort);

	virtual void OnRender() override;

private:
	ControllerPort* controllerPort;
};
