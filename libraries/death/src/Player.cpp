#include <death/Player.h>

namespace death
{

	Player::Player(class GameInstance * in_game_instance):
		game_instance(in_game_instance)
	{
		assert(in_game_instance != nullptr);
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

}; // namespace death
