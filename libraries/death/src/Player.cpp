#include <death/Player.h>

namespace death
{

	Player::Player(class GameInstance * in_game_instance):
		game_instance(in_game_instance)
	{
		assert(in_game_instance != nullptr);
	}
	
	Game * Player::GetGame()
    {
        if (game_instance == nullptr)
            return game_instance;
        return game_instance->GetGame();
    }

	Game const * Player::GetGame() const
    {
        if (game_instance == nullptr)
            return game_instance;
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
        gamepad = in_physical_gamepad->CapturePhysicalGamepad();
        if (gamepad == nullptr)
            return false;
		return true;
	}

}; // namespace death
