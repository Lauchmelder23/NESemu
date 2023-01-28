#pragma once

#include <memory>
#include <array>
#include <type_traits>
#include "Types.hpp"

#include "controllers/Controller.hpp"

class ControllerPort
{
	friend class ControllerPortViewer;

public:
	ControllerPort();
	~ControllerPort();

	void Write(Byte val);
	Byte Read(Word addr);

	void Tick();

	template<
		typename T,
		std::enable_if_t<std::is_base_of<Controller, T>::value, bool> = true
	>
	void PlugInController(int port)
	{
		connectedDevices[port] = new T;
	}

private:
	PortLatch latch;
	std::array<Controller*, 2> connectedDevices;
};