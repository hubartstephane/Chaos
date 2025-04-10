#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool GameGamepadManager::DoPoolGamepad(PhysicalGamepad * physical_gamepad)
	{
		if (game != nullptr)
			return game->OnPhysicalGamepadInput(physical_gamepad);
		return true;
	}

}; // namespace chaos

