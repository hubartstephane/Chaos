#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	CHAOS_IMPLEMENT_GAMEPLAY_GETTERS(Player);

	// =================================================
	// Player
	// =================================================

	void Player::OnLevelChanged(Level * new_level, Level * old_level, LevelInstance * new_level_instance)
	{
		health = max_health;
		invulnerability_timer = std::max(invulnerability_duration, 0.0f);
	}

	bool Player::Initialize(GameInstance * in_game_instance)
	{
		// ensure valid arguments and not already initialized
		assert(in_game_instance != nullptr);
		assert(game_instance == nullptr);
		// some initialization
		game_instance = in_game_instance;
		// read JSON properties
		if (!ReadConfigurableProperties(ReadConfigurablePropertiesContext::Initialization, false))
			return false;
		return true;
	}

	bool Player::OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context)
	{
		CHAOS_JSON_ATTRIBUTE(config, life_count);
		CHAOS_JSON_ATTRIBUTE(config, max_health);
		CHAOS_JSON_ATTRIBUTE(config, invulnerability_duration);
		CHAOS_JSON_ATTRIBUTE(config, gamepad_filter_settings);
		health = max_health;
		return true;
	}

	bool Player::CapturePhysicalGamepad(PhysicalGamepad * in_physical_gamepad)
	{
		// if we already have a device, ignore
		if (gamepad != nullptr)
			return false;
		// if no entry or the device is already owned by someone, exit
		if (in_physical_gamepad == nullptr || in_physical_gamepad->IsAllocated())
			return false;
		// try capture the device
		shared_ptr<PlayerGamepadCallbacks> gamepad_callback = new PlayerGamepadCallbacks(this);

		gamepad = in_physical_gamepad->CaptureDevice(&gamepad_filter_settings, gamepad_callback.get());
		if (gamepad == nullptr)
			return false;
		return true;
	}

	void Player::OnGamepadDisconnected()
	{
		Game * game = GetGame();
		if (game != nullptr)
			game->RequirePauseGame();
	}

	bool Player::DoTick(float delta_time)
	{
		bool mute_force_feedback   = true; // no forcefeedback except during main loop
		bool paused_force_feedback = true;

		// tick player displacement
		Game* game = GetGame();
		if (game != nullptr && !game->IsFreeCameraMode() && game->IsPlaying())
		{
			// tick other player objects
			TickInternal(delta_time);

			mute_force_feedback = false;
			paused_force_feedback = false;
		}

		// update the forcefeedback mute state
		if (gamepad != nullptr)
		{
			gamepad->SetForceFeedbackMuted(mute_force_feedback || (GetInputMode() != InputMode::Gamepad));
			gamepad->SetForceFeedbackPaused(paused_force_feedback);
		}

		return true;
	}

	void Player::TickInternal(float delta_time)
	{
		// tick the invulnerability
		TickInvulnerability(delta_time);
		// tick the player displacement
		TickPlayerDisplacement(delta_time);
	}

	void Player::TickInvulnerability(float delta_time)
	{
		invulnerability_timer -= delta_time;
		if (invulnerability_timer < 0.0f)
			invulnerability_timer = 0.0f;
	}

	void Player::TickPlayerDisplacement(float delta_time)
	{
		if (displacement_component != nullptr)
			displacement_component->Tick(delta_time);
	}

	void Player::SetScore(int in_score, bool increment)
	{
		if (increment)
			score += in_score;
		else
			score = in_score;
	}

	void Player::SetLifeCount(int in_life, bool increment)
	{
		// update life
		if (increment)
			life_count += in_life;
		else
			life_count = in_life;
		// clamp life
		life_count = std::max(life_count, 0);
	}

	bool Player::SerializeIntoJSON(nlohmann::json * json) const
	{
		if (!JSONSerializableInterface::SerializeIntoJSON(json))
			return false;

		JSONTools::SetAttribute(json, "LIFE_COUNT", life_count);
		JSONTools::SetAttribute(json, "HEALTH", health);
		JSONTools::SetAttribute(json, "MAX_HEALTH", max_health);
		JSONTools::SetAttribute(json, "INVULNERABILITY_TIMER", invulnerability_timer);
		JSONTools::SetAttribute(json, "INVULNERABILITY_DURATION", invulnerability_duration);
		JSONTools::SetAttribute(json, "SCORE", score);

		if (pawn != nullptr)
			JSONTools::SetAttribute(json, "PAWN", *pawn);

		return true;
	}

	bool Player::SerializeFromJSON(JSONReadConfiguration config)
	{
		if (!JSONSerializableInterface::SerializeFromJSON(config))
			return false;

		JSONTools::GetAttribute(config, "LIFE_COUNT", life_count);
		JSONTools::GetAttribute(config, "HEALTH", health);
		JSONTools::GetAttribute(config, "MAX_HEALTH", max_health);
		JSONTools::GetAttribute(config, "INVULNERABILITY_TIMER", invulnerability_timer);
		JSONTools::GetAttribute(config, "INVULNERABILITY_DURATION", invulnerability_duration);
		JSONTools::GetAttribute(config, "SCORE", score);

		// XXX : the indirection is important to avoid a reallocation of the pawn
		if (pawn != nullptr)
			JSONTools::GetAttribute(config, "PAWN", *pawn);

		return true;
	}

	void Player::OnLifeLost()
	{
        health = max_health;
		invulnerability_timer = std::max(invulnerability_duration, 0.0f);
	}

	void Player::SetHealth(float in_value, bool in_increment)
	{
		// compute new life
		float old_health = health;
		float new_health = health;

		if (in_increment)
			new_health += in_value;
		else
			new_health = in_value;

		if (new_health < 0.0f)
			new_health = 0.0f;
		else if (new_health > max_health)
			new_health = max_health;

		bool invulnerable = false;
#if _DEBUG
		if (GetGame()->GetCheatMode())
			invulnerable = true;
#endif
		// invulnerability ?
		if (new_health < old_health && !invulnerable)
		{
			if (invulnerability_timer > 0)
				invulnerable = true;
			else
				invulnerability_timer = std::max(invulnerability_duration, 0.0f);
		}

		// commit life lost
		if (!invulnerable || new_health > old_health)
			health = new_health;
		OnHealthChanged(old_health, new_health, invulnerable);
	}

	void Player::OnHealthChanged(float old_health, float new_health, bool invulnerable)
	{
		// special FX
		if (old_health > new_health && !invulnerable)
		{
			PlayHealthChangedEffects(old_health > new_health);
		}
	}

	void Player::PlayHealthChangedEffects(bool health_lost)
	{
		// force feedback effect
		if (gamepad != nullptr)
			gamepad->AddForceFeedbackEffect(new DefaultForceFeedbackEffect(0.5f, { 1.0f, 1.0f }));
		// camera effect
		Camera* camera = GetLevelInstance()->GetCamera(0);
		if (camera != nullptr)
		{
			ShakeCameraComponent* shake_component = camera->FindComponentByClass<ShakeCameraComponent>();
			if (shake_component != nullptr)
				shake_component->RestartModifier();
		}
	}

	bool Player::IsDead() const
	{
		// no more health
		if (health <= 0.0f)
			return true;
		return false;
	}

	bool Player::InvokeWithUpgradedInputDevice(InputDeviceInterface const * in_input_device, InvokeWithUpgradedInputDeviceFunction in_func)
	{
		if (gamepad != nullptr)
		{
			auto composed_device = MakeComposedInputDevices(in_input_device, gamepad.get());
			return in_func(&composed_device);
		}
		return InputReceiverInterface::InvokeWithUpgradedInputDevice(in_input_device, in_func);
	}

	bool Player::TraverseInputReceiver(InputReceiverTraverser & in_traverser)
	{
		if (displacement_component != nullptr)
			if (in_traverser.Traverse(displacement_component.get()))
				return true;
		return InputReceiverInterface::TraverseInputReceiver(in_traverser);
	}

	bool Player::EnumerateInputActions(InputActionProcessor& in_action_processor, EnumerateInputActionContext in_context)
	{
		if (Game* game = GetGame())
		{
			auto pause_condition = JustActivated(CombinedInput(Key::GamepadSpecialLeft, Key::GamepadSpecialRight));

			if (in_action_processor.CheckAndProcess(pause_condition, "Toggle Pause", [&]()
			{
				game->RequireTogglePause();
			}))
			{
				return true;
			}
		}
		return InputReceiverInterface::EnumerateInputActions(in_action_processor, in_context);
	}

	// =================================================
	// PlayerGamepadCallbacks
	// =================================================

	PlayerGamepadCallbacks::PlayerGamepadCallbacks(Player * in_player) :
		player(in_player)
	{
		assert(in_player != nullptr);
	}

	bool PlayerGamepadCallbacks::OnGamepadDisconnected(Gamepad * in_gamepad)
	{
		player->OnGamepadDisconnected();
		return true;
	}

}; // namespace chaos

