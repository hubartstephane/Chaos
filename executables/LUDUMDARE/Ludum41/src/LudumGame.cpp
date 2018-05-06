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


size_t LudumLevel::GetBrickCount() const
{
	size_t result = 0;
	for (size_t i = 0; i < bricks.size(); ++i)
	{
		std::vector<int> const & line = bricks[i];
		for (int b : line)
			if (b != LudumLevel::NONE)
				++result;
	}
	return result;
}


LudumGame::~LudumGame()
{		
	SerializeBestScore(true);
}

void LudumGame::SerializeBestScore(bool save)
{
	// get application
	chaos::Application * application = chaos::Application::GetInstance();
	if (application == nullptr)
		return;
	// get user temp directory
	boost::filesystem::path filepath = application->GetUserLocalTempPath() / "best_score.txt";

	// save the score
	if (save)
	{
		std::ofstream file(filepath.string().c_str());
		if (!file)
			return;
		file << best_score;
	}
	// load the score
	else
	{
		std::ifstream file(filepath.string().c_str());
		if (!file)
			return;
		file >> best_score;
	}
}


void LudumGame::IncrementScore(int delta)
{
	if (delta <= 0)
		return;
	current_score += delta * combo_multiplier;
	should_update_score = true;
}

bool LudumGame::IsPlaying() const
{
	if (game_automata == nullptr)
		return false;
	if (game_automata->GetCurrentState() == nullptr)
		return false;
	if (game_automata->GetCurrentState()->GetStateID() != LudumAutomata::STATE_PLAYING)
		return false;
	return true;
}

void LudumGame::CreateGameTitle()
{
	CreateTitle("AsciiPaouf 2", false);

	if (best_score > 0)
	{
		chaos::ParticleTextGenerator::GeneratorParams params;
		params.line_height = 50;
		params.hotpoint_type = chaos::Hotpoint::CENTER;
		params.position.x = 0.0f;
		params.position.y = -130.0f;

		params.character_set_name = "normal";

		std::string str = chaos::StringTools::Printf("Best score : %d", best_score);
		best_score_allocations = CreateTextParticles(str.c_str(), params);
	}
}

chaos::ParticleRangeAllocation * LudumGame::CreateScoringParticles(bool & update_flag, char const * format, int value, float Y)
{
	// test flag
	if (!update_flag)
		return nullptr;
	update_flag = false;

	// get world size
	chaos::box2 world = GetWorldBox();

	std::pair<glm::vec2, glm::vec2> corners = world.GetCorners();

	// set the values
	chaos::ParticleTextGenerator::GeneratorParams params;
	params.line_height = 30;
	params.hotpoint_type = chaos::Hotpoint::TOP_LEFT;
	params.position.x = corners.first.x + 20.0f;
	params.position.y = corners.second.y - Y;
	params.character_set_name = "normal";

	// format text and create particles
	std::string str = chaos::StringTools::Printf(format, value);
	return CreateTextParticles(str.c_str(), params);	
}

