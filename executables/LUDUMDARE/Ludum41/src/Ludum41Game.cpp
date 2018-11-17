#include "Ludum41Game.h"
#include "Ludum41Particles.h"
#include "Ludum41HUD.h"

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

LudumGame::LudumGame()
{
	game_name = "AsciiPaouf 2";
}

death::PlayingHUD * LudumGame::DoCreatePlayingHUD()
{
	return new LudumPlayingHUD(this);
}

void LudumGame::IncrementScore(int delta)
{
	if (delta <= 0)
		return;
	current_score += delta * combo_multiplier;
}

void LudumGame::OnEnterMainMenu(bool very_first)
{
	death::Game::OnEnterMainMenu(very_first);

	chaos::MathTools::ResetRandSeed();
	if (very_first)
		glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool LudumGame::OnEnterPause()
{
	death::Game::OnEnterPause();
	if (sequence_challenge != nullptr)
		sequence_challenge->Show(false);

	return true;
}

bool LudumGame::OnLeavePause()
{
	death::Game::OnLeavePause();
	if (sequence_challenge != nullptr)
		sequence_challenge->Show(true);

	return true;
}

bool LudumGame::OnEnterGame()
{
	death::Game::OnEnterGame();	
	CreateAllGameObjects(0);
	return true;
}

bool LudumGame::OnLeaveGame(bool gameover)
{
	death::Game::OnLeaveGame(gameover);	
	return true;
}

bool LudumGame::OnAbordGame()
{
	death::Game::OnAbordGame();
	DestroyGameObjects();
	return true;
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
	if (death::Game::OnKeyEvent(key, action))
		return true;
	return false;
}

bool LudumGame::OnGamepadInput(chaos::MyGLFW::GamepadData & in_gamepad_data)
{
	// press start or go to pause
	if (death::Game::OnGamepadInput(in_gamepad_data))
		return true;
	// maybe this correspond to current challenge
	SendGamepadButtonToChallenge(&in_gamepad_data);

	return false;
}

void LudumGame::DoDisplay(chaos::RenderParams const & render_params, chaos::GPUProgramProvider & uniform_provider)
{
	// clear the color buffers
	glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

	// clear the depth buffers
	float far_plane = 1000.0f;
	glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

	// some states
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// draw particle system
	if (particle_manager != nullptr)
		particle_manager->Display(&uniform_provider);

	// super method
	death::Game::DoDisplay(render_params, uniform_provider);
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
	death::Game::ResetGameVariables();

	target_brick_offset = 0.0f;
	brick_offset = 0.0f;

	current_life  = initial_life;
	player_length = player_initial_length;
	ball_power    = 1.0f;
	ball_speed    = ball_initial_speed;
	ball_time_dilation = 1.0f;
	challenge_timer    = challenge_frequency;
	pending_split_count = 0;
	ball_collision_speed = 0.0f;

	combo_multiplier = 1;
	current_level = 0;
}

void LudumGame::OnGameOver()
{
	death::Game::OnGameOver();
	DestroyGameObjects();
}

void LudumGame::DisplacePlayer(double delta_time)
{
	float value = left_stick_position.x;
	if (abs(right_stick_position.x) > abs(left_stick_position.x))
		value = right_stick_position.x;

	glm::vec2 position = GetPlayerPosition();
	SetPlayerPosition(glm::vec2(position.x + value, PLAYER_Y));
	RestrictPlayerToWorld();
}


size_t LudumGame::CanStartChallengeBallIndex(bool reverse) const
{
	size_t ball_count = GetBallCount();
	if (ball_count > 0)
	{			
		ParticleMovableObject const * balls = GetBallParticles();
		if (balls != nullptr)
		{
			glm::vec2 view_size = GetViewSize();

			for (size_t i = 0; i < ball_count ; ++i)
			{
				if (reverse ^ (balls->velocity.y <= 0.0f)) // going up
					continue;					
				if (reverse ^ (balls->bounding_box.position.y > -view_size.y * 0.5f * 0.75f)) // wait until particle is high enough on screen
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

bool LudumGame::CheckGameOverCondition()
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
			ball_collision_speed = 0.0f;
			ball_power = 1.0f;
			ball_speed = ball_initial_speed;
			SetPlayerLength(player_initial_length);
			balls_allocations = CreateBalls(1, true);	
		}
		return true;
	}
	return false;
}

void LudumGame::OnBallCollide(bool collide_brick)
{
	PlaySound("ball", false, false);

	ball_collision_speed = min(ball_collision_max_speed, ball_collision_speed + ball_collision_speed_increment);

	if (collide_brick)
		IncrementScore(points_per_brick);
}

void LudumGame::TickBrickOffset(double delta_time)
{
	if (target_brick_offset > brick_offset)
	{
		brick_offset = brick_offset + (float)delta_time * brick_offset_speed;
		if (brick_offset > target_brick_offset)
			brick_offset = target_brick_offset;
	}
	else if (target_brick_offset < brick_offset)
	{
		brick_offset = brick_offset - (float)delta_time * brick_offset_speed;
		if (brick_offset < target_brick_offset)
			brick_offset = target_brick_offset;
	}
}

bool LudumGame::TickGameLoop(double delta_time)
{
	if (!death::Game::TickGameLoop(delta_time))
		return false;

	// displace the player
	DisplacePlayer(delta_time);
	// some other calls
	TickBrickOffset(delta_time);
	TickChallenge(delta_time);
	TickBallSplit(delta_time);
	return true;
}

void LudumGame::SendKeyboardButtonToChallenge(unsigned int c)
{
	if (!IsPlaying())
		return;
	if (sequence_challenge != nullptr)
		sequence_challenge->OnKeyboardButtonReceived((char)c);
}

void LudumGame::SendGamepadButtonToChallenge(chaos::MyGLFW::GamepadData * in_gamepad_data)
{
	if (!IsPlaying())
		return;
	if (sequence_challenge != nullptr)
		sequence_challenge->OnGamepadButtonReceived(in_gamepad_data);
}

void LudumGame::OnMouseMove(double x, double y)
{
	left_stick_position.x = mouse_sensitivity * (float)x;
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
}

void LudumGame::DestroyGameObjects()
{
	player_allocations = nullptr;
	balls_allocations = nullptr;

	sequence_challenge = nullptr;
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

chaos::ParticleAllocation * LudumGame::CreateBricks(LudumLevel const * level)
{
	if (level == nullptr)
		return nullptr;

	glm::vec4 const indestructible_color = glm::vec4(1.0f, 0.4f, 0.0f, 1.0f);

	glm::vec4 const colors[] = {
		glm::vec4(0.7f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 0.7f, 0.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 0.7f, 1.0f)
	};

	size_t color_count = sizeof(colors) / sizeof(colors[0]);

	// create the bricks resource
	size_t brick_count = level->GetBrickCount();
	chaos::ParticleAllocation * result = GetGameParticleCreator().CreateParticles("brick", brick_count, death::GameHUDKeys::BRICK_LAYER_ID);
	if (result == nullptr)
		return nullptr;

	chaos::ParticleAccessor<ParticleBrick> particles = result->GetParticleAccessor<ParticleBrick>();
	if (particles.GetCount() == 0)
		return nullptr;

	// compute the brick size
	float BRICK_ASPECT = 16.0f / 9.0f;

	glm::vec2 view_size = GetViewSize();

	glm::vec2 particle_size;
	particle_size.x = view_size.x / (float)brick_per_line;
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
				particles[k].color = indestructible_color;
				particles[k].indestructible = true;
				particles[k].life = 1.0f;
			}
			else 
			{
				particles[k].indestructible = false;

				size_t color_index = min((size_t)b, color_count - 1);
				particles[k].color = colors[color_index];
				particles[k].life = (float)b;
			}

			particles[k].starting_life = particles[k].life;

			// position
			glm::vec2 position;
			position.x = -view_size.x * 0.5f + particle_size.x * (float)j;
			position.y = view_size.y * 0.5f - particle_size.y * (float)i;

			particles[k].bounding_box.position = chaos::Hotpoint::Convert(position, particle_size, chaos::Hotpoint::TOP_LEFT, chaos::Hotpoint::CENTER);
			particles[k].bounding_box.half_size = 0.5f * particle_size;

			++k;
		}
	}

	return result;
}

