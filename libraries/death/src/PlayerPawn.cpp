#include <death/PlayerPawn.h>
#include <death/GameCheckpoint.h>

#include <death/GameGettersImpl.h>

namespace death
{
	DEATH_GAMEGETTERS_IMPLEMENT(PlayerPawn);

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
	
}; // namespace death

