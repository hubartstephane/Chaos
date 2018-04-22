#include "LudumGame.h"
#include "LudumWindow.h"
#include "LudumParticles.h"

#include <chaos/JSONTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/FileTools.h>
#include <chaos/WinTools.h>
#include <chaos/Application.h>
#include <chaos/InputMode.h>
#include <chaos/GeometryFramework.h>
#include <chaos/CollisionFramework.h>

bool LudumGame::IsPlaying() const
{
	if (game_automata->GetCurrentState()->GetStateID() != LudumAutomata::STATE_PLAYING)
		return false;
	return true;
}

void LudumGame::CreateGameTitle()
{
	CreateTitle("AsciiPaouf II");
}

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
		chaos::MathTools::ResetRandSeed();

		glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		StartMainMenuMusic(true);
		CreateGameTitle();
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
	CreateAllGameObjects(0);
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
		left_stick_position = gamepad_sensitivity * simulated_stick;
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
		left_stick_position = gamepad_sensitivity * lsp;
	else
	{
		if (physical_gamepad->IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_LEFT))
			left_stick_position.x = -gamepad_sensitivity * 1.0f;
		else if (physical_gamepad->IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_RIGHT))
			left_stick_position.x =  gamepad_sensitivity * 1.0f;
	}

	glm::vec2 rsp = physical_gamepad->GetXBOXStickDirection(chaos::MyGLFW::XBOX_RIGHT_AXIS);
	if (glm::length2(rsp) > 0.0f)
		right_stick_position = gamepad_sensitivity * rsp;

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

chaos::box2 LudumGame::GetWorldBox() const
{
	chaos::box2 result;
	result.position  = glm::vec2(0.0f, 0.0f);
	result.half_size = GetWorldSize() * 0.5f;
	return result;
}

void LudumGame::Display(chaos::box2 const & viewport)
{
	// clear the color buffers
	glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

#if 0
	int state_id = GetCurrentStateID();
	if (state_id == LudumAutomata::STATE_MAINMENU)
		clear_color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	else if (state_id == LudumAutomata::STATE_PAUSE)
		clear_color = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	else if (state_id == LudumAutomata::STATE_PLAYING)
		clear_color = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
#endif
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
	ball_time_dilation = 1.0f;
	challenge_timer    = challenge_frequency;
}
void LudumGame::OnGameOver()
{
	DestroyGameObjects();




}

void LudumGame::DisplacePlayer(double delta_time)
{
	float value = left_stick_position.x;
	if (abs(right_stick_position.x) > abs(left_stick_position.x))
		value = right_stick_position.x;

	glm::vec2 position = GetPlayerPosition();
	SetPlayerPosition(position.x + value);
}

void LudumGame::TickGameLoop(double delta_time)
{
	DisplacePlayer(delta_time);

	if (sequence_challenge != nullptr)
	{
		sequence_challenge->Tick(delta_time);
	}
	else
	{
		// start a challenge (only if one ball is going upward)
		challenge_timer = max(0.0f, challenge_timer - (float)delta_time);
		if(challenge_timer <= 0.0f)
		{
			size_t ball_count = GetBallCount();
			if (ball_count > 0)
			{
				ParticleMovableObject const * balls = GetBallParticles();
				if (balls != nullptr)
				{
					size_t i = 0;
					for (; i < ball_count ; ++i)
					{
						if (balls->velocity.y > 0.0f)
							break;					
					}
					if (i != ball_count)
						sequence_challenge = CreateSequenceChallenge(0);				
				}			
			}
		}		
	}
}

void LudumGame::SendKeyboardButtonToChallenge(int key)
{
	if (!IsPlaying())
		return;
	if (sequence_challenge != nullptr)
		sequence_challenge->OnKeyboardButtonReceived('a' + key - GLFW_KEY_A);
}

void LudumGame::SendGamepadButtonToChallenge(chaos::MyGLFW::PhysicalGamepad * physical_gamepad)
{
	if (!IsPlaying())
		return;
	if (sequence_challenge != nullptr)
		sequence_challenge->OnGamepadButtonReceived(physical_gamepad);
}

void LudumGame::OnMouseMove(double x, double y)
{
	left_stick_position.x = mouse_sensitivity * (float)x;
}

void LudumGame::OnMouseButton(int button, int action, int modifier)
{
	chaos::StateMachine::State const * state = game_automata->GetCurrentState();
	if (state != nullptr)
	{
#if _DEBUG
		if (state->GetStateID() == LudumAutomata::STATE_PLAYING)
		{
			if (button == 0 && action == GLFW_PRESS)
				sequence_challenge = CreateSequenceChallenge(0);
		}
		else 
#endif			
		if (state->GetStateID() == LudumAutomata::STATE_MAINMENU)
		{
			if (action == GLFW_PRESS)
				RequireStartGame();
		}		
	}
}

