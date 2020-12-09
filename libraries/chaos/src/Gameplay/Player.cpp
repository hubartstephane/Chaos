#include <chaos/Chaos.h>

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
		game_instance = in_game_instance;

		// read the configuration
		Game* game = GetGame();
		if (game != nullptr)
		{
			if (!InitializeGameValues(in_game_instance->player_configuration, game->configuration_path, false)) // false => not hot relead
				return false;
			OnGameValuesChanged(false);
		}
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
		gamepad = in_physical_gamepad->CaptureDevice(gamepad_callback.get());
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

	void Player::HandleSpecialGamepadCommands(float delta_time)
	{
		if (gamepad == nullptr)
			return;
		// get the gamepad data
		GamepadState const* gamepad_state = gamepad->GetGamepadState();
		if (gamepad_state == nullptr)
			return;
		// maybe a game/pause resume
		if ((gamepad_state->GetButtonStateChange(XBoxButton::BUTTON_SELECT) == ButtonStateChange::BECOME_PRESSED) ||
			(gamepad_state->GetButtonStateChange(XBoxButton::BUTTON_START) == ButtonStateChange::BECOME_PRESSED))
		{
			Game* game = GetGame();
			if (game != nullptr)
				game->RequireTogglePause();
		}
	}

	bool Player::DoTick(float delta_time)
	{	
		// remove previous frame cached input
		ResetCachedInputs();
		// cache values for stick displacement
		CacheKeyboardPlayerDisplacementInputs();
		// cache values for stick displacement
		CacheGamepadPlayerDisplacementInputs();

		bool mute_force_feedback   = true; // no forcefeedback except during main loop
		bool paused_force_feedback = true;

		// tick player displacement
		Game* game = GetGame();
		if (game != nullptr && !game->IsFreeCameraMode() && game->IsPlaying())
		{
			// transform keyboard inputs as stick input
			HandleKeyboardInputs(delta_time);
			// handle gamepad inputs
			HandleGamepadInputs(delta_time);
			// tick other player objects
			TickInternal(delta_time);

			mute_force_feedback = false;
			paused_force_feedback = false;
		}

		// some gamepad commands that should be handled even when not in Game
		HandleSpecialGamepadCommands(delta_time);
		// update the forcefeedback mute state
		if (gamepad != nullptr)
		{
			gamepad->SetForceFeedbackMuted(mute_force_feedback || (GetInputMode() != InputMode::GAMEPAD));
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

	void Player::CacheKeyboardPlayerDisplacementInputs()
	{
		// test whether the stick position can be overriden
		glm::vec2 simulated_stick = glm::vec2(0.0f, 0.0f);

		if (CheckKeyPressed(GLFW_KEY_LEFT))
			simulated_stick.x -= 1.0f;
		if (CheckKeyPressed(GLFW_KEY_RIGHT))
			simulated_stick.x += 1.0f;

		if (CheckKeyPressed(GLFW_KEY_DOWN))
			simulated_stick.y += 1.0f;
		if (CheckKeyPressed(GLFW_KEY_UP))
			simulated_stick.y -= 1.0f;

		if (glm::length2(simulated_stick) > 0)
			left_stick_position = simulated_stick;
	}

	void Player::CacheGamepadPlayerDisplacementInputs()
	{
		// early exit
		if (gamepad == nullptr)
			return;
		// get the gamepad data
		GamepadState const * gamepad_state = gamepad->GetGamepadState();
		if (gamepad_state == nullptr)
			return;
		// change the application mode
		if (gamepad_state->IsAnyAction())			
			SetInputMode(InputMode::GAMEPAD);

		// cache the LEFT stick position (it is aliases with the DPAD)
		glm::vec2 lsp = gamepad_state->GetStickValue(XBoxStick::LEFT_STICK);
		if (glm::length2(lsp) > 0.0f)
			left_stick_position = lsp;
		else
		{
			if (gamepad_state->IsButtonPressed(XBoxButton::BUTTON_LEFT, false))
				left_stick_position.x = -1.0f;
			else if (gamepad_state->IsButtonPressed(XBoxButton::BUTTON_RIGHT, false))
				left_stick_position.x = 1.0f;

			if (gamepad_state->IsButtonPressed(XBoxButton::BUTTON_UP, false))
				left_stick_position.y = -1.0f;
			else if (gamepad_state->IsButtonPressed(XBoxButton::BUTTON_DOWN, false))
				left_stick_position.y = 1.0f;
		}

		// cache the RIGHT stick position
		glm::vec2 rsp = gamepad_state->GetStickValue(XBoxStick::RIGHT_STICK);
		if (glm::length2(rsp) > 0.0f)
			right_stick_position = rsp;

		// cache the TRIGGERS
		left_trigger  = gamepad_state->GetAxisValue(XBoxAxis::LEFT_TRIGGER);
		right_trigger = gamepad_state->GetAxisValue(XBoxAxis::RIGHT_TRIGGER);
	}

	void Player::HandleKeyboardInputs(float delta_time)
	{

	}

	void Player::ResetCachedInputs()
	{
		left_stick_position  = glm::vec2(0.0f, 0.0f);
		right_stick_position = glm::vec2(0.0f, 0.0f);
		left_trigger  = 0.0f;
		right_trigger = 0.0f;
	}

	void Player::HandleGamepadInputs(float delta_time)
	{
		// early exit
		if (gamepad == nullptr)
			return;
		// get the gamepad data
		GamepadState const * gamepad_state = gamepad->GetGamepadState();
		if (gamepad_state == nullptr)
			return;
		// Handle the inputs as we want
		InternalHandleGamepadInputs(delta_time, gamepad_state);
	}

	void Player::InternalHandleGamepadInputs(float delta_time, GamepadState const * gamepad_state)
	{

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

	bool Player::SerializeIntoJSON(nlohmann::json& json_entry) const
	{
		if (!JSONSerializable::SerializeIntoJSON(json_entry))
			return false;

		JSONTools::SetAttribute(json_entry, "LIFE_COUNT", life_count);
		JSONTools::SetAttribute(json_entry, "HEALTH", health);
		JSONTools::SetAttribute(json_entry, "MAX_HEALTH", max_health);
		JSONTools::SetAttribute(json_entry, "INVULNERABILITY_TIMER", invulnerability_timer);
		JSONTools::SetAttribute(json_entry, "INVULNERABILITY_DURATION", invulnerability_duration);
		JSONTools::SetAttribute(json_entry, "SCORE", score);

		if (pawn != nullptr)
			JSONTools::SetAttribute(json_entry, "PAWN", *pawn);

		return true;
	}

	bool Player::SerializeFromJSON(nlohmann::json const& json_entry)
	{
		if (!JSONSerializable::SerializeFromJSON(json_entry))
			return false;

		JSONTools::GetAttribute(json_entry, "LIFE_COUNT", life_count);
		JSONTools::GetAttribute(json_entry, "HEALTH", health);
		JSONTools::GetAttribute(json_entry, "MAX_HEALTH", max_health);
		JSONTools::GetAttribute(json_entry, "INVULNERABILITY_TIMER", invulnerability_timer);
		JSONTools::GetAttribute(json_entry, "INVULNERABILITY_DURATION", invulnerability_duration);
		JSONTools::GetAttribute(json_entry, "SCORE", score);

		// XXX : the indirection is important to avoid a reallocation of the pawn
		if (pawn != nullptr)
			JSONTools::GetAttribute(json_entry, "PAWN", *pawn);

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
			gamepad->AddForceFeedbackEffect(new DefaultForceFeedbackEffect(0.09f, 1.0f, 1.0f));
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

    bool Player::CheckButtonPressed(int const* keyboard_buttons, XBoxButton gamepad_button)
    {
        // keyboard input
        if (keyboard_buttons != nullptr)
        {
			WindowApplication const* application = Application::GetInstance();
			if (application != nullptr)
			{
				for (int i = 0; keyboard_buttons[i] >= 0; ++i)
					if (application->GetKeyState(keyboard_buttons[i]))
						return true;
			}
        }

        // gamepad input
        if (gamepad_button != XBoxButton::UNKNOWN)
            if (gamepad != nullptr && gamepad->IsButtonPressed(gamepad_button))
                return true;
        return false;
    }

	bool Player::InitializeGameValues(nlohmann::json const& config, boost::filesystem::path const& config_path, bool hot_reload)
	{
		CHAOS_JSON_ATTRIBUTE(config, life_count);
		CHAOS_JSON_ATTRIBUTE(config, max_health);
		CHAOS_JSON_ATTRIBUTE(config, invulnerability_duration);		
		return true;
	}

	void Player::OnGameValuesChanged(bool hot_reload)
	{
		health = max_health;
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

