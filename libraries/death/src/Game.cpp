#include <death/Game.h>

#include <chaos/Application.h>
#include <chaos/InputMode.h>

namespace death
{
	bool Game::OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
	{
		// ignore invalid gamepad : should never happen
		if (!physical_gamepad->IsAnyAction())
			return true;

		// copy the gamepad information into the game
		gamepad_data = physical_gamepad->GetGamepadData();

		// change the application mode
		chaos::Application::SetApplicationInputMode(chaos::InputMode::Gamepad);

		return true;
	}


}; // namespace death

