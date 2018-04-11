#include "LudumGame.h"

bool LudumGamepadManager::DoPoolGamepad(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
{
	if (game != nullptr)
		return game->OnPhysicalGamepadInput(physical_gamepad);
	return true;
}

void LudumGame::Tick(double delta_time)
{
	// catch all stick inputs
	gamepad_manager->Tick((float)delta_time); 

	if (pending_gameover)
	{
		//GameOver();
	}
	else if (pending_restart_game)
	{

	}
	else
	{
		


		
	}

	// clear the cached inputs
	ResetPlayerCachedInputs();
}

bool LudumGame::OnKeyEvent(int key, int action)
{









	return true;
}

void LudumGame::Display(chaos::box2 const & viewport)
{
	// clear the buffers
	glm::vec4 clear_color(1.0f, 0.0f, 0.0f, 0.0f);
	glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

	float far_plane = 1000.0f;
	glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

	// some states
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE); 




}

bool LudumGame::InitializeGame(GLFWwindow * in_glfw_window)
{
	// initialize the window
	assert(in_glfw_window != nullptr);
	glfw_window = in_glfw_window;

	// initialize the gamepad manager
	gamepad_manager = new LudumGamepadManager(this);
	if (gamepad_manager == nullptr)
		return false;

	return true;
}

bool LudumGame::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
{






	return true;
}

void LudumGame::ResetPlayerCachedInputs()
{
	stick_position = glm::vec2(0.0f, 0.0f);
}

bool LudumGame::OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
{
#if 0

	if (!game_started)
	{
		if (physical_gamepad->IsAnyButtonPressed())
			StartGame();
	}
	else
	{
		if ((physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_SELECT) == chaos::MyGLFW::BUTTON_BECOME_PRESSED) ||
			(physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_START) == chaos::MyGLFW::BUTTON_BECOME_PRESSED))
			SetPause(!game_paused);
	}

	glm::vec2 left_stick_position = physical_gamepad->GetXBOXStickDirection(chaos::MyGLFW::XBOX_LEFT_AXIS);
	glm::vec2 right_stick_position = physical_gamepad->GetXBOXStickDirection(chaos::MyGLFW::XBOX_RIGHT_AXIS);

	if (left_stick_position.x != 0.0f || left_stick_position.y != 0.0f)
		stick_position = left_stick_position;
	else if (right_stick_position.length() > 0.0f)
		stick_position = right_stick_position;
#endif
	return true;
}