#include "Ludum41CustomGameInstance.h"
#include "Ludum41CustomGame.h"
#include "Ludum41CustomLevel.h"
#include "Ludum41CustomLevelInstance.h"
#include "Ludum41CustomPlayer.h"

#include <death/SoundContext.h>

DEATH_GAMEFRAMEWORK_IMPLEMENT_GAMEINSTANCE(Ludum);

LudumGameInstance::LudumGameInstance(death::Game * in_game) : 
	death::GameInstance(in_game)
{
}

size_t LudumGameInstance::CanStartChallengeBallIndex(bool going_down) const
{
	size_t ball_count = GetBallCount();
	if (ball_count > 0)
	{
		ParticleMovableObject const * balls = GetBallParticles();
		if (balls != nullptr)
		{
			glm::vec2 canvas_size = game->GetCanvasSize();

			for (size_t i = 0; i < ball_count; ++i)
			{
				if (going_down ^ (balls->velocity.y <= 0.0f)) // going up
					continue;
				if (going_down ^ (balls->bounding_box.position.y > -canvas_size.y * 0.5f * 0.75f)) // wait until particle is high enough on screen
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
		challenge_timer = std::max(0.0f, challenge_timer - (float)delta_time);
		if (challenge_timer <= 0.0f)
			if (current_background_fillratio > 0.0f) // no challenge when waiting for level change
				if (CanStartChallengeBallIndex(false) != std::numeric_limits<size_t>::max()) // any ball going up
					sequence_challenge = CreateSequenceChallenge();
	}
}


void LudumGameInstance::OnInputModeChanged(int new_mode, int old_mode)
{
	if (sequence_challenge != nullptr)
	{
		if (chaos::InputMode::IsPlatformChanged(new_mode, old_mode))
		{
			sequence_challenge->particle_range = CreateChallengeParticles(sequence_challenge.get());
			sequence_challenge->Show(game->IsPlaying());
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


void LudumGameInstance::TickBackgroundFillRatio(double delta_time)
{
	LudumGame const* game = GetLudumGame();
	if (game == nullptr)
		return;

	LudumLevelInstance const* level_instance = GetLudumLevelInstance();
	if (level_instance == nullptr)
		return;

	LudumLevel const* level = GetLudumLevel();
	if (level == nullptr)
		return;

	size_t brick_count = level_instance->GetBrickCount();
	size_t indestructible_brick_count = level->indestructible_brick_count;
	size_t destructible_brick_count = level->destructible_brick_count;

	float target_fillratio = (float)(brick_count - indestructible_brick_count) / (float)(destructible_brick_count);

	if (target_fillratio < current_background_fillratio)
	{
		current_background_fillratio -= game->background_fillratio_changespeed * (float)delta_time;
		current_background_fillratio = std::max(current_background_fillratio, target_fillratio);
	}

	// tick for completion timer
	if (current_background_fillratio <= 0.0f)
		complete_level_timer += (float)delta_time;
}

void LudumGameInstance::FillUniformProvider(chaos::GPUProgramProvider& main_uniform_provider)
{
	death::GameInstance::FillUniformProvider(main_uniform_provider);

    LudumGame const* ludum_game = GetLudumGame();
    if (ludum_game == nullptr)
        return;

    float delay = 0.5f * ludum_game->delay_before_next_level;

    float fill_ratio =
        current_background_fillratio * 0.5f +
        0.5f * std::max((delay - complete_level_timer) / delay, 0.0f);
	main_uniform_provider.AddVariableValue("fill_ratio", fill_ratio);
}


void LudumGameInstance::TickBallSplit(double delta_time)
{
	LudumGame const * ludum_game = GetLudumGame();

	if (pending_split_count <= 0)
		return;

	size_t ball_count = GetBallCount();
	if (ball_count > (size_t)ludum_game->max_ball_count || ball_count == 0)
		return;

	size_t ball_candidate = CanStartChallengeBallIndex(false); // any ball going up (do not split a ball that could be lost)
	if (ball_candidate == std::numeric_limits<size_t>::max())
		return;

	balls_allocations->Resize(ball_count + 1);

	ParticleMovableObject * balls = GetBallParticles();

	ParticleMovableObject * parent_ball = &balls[ball_candidate];
	ParticleMovableObject * new_ball = &balls[ball_count];
	*new_ball = *parent_ball;

	RotateVelocity(parent_ball->velocity, ludum_game->split_angle);
	RotateVelocity(new_ball->velocity, -ludum_game->split_angle);

	pending_split_count = 0;
}

void LudumGameInstance::TickBrickOffset(double delta_time)
{
	LudumGame const * ludum_game = GetLudumGame();

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
	if (!game->IsPlaying())
		return;
	if (sequence_challenge != nullptr)
		sequence_challenge->OnGamepadButtonReceived(in_gamepad_data);
}

void LudumGameInstance::OnChallengeCompleted(LudumChallenge * challenge, bool success, size_t challenge_size)
{
	LudumGame const * ludum_game = GetLudumGame();

	// rewards/punishment
	auto const & rewards_punishments = (success) ? ludum_game->rewards : ludum_game->punishments;

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
	challenge_timer = ludum_game->challenge_frequency;

	// update the score
	if (success)
	{
		IncrementScore(ludum_game->points_per_challenge * (int)challenge_size);
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
	target_brick_offset = 0.0f;
	brick_offset = 0.0f;
	current_background_fillratio = 1.0f;
	complete_level_timer = 0.0f;
}

bool LudumGameInstance::CanCompleteLevel() const
{
	if (!death::GameInstance::CanCompleteLevel())
		return false;
	if (current_background_fillratio > 0.0f)
		return false;
	LudumGame const* ludum_game = GetLudumGame();
	if (ludum_game != nullptr && complete_level_timer < ludum_game->delay_before_next_level)
		return false;
	return true;
}

bool LudumGameInstance::DoTick(double delta_time)
{
	if (!death::GameInstance::DoTick(delta_time)) // ticking GameInstance, tick Players ... this is usefull to work with inputs
		return false;

	// some other calls
	if (game->IsPlaying() && !game->IsFreeCameraMode())
	{
		TickBrickOffset(delta_time);
		TickChallenge(delta_time);
		TickBallSplit(delta_time);
		TickBackgroundFillRatio(delta_time);
	}
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
	LudumLevel const * level = GetLudumLevel();

	LudumLevelInstance const * level_instance = GetLudumLevelInstance();

	size_t brick_count = level_instance->GetBrickCount();
	return (brick_count > level->indestructible_brick_count);
}

void LudumGameInstance::OnBrickLifeChallenge(bool success)
{
	LudumGame const * ludum_game = GetLudumGame();

	LudumLevelInstance * level_instance = GetLudumLevelInstance();


	size_t brick_count = level_instance->GetBrickCount();
	if (brick_count == 0)
		return;

	ParticleBrick * bricks = level_instance->GetBricks();
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
		IncrementScore(destroyed_count * ludum_game->points_per_brick);
	}
	else
	{
		// increase all particles life
		for (size_t i = 0; i < brick_count; ++i)
		{
			ParticleBrick & p = bricks[i];
			if (!p.indestructible && p.life < ludum_game->max_brick_life && p.life > 0.0f)
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
	LudumGame const * ludum_game = GetLudumGame();

	size_t ball_count = GetBallCount();
	return (ball_count < (size_t)ludum_game->max_ball_count);
}

void LudumGameInstance::OnSplitBallChallenge(bool success)
{
	if (!success)
		return;
	pending_split_count++;
}

bool LudumGameInstance::IsBallSpeedChallengeValid(bool success)
{
	LudumGame const * ludum_game = GetLudumGame();

	if (success)
		return (ball_speed > ludum_game->ball_initial_speed); // can still decrease speed ?
	else
		return (ball_speed < ludum_game->ball_max_speed); // can still increase speed ?
}

void LudumGameInstance::OnBallSpeedChallenge(bool success)
{
	LudumGame const * ludum_game = GetLudumGame();

	if (success)
		ball_speed = ball_speed - ludum_game->ball_speed_increment;
	else
		ball_speed = ball_speed + ludum_game->ball_speed_increment;

	ball_speed = std::clamp(ball_speed, ludum_game->ball_initial_speed, ludum_game->ball_max_speed);
}

void LudumGameInstance::OnBrickOffsetChallenge(bool success)
{
	LudumGame const * ludum_game = GetLudumGame();

	if (success)
		target_brick_offset -= ludum_game->brick_offset_increment;
	else
		target_brick_offset += ludum_game->brick_offset_increment;

	target_brick_offset = std::clamp(target_brick_offset, 0.0f, ludum_game->max_brick_offset);
}

bool LudumGameInstance::IsBrickOffsetChallengeValid(bool success)
{
	LudumGame const * ludum_game = GetLudumGame();

	if (success)
		return (target_brick_offset > 0.0f);
	else
		return (target_brick_offset < ludum_game->max_brick_offset);
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
	ball_power = std::clamp(ball_power, 0.5f, 3.0f);
}

bool LudumGameInstance::IsExtraBallChallengeValid(bool success)
{
	LudumPlayer * player = GetLudumPlayer(0);
	if (player == nullptr)
		return false;

	int current_life = player->GetLifeCount();
	if (success)
		return current_life < player->GetMaxLifeCount(); // do not add life is already max
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
	LudumGame const * ludum_game = GetLudumGame();

	LudumPlayer * player = GetLudumPlayer(0);
	if (player == nullptr)
		return false;

	float player_length = player->GetPlayerLength();
	if (success)
		return (player_length < ludum_game->player_max_length); // can only increment if bar is not at max already
	else
		return (player_length > ludum_game->player_min_length); // can only decrement if bar is not at min already
}

void LudumGameInstance::OnLongBarChallenge(bool success)
{
	LudumGame const * ludum_game = GetLudumGame();

	LudumPlayer * player = GetLudumPlayer(0);
	if (player == nullptr)
		return;

	if (success)
		player->SetPlayerLength(ludum_game->player_length_increment, true);
	else
		player->SetPlayerLength(-ludum_game->player_length_decrement, true);
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
		std::cos(angle),
		std::sin(angle));
}



chaos::ParticleAllocationBase * LudumGameInstance::CreateBalls(size_t count, bool full_init)
{
	LudumGame const * ludum_game = GetLudumGame();

	// create the object
	chaos::ParticleAllocationBase * result = game->GetGameParticleCreator().SpawnParticles(death::GameHUDKeys::BALL_LAYER_ID, "ball", 1, true);
	if (result == nullptr)
		return nullptr;

	// set the color
	chaos::ParticleAccessor<ParticleMovableObject> particles = result->GetParticleAccessor();
	if (particles.GetCount() == 0)
		return nullptr;

	for (size_t i = 0 ; i < count ; ++i)
	{	
		particles[i].color         = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		particles[i].bounding_box.position  = glm::vec2(0.0f, 0.0f);
		particles[i].bounding_box.half_size = 0.5f * glm::vec2(ludum_game->ball_size, ludum_game->ball_size);
		
		if (full_init)
		{
			particles[i].delay_before_move = ludum_game->delay_before_ball_move;
			particles[i].velocity = ball_speed * GenerateBallRandomDirection();
		}
	}
	return result;
}

ParticleMovableObject * LudumGameInstance::GetBallParticles()
{
	if (balls_allocations == nullptr)
		return nullptr;	
	chaos::ParticleAccessor<ParticleMovableObject> particles = balls_allocations->GetParticleAccessor();
	if (particles.GetCount() == 0)
		return nullptr;
	return &particles[0];
}

ParticleMovableObject const * LudumGameInstance::GetBallParticles() const
{
	if (balls_allocations == nullptr)
		return nullptr;
	chaos::ParticleConstAccessor<ParticleMovableObject> p = balls_allocations->GetParticleAccessor();
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
	LudumGame const * ludum_game = GetLudumGame();

	size_t key_index = (size_t)(rand() % ludum_game->gamepad_buttons.size());
	if (key_index >= ludum_game->gamepad_buttons.size())
		key_index = ludum_game->gamepad_buttons.size() - 1;
	return ludum_game->gamepad_buttons[key_index];
}


LudumChallenge * LudumGameInstance::CreateSequenceChallenge()
{
	LudumGame const * ludum_game = GetLudumGame();
	LudumLevel const * ludum_level = GetLudumLevel();
	LudumLevelInstance * ludum_level_instance = GetLudumLevelInstance();	

	// if the level has a dedicated text, use it
	if (ludum_level != nullptr && ludum_level_instance != nullptr)
	{
		size_t word_count = ludum_level->dictionnary.size();
		if (word_count > 0)
		{
			size_t index = ludum_level_instance->word_index % word_count;

			ludum_level_instance->word_index = (ludum_level_instance->word_index + 1) % word_count;

			return CreateSequenceChallenge(ludum_level->dictionnary[index]);
		}
	}

	// search a word with random length
	size_t len = ludum_game->min_word_size + rand() % (ludum_game->max_word_size - ludum_game->min_word_size);

	auto it = ludum_game->dictionnary.find(len);

	// no word of this size (search a word with the lengh the more near the request) 
	if (it == ludum_game->dictionnary.end())
	{
		auto better_it = ludum_game->dictionnary.begin();

		int min_distance = std::numeric_limits<int>::max();
		for (it = ludum_game->dictionnary.begin(); it != ludum_game->dictionnary.end(); ++it)
		{
			int distance = abs((int)len - (int)it->first);
			if (distance < min_distance)
			{
				min_distance = distance;
				better_it = it;
			}
		}
		if (it == ludum_game->dictionnary.end()) // should never happen
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

	std::string const & keyboard_challenge = words[index];
	return CreateSequenceChallenge(keyboard_challenge);
}

LudumChallenge * LudumGameInstance::CreateSequenceChallenge(std::string keyboard_challenge)
{
	LudumGame const * ludum_game = GetLudumGame();

	// remove all spaces from the challenge
	std::string nospace_keyboard_challenge = LudumChallenge::NoSpaceKeyboardChallenge(keyboard_challenge);

	// the gamepad length is the same that the no space version
	size_t len = nospace_keyboard_challenge.size();
	if (len == 0)
		return nullptr;

	// compose a gamepad combinaison of the same length
	std::vector<int> gamepad_challenge;
	for (size_t i = 0; i < len; ++i)
		gamepad_challenge.push_back(GetRandomButtonID());

	// create the challenge
	LudumChallenge * result = new LudumChallenge;
	if (result != nullptr)
	{
		// initialize the challenges
		result->gamepad_challenge = std::move(gamepad_challenge);
		result->keyboard_challenge = std::move(keyboard_challenge);
		// initialize the positions
		result->gamepad_challenge_position = 0;
		result->keyboard_challenge_position = 0;
		while (result->keyboard_challenge[result->keyboard_challenge_position] == ' ')
			++result->keyboard_challenge_position;
		// initialize other values
		result->game_instance = this;
		result->particle_range = CreateChallengeParticles(result);
		result->Show(game->IsPlaying());
		result->SetTimeout(ludum_game->challenge_duration);

		ball_time_dilation = ludum_game->challenge_time_dilation;
	}
	return result;
}

std::string LudumGameInstance::GenerateGamepadChallengeString(std::vector<int> const & gamepad_challenge)
{
	LudumGame const * ludum_game = GetLudumGame();

	std::string result;

	for (size_t i = 0; i < gamepad_challenge.size(); ++i)
	{
		int button_index = gamepad_challenge[i];

		auto const it = ludum_game->gamepad_button_map.find(button_index);
		if (it == ludum_game->gamepad_button_map.end())
			continue;
		result = result + "[" + it->second.second + "]";
	}
	return result;
}

chaos::ParticleAllocationBase * LudumGameInstance::CreateChallengeParticles(LudumChallenge * challenge)
{
	int  input_mode = chaos::MyGLFW::SingleWindowApplication::GetApplicationInputMode();
	bool keyboard = chaos::InputMode::IsPCMode(input_mode);

	chaos::ParticleLayerBase * layer = game->GetParticleManager()->FindLayer(death::GameHUDKeys::CHALLENGE_LAYER_ID);
	if (layer == nullptr)
		return nullptr;

	chaos::ParticleTextGenerator::GeneratorResult result;
	chaos::ParticleTextGenerator::GeneratorParams params;

	params.line_height = CHALLENGE_SIZE;
	params.hotpoint_type = chaos::Hotpoint::TOP | chaos::Hotpoint::HMIDDLE;
	params.position.x = 0.0f;
	params.position.y = CHALLENGE_PLACEMENT_Y;
	params.font_info_name = "challenge";

	if (keyboard)
	{
		game->GetTextGenerator()->Generate(challenge->keyboard_challenge.c_str(), result, params);
	}
	else
	{
		std::string gamepad_string = GenerateGamepadChallengeString(challenge->gamepad_challenge);
		game->GetTextGenerator()->Generate(gamepad_string.c_str(), result, params);
	}

	// create the text
	chaos::ParticleAllocationBase * allocation = chaos::ParticleTextGenerator::CreateTextAllocation(layer, result);
	// and initialize additionnal data
	if (allocation != nullptr)
	{
		chaos::ParticleAccessor<ParticleChallenge> particles = allocation->GetParticleAccessor();
		for (size_t i = 0; i < particles.GetCount(); ++i)
		{
			ParticleChallenge & p = particles[i];
			p.challenge = challenge;
			p.index = i;
		}
	}

	return allocation;
}

void LudumGameInstance::OnBallCollide(bool collide_brick)
{
	LudumGame const * ludum_game = GetLudumGame();


	game->PlaySound("ball", false, false, 0.0f, death::SoundContext::GAME);

	ball_collision_speed = std::min(ludum_game->ball_collision_max_speed, ball_collision_speed + ludum_game->ball_collision_speed_increment);

	if (collide_brick)
		IncrementScore(ludum_game->points_per_brick);
}

void LudumGameInstance::DestroyGameObjects()
{
	balls_allocations = nullptr;
	sequence_challenge = nullptr;
}

bool LudumGameInstance::Initialize(death::Game * in_game)
{
	if (!death::GameInstance::Initialize(in_game))
		return false;

	if (balls_allocations == nullptr)
		balls_allocations = CreateBalls(1, true);

	LudumGame const * ludum_game = GetLudumGame();
	challenge_timer = ludum_game->challenge_frequency;
	ball_speed = ludum_game->ball_initial_speed;

	return true;
}

void LudumGameInstance::OnPlayerEntered(death::Player * player)
{
	death::GameInstance::OnPlayerEntered(player);

	// create the player pawn
	chaos::ParticleAllocationBase * player_allocation = game->GetGameParticleCreator().SpawnParticles(death::GameHUDKeys::GAMEOBJECT_LAYER_ID, "player", 1, true);
	if (player_allocation == nullptr)
		return;
	player->SetPlayerAllocation(player_allocation);
	// initialize the player particle
	chaos::ParticleAccessor<ParticleObject> particles = player_allocation->GetParticleAccessor();
	if (particles.GetCount() == 0)
		return;
	particles[0].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	particles[0].bounding_box.position = glm::vec2(0.0f, 0.0f);
	particles[0].bounding_box.half_size = glm::vec2(0.0f, 0.0f);
	// set the player length
	LudumPlayer * ludum_player = auto_cast(player);
	if (ludum_player != nullptr)
	{
		LudumGame * ludum_game = GetLudumGame();
		if (ludum_game != nullptr)
			ludum_player->SetPlayerLength(ludum_game->player_initial_length, false);
	}
}

void LudumGameInstance::OnPlayerLeaved(death::Player * player)
{
	death::GameInstance::OnPlayerLeaved(player);

}

bool LudumGameInstance::DoCheckGameOverCondition()
{
	LudumPlayer * ludum_player = GetLudumPlayer(0);
	if (ludum_player != nullptr)
	{
		size_t ball_count = GetBallCount();
		if (ball_count == 0)
		{
			ludum_player->SetLifeCount(-1, true);
			if (ludum_player->GetLifeCount() <= 0)
				return true;
			else
			{
				LudumGame * ludum_game = GetLudumGame();

				ludum_game->PlaySound("balllost", false, false, 0.0f, death::SoundContext::GAME);
				combo_multiplier = 1;
				ball_collision_speed = 0.0f;
				ball_power = 1.0f;
				ball_speed = ludum_game->ball_initial_speed;
				pending_split_count = 0;
		//		brick_offset = 0.0f;
				target_brick_offset = 0.0f;
				ball_time_dilation = 1.0f;
				challenge_timer = ludum_game->challenge_frequency;
				ludum_player->SetPlayerLength(ludum_game->player_initial_length, false);
				balls_allocations = CreateBalls(1, true);
				ludum_player->OnLifeLost();
				return false;
			}			
		}
	}
	return death::GameInstance::DoCheckGameOverCondition();
}