void LudumGame::OnChallengeCompleted(LudumSequenceChallenge * challenge, bool success)
{
	sequence_challenge = nullptr;
	ball_time_dilation = 1.0f;
	challenge_timer    = challenge_frequency;
}

void LudumGame::DestroyGameObjects()
{
	player_allocations = nullptr;
	bricks_allocations = nullptr;
	lifes_allocations = nullptr;
	balls_allocations = nullptr;

	sequence_challenge = nullptr;

}

chaos::ParticleRangeAllocation * LudumGame::CreateGameObjects(char const * name, size_t count, int layer_id)
{
	// find layer of concern
	chaos::ParticleLayer * layer = particle_manager->FindLayer(layer_id);
	if (layer == nullptr)
		return nullptr;

	// find bitmap set
	chaos::BitmapAtlas::BitmapSet const * bitmap_set = texture_atlas->GetBitmapSet("sprites");
	if (bitmap_set == nullptr)
		return nullptr;

	// find bitmap entry
	chaos::BitmapAtlas::BitmapEntry const * entry = bitmap_set->GetEntry(name);
	if (entry == nullptr)
		return nullptr;

	// allocate the objects
	chaos::ParticleRangeAllocation * allocation = layer->SpawnParticlesAndKeepRange(count);
	if (allocation == nullptr)
		return nullptr;

	ParticleObject * particle = (ParticleObject *)allocation->GetParticleBuffer();
	if (particle == nullptr)
		return nullptr;

	for (size_t i = 0 ; i < count ; ++i)
		particle->texcoords = chaos::ParticleTools::GetParticleTexcoords(*entry, texture_atlas->GetAtlasDimension());

	return allocation;
}




glm::vec2 LudumGame::GenerateBallRandomDirection() const
{
	// direction upward
	float angle = 3.14f * 0.5f + chaos::MathTools::RandFloat(-3.14f * 0.33f, 3.14f * 0.33f);

	return glm::vec2(
		chaos::MathTools::Cos(angle),
		chaos::MathTools::Sin(angle));
}

chaos::ParticleRangeAllocation * LudumGame::CreateBall()
{

	// create the object
	chaos::ParticleRangeAllocation * result = CreateGameObjects("ball", 1, BALL_LAYER_ID);
	if (result == nullptr)
		return nullptr;

	// set the color
	ParticleMovableObject * particle = (ParticleMovableObject *)result->GetParticleBuffer();
	if (particle == nullptr)
		return nullptr;
	particle->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);


	particle->corners = chaos::ParticleTools::GetParticleCorners(glm::vec2(0.0f, 0.0f), glm::vec2(ball_size, ball_size), chaos::Hotpoint::CENTER);

	particle->delay_before_move = delay_before_ball_move;
	particle->velocity = ball_speed * GenerateBallRandomDirection();

	//particle->corners.bottomleft = glm::vec2(0.0f, 0.0f);
	//particle->corners.topright   = glm::vec2(0.0f, 0.0f);

	return result;
}








chaos::ParticleRangeAllocation * LudumGame::CreatePlayer()
{
	// create the object
	chaos::ParticleRangeAllocation * result = CreateGameObjects("player", 1);
	if (result == nullptr)
		return nullptr;

	// set the color
	ParticleObject * particle = (ParticleObject *)result->GetParticleBuffer();
	if (particle == nullptr)
		return nullptr;
	particle->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	particle->corners.bottomleft = glm::vec2(0.0f, 0.0f);
	particle->corners.topright   = glm::vec2(0.0f, 0.0f);
	
	return result;
}


ParticleObject const * LudumGame::GetObjectParticle(chaos::ParticleRangeAllocation * allocation, size_t index) const
{
	if (allocation == nullptr)
		return nullptr;
	if (index >= allocation->GetParticleCount())
		return nullptr;

	ParticleObject const * p = (ParticleObject const*)player_allocations->GetParticleBuffer();
	if (p == nullptr)
		return nullptr;

	return &p[index];
}


ParticleMovableObject * LudumGame::GetBallParticles()
{
	if (balls_allocations == nullptr)
		return nullptr;	
	return (ParticleMovableObject *)balls_allocations->GetParticleBuffer();
}

ParticleMovableObject const * LudumGame::GetBallParticles() const
{
	if (balls_allocations == nullptr)
		return nullptr;
	return (ParticleMovableObject const *)balls_allocations->GetParticleBuffer();
}

size_t LudumGame::GetBallCount() const
{
	if (balls_allocations == nullptr)
		return 0;	
	return balls_allocations->GetParticleCount();
}