chaos::ParticleAllocation * LudumGame::CreateBalls(size_t count, bool full_init)
{

	// create the object
	chaos::ParticleAllocation * result = GetGameParticleCreator().CreateParticles("ball", 1, death::GameHUDKeys::BALL_LAYER_ID);
	if (result == nullptr)
		return nullptr;

	// set the color
	chaos::ParticleAccessor<ParticleMovableObject> particles = result->GetParticleAccessor<ParticleMovableObject>();
	if (particles.GetCount() == 0)
		return nullptr;

	for (size_t i = 0 ; i < count ; ++i)
	{	
		particles[i].color         = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		particles[i].bounding_box.position  = glm::vec2(0.0f, 0.0f);
		particles[i].bounding_box.half_size = 0.5f * glm::vec2(ball_size, ball_size);
		
		if (full_init)
		{
			particles[i].delay_before_move = delay_before_ball_move;
			particles[i].velocity = ball_speed * GenerateBallRandomDirection();
		}
	}
	return result;
}


chaos::ParticleAllocation * LudumGame::CreatePlayer()
{
	// create the object
	chaos::ParticleAllocation * result = GetGameParticleCreator().CreateParticles("player", 1, death::GameHUDKeys::GAMEOBJECT_LAYER_ID);
	if (result == nullptr)
		return nullptr;

	// set the color
	chaos::ParticleAccessor<ParticleObject> particles = result->GetParticleAccessor<ParticleObject>();
	if (particles.GetCount() == 0)
		return nullptr;

	particles->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	particles->bounding_box.position  = glm::vec2(0.0f, 0.0f);
	particles->bounding_box.half_size = glm::vec2(0.0f, 0.0f);
	
	return result;
}

