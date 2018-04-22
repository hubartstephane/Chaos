#include "LudumGame.h"
#include "LudumWindow.h"

#include <chaos/JSONTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/FileTools.h>
#include <chaos/WinTools.h>
#include <chaos/Application.h>
#include <chaos/InputMode.h>



void LudumGame::CreateTitle(char const * title)
{
	chaos::ParticleTextGenerator::GeneratorParams params;
	params.line_height = TITLE_SIZE;
	params.hotpoint_type = chaos::Hotpoint::CENTER;
	params.position.y = TITLE_PLACEMENT_Y;

	text_allocations = CreateTextParticles(title, params);
}

void LudumGame::DestroyTitle()
{
	text_allocations = nullptr;
}

void LudumGame::OnStartGame(bool very_first)
{
	if (very_first)
	{
		StartMainMenuMusic(true);
		CreateTitle("AsciiPaouf II");
	}
}

bool LudumGame::OnEnterPause()
{
	StartPauseMusic(true);
	CreateTitle("Pause");

	return true;
}

bool LudumGame::OnLeavePause()
{
	StartGameMusic(false);
	DestroyTitle();

	return true;
}

bool LudumGame::OnEnterGame()
{
	StartGameMusic(true);
	DestroyTitle();
	ResetGameVariables();
	CreateGameObjects(0);
	return true;
}

bool LudumGame::OnLeaveGame()
{
	StartMainMenuMusic(true);





	return true;
}

void LudumGame::HandleKeyboardInputs()
{
	// test whether the stick position can be overriden
	glm::vec2 simulated_stick = glm::vec2(0.0f, 0.0f);

	if (glfwGetKey(glfw_window, GLFW_KEY_LEFT))
		simulated_stick.x -= 1.0f;
	if (glfwGetKey(glfw_window, GLFW_KEY_RIGHT))
		simulated_stick.x += 1.0f;

	if (glfwGetKey(glfw_window, GLFW_KEY_DOWN))
		simulated_stick.y -= 1.0f;
	if (glfwGetKey(glfw_window, GLFW_KEY_UP))
		simulated_stick.y += 1.0f;

	if (glm::length2(simulated_stick) > 0)
		left_stick_position = simulated_stick;
}

void LudumGame::UpdateGameState(double delta_time)
{
	game_automata->Tick(delta_time);
}

void LudumGame::Tick(double delta_time)
{
	// catch all stick inputs
	gamepad_manager->Tick((float)delta_time);
	// handle keyboard inputs
	HandleKeyboardInputs();
	// update the game state
	UpdateGameState(delta_time);
	// clear the cached inputs
	ResetPlayerCachedInputs();
	// tick the particle manager
	if (particle_manager != nullptr)
		particle_manager->Tick((float)delta_time);
}

bool LudumGame::RequireGameOver()
{
	if (game_automata->playing_to_gameover->TriggerTransition(true))
		return true;
	return false;
}

bool LudumGame::RequireTogglePause()
{
	if (game_automata->playing_to_pause->TriggerTransition(true))
		return true;
	if (game_automata->pause_to_playing->TriggerTransition(true))
		return true;
	return false;
}

bool LudumGame::RequireExitGame()
{
	if (game_automata->playing_to_main_menu->TriggerTransition(true))
		return true;
	return false;
}

bool LudumGame::RequireStartGame()
{
	if (game_automata->main_menu_to_playing->TriggerTransition(true))
		return true;
	return false;
}

bool LudumGame::OnKeyEvent(int key, int action)
{
	// MAIN MENU to PLAYING
	if (action == GLFW_PRESS)
		if (RequireStartGame())
			return true;

	// PLAYING to PAUSE
	if ((key == GLFW_KEY_KP_ENTER || key == GLFW_KEY_ENTER) && action == GLFW_PRESS)
		if (RequireTogglePause())
			return true;

	// QUIT GAME
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		if (RequireExitGame())
			return true;

	// FORCE GAMEOVER
#if _DEBUG
	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
		if (RequireGameOver())
			return true;
#endif

	// CHALLENGE
	if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z && action == GLFW_PRESS)
		SendKeyboardButtonToChallenge(key);

	return false;
}







void LudumGame::ResetPlayerCachedInputs()
{
	left_stick_position  = glm::vec2(0.0f, 0.0f);
	right_stick_position = glm::vec2(0.0f, 0.0f);
}