ParticleObject * LudumGame::GetObjectParticle(chaos::ParticleRangeAllocation * allocation, size_t index)
{
	if (allocation == nullptr)
		return nullptr;
	if (index >= allocation->GetParticleCount())
		return nullptr;

	ParticleObject * p = (ParticleObject *)player_allocations->GetParticleBuffer();
	if (p == nullptr)
		return nullptr;

	return &p[index];
}







ParticleObject * LudumGame::GetPlayerParticle()
{
	return GetObjectParticle(player_allocations.get(), 0);
}

ParticleObject const * LudumGame::GetPlayerParticle() const
{
	return GetObjectParticle(player_allocations.get(), 0);
}




chaos::box2 LudumGame::GetObjectBox(chaos::ParticleRangeAllocation * allocation, size_t index) const
{
	ParticleObject const * object = GetObjectParticle(allocation, index);
	if (object == nullptr)
		return chaos::box2();
	return chaos::ParticleCornersToBox(object->corners);
}

chaos::box2 LudumGame::GetPlayerBox() const
{
	return GetObjectBox(player_allocations.get(), 0);
}


void LudumGame::SetObjectBox(chaos::ParticleRangeAllocation * allocation, size_t index, chaos::box2 const & box) 
{
	ParticleObject * object = GetObjectParticle(allocation, index);
	if (object == nullptr)
		return;
	object->corners = chaos::BoxToParticleCorners(box);
}

void LudumGame::SetPlayerBox(chaos::box2 const & box)
{
	return SetObjectBox(player_allocations.get(), 0, box);
}

glm::vec2 LudumGame::GetObjectPosition(chaos::ParticleRangeAllocation * allocation, size_t index) const
{
	return GetObjectBox(allocation, index).position;
}

glm::vec2 LudumGame::GetPlayerPosition() const
{
	return GetPlayerBox().position;
}

void LudumGame::SetObjectPosition(chaos::ParticleRangeAllocation * allocation, size_t index, glm::vec2 const & position)
{
	ParticleObject * particle = GetObjectParticle(allocation, index);
	if (particle == nullptr)
		return;

	chaos::box2 box = chaos::ParticleCornersToBox(particle->corners);
	box.position = position;
	particle->corners = chaos::BoxToParticleCorners(box);
}

void LudumGame::SetPlayerPosition(float position)
{
	SetObjectPosition(player_allocations.get(), 0, glm::vec2(position, PLAYER_Y));
	RestrictedPlayerToScreen();
}


void LudumGame::RestrictedObjectToScreen(chaos::ParticleRangeAllocation * allocation, size_t index)
{
	ParticleObject * particle = GetObjectParticle(allocation, index);
	if (particle == nullptr)
		return;

	chaos::box2 box   = chaos::ParticleCornersToBox(particle->corners);
	chaos::box2 world = GetWorldBox();
	chaos::RestrictToInside(world, box, false);
	SetObjectPosition(allocation, index, box.position);
}



void LudumGame::RestrictedPlayerToScreen()
{
	RestrictedObjectToScreen(player_allocations.get(), 0);
}

void LudumGame::SetPlayerLength(float length)
{

	length = chaos::MathTools::Clamp(length, player_min_length, player_max_length);

	chaos::box2 box = GetPlayerBox();
	box.half_size = glm::vec2(length * 0.5f, PLAYER_HEIGHT * 0.5f);
	SetPlayerBox(box);

	player_length = length;
	RestrictedPlayerToScreen();
}



void LudumGame::CreateAllGameObjects(int level)
{
	if (player_allocations == nullptr)
	{
		player_allocations = CreatePlayer();
		SetPlayerLength(player_length);
		SetPlayerPosition(0.0f);
	}

	if (balls_allocations == nullptr)
	{
		balls_allocations = CreateBall();	
	}


}

void LudumGame::OnLifeChallenge(class LudumSequenceChallenge_LifeBallCallbacks * challenge, bool success)
{

}

void LudumGame::OnBallSpeedChallenge(class LudumSequenceChallenge_SpeedDownBallCallbacks * challenge, bool success)
{
	if (success)
		ball_speed = ball_speed - ball_speed_increment;
	else
		ball_speed = ball_speed + ball_speed_increment;

	ball_speed = chaos::MathTools::Clamp(ball_speed, ball_initial_speed, ball_max_speed);
}

void LudumGame::OnExtraBallChallenge(class LudumSequenceChallenge_ExtraBallCallbacks * challenge, bool success)
{

}

void LudumGame::OnLongBarChallenge(class LudumSequenceChallenge_LongBarBallCallbacks * challenge, bool success)
{
	if (success)
		SetPlayerLength(player_length + player_length_increment);
	else
		SetPlayerLength(player_length - player_length_decrement);

}














