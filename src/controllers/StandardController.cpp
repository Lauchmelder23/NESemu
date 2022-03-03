#include "StandardController.hpp"

#include "Input.hpp"

StandardController::StandardController() :
	Controller(0)
{
}

void StandardController::OUT(PortLatch latch)
{
	if (!latch.Ports.Controller)
		return;

	StandardButtons pressed;

	pressed.Buttons.A		= Input::IsKeyDown(GLFW_KEY_S);
	pressed.Buttons.B		= Input::IsKeyDown(GLFW_KEY_A);
	pressed.Buttons.Select	= Input::IsKeyDown(GLFW_KEY_RIGHT_SHIFT);
	pressed.Buttons.Start	= Input::IsKeyDown(GLFW_KEY_ENTER);
	pressed.Buttons.Up		= Input::IsKeyDown(GLFW_KEY_UP);
	pressed.Buttons.Down	= Input::IsKeyDown(GLFW_KEY_DOWN);
	pressed.Buttons.Left	= Input::IsKeyDown(GLFW_KEY_LEFT);
	pressed.Buttons.Right	= Input::IsKeyDown(GLFW_KEY_RIGHT);

	if (pressed.Raw == 0xFF)
		volatile int dkjf = 3;

	outRegister = pressed.Raw;
}