ParticleMovableObject * LudumGame::GetBallParticles()
{
	if (balls_allocations == nullptr)
		return nullptr;	
	chaos::ParticleAccessor<ParticleMovableObject> particles = balls_allocations->GetParticleAccessor<ParticleMovableObject>();
	if (particles.GetCount() == 0)
		return nullptr;
	return &particles[0];
}

ParticleMovableObject const * LudumGame::GetBallParticles() const
{
	if (balls_allocations == nullptr)
		return nullptr;
	chaos::ParticleConstAccessor<ParticleMovableObject> p = balls_allocations->GetParticleAccessor<ParticleMovableObject>();
	if (p.GetCount() == 0)
		return nullptr;

	return &p[0];
}

size_t LudumGame::GetBallCount() const
{
	if (balls_allocations == nullptr)
		return 0;	
	return balls_allocations->GetParticleCount();
}

void LudumGame::RestrictObjectToWorld(chaos::ParticleAllocation * allocation, size_t index)
{
	ParticleObject * particle = GetObjectParticle(allocation, index);
	if (particle == nullptr)
		return;

	chaos::box2 box = particle->bounding_box;
	chaos::box2 world = GetWorldBox();
	chaos::RestrictToInside(world, box, false);
	SetObjectPosition(allocation, index, box.position);
}


void LudumGame::RestrictPlayerToWorld()
{
	RestrictObjectToWorld(player_allocations.get(), 0);
}

void LudumGame::SetPlayerLength(float length)
{

	length = chaos::MathTools::Clamp(length, player_min_length, player_max_length);

	chaos::box2 box = GetPlayerBox();
	box.half_size = glm::vec2(length * 0.5f, PLAYER_HEIGHT * 0.5f);
	SetPlayerBox(box);

	player_length = length;
	RestrictPlayerToWorld();
}



void LudumGame::CreateAllGameObjects(int level)
{
	if (player_allocations == nullptr)
	{
		player_allocations = CreatePlayer();
		SetPlayerLength(player_length);
		SetPlayerPosition(glm::vec2(0.0f, PLAYER_Y));
		RestrictPlayerToWorld();
	}

	if (balls_allocations == nullptr)
		balls_allocations = CreateBalls(1, true);	
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

	chaos::ParticleAccessor<ParticleBrick> particles = bricks_allocations->GetParticleAccessor<ParticleBrick>();
	if (particles.GetCount() == 0)
		return nullptr;

	return &particles[0];
}

ParticleBrick const * LudumGame::GetBricks() const 
{
	if (bricks_allocations == nullptr)
		return nullptr;

//	size_t brick_count = bricks_allocations->GetParticleCount();
//	if (brick_count == 0)
//		return nullptr;

	chaos::ParticleConstAccessor<ParticleBrick> p = bricks_allocations->GetParticleConstAccessor<ParticleBrick>();
	if (p.GetCount() == 0)
		return nullptr;
	return &p[0];
}

