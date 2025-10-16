#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	GameGamepadManager::GameGamepadManager(Game* in_game, GamepadInputFilterSettings const& in_filter_settings):
		GamepadManager(in_filter_settings),
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

