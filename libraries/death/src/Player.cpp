#include <death/Player.h>

namespace death
{
	void Player::SetPlayerAllocation(chaos::ParticleAllocation * in_allocation)
	{
		player_allocations = in_allocation;
	}

}; // namespace death
