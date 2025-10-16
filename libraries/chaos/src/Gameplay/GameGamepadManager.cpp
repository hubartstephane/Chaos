#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	GameGamepadManager::GameGamepadManager(Game* in_game, GamepadInputUpdateSettings const& in_update_settings):
		GamepadManager(in_update_settings),
		game(in_game)
	{
		assert(in_game != nullptr);
	}

	bool GameGamepadManager::DoPollGamepad(PhysicalGamepad * physical_gamepad)
	{
		if (game != nullptr)
			return game->OnPhysicalGamepadInput(physical_gamepad);
		return true;
	}

}; // namespace chaos

