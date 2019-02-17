#include "Ludum41GameInstance.h"
#include "Ludum41Game.h"
#include "Ludum41Level.h"
#include "Ludum41LevelInstance.h"
#include "Ludum41Player.h"

DEATH_GAMEFRAMEWORK_IMPLEMENT_GAMEINSTANCE(Ludum);

LudumGameInstance::LudumGameInstance(death::Game * in_game) : 
	death::GameInstance(in_game)
{
}

size_t LudumGameInstance::CanStartChallengeBallIndex(bool reverse) const
{
	size_t ball_count = GetBallCount();
	if (ball_count > 0)
	{
		ParticleMovableObject const * balls = GetBallParticles();
		if (balls != nullptr)
		{
			glm::vec2 view_size = GetViewSize();

			for (size_t i = 0; i < ball_count; ++i)
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


void LudumGameInstance::TickChallenge(double delta_time)
{
	if (sequence_challenge != nullptr)
	{
		sequence_challenge->Tick(delta_time);
	}
	else
	{
		// start a challenge (only if one ball is going upward)
		challenge_timer = max(0.0f, challenge_timer - (float)delta_time);
		if (challenge_timer <= 0.0f)
			if (CanStartChallengeBallIndex() != std::numeric_limits<size_t>::max())
				sequence_challenge = CreateSequenceChallenge(0);
	}
}


void LudumGameInstance::OnInputModeChanged(int new_mode, int old_mode)
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

static void RotateVelocity(glm::vec2 & src, float angle)
{
	glm::mat4x4 rot = glm::rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec4 v(src.x, src.y, 0.0f, 1.0f);
	v = rot * v;
	src.x = v.x;
	src.y = v.y;
}


void LudumGameInstance::TickBallSplit(double delta_time)
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

bool LudumGameInstance::OnCharEvent(unsigned int c)
{
	// CHALLENGE
	if (c >= 'a' && c <= 'z')
	{
		SendKeyboardButtonToChallenge((char)c);
		return true;
	}
	else if (c >= 'A' && c <= 'Z')
	{
		SendKeyboardButtonToChallenge((char)(c - 'A' + 'a'));
		return true;
	}
	return death::GameInstance::OnCharEvent(c);
}
bool LudumGameInstance::OnGamepadInput(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
{
	// press start or go to pause
	if (death::GameInstance::OnGamepadInput(in_physical_gamepad))
		return true;
	// maybe this correspond to current challenge
	SendGamepadButtonToChallenge(in_physical_gamepad->GetGamepadData());

	return false;
}

void LudumGameInstance::TickBrickOffset(double delta_time)
{
	LudumGame const * ludum_game = GetLudumGame();
	if (ludum_game == nullptr)
		return;

	if (target_brick_offset > brick_offset)
	{
		brick_offset = brick_offset + (float)delta_time * ludum_game->brick_offset_speed;
		if (brick_offset > target_brick_offset)
			brick_offset = target_brick_offset;
	}
	else if (target_brick_offset < brick_offset)
	{
		brick_offset = brick_offset - (float)delta_time * ludum_game->brick_offset_speed;
		if (brick_offset < target_brick_offset)
			brick_offset = target_brick_offset;
	}
}

void LudumGameInstance::SendKeyboardButtonToChallenge(unsigned int c)
{
	if (!game->IsPlaying())
		return;
	if (sequence_challenge != nullptr)
		sequence_challenge->OnKeyboardButtonReceived((char)c);
}

void LudumGameInstance::SendGamepadButtonToChallenge(chaos::MyGLFW::GamepadData const * in_gamepad_data)
{
	if (!IsPlaying())
		return;
	if (sequence_challenge != nullptr)
		sequence_challenge->OnGamepadButtonReceived(in_gamepad_data);
}

void LudumGameInstance::OnChallengeCompleted(LudumChallenge * challenge, bool success, size_t challenge_size)
{
	// rewards/punishment
	auto const & rewards_punishments = (success) ? rewards : punishments;

	LudumChallengeRewardPunishment * selected_rp = nullptr;

	size_t count = rewards_punishments.size();
	if (count > 0)
	{
		size_t index = (size_t)rand();
		for (size_t i = 0; (i < count) && (selected_rp == nullptr); ++i)
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
	challenge_timer = challenge_frequency;

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

void LudumGameInstance::OnEnterPause()
{
	death::GameInstance::OnEnterPause();
	if (sequence_challenge != nullptr)
		sequence_challenge->Show(false);
}

void LudumGameInstance::OnLeavePause()
{
	death::GameInstance::OnLeavePause();
	if (sequence_challenge != nullptr)
		sequence_challenge->Show(true);
}

void LudumGameInstance::OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance)
{
	death::GameInstance::OnLevelChanged(new_level, old_level, new_level_instance);

	bricks_allocations = nullptr;
	target_brick_offset = 0.0f;
	brick_offset = 0.0f;
}


bool LudumGameInstance::DoTick(double delta_time)
{
	if (!death::GameInstance::DoTick(delta_time))
		return false;

	// some other calls
	TickBrickOffset(delta_time);
	TickChallenge(delta_time);
	TickBallSplit(delta_time);
	return true;
}

death::Player * LudumGameInstance::DoCreatePlayer()
{
	return new LudumPlayer(this);
}

void LudumGameInstance::IncrementScore(int delta)
{
	if (delta <= 0)
		return;
	death::Player * player = GetPlayer(0);
	if (player == nullptr)
		return;
	player->SetScore(delta, true);
}

bool LudumGameInstance::IsBrickLifeChallengeValid(bool success)
{
	LudumLevel const * level = dynamic_cast<LudumLevel const *>(GetLevel());
	if (level == nullptr)
		return false;

	size_t brick_count = GetBrickCount();
	return (brick_count > level->indestructible_brick_count);
}

void LudumGameInstance::OnBrickLifeChallenge(bool success)
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

bool LudumGameInstance::IsSplitBallChallengeValid(bool success)
{
	int ball_count = GetBallCount();
	return (ball_count < max_ball_count);
}

void LudumGameInstance::OnSplitBallChallenge(bool success)
{
	if (!success)
		return;
	pending_split_count++;
}

bool LudumGameInstance::IsBallSpeedChallengeValid(bool success)
{
	if (success)
		return (ball_speed > ball_initial_speed); // can still decrease speed ?
	else
		return (ball_speed < ball_max_speed); // can still increase speed ?
}

void LudumGameInstance::OnBallSpeedChallenge(bool success)
{
	if (success)
		ball_speed = ball_speed - ball_speed_increment;
	else
		ball_speed = ball_speed + ball_speed_increment;

	ball_speed = chaos::MathTools::Clamp(ball_speed, ball_initial_speed, ball_max_speed);
}

void LudumGameInstance::OnBrickOffsetChallenge(bool success)
{
	if (success)
		target_brick_offset -= brick_offset_increment;
	else
		target_brick_offset += brick_offset_increment;

	target_brick_offset = chaos::MathTools::Clamp(target_brick_offset, 0.0f, max_brick_offset);
}

bool LudumGameInstance::IsBrickOffsetChallengeValid(bool success)
{
	if (success)
		return (target_brick_offset > 0.0f);
	else
		return (target_brick_offset < max_brick_offset);
}

bool LudumGameInstance::IsBallPowerChallengeValid(bool success)
{
	if (success)
		return (ball_power < 3); // can still increase power ?
	else
		return (ball_power > 0.5f); // can still decrease power ?
}

void LudumGameInstance::OnBallPowerChallenge(bool success)
{
	if (success)
		ball_power = (ball_power == 0.5f) ? 1.0f : (ball_power + 1.0f);
	else
		ball_power = (ball_power == 1.0f) ? 0.5f : (ball_power - 1.0f);
	ball_power = chaos::MathTools::Clamp(ball_power, 0.5f, 3.0f);
}

bool LudumGameInstance::IsExtraBallChallengeValid(bool success)
{
	death::Player * player = GetPlayer(0);
	if (player == nullptr)
		return false;

	int current_life = player->GetLifeCount();
	if (success)
		return current_life < max_life; // do not add life is already max
	else
		return current_life > 1; // do not remove life if last
}

void LudumGameInstance::OnExtraBallChallenge(bool success)
{
	death::Player * player = GetPlayer(0);
	if (player == nullptr)
		return;
	player->SetLifeCount((success ? +1 : -1), true);
}

bool LudumGameInstance::IsLongBarChallengeValid(bool success)
{
	LudumPlayer * player = GetLudumPlayer(0);
	if (player == nullptr)
		return false;

	float player_length = player->GetPlayerLength();
	if (success)
		return (player_length < player_max_length); // can only increment if bar is not at max already
	else
		return (player_length > player_min_length); // can only decrement if bar is not at min already
}

void LudumGameInstance::OnLongBarChallenge(bool success)
{
	LudumPlayer * player = GetLudumPlayer(0);
	if (player == nullptr)
		return;

	if (success)
		player->SetPlayerLength(player_length_increment, true);
	else
		player->SetPlayerLength(player_length_decrement, true);
}

glm::vec2 LudumGameInstance::GenerateBallRandomDirection() const
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



chaos::ParticleAllocation * LudumGameInstance::CreateBalls(size_t count, bool full_init)
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

ParticleMovableObject * LudumGameInstance::GetBallParticles()
{
	if (balls_allocations == nullptr)
		return nullptr;	
	chaos::ParticleAccessor<ParticleMovableObject> particles = balls_allocations->GetParticleAccessor<ParticleMovableObject>();
	if (particles.GetCount() == 0)
		return nullptr;
	return &particles[0];
}

ParticleMovableObject const * LudumGameInstance::GetBallParticles() const
{
	if (balls_allocations == nullptr)
		return nullptr;
	chaos::ParticleConstAccessor<ParticleMovableObject> p = balls_allocations->GetParticleAccessor<ParticleMovableObject>();
	if (p.GetCount() == 0)
		return nullptr;

	return &p[0];
}

size_t LudumGameInstance::GetBallCount() const
{
	if (balls_allocations == nullptr)
		return 0;	
	return balls_allocations->GetParticleCount();
}


int LudumGameInstance::GetRandomButtonID() const
{
	size_t key_index = (size_t)(rand() % gamepad_buttons.size());
	if (key_index >= gamepad_buttons.size())
		key_index = gamepad_buttons.size() - 1;
	return gamepad_buttons[key_index];
}


LudumChallenge * LudumGameInstance::CreateSequenceChallenge(size_t len)
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


std::string LudumGameInstance::GenerateGamepadChallengeString(std::vector<int> const & gamepad_challenge)
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

chaos::ParticleAllocation * LudumGameInstance::CreateChallengeParticles(LudumChallenge * challenge)
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

void LudumGameInstance::CreateAllGameObjects(int level)
{
	if (balls_allocations == nullptr)
		balls_allocations = CreateBalls(1, true);	
}

void LudumGameInstance::OnBallCollide(bool collide_brick)
{
	game->PlaySound("ball", false, false);

	ball_collision_speed = min(ball_collision_max_speed, ball_collision_speed + ball_collision_speed_increment);

	if (collide_brick)
		IncrementScore(points_per_brick);
}

void LudumGameInstance::DestroyGameObjects()
{
	balls_allocations = nullptr;
	sequence_challenge = nullptr;
}

#if 0
void LudumGame::ResetGameVariables()
{
	death::Game::ResetGameVariables();

	brick_offset = 0.0f;

	ball_power    = 1.0f;
	ball_speed    = ball_initial_speed;
	ball_time_dilation = 1.0f;
	challenge_timer    = challenge_frequency;
	pending_split_count = 0;
	ball_collision_speed = 0.0f;

	combo_multiplier = 1;
}
bool LudumGame::CheckGameOverCondition()
{

#if 0

	size_t ball_count = GetBallCount();
	if (ball_count == 0)
	{
		SetCurrentLife(GetCurrentLife() - 1);
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
#endif
	return false;
}





#endif