bool LudumGame::OnPhysicalGamepadInput(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
{
	// ignore invalid gamepad : should never happen
	if (!physical_gamepad->IsAnyAction())
		return true;

	// change the application mode
	chaos::Application::SetApplicationInputMode(chaos::InputMode::Gamepad);

	// cache the stick position
	glm::vec2 lsp = physical_gamepad->GetXBOXStickDirection(chaos::MyGLFW::XBOX_LEFT_AXIS);
	if (glm::length2(lsp) > 0.0f)
		left_stick_position = lsp;

	glm::vec2 rsp = physical_gamepad->GetXBOXStickDirection(chaos::MyGLFW::XBOX_RIGHT_AXIS);
	if (glm::length2(rsp) > 0.0f)
		right_stick_position = rsp;


	// maybe this correspond to current challenge
	SendGamepadButtonToChallenge(physical_gamepad);

	// maybe a start game
	if (physical_gamepad->IsAnyButtonPressed())
		if (game_automata->main_menu_to_playing->TriggerTransition(true))
			return true;

	// maybe a game/pause resume
	if (
		(physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_SELECT) == chaos::MyGLFW::BUTTON_BECOME_PRESSED) ||
		(physical_gamepad->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_START) == chaos::MyGLFW::BUTTON_BECOME_PRESSED))
	{
		if (RequireTogglePause())
			return true;
	}

	return true;
}

glm::vec2 LudumGame::GetWorldSize() const
{
	glm::vec2 result;
	result.x = 1600.0f;
	result.y = result.x / LudumWindow::GetViewportAspect();
	return result;
}

void LudumGame::Display(chaos::box2 const & viewport)
{
	// clear the color buffers
	glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	int state_id = GetCurrentStateID();
	if (state_id == LudumAutomata::STATE_MAINMENU)
		clear_color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	else if (state_id == LudumAutomata::STATE_PAUSE)
		clear_color = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	else if (state_id == LudumAutomata::STATE_PLAYING)
		clear_color = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

	// clear the depth buffers
	float far_plane = 1000.0f;
	glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

	// some states
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// a variable provider
	chaos::GPUProgramProvider main_uniform_provider;
	main_uniform_provider.AddVariableValue("viewport_size", viewport.half_size * 2.0f);

	glm::vec2 world_size;
	world_size.x = 1600.0f;
	world_size.y = world_size.x * (viewport.half_size.y / viewport.half_size.x);
	main_uniform_provider.AddVariableValue("world_size", world_size);

	// draw particle system
	if (particle_manager != nullptr)
		particle_manager->Display(&main_uniform_provider);
}

void LudumGame::OnInputModeChanged(int new_mode, int old_mode)
{
	if (sequence_challenge != nullptr)
		sequence_challenge->particle_range = CreateChallengeText(sequence_challenge.get());	
}


void LudumGame::ResetGameVariables()
{
	current_life  = initial_life;
	player_length = player_initial_length;
	ball_speed    = ball_initial_speed;
}
void LudumGame::OnGameOver()
{
	DestroyGameObjects();




}

void LudumGame::TickGameLoop(double delta_time)
{

}

void LudumGame::SendKeyboardButtonToChallenge(int key)
{
	if (sequence_challenge != nullptr)
		sequence_challenge->OnKeyboardButtonReceived('a' + key - GLFW_KEY_A);
}

void LudumGame::SendGamepadButtonToChallenge(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
{
	if (sequence_challenge != nullptr)
		sequence_challenge->OnGamepadButtonReceived(physical_gamepad);
}

void LudumGame::OnMouseButton(int button, int action, int modifier)
{
	chaos::StateMachine::State const * state = game_automata->GetCurrentState();

	if (state != nullptr && state->GetStateID() == LudumAutomata::STATE_PLAYING)
	{
		if (button == 0 && action == GLFW_PRESS)
		{
			int len = min_word_size + rand() % (max_word_size - min_word_size);
			sequence_challenge = CreateSequenceChallenge((size_t)len);
		}	
	}
}

void LudumGame::OnChallengeCompleted(LudumSequenceChallenge * challenge)
{
	sequence_challenge = nullptr;
}

void LudumGame::DestroyGameObjects()
{
	player_allocations = nullptr;
	bricks_allocations = nullptr;
	lifes_allocations = nullptr;
	balls_allocations = nullptr;

}


void LudumGame::CreateGameObjects(int level)
{

}