void LudumGame::UpdateLifeParticles()
{
	// get the number of particles already existing
	size_t life_particles = 0;
	if (life_allocations != nullptr)
		life_particles = life_allocations->GetParticleCount();

	// no changes ?
	if (life_particles == current_life)
		return;

	// some life lost (destroy particles)
	if ((size_t)current_life < life_particles)
	{
		life_allocations->MarkParticlesToDestroy(current_life, life_particles - current_life);
		return;
	}

	// some life gained
	assert(life_particles < (size_t)current_life);
	
	//if (life_allocations == nullptr)
		
	life_allocations = CreateGameObjects("life", current_life, LIFE_LAYER_ID);
	if (life_allocations == nullptr)
		return;

	// set the color
	ParticleObject * particle = (ParticleObject *)life_allocations->GetParticleBuffer();
	if (particle == nullptr)
		return;

	glm::vec2 world_size = GetWorldSize();

	glm::vec2 particle_size;
	particle_size.x = 35.0f;
	particle_size.y = 20.0f;

	for (size_t i = 0; i < (size_t)current_life; ++i)
	{
		glm::vec2 position;
		position.x = -world_size.x * 0.5f + 20.0f + (particle_size.x + 5.0f) * (float)i;
		position.y = -world_size.y * 0.5f + 15.0f;

		particle[i].box.position = chaos::Hotpoint::Convert(position, particle_size, chaos::Hotpoint::BOTTOM_LEFT, chaos::Hotpoint::CENTER);
		particle[i].box.half_size = 0.5f * particle_size;

		particle[i].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

void LudumGame::UpdateComboParticles()
{
	// destroy combo multiplier if non necessary
	if (combo_multiplier <= 1)
	{
		combo_allocations = nullptr;
		return;
	}
	// test whether the combo has changed
	chaos::ParticleRangeAllocation * allocation = CreateScoringParticles(should_update_combo, "Combo : %d x", combo_multiplier, 60.0f);
	if (allocation != nullptr)
		combo_allocations = allocation;
}

void LudumGame::UpdateScoreParticles()
{
	chaos::ParticleRangeAllocation * allocation = CreateScoringParticles(should_update_score, "Score : %d", current_score, 20.0f);
	if (allocation != nullptr)
		score_allocations = allocation;
}

void LudumGame::CreateTitle(char const * title, bool normal)
{
	chaos::ParticleTextGenerator::GeneratorParams params;
	params.line_height = TITLE_SIZE;
	params.hotpoint_type = chaos::Hotpoint::CENTER;
	params.position.y = TITLE_PLACEMENT_Y;

	params.character_set_name = (normal) ? "normal" : "title";

	text_allocations = CreateTextParticles(title, params);
}

void LudumGame::DestroyTitle()
{
	text_allocations = nullptr;
	best_score_allocations = nullptr;
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
	CreateTitle("Pause", true);

	if (sequence_challenge != nullptr)
		sequence_challenge->Show(false);

	return true;
}

bool LudumGame::OnLeavePause()
{
	StartGameMusic(false);
	DestroyTitle();

	if (sequence_challenge != nullptr)
		sequence_challenge->Show(true);

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


void LudumGame::Tick(double delta_time)
{
	// catch all stick inputs
	gamepad_manager->Tick((float)delta_time);
	// handle keyboard inputs
	HandleKeyboardInputs();
	// update the game automata
	game_automata->Tick(delta_time); 
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

bool LudumGame::OnCharEvent(unsigned int c)
{
	// CHALLENGE
	if (c >= 'a' && c <= 'z')
		SendKeyboardButtonToChallenge((char)c);
	else if (c >= 'A' && c <= 'Z')
		SendKeyboardButtonToChallenge((char)(c - 'A' + 'a'));
	return true;
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
#if 0
	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
		if (RequireGameOver())
			return true;
#endif

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
	{
		if (chaos::InputMode::IsPlatformChanged(new_mode, old_mode))
		{
			sequence_challenge->particle_range = CreateChallengeParticles(sequence_challenge.get());
			sequence_challenge->Show(IsPlaying());
		}
	}
}

void LudumGame::ResetGameVariables()
{
	current_life  = initial_life;
	player_length = player_initial_length;
	ball_speed    = ball_initial_speed;
	ball_time_dilation = 1.0f;
	challenge_timer    = challenge_frequency;
	pending_split_count = 0;
	ball_collision_speed = 0.0f;
	heart_warning = 1.0f;

	current_score = 0;
	combo_multiplier = 1;
	current_level = 0;

	should_update_score = true;
	should_update_combo = true;
}

void LudumGame::OnGameOver()
{
	if (best_score < current_score)
	{
		best_score = current_score;
		SerializeBestScore(true);
	}
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


size_t LudumGame::CanStartChallengeBallIndex(bool reverse) const
{
	size_t ball_count = GetBallCount();
	if (ball_count > 0)
	{			
		ParticleMovableObject const * balls = GetBallParticles();
		if (balls != nullptr)
		{
			glm::vec2 world_size = GetWorldSize();

			for (size_t i = 0; i < ball_count ; ++i)
			{
				if (reverse ^ (balls->velocity.y <= 0.0f)) // going up
					continue;					
				if (reverse ^ (balls->box.position.y > -world_size.y * 0.5f * 0.75f)) // wait until particle is high enough on screen
					return i;
			}
		}			
	}
	return std::numeric_limits<size_t>::max();
}


void LudumGame::TickChallenge(double delta_time)
{
	if (sequence_challenge != nullptr)
	{
		sequence_challenge->Tick(delta_time);
	}
	else
	{
		// start a challenge (only if one ball is going upward)
		challenge_timer = max(0.0f, challenge_timer - (float)delta_time);
		if(challenge_timer <= 0.0f)
			if (CanStartChallengeBallIndex() != std::numeric_limits<size_t>::max())
				sequence_challenge = CreateSequenceChallenge(0);	
	}
}

static void RotateVelocity(glm::vec2 & src, float angle)
{
	glm::mat4x4 rot = glm::rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec4 v(src.x, src.y, 0.0f, 1.0f);
	v = rot * v;
	src.x = v.x;
	src.y = v.y;
}


void LudumGame::TickBallSplit(double delta_time)
{
	if (pending_split_count <= 0)
		return;

	size_t ball_count = GetBallCount();
	if (ball_count > (size_t)max_ball_count || ball_count == 0)
		return;

	size_t ball_candidate = CanStartChallengeBallIndex();
	if (ball_candidate == std::numeric_limits<size_t>::max())
		return;

	balls_allocations->Resize(ball_count + 1);

	ParticleMovableObject * balls = GetBallParticles();

	ParticleMovableObject * parent_ball = &balls[ball_candidate];
	ParticleMovableObject * new_ball = &balls[ball_count];
	*new_ball = *parent_ball;

	RotateVelocity(parent_ball->velocity, split_angle);
	RotateVelocity(new_ball->velocity, -split_angle);

	pending_split_count = 0;
}

void LudumGame::ChangeLife(int delta_life)
{
	if (delta_life == 0)
		return;
	current_life = chaos::MathTools::Clamp(current_life + delta_life, 0, max_life);
}

bool LudumGame::TickGameOverDetection(double delta_time)
{
	size_t ball_count = GetBallCount();
	if (ball_count == 0)
	{
		ChangeLife(-1);
		if (current_life <= 0)
			RequireGameOver();
		else
		{
			PlaySound("balllost", false, false);
			combo_multiplier = 1;
			should_update_combo = true;
			ball_collision_speed = 0.0f;
			balls_allocations = CreateBalls(1, true);	
		}
		return false;
	}
	return true;
}

void LudumGame::OnBallCollide(bool collide_brick)
{
	PlaySound("ball", false, false);

	ball_collision_speed = min(ball_collision_max_speed, ball_collision_speed + ball_collision_speed_increment);

	if (collide_brick)
		IncrementScore(points_per_brick);
}


LudumLevel * LudumGame::GetCurrentLevel()
{
	return GetLevel(current_level);
}

LudumLevel const * LudumGame::GetCurrentLevel() const
{
	return GetLevel(current_level);
}

LudumLevel * LudumGame::GetLevel(int level_number)
{
	if (levels.size() == 0)
		return nullptr;
	return levels[level_number % (int)levels.size()].get();
}

LudumLevel const * LudumGame::GetLevel(int level_number) const
{
	if (levels.size() == 0)
		return nullptr;
	return levels[level_number % (int)levels.size()].get();
}

void LudumGame::TickLevelCompleted(double delta_time)
{
	LudumLevel const * level = GetCurrentLevel();
	if (level == nullptr)
		return;

	size_t brick_count = GetBrickCount();
	if (brick_count == level->indestructible_brick_count) // no more destructible
	{		
		if (CanStartChallengeBallIndex(true) != std::numeric_limits<size_t>::max())
		{
			current_level = (current_level + 1) % (int)levels.size();
			bricks_allocations = CreateBricks(current_level);
		}
	}
}

void LudumGame::TickHeartWarning(double delta_time)
{
	if (current_life == 1)
	{
		heart_warning -= heart_beat_speed * (float)delta_time;
		if (heart_warning <= 0.0f)
		{
			PlaySound("heartbeat", false, false);

			float fractionnal_part, integer_part;
			fractionnal_part = modf(heart_warning, &integer_part);

			heart_warning = (1.0f + fractionnal_part);
		}
	}
	else
		heart_warning = 1.0f;
}



void LudumGame::TickGameLoop(double delta_time)
{
	DisplacePlayer(delta_time);

	if (TickGameOverDetection(delta_time))
	{
		// create the score text
		UpdateScoreParticles();
		// create the combo text
		UpdateComboParticles();
		// create the life 
		UpdateLifeParticles();
		// some other calls
		TickLevelCompleted(delta_time);
		TickChallenge(delta_time);
		TickBallSplit(delta_time);
		TickHeartWarning(delta_time);
	}
}

void LudumGame::SendKeyboardButtonToChallenge(unsigned int c)
{
	if (!IsPlaying())
		return;
	if (sequence_challenge != nullptr)
		sequence_challenge->OnKeyboardButtonReceived((char)c);
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
#if 0
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

void LudumGame::OnChallengeCompleted(LudumChallenge * challenge, bool success, size_t challenge_size)
{
	// rewards/punishment
	auto const & rewards_punishments = (success) ? rewards : punishments;
	
	LudumChallengeRewardPunishment * selected_rp = nullptr;

	size_t count = rewards_punishments.size();
	if (count > 0)
	{
		size_t index = (size_t)rand();
		for (size_t i = 0 ; (i < count) && (selected_rp == nullptr) ; ++i)
		{
			LudumChallengeRewardPunishment * rp = rewards_punishments[(i + index) % count].get();
			if (rp != nullptr && rp->IsRewardPunishmentValid(this, success))
				selected_rp = rp;
		}

		if (selected_rp != nullptr)
		{
			if (selected_rp->CheckRewardPunishmentCondition(this, success))
				selected_rp->OnRewardPunishment(this, success);			
		}		
	}

	// reset some values
	sequence_challenge = nullptr;
	ball_time_dilation = 1.0f;
	challenge_timer    = challenge_frequency;

	// update the score
	if (success)
	{
		IncrementScore(points_per_challenge * challenge_size);
		++combo_multiplier;
	}
	else
	{
		combo_multiplier = 1;
	}
	should_update_combo = true;
}

void LudumGame::DestroyGameObjects()
{
	player_allocations = nullptr;
	bricks_allocations = nullptr;
	life_allocations = nullptr;
	balls_allocations = nullptr;
	score_allocations = nullptr;
	combo_allocations = nullptr;
	best_score_allocations = nullptr;

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

	size_t particle_size = layer->GetParticleSize();

	char * buffer = (char *)allocation->GetParticleBuffer();
	if (buffer == nullptr)
		return nullptr;

	for (size_t i = 0 ; i < count ; ++i)
	{
		ParticleObject * particle = (ParticleObject*)(&buffer[i * particle_size]);		
		particle->texcoords = chaos::ParticleTools::GetParticleTexcoords(*entry, texture_atlas->GetAtlasDimension());
	}
		
	return allocation;
}




glm::vec2 LudumGame::GenerateBallRandomDirection() const
{
	float direction = (rand() % 2) ? 1.0f : -1.0f;

	// direction upward
	float angle = 
		3.14f * 0.5f +									// up
		direction * 3.14f * 0.125f +    // small base angle to the left or the right
		direction * chaos::MathTools::RandFloat(0, 3.14f * 0.125f); // final adjustement

	return glm::vec2(
		chaos::MathTools::Cos(angle),
		chaos::MathTools::Sin(angle));
}

chaos::ParticleRangeAllocation * LudumGame::CreateBricks(int level_number)
{
	glm::vec4 const indestructible_color = glm::vec4(1.0f, 0.4f, 0.0f, 1.0f);

	glm::vec4 const colors[] = {
		glm::vec4(0.7f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 0.7f, 0.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 0.7f, 1.0f)
	};

	size_t color_count = sizeof(colors) / sizeof(colors[0]);

	// get the level
	LudumLevel * level = GetLevel(level_number);
	if (level == nullptr)
		return false;

	// create the bricks resource
	size_t brick_count = level->GetBrickCount();
	chaos::ParticleRangeAllocation * result = CreateGameObjects("brick", brick_count, BRICK_LAYER_ID);
	if (result == nullptr)
		return nullptr;

	ParticleBrick * particle = (ParticleBrick *)result->GetParticleBuffer();
	if (particle == nullptr)
		return nullptr;

	// compute the brick size
	float BRICK_ASPECT = 16.0f / 9.0f;

	glm::vec2 world_size = GetWorldSize();

	glm::vec2 particle_size;
	particle_size.x = world_size.x / (float)brick_per_line;
	particle_size.y = particle_size.x / BRICK_ASPECT;

	// fill the brick
	size_t k = 0;
	for (size_t i = 0; i < level->bricks.size(); ++i)
	{
		std::vector<int> const & line = level->bricks[i];
		for (size_t j = 0; j < line.size(); ++j)
		{
			int b = line[j];
			if (b == LudumLevel::NONE)
				continue;
			if (b < 0 && b != LudumLevel::INDESTRUCTIBLE)
				continue;

			// compute color / indestructible / life
			size_t life = 1;

			if (b == LudumLevel::INDESTRUCTIBLE)
			{
				particle[k].color = indestructible_color;
				particle[k].indestructible = true;
				particle[k].life = 1;
			}
			else 
			{
				particle[k].indestructible = false;

				size_t color_index = min((size_t)b, color_count - 1);
				particle[k].color = colors[color_index];
				particle[k].life = (size_t)b;
			}

			particle[k].starting_life = particle[k].life;

			// position
			glm::vec2 position;
			position.x = -world_size.x * 0.5f + particle_size.x * (float)j;
			position.y =  world_size.y * 0.5f - particle_size.y * (float)i;

			particle[k].box.position = chaos::Hotpoint::Convert(position, particle_size, chaos::Hotpoint::TOP_LEFT, chaos::Hotpoint::CENTER);
			particle[k].box.half_size = 0.5f * particle_size;

			++k;
		}
	}









#if 0



	size_t line_count =  brick_line_count;
	size_t element_per_line = brick_per_line;
	size_t brick_count = line_count * element_per_line;


	






	// set the color
	ParticleBrick * particle = (ParticleBrick *)result->GetParticleBuffer();
	if (particle == nullptr)
		return nullptr;

	glm::vec4 const colors[] = {	
		glm::vec4(0.7f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 0.7f, 0.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 0.7f, 1.0f)
		//glm::vec4(0.7f, 0.7f, 0.0f, 1.0f),
		//glm::vec4(0.0f, 0.7f, 0.7f, 1.0f),
		//glm::vec4(0.7f, 0.0f, 0.7f, 1.0f),
		//glm::vec4(0.7f, 0.7f, 0.7f, 1.0f)	
	};

	size_t color_count = sizeof(colors) / sizeof(colors[0]);

	for (size_t i = 0 ; i < brick_count ; ++i)
	{
		// particle color
		size_t color_index = (rand() % color_count);
		particle[i].color = colors[color_index];

		// particle life
		size_t life = (min_brick_life >= max_brick_life)?
			min_brick_life:
			min_brick_life + (rand() % (max_brick_life - min_brick_life));
						
		particle[i].life = particle[i].starting_life = life;
	
		// position
		int column = i % element_per_line;
		int line   = i / element_per_line;
		
		glm::vec2 position;
		position.x = -world_size.x * 0.5f + particle_size.x * (float)column;
		position.y =  world_size.y * 0.5f - particle_size.y * (float)line;

		particle[i].box.position = chaos::Hotpoint::Convert(position, particle_size, chaos::Hotpoint::TOP_LEFT, chaos::Hotpoint::CENTER);
		particle[i].box.half_size = 0.5f * particle_size;
	}

#endif

	return result;
}

chaos::ParticleRangeAllocation * LudumGame::CreateBalls(size_t count, bool full_init)
{

	// create the object
	chaos::ParticleRangeAllocation * result = CreateGameObjects("ball", 1, BALL_LAYER_ID);
	if (result == nullptr)
		return nullptr;

	// set the color
	ParticleMovableObject * particle = (ParticleMovableObject *)result->GetParticleBuffer();
	if (particle == nullptr)
		return nullptr;

	for (size_t i = 0 ; i < count ; ++i)
	{	
		particle[i].color         = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		particle[i].box.position  = glm::vec2(0.0f, 0.0f);
		particle[i].box.half_size = 0.5f * glm::vec2(ball_size, ball_size);
		
		if (full_init)
		{
			particle[i].delay_before_move = delay_before_ball_move;
			particle[i].velocity = ball_speed * GenerateBallRandomDirection();		
		}
	}
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

	particle->box.position  = glm::vec2(0.0f, 0.0f);
	particle->box.half_size = glm::vec2(0.0f, 0.0f);
	
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
	return object->box;
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
	object->box = box;
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
	particle->box.position = position;
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

	chaos::box2 box = particle->box;
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
		balls_allocations = CreateBalls(1, true);	

	if (bricks_allocations == nullptr)
		bricks_allocations = CreateBricks(current_level);	
}

size_t LudumGame::GetBrickCount() const
{
	if (bricks_allocations == nullptr)
		return 0;
	return bricks_allocations->GetParticleCount();
}

ParticleBrick * LudumGame::GetBricks()
{
	if (bricks_allocations == nullptr)
		return nullptr;

	size_t brick_count = bricks_allocations->GetParticleCount();
	if (brick_count == 0)
		return nullptr;

	return (ParticleBrick*)bricks_allocations->GetParticleBuffer();
}

ParticleBrick const * LudumGame::GetBricks() const 
{
	if (bricks_allocations == nullptr)
		return nullptr;

	size_t brick_count = bricks_allocations->GetParticleCount();
	if (brick_count == 0)
		return nullptr;

	return (ParticleBrick const *)bricks_allocations->GetParticleBuffer();
}

bool LudumGame::IsBrickLifeChallengeValid(bool success)
{
	LudumLevel const * level = GetCurrentLevel();
	if (level == nullptr)
		return false;

	size_t brick_count = GetBrickCount();
	return (brick_count > level->indestructible_brick_count);
}

void LudumGame::OnBrickLifeChallenge(bool success)
{
	size_t brick_count = GetBrickCount();
	if (brick_count == 0)
		return;

	ParticleBrick * bricks = GetBricks();
	if (bricks == nullptr)
		return;

	if (success)
	{
		// decrease all particles life
		int destroyed_count = 0;

		for (size_t i = 0; i < brick_count; ++i)
		{
			ParticleBrick & p = bricks[i];			
			if (p.life > 0 && !p.indestructible)
			{
				++destroyed_count;
				--p.life;
			}			
		}
		IncrementScore(destroyed_count * points_per_brick);
	}
	else
	{
		// increase all particles life
		for (size_t i = 0; i < brick_count; ++i)
		{
			ParticleBrick & p = bricks[i];
			if (!p.indestructible && p.life < max_brick_life && p.life > 0)
			{
				++p.life;
				if (p.life > p.starting_life)
					p.starting_life = p.life;
			}
		}
	}
}









bool LudumGame::IsSplitBallChallengeValid(bool success)
{
	int ball_count = GetBallCount();
	return (ball_count < max_ball_count);
}

void LudumGame::OnSplitBallChallenge(bool success)
{
	if (!success)
		return;
	pending_split_count++;
}

bool LudumGame::IsBallSpeedChallengeValid(bool success)
{
	if (success)
		return (ball_speed > ball_initial_speed); // can still decrease speed ?
	else
		return (ball_speed < ball_max_speed); // can still increase speed ?
}

void LudumGame::OnBallSpeedChallenge(bool success)
{
	if (success)
		ball_speed = ball_speed - ball_speed_increment;
	else
		ball_speed = ball_speed + ball_speed_increment;

	ball_speed = chaos::MathTools::Clamp(ball_speed, ball_initial_speed, ball_max_speed);
}

bool LudumGame::IsExtraBallChallengeValid(bool success)
{
	if (success)
		return current_life < max_life; // do not add life is already max
	else
		return current_life > 1; // do not remove life if last
}

void LudumGame::OnExtraBallChallenge(bool success)
{
	ChangeLife(success ? +1 : -1);
}

bool LudumGame::IsLongBarChallengeValid(bool success)
{
	if (success)
		return (player_length < player_max_length); // can only increment if bar is not at max already
	else
		return (player_length > player_min_length); // can only decrement if bar is not at min already
}

void LudumGame::OnLongBarChallenge(bool success)
{
	if (success)
		SetPlayerLength(player_length + player_length_increment);
	else
		SetPlayerLength(player_length - player_length_decrement);
}














