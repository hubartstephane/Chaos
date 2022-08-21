#include "chaos/Chaos.h"

namespace chaos
{
	CHAOS_IMPLEMENT_GAMEPLAY_GETTERS(PlayerDisplacementComponent);

	// =============================================
	// PlayerDisplacementComponent
	// =============================================

	bool PlayerDisplacementComponent::Initialize(Player* in_player)
	{
		// ensure valid arguments and not already initialized
		assert(in_player != nullptr);
		assert(player == nullptr);
		player = in_player;
		return true;
	}

}; // namespace chaos