bool LudumGame::IsBrickLifeChallengeValid(bool success)
{
	LudumLevel const * level = dynamic_cast<LudumLevel const *>(GetCurrentLevel());
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
			if (p.life > 0.0f && !p.indestructible)
			{
				++destroyed_count;
				p.life -= 1.0f;
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
			if (!p.indestructible && p.life < max_brick_life && p.life > 0.0f)
			{
				p.life += 1.0f;
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

void LudumGame::OnBrickOffsetChallenge(bool success)
{
	if (success)
		target_brick_offset -= brick_offset_increment;
	else
		target_brick_offset += brick_offset_increment;

	target_brick_offset = chaos::MathTools::Clamp(target_brick_offset, 0.0f, max_brick_offset);
}

bool LudumGame::IsBrickOffsetChallengeValid(bool success)
{
	if (success)
		return (target_brick_offset > 0.0f);
	else
		return (target_brick_offset < max_brick_offset);
}

bool LudumGame::IsBallPowerChallengeValid(bool success)
{
	if (success)
		return (ball_power < 3); // can still increase power ?
	else
		return (ball_power > 0.5f); // can still decrease power ?
}

void LudumGame::OnBallPowerChallenge(bool success)
{
	if (success)
		ball_power = (ball_power == 0.5f) ? 1.0f : (ball_power + 1.0f);
	else
		ball_power = (ball_power == 1.0f) ? 0.5f : (ball_power - 1.0f);
	ball_power = chaos::MathTools::Clamp(ball_power, 0.5f, 3.0f);
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

chaos::SM::StateMachine * LudumGame::DoCreateGameStateMachine()
{
	return new LudumStateMachine(this);
}

bool LudumGame::DeclareParticleClasses()
{
	chaos::ClassTools::DeclareClass<ParticleObject>("ParticleObject");
	chaos::ClassTools::DeclareClass<ParticleBackground>("ParticleBackground");
	chaos::ClassTools::DeclareClass<ParticleBrick, ParticleObject>("ParticleBrick");
	chaos::ClassTools::DeclareClass<ParticleMovableObject, ParticleObject>("ParticleMovableObject");
	chaos::ClassTools::DeclareClass<ParticleChallenge, ParticleObject>("ParticleChallenge");

	return true;
}

bool LudumGame::InitializeGameValues(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!death::Game::InitializeGameValues(config, config_path))
		return false;

	DEATHGAME_JSON_ATTRIBUTE(initial_life);
	DEATHGAME_JSON_ATTRIBUTE(max_life);
	DEATHGAME_JSON_ATTRIBUTE(max_ball_count);
	DEATHGAME_JSON_ATTRIBUTE(ball_size);
	DEATHGAME_JSON_ATTRIBUTE(player_max_length);
	DEATHGAME_JSON_ATTRIBUTE(player_min_length);
	DEATHGAME_JSON_ATTRIBUTE(player_initial_length);
	DEATHGAME_JSON_ATTRIBUTE(player_length_increment);
	DEATHGAME_JSON_ATTRIBUTE(player_length_decrement);
	DEATHGAME_JSON_ATTRIBUTE(ball_max_speed);
	DEATHGAME_JSON_ATTRIBUTE(ball_initial_speed);
	DEATHGAME_JSON_ATTRIBUTE(ball_speed_increment);
	DEATHGAME_JSON_ATTRIBUTE(challenge_time_dilation);
	DEATHGAME_JSON_ATTRIBUTE(challenge_frequency);
	DEATHGAME_JSON_ATTRIBUTE(challenge_duration);
	DEATHGAME_JSON_ATTRIBUTE(delay_before_ball_move);
	DEATHGAME_JSON_ATTRIBUTE(min_brick_life);
	DEATHGAME_JSON_ATTRIBUTE(max_brick_life);
	DEATHGAME_JSON_ATTRIBUTE(brick_per_line);
	DEATHGAME_JSON_ATTRIBUTE(brick_line_count);
	DEATHGAME_JSON_ATTRIBUTE(ball_collision_speed_increment);
	DEATHGAME_JSON_ATTRIBUTE(ball_collision_max_speed);
	DEATHGAME_JSON_ATTRIBUTE(points_per_brick);
	DEATHGAME_JSON_ATTRIBUTE(points_per_challenge);
	DEATHGAME_JSON_ATTRIBUTE(split_angle);
	DEATHGAME_JSON_ATTRIBUTE(ball_angle_limit);
	DEATHGAME_JSON_ATTRIBUTE(max_brick_offset);
	DEATHGAME_JSON_ATTRIBUTE(brick_offset_speed);
	DEATHGAME_JSON_ATTRIBUTE(brick_offset_increment);

	return true;
}

death::GameLevel * LudumGame::DoLoadLevel(int level_index, chaos::FilePathParam const & path)
{
	std::vector<std::string> level_content = chaos::FileTools::ReadFileLines(path);
	if (level_content.size() == 0)
		return nullptr;

	LudumLevel * result = new LudumLevel;
	if (result == nullptr)
		return nullptr;

	std::vector<int> line;
	for (size_t i = 0; i < level_content.size(); ++i)
	{
		line.resize(0);

		char const * l = level_content[i].c_str();
		for (size_t j = 0; l[j] != 0; ++j)
		{
			char c = l[j];

			// indestructible
			if (c == 'B')
			{
				line.push_back(LudumLevel::INDESTRUCTIBLE);
				result->indestructible_brick_count++;
				continue;
			}
			// separator
			if (!std::isdigit(c))
			{
				line.push_back(LudumLevel::NONE);
				continue;
			}
			// life
			int brick_type = (int)(c - '0');
			line.push_back(brick_type);
		}
		result->bricks.push_back(std::move(line));
	}

	return result;
}

bool LudumGame::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	if (!death::Game::InitializeFromConfiguration(config, config_path))
		return false;
	// the dictionnary
	if (!InitializeDictionnary(config, config_path))
		return false;
	// build the rewards/punishment values
	if (!InitializeRewardsAndPunishments())
		return false;
	return true;
}

bool LudumGame::IsWordValid(std::string const & word) const
{
	char const * valid_letters = "abcdefghijklmnopqrstuvwxyz";

	size_t len = word.length();

	for (size_t i = 0; i < len; ++i)
		if (strchr(valid_letters, word[i]) == nullptr)
			return false;
	return true;
}

void LudumGame::ReplaceSpecialLetters(std::string & word) const
{
	size_t len = word.length();

	for (size_t i = 0; i < len; ++i)
	{
		char & c = word[i];
		if (c == 'é' || c == 'è' || c == 'ê')
			c = 'e';
		else if (c == 'à' || c == 'â')
			c = 'a';
		else if (c == 'î')
			c = 'i';
		else if (c == 'ù')
			c = 'u';
	}
}

bool LudumGame::InitializeDictionnary(nlohmann::json const & config, boost::filesystem::path const & config_path)
{
	// detect whether the language is french

#if 1
	LCID lcid = GetUserDefaultLCID();
	LANGID language = LANGIDFROMLCID(lcid);
#else
	LANGID language = GetUserDefaultUILanguage();
#endif
	WORD   primary_language = PRIMARYLANGID(language);
	WORD   subg_language = SUBLANGID(language);

	bool french_dictionnary = (primary_language == LANG_FRENCH);

	// read the min and max words
	chaos::JSONTools::GetAttribute(config, "min_word_size", min_word_size, 1);
	chaos::JSONTools::GetAttribute(config, "max_word_size", max_word_size, 10);
	if (min_word_size > max_word_size)
		std::swap(min_word_size, max_word_size);

	// read the dictionnary file
	std::string dictionnary_path;
#if _DEBUG
	if (!chaos::JSONTools::GetAttribute(config, (french_dictionnary) ? "small_french_dictionnary" : "small_english_dictionnary", dictionnary_path))
		return false;
#else
	if (!chaos::JSONTools::GetAttribute(config, (french_dictionnary) ? "french_dictionnary" : "english_dictionnary", dictionnary_path))
		return false;
#endif

	std::vector<std::string> words = chaos::FileTools::ReadFileLines(dictionnary_path);
	if (words.size() < 100)
		return false;

	// transform the vector into a map (by word length)

	for (std::string & word : words)
	{
		// test word length
		size_t len = word.length();
		if (len < (size_t)min_word_size)
			continue;
		if (len >(size_t)max_word_size)
			continue;
		// replace accents (not all special characters handled)
		ReplaceSpecialLetters(word);
		// keep words with only simple characters 
		if (!IsWordValid(word))
			continue;
		// add the word into the map

		auto it = dictionnary.find(len);
		if (it == dictionnary.end())
		{
			auto x = dictionnary.insert(std::make_pair(len, std::vector<std::string>()));
			it = dictionnary.find(len);
		}
		it->second.push_back(std::move(word));
	}

	if (dictionnary.size() == 0)
		return false;

	return true;
}

bool LudumGame::InitializeGamepadButtonInfo()
{
	if (!death::Game::InitializeGamepadButtonInfo())
		return false;

	gamepad_buttons.push_back(chaos::MyGLFW::XBOX_BUTTON_A);
	gamepad_buttons.push_back(chaos::MyGLFW::XBOX_BUTTON_B);
	gamepad_buttons.push_back(chaos::MyGLFW::XBOX_BUTTON_X);
	gamepad_buttons.push_back(chaos::MyGLFW::XBOX_BUTTON_Y);

	return true;
}

bool LudumGame::CreateBackgroundImage()
{
	background_allocations = GetGameParticleCreator().SpawnParticles(death::GameHUDKeys::BACKGROUND_LAYER_ID, 1);
	if (background_allocations == nullptr)
		return false;

	chaos::ParticleAccessor<ParticleBackground> particles = background_allocations->GetParticleAccessor<ParticleBackground>();
	if (particles.GetCount() == 0)
		return false;

	particles->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	return true;
}

bool LudumGame::InitializeRewardsAndPunishments()
{
#if 1
	rewards.push_back(new LudumChallengeRewardPunishment_BrickOffset);
	rewards.push_back(new LudumChallengeRewardPunishment_BallPower);
	rewards.push_back(new LudumChallengeRewardPunishment_BarSize);
	rewards.push_back(new LudumChallengeRewardPunishment_SpeedDownBall);
	rewards.push_back(new LudumChallengeRewardPunishment_SplitBall);
	rewards.push_back(new LudumChallengeRewardPunishment_BrickLife);
	rewards.push_back(new LudumChallengeRewardPunishment_ExtraLife);
#endif

#if 1
	punishments.push_back(new LudumChallengeRewardPunishment_BrickOffset);
	punishments.push_back(new LudumChallengeRewardPunishment_BallPower);
	punishments.push_back(new LudumChallengeRewardPunishment_BarSize);
	punishments.push_back(new LudumChallengeRewardPunishment_SpeedDownBall);
	punishments.push_back(new LudumChallengeRewardPunishment_BrickLife);
	punishments.push_back(new LudumChallengeRewardPunishment_ExtraLife);
#endif
	return true;
}

std::string LudumGame::GenerateGamepadChallengeString(std::vector<int> const & gamepad_challenge)
{
	std::string result;

	for (size_t i = 0; i < gamepad_challenge.size(); ++i)
	{
		int button_index = gamepad_challenge[i];

		auto const it = gamepad_button_map.find(button_index);
		if (it == gamepad_button_map.end())
			continue;
		result = result + "[" + it->second.second + "]";
	}
	return result;
}

chaos::ParticleAllocation * LudumGame::CreateChallengeParticles(LudumChallenge * challenge)
{
	int  input_mode = chaos::MyGLFW::SingleWindowApplication::GetApplicationInputMode();
	bool keyboard = chaos::InputMode::IsPCMode(input_mode);

	chaos::ParticleLayer * layer = particle_manager->FindLayer(death::GameHUDKeys::CHALLENGE_LAYER_ID);
	if (layer == nullptr)
		return nullptr;

	chaos::ParticleTextGenerator::GeneratorResult result;
	chaos::ParticleTextGenerator::GeneratorParams params;

	params.line_height = CHALLENGE_SIZE;
	params.hotpoint_type = chaos::Hotpoint::TOP | chaos::Hotpoint::HMIDDLE;
	params.position.x = 0.0f;
	params.position.y = CHALLENGE_PLACEMENT_Y;

	if (keyboard)
	{
		particle_text_generator->Generate(challenge->keyboard_challenge.c_str(), result, params);
	}
	else
	{
		std::string gamepad_string = GenerateGamepadChallengeString(challenge->gamepad_challenge);
		particle_text_generator->Generate(gamepad_string.c_str(), result, params);
	}

	// create the text
	chaos::ParticleAllocation * allocation = chaos::ParticleTextGenerator::CreateTextAllocation(layer, result);
	// and initialize additionnal data
	if (allocation != nullptr)
	{
		chaos::ParticleAccessor<ParticleChallenge> particles = allocation->GetParticleAccessor<ParticleChallenge>();
		for (size_t i = 0; i < particles.GetCount(); ++i)
		{
			ParticleChallenge & p = particles[i];
			p.challenge = challenge;
			p.index = i;
		}
	}

	return allocation;
}

//chaos::ParticleLayer * LudumGame::AddParticleLayer()


bool LudumGame::InitializeParticleManager()
{
	if (!death::Game::InitializeParticleManager())
		return false;

	int render_order = 0;

	// create layers
	particle_manager->AddLayer<ParticleBackgroundTrait>(++render_order, death::GameHUDKeys::BACKGROUND_LAYER_ID, "background");
	particle_manager->AddLayer<ParticleObjectTrait>(++render_order, death::GameHUDKeys::BACKGROUND_GAMEOBJECT_LAYER_ID, "gameobject");
	particle_manager->AddLayer<ParticleObjectTrait>(++render_order, death::GameHUDKeys::GAMEOBJECT_LAYER_ID, "gameobject");

	ParticleMovableObjectTrait movable_trait;
	movable_trait.game = this;
	particle_manager->AddLayer<ParticleMovableObjectTrait>(++render_order, death::GameHUDKeys::BALL_LAYER_ID, "gameobject", movable_trait);

	ParticleBrickTrait brick_trait;
	brick_trait.game = this;
	particle_manager->AddLayer<ParticleBrickTrait>(++render_order, death::GameHUDKeys::BRICK_LAYER_ID, "gameobject", brick_trait);

	particle_manager->AddLayer<ParticleChallengeTrait>(++render_order, death::GameHUDKeys::CHALLENGE_LAYER_ID, "challenge");
	particle_manager->AddLayer<ParticleObjectTrait>(++render_order, death::GameHUDKeys::TEXT_LAYER_ID, "text");

	return true;
}


int LudumGame::GetRandomButtonID() const
{
	size_t key_index = (size_t)(rand() % gamepad_buttons.size());
	if (key_index >= gamepad_buttons.size())
		key_index = gamepad_buttons.size() - 1;
	return gamepad_buttons[key_index];
}


LudumChallenge * LudumGame::CreateSequenceChallenge(size_t len)
{
	if (len == 0)
		len = min_word_size + rand() % (max_word_size - min_word_size);

	auto it = dictionnary.find(len);

	// no word of this size (search a word with the lengh the more near the request) 
	if (it == dictionnary.end())
	{
		auto better_it = dictionnary.begin();

		int min_distance = std::numeric_limits<int>::max();
		for (it = dictionnary.begin(); it != dictionnary.end(); ++it)
		{
			int distance = abs((int)len - (int)it->first);
			if (distance < min_distance)
			{
				min_distance = distance;
				better_it = it;
			}
		}
		if (it == dictionnary.end()) // should never happen
			return nullptr;
	}

	// get the list of words with given length
	std::vector<std::string> const & words = it->second;
	if (words.size() == 0)
		return nullptr;

	// search a word in the list
	size_t index = (size_t)(rand() % words.size());
	if (index >= words.size())
		index = words.size() - 1; // should never happen

	std::string keyboard_challenge = words[index];
	len = keyboard_challenge.size();

	// compose a gamepad combinaison of the same length
	std::vector<int> gamepad_challenge;
	for (size_t i = 0; i < len; ++i)
		gamepad_challenge.push_back(GetRandomButtonID());

	// create the challenge
	LudumChallenge * result = new LudumChallenge;
	if (result != nullptr)
	{
		result->gamepad_challenge = std::move(gamepad_challenge);
		result->keyboard_challenge = std::move(keyboard_challenge);
		result->game = this;
		result->particle_range = CreateChallengeParticles(result);
		result->Show(IsPlaying());

		result->SetTimeout(challenge_duration);

		ball_time_dilation = challenge_time_dilation;
	}
	return result;
}

void LudumGame::OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance, death::GameLevelInstance * old_level_instance)
{
	death::Game::OnLevelChanged(new_level, old_level, new_level_instance, old_level_instance);

	bricks_allocations = nullptr;
	target_brick_offset = 0.0f;
	brick_offset = 0.0f;

	if (new_level != nullptr)
		bricks_allocations = CreateBricks(dynamic_cast<LudumLevel const*>(new_level));
}

