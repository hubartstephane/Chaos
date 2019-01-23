#include <death/Player.h>

namespace death
{

	Player::Player(class GameInstance * in_game_instance) :
		game_instance(in_game_instance)
	{
		assert(in_game_instance != nullptr);
	}

	Game * Player::GetGame()
	{
		if (game_instance == nullptr)
			return nullptr;
		return game_instance->GetGame();
	}

	Game const * Player::GetGame() const
	{
		if (game_instance == nullptr)
			return nullptr;
		return game_instance->GetGame();
	}

	void Player::SetPlayerAllocation(chaos::ParticleAllocation * in_allocation)
	{
		player_allocations = in_allocation;
	}

	bool Player::OnKeyEvent(int key, int action)
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

	bool Player::CapturePhysicalGamepad(chaos::MyGLFW::PhysicalGamepad * in_physical_gamepad)
	{
		// if no entry or the device is already owned by someone, exit
		if (in_physical_gamepad == nullptr || in_physical_gamepad->IsAllocated())
			return false;
		// if we already have a device, ignore
		if (gamepad != nullptr)
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
		// remove previous frame cached input
		ResetCachedInputs();
		// transform keyboard inputs as stick input
		HandleKeyboardInputs(delta_time);
		// handle gamepad inputs
		HandleGamepadInputs(delta_time);
		// tick player displacement
		TickPlayerDisplacement(delta_time);

		return true;
	}

	void Player::TickPlayerDisplacement(double delta_time)
	{

	}

	void Player::HandleKeyboardInputs(double delta_time)
	{
		// get the data
		Game * game = GetGame();
		if (game == nullptr)
			return;

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
			left_stick_position = game->gamepad_sensitivity * simulated_stick;
	}

	void Player::ResetCachedInputs()
	{
		left_stick_position = glm::vec2(0.0f, 0.0f);
		right_stick_position = glm::vec2(0.0f, 0.0f);
	}

	void Player::HandleGamepadInputs(double delta_time)
	{
		// early exit
		if (gamepad == nullptr)
			return;

		chaos::MyGLFW::GamepadData const * gamepad_data = gamepad->GetGamepadData();
		if (gamepad_data == nullptr)
			return;

		// create a copy of gamedata input to be given back to game instance
		chaos::MyGLFW::GamepadData gpd = *gamepad_data;

		// Handle the inputs as we want, modifying the object by consuming data inside
		InternalHandleGamepadInputs(delta_time, gpd);

		// give remaining input back to game instance
		if (game_instance != nullptr && (gpd.IsAnyAction() || gpd.IsAnyAxisAction()))
			game_instance->HandlePlayerGamepadInput(delta_time, gpd);
	}

	void Player::InternalHandleGamepadInputs(double delta_time, chaos::MyGLFW::GamepadData & gpd)
	{
		// get data
		Game * game = GetGame();
		if (game == nullptr)
			return;

		// cache the stick position
		float gamepad_sensitivity = game->gamepad_sensitivity;

		glm::vec2 lsp = gpd.GetXBOXStickDirection(chaos::MyGLFW::XBOX_LEFT_AXIS);
		if (glm::length2(lsp) > 0.0f)
			left_stick_position = gamepad_sensitivity * lsp;
		else
		{
			if (gpd.IsButtonPressedAndConsume(chaos::MyGLFW::XBOX_BUTTON_LEFT, true))
				left_stick_position.x = -gamepad_sensitivity * 1.0f;
			else if (gpd.IsButtonPressedAndConsume(chaos::MyGLFW::XBOX_BUTTON_RIGHT, true))
				left_stick_position.x = gamepad_sensitivity * 1.0f;

			if (gpd.IsButtonPressedAndConsume(chaos::MyGLFW::XBOX_BUTTON_UP, true))
				left_stick_position.y = -gamepad_sensitivity * 1.0f;
			else if (gpd.IsButtonPressedAndConsume(chaos::MyGLFW::XBOX_BUTTON_DOWN, true))
				left_stick_position.y = gamepad_sensitivity * 1.0f;
		}

		glm::vec2 rsp = gpd.GetXBOXStickDirection(chaos::MyGLFW::XBOX_RIGHT_AXIS);
		if (glm::length2(rsp) > 0.0f)
			right_stick_position = gamepad_sensitivity * rsp;
	}

	void Player::SetScore(int in_score, bool increment)
	{
		if (increment)
			score += in_score;
		else
			score = in_score;	
	}
	
	PlayerGamepadCallbacks::PlayerGamepadCallbacks(Player * in_player):
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
