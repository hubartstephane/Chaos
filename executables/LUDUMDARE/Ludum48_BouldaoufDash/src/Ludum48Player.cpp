#include "Ludum48PCH.h"
#include "Ludum48Player.h"
#include "Ludum48Level.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48Game.h"
#include "Ludum48GameInstance.h"
#include "Ludum48Particles.h"

bool LudumPlayer::Initialize(GameInstance * in_game_instance)
{
	if (!Player::Initialize(in_game_instance))
		return false;



	return true;
}

void LudumPlayer::OnLifeLost()
{
	Player::OnLifeLost();

	death_timer = -1.0f;
	suicidal_timer = -1.0f;


	GetGame()->ReloadCurrentLevel();


}

void LudumPlayer::OnLevelChanged(Level * new_level, Level * old_level, LevelInstance * new_level_instance)
{
	Player::OnLevelChanged(new_level, old_level, new_level_instance);

	death_timer = -1.0f;
	suicidal_timer = -1.0f;


}

bool LudumPlayer::IsDead() const
{
	if (Player::IsDead())
		return true;
	if (death_timer == 0.0f)
		return true;
	return false;
}

bool LudumPlayer::EnumerateInputActions(InputActionProcessor& in_action_processor, EnumerateInputActionContext in_context)
{
	auto suicide_input = CombinedInput(Key::Space, Key::GamepadY);
	QueryAndReserveInput(in_action_processor, suicide_input, "Suicide", suicide_pressed);


	LudumLevelInstance * ludum_level_instance = GetLevelInstance();
	if (ludum_level_instance != nullptr)
	{
		// fake move
		auto FakeMoveInput = CombinedInput(Key::LeftControl, Key::RightControl, Key::GamepadA);

		bool fake_move_pressed = false;

		auto FakeMoveInputCondition = QueryInput(FakeMoveInput, &fake_move_pressed, chaos::InputStatusCheckType::Active);

		if (in_action_processor.CheckAndProcess(FakeMoveInputCondition, "Fake Move", [&]()
		{
			ludum_level_instance->SetCachedFakeMove(fake_move_pressed);
		}))
		{
			return true;
		}
	
		// move
		auto MoveInput = CombinedInput(Input2D::GamepadLeftStick, MappedInput2D::gamepad_dpad, MappedInput2D::keyboard_arrows);
		//auto MoveInput = CombinedInput(Input2D::GamepadLeftStick, MappedInput2D::keyboard_arrows);
		Input2DState stick_state;

		auto MoveInputCondition = QueryInput(MoveInput, &stick_state);

		if (in_action_processor.CheckAndProcess(MoveInputCondition, "Move", [&]()
		{
			ludum_level_instance->SetMoveInputState(stick_state);
		}))
		{
			return true;
		}	
	
	}
	return Super::EnumerateInputActions(in_action_processor, in_context);
}

bool LudumPlayer::DoTick(float delta_time)
{
	Player::DoTick(delta_time);

	// count down
	if (pawn == nullptr || pawn->GetParticle<ParticleDefault>(0) == nullptr)
	{
		if (death_timer < 0.0f)
			death_timer = max_death_timer;
		death_timer = std::max(0.0f, death_timer - delta_time);
	}

	// suicidal
	if (death_timer < 0.0f)
	{
		if (suicide_pressed)
		{
			suicidal_timer = std::min(std::max(suicidal_timer, 0.0f) + delta_time, max_suicidal_timer);
			if (suicidal_timer == max_suicidal_timer)
			{
				death_timer = 0.0f; // kill the player
			}
		}
		else
		{
			suicidal_timer = -1.0f;



		}



	}


	return true;


}

bool LudumPlayer::OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context)
{
	if (!Player::OnReadConfigurableProperties(config, context))
		return false;
	if (life_count != 8)
		cheater_farid = true;
	return true;
}
