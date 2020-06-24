#include <death/Player.h>
#include <death/GameCheckpoint.h>
#include <death/ShakeCameraComponent.h>
#include <death/GameGettersImpl.h>

#include <chaos/ParticleDefault.h>

namespace death
{

	DEATH_GAMEGETTERS_IMPLEMENT(Player);

	// =================================================
	// Player
	// =================================================

	void Player::OnLevelChanged(death::Level * new_level, death::Level * old_level, death::LevelInstance * new_level_instance)
	{
        health = max_health;
		invulnerability_timer = std::max(invulnerability_duration, 0.0f);
	}

	bool Player::Initialize(death::GameInstance * in_game_instance)
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

	bool Player::CapturePhysicalGamepad(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
	{
		// if we already have a device, ignore
		if (gamepad != nullptr)
			return false;
		// if no entry or the device is already owned by someone, exit
		if (in_physical_gamepad == nullptr || in_physical_gamepad->IsAllocated())
			return false;
		// try capture the device
		chaos::shared_ptr<PlayerGamepadCallbacks> gamepad_callback = new PlayerGamepadCallbacks(this);
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
		chaos::MyGLFW::GamepadData const* gamepad_data = gamepad->GetGamepadData();
		if (gamepad_data == nullptr)
			return;
		// maybe a game/pause resume
		if ((gamepad_data->GetButtonStateChange(chaos::XBoxButton::BUTTON_SELECT) == chaos::ButtonStateChange::BECOME_PRESSED) ||
			(gamepad_data->GetButtonStateChange(chaos::XBoxButton::BUTTON_START) == chaos::ButtonStateChange::BECOME_PRESSED))
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
			gamepad->SetForceFeedbackMuted(mute_force_feedback || (GetInputMode() != chaos::InputMode::GAMEPAD));
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
		chaos::MyGLFW::GamepadData const * gamepad_data = gamepad->GetGamepadData();
		if (gamepad_data == nullptr)
			return;
		// change the application mode
		if (gamepad_data->IsAnyAction())			
			SetInputMode(chaos::InputMode::GAMEPAD);

		// cache the LEFT stick position (it is aliases with the DPAD)
		glm::vec2 lsp = gamepad_data->GetXBOXStickDirection(chaos::XBoxAxis::LEFT_AXIS);
		if (glm::length2(lsp) > 0.0f)
			left_stick_position = lsp;
		else
		{
			if (gamepad_data->IsButtonPressed(chaos::XBoxButton::BUTTON_LEFT, false))
				left_stick_position.x = -1.0f;
			else if (gamepad_data->IsButtonPressed(chaos::XBoxButton::BUTTON_RIGHT, false))
				left_stick_position.x = 1.0f;

			if (gamepad_data->IsButtonPressed(chaos::XBoxButton::BUTTON_UP, false))
				left_stick_position.y = -1.0f;
			else if (gamepad_data->IsButtonPressed(chaos::XBoxButton::BUTTON_DOWN, false))
				left_stick_position.y = 1.0f;
		}

		// cache the RIGHT stick position
		glm::vec2 rsp = gamepad_data->GetXBOXStickDirection(chaos::XBoxAxis::RIGHT_AXIS);
		if (glm::length2(rsp) > 0.0f)
			right_stick_position = rsp;

		// cache the TRIGGERS
		left_trigger  = gamepad_data->GetAxisValue(chaos::XBoxAxis::LEFT_TRIGGER);
		right_trigger = gamepad_data->GetAxisValue(chaos::XBoxAxis::RIGHT_TRIGGER);
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
		chaos::MyGLFW::GamepadData const * gamepad_data = gamepad->GetGamepadData();
		if (gamepad_data == nullptr)
			return;
		// Handle the inputs as we want
		InternalHandleGamepadInputs(delta_time, gamepad_data);
	}

	void Player::InternalHandleGamepadInputs(float delta_time, chaos::MyGLFW::GamepadData const * gamepad_data)
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

	bool Player::DoSaveIntoCheckpoint(PlayerCheckpoint * checkpoint) const
	{
		checkpoint->life_count = life_count;
		checkpoint->health = health;
		checkpoint->max_health = max_health;
		checkpoint->invulnerability_timer = invulnerability_timer;
		checkpoint->invulnerability_duration = invulnerability_duration;
		checkpoint->score = score;

		if (pawn != nullptr)
			checkpoint->pawn_checkpoint = pawn->SaveIntoCheckpoint();

		return true;
	}

	bool Player::DoLoadFromCheckpoint(PlayerCheckpoint const * checkpoint)
	{
		life_count = checkpoint->life_count;
		health = checkpoint->health;
		max_health = checkpoint->max_health;
		invulnerability_timer = checkpoint->invulnerability_timer;
		invulnerability_duration = checkpoint->invulnerability_duration;
		score = checkpoint->score;

		if (pawn != nullptr)
			pawn->LoadFromCheckpoint(checkpoint->pawn_checkpoint.get());

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
			// force feedback effect
			if (gamepad != nullptr)
				gamepad->AddForceFeedbackEffect(new chaos::MyGLFW::DefaultForceFeedbackEffect(0.09f, 1.0f, 1.0f));
			// camera effect
            death::Camera* camera = GetLevelInstance()->GetCamera(0);
            if (camera != nullptr)
            {
                death::ShakeCameraComponent* shake_component = camera->FindComponentByClass<death::ShakeCameraComponent>();
                if (shake_component != nullptr)
                    shake_component->RestartModifier();
            }
        }
    }

	bool Player::IsDead() const
	{
		// no more health
		if (health <= 0.0f)
			return true;
		return false;
	}

    bool Player::CheckButtonPressed(int const* keyboard_buttons, int gamepad_button)
    {
        // keyboard input
        if (keyboard_buttons != nullptr)
        {
            death::Game * game = GetGame();
            if (game != nullptr)
            {
                GLFWwindow * glfw_window = game->GetGLFWWindow();
                if (glfw_window != nullptr)
                    for (int i = 0; keyboard_buttons[i] >= 0 ; ++i)
                        if (CheckKeyPressed(keyboard_buttons[i]))
                            return true;
            }
        }

        // gamepad input
        if (gamepad_button >= 0)
            if (gamepad != nullptr && gamepad->IsButtonPressed(gamepad_button))
                return true;
        return false;
    }

	bool Player::InitializeGameValues(nlohmann::json const& config, boost::filesystem::path const& config_path, bool hot_reload)
	{
		DEATHGAME_JSON_ATTRIBUTE(life_count);
		DEATHGAME_JSON_ATTRIBUTE(max_health);
		DEATHGAME_JSON_ATTRIBUTE(invulnerability_duration);		
		return true;
	}

	void Player::OnGameValuesChanged(bool hot_reload)
	{
		health = max_health;
	}

	GLFWwindow* Player::GetGLFWWindow() const
	{
		Game const* game = GetGame();
		if (game == nullptr)
			return nullptr;
		return game->GetGLFWWindow();
	}

	// =================================================
	// PlayerGamepadCallbacks
	// =================================================

	PlayerGamepadCallbacks::PlayerGamepadCallbacks(Player * in_player) :
		player(in_player)
	{
		assert(in_player != nullptr);
	}

	bool PlayerGamepadCallbacks::OnGamepadDisconnected(chaos::MyGLFW::Gamepad * in_gamepad)
	{
		player->OnGamepadDisconnected();
		return true;
	}

}; // namespace death

