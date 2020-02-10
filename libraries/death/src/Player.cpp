#include <death/Player.h>
#include <death/GameCheckpoint.h>

#include <chaos/ParticleDefault.h>

namespace death
{

	// =================================================
	// Player
	// =================================================

	Player::Player(GameInstance * in_game_instance) :
		GameInstanceEntity(in_game_instance)
	{
	}

	Player::~Player()
	{

	}

	void Player::OnLevelChanged(death::GameLevel * new_level, death::GameLevel * old_level, death::GameLevelInstance * new_level_instance)
	{
        health = max_health;
	}

	void Player::SetPlayerAllocation(chaos::ParticleAllocationBase * in_allocation)
	{
		player_allocations = in_allocation;
	}

	bool Player::OnKeyEvent(int key, int scan_code, int action, int modifier)
	{
		return false;
	}

	bool Player::OnCharEvent(unsigned int c)
	{
		return false;
	}

	bool Player::OnMouseButton(int button, int action, int modifier)
	{
		return false;
	}

	bool Player::OnMouseMove(double x, double y)
	{
		return false;
	}

	bool Player::Initialize(death::GameInstance * in_game_instance)
	{
		if (!GameInstanceEntity::Initialize(in_game_instance))
			return false;

		Game * game = GetGame();
		if (game != nullptr)
			life_count = game->initial_life;
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

	bool Player::DoTick(double delta_time)
	{
		Game * game = GetGame();

		// tick the invulnerability
		TickInvulnerability(delta_time);
		// remove previous frame cached input
		ResetCachedInputs();
		// cache values for stick displacement
		CacheKeyboardPlayerDisplacementInputs();
		// cache values for stick displacement
		CacheGamepadPlayerDisplacementInputs();

		// tick player displacement
		if (game != nullptr && !game->IsFreeCameraMode() && GetGame()->IsPlaying())
		{
			// transform keyboard inputs as stick input
			HandleKeyboardInputs(delta_time);
			// handle gamepad inputs
			HandleGamepadInputs(delta_time);
			// tick the player displacement
			TickPlayerDisplacement(delta_time);
		}
		return true;
	}

	void Player::TickInvulnerability(double delta_time)
	{
		invulnerability_timer -= (float)delta_time;
		if (invulnerability_timer < 0.0f)
			invulnerability_timer = 0.0f;
	}

	void Player::TickPlayerDisplacement(double delta_time)
	{

	}

	void Player::CacheKeyboardPlayerDisplacementInputs()
	{
		// get the data
		Game * game = GetGame();
		if (game == nullptr)
			return;
		// get the GLFW object to get the key state
		GLFWwindow * glfw_window = game->GetGLFWWindow();
		if (glfw_window == nullptr)
			return;

		// test whether the stick position can be overriden
		glm::vec2 simulated_stick = glm::vec2(0.0f, 0.0f);

		if (glfwGetKey(glfw_window, GLFW_KEY_LEFT))
			simulated_stick.x -= 1.0f;
		if (glfwGetKey(glfw_window, GLFW_KEY_RIGHT))
			simulated_stick.x += 1.0f;

		if (glfwGetKey(glfw_window, GLFW_KEY_DOWN))
			simulated_stick.y += 1.0f;
		if (glfwGetKey(glfw_window, GLFW_KEY_UP))
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
			chaos::Application::SetApplicationInputMode(chaos::InputMode::Gamepad);

		// cache the LEFT stick position (it is aliases with the DPAD)
		glm::vec2 lsp = gamepad_data->GetXBOXStickDirection(chaos::MyGLFW::XBOX_LEFT_AXIS);
		if (glm::length2(lsp) > 0.0f)
			left_stick_position = lsp;
		else
		{
			if (gamepad_data->IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_LEFT, false))
				left_stick_position.x = -1.0f;
			else if (gamepad_data->IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_RIGHT, false))
				left_stick_position.x = 1.0f;

			if (gamepad_data->IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_UP, false))
				left_stick_position.y = -1.0f;
			else if (gamepad_data->IsButtonPressed(chaos::MyGLFW::XBOX_BUTTON_DOWN, false))
				left_stick_position.y = 1.0f;
		}

		// cache the RIGHT stick position
		glm::vec2 rsp = gamepad_data->GetXBOXStickDirection(chaos::MyGLFW::XBOX_RIGHT_AXIS);
		if (glm::length2(rsp) > 0.0f)
			right_stick_position = rsp;

		// cache the TRIGGERS
		left_trigger  = gamepad_data->GetAxisValue(chaos::MyGLFW::XBOX_LEFT_TRIGGER);
		right_trigger = gamepad_data->GetAxisValue(chaos::MyGLFW::XBOX_RIGHT_TRIGGER);

		// get data
		Game * game = GetGame();
		if (game == nullptr)
			return;
		// maybe a game/pause resume
		if ((gamepad_data->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_SELECT) == chaos::MyGLFW::BUTTON_BECOME_PRESSED) || 
			(gamepad_data->GetButtonChanges(chaos::MyGLFW::XBOX_BUTTON_START) == chaos::MyGLFW::BUTTON_BECOME_PRESSED))
			game->RequireTogglePause();
	}

	void Player::HandleKeyboardInputs(double delta_time)
	{

	}

	void Player::ResetCachedInputs()
	{
		left_stick_position  = glm::vec2(0.0f, 0.0f);
		right_stick_position = glm::vec2(0.0f, 0.0f);
		left_trigger  = 0.0f;
		right_trigger = 0.0f;
	}

	void Player::HandleGamepadInputs(double delta_time)
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

	void Player::InternalHandleGamepadInputs(double delta_time, chaos::MyGLFW::GamepadData const * gamepad_data)
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
		if (increment)
			life_count += in_life;
		else
			life_count = in_life;
	}

	chaos::ParticleDefault::Particle * Player::GetPlayerParticle()
	{
		return chaos::ParticleTools::GetParticle(GetPlayerAllocation(), 0);
	}

	chaos::ParticleDefault::Particle const * Player::GetPlayerParticle() const
	{
		return chaos::ParticleTools::GetParticle(GetPlayerAllocation(), 0);
	}

	glm::vec2 Player::GetPlayerPosition() const
	{
		return chaos::ParticleTools::GetParticlePosition(GetPlayerAllocation(), 0);
	}

	chaos::box2 Player::GetPlayerBox() const
	{
		return chaos::ParticleTools::GetParticleBox(GetPlayerAllocation(), 0);
	}

	bool Player::SetPlayerPosition(glm::vec2 const & position)
	{
		return chaos::ParticleTools::SetParticlePosition(GetPlayerAllocation(), 0, position);
	}
	bool Player::SetPlayerBox(chaos::box2 const & box)
	{
		return chaos::ParticleTools::SetParticleBox(GetPlayerAllocation(), 0, box);
	}

	bool Player::DoSaveIntoCheckpoint(PlayerCheckpoint * checkpoint) const
	{
		checkpoint->life_count = life_count;
        checkpoint->health = health;
        checkpoint->max_health = max_health;
		checkpoint->score = score;
		checkpoint->player_box = GetPlayerBox();
		return true;
	}

	bool Player::DoLoadFromCheckpoint(PlayerCheckpoint const * checkpoint)
	{
		life_count = checkpoint->life_count;
        health     = checkpoint->health;
        max_health = checkpoint->max_health;
		score      = checkpoint->score;
		SetPlayerBox(checkpoint->player_box);
		return true;
	}

	void Player::OnLifeLost()
	{
        health = max_health;
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
				invulnerability_timer = invulnerability_duration;
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
			// 2 seconds force feedback effect
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
                        if (glfwGetKey(glfw_window, keyboard_buttons[i]) != GLFW_RELEASE)
                            return true;
            }
        }

        // gamepad input
        if (gamepad_button >= 0)
            if (gamepad != nullptr && gamepad->IsButtonPressed(gamepad_button))
                return true;
        return false;
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

