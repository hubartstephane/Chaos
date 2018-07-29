#include <death/Game.h>
#include <death/GamepadManager.h>

#include <chaos/Application.h>
#include <chaos/InputMode.h>

namespace death
{
	bool Game::CreateAllMusics()
	{
		return true;
	}

	bool Game::DeclareParticleClasses()
	{
		return true;
	}

	bool Game::CreateGameAutomata()
	{
		return true;
	}

	bool Game::InitializeGame(GLFWwindow * in_glfw_window)
	{
		// initialize the window
		assert(in_glfw_window != nullptr);
		glfw_window = in_glfw_window;

		// initialize the gamepad manager
		gamepad_manager = new GamepadManager(this);
		if (gamepad_manager == nullptr)
			return false;

		// create game automata
		if (!CreateGameAutomata())
			return false;

		// create the musics
		if (!CreateAllMusics())
			return false;

		// initialize particle classes
		if (!DeclareParticleClasses())
			return false;

		return true;
	}

	void Game::HandleGamepadInput(chaos::MyGLFW::GamepadData & in_gamepad_data)
	{

	}

	void Game::OnGamepadInput(chaos::MyGLFW::GamepadData & in_gamepad_data)
	{

	}

	bool Game::OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
	{
		// ignore invalid gamepad : should never happen
		if (!physical_gamepad->IsAnyAction())
			return true;

		// change the application mode
		chaos::Application::SetApplicationInputMode(chaos::InputMode::Gamepad);

		// copy the gamepad information into the game and handle it
		gamepad_data = physical_gamepad->GetGamepadData();

		HandleGamepadInput(gamepad_data);

		// special action on gamepad input
		OnGamepadInput(gamepad_data);


		return true;
	}


}; // namespace death

