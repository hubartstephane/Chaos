#include <chaos/MathTools.h>

#include <death/PlayerDisplacementComponent.h>
#include <death/GameGettersImpl.h>
#include <death/GameFramework.h>

namespace death
{
	DEATH_GAMEGETTERS_IMPLEMENT(PlayerDisplacementComponent);

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

}; // namespace death