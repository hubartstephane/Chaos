#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	CHAOS_IMPLEMENT_GAMEPLAY_GETTERS(PlayerPawn);

	// =============================================
	// PlayerPawn
	// =============================================

	PlayerPawn::PlayerPawn()
	{
		SetParticleMaster(true);
	}

	bool PlayerPawn::Initialize(Player* in_player)
	{
		// ensure valid arguments and not already initialized
		assert(in_player != nullptr);
		assert(player == nullptr);
		player = in_player;
		return true;
	}
	
}; // namespace chaos

