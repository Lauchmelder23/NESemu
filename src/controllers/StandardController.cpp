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

	pressed.Buttons.A		= Input::IsKeyDown(GLFW_KEY_L);
	pressed.Buttons.B		= Input::IsKeyDown(GLFW_KEY_K);
	pressed.Buttons.Select	= Input::IsKeyDown(GLFW_KEY_RIGHT_SHIFT);
	pressed.Buttons.Start	= Input::IsKeyDown(GLFW_KEY_ENTER);
	pressed.Buttons.Up		= Input::IsKeyDown(GLFW_KEY_W);
	pressed.Buttons.Down	= Input::IsKeyDown(GLFW_KEY_S);
	pressed.Buttons.Left	= Input::IsKeyDown(GLFW_KEY_A);
	pressed.Buttons.Right	= Input::IsKeyDown(GLFW_KEY_D);

	outRegister = pressed.Raw;
}
