#include "Ludum41HUD.h"
#include "Ludum41Game.h"

bool LudumPlayingHUD::DoTick(double delta_time)
{
	// call super method
	PlayingHUD::DoTick(delta_time);
	// populate the HUD
	LudumGame const * ludum_game = dynamic_cast<LudumGame const *>(game);
	if (ludum_game != nullptr)
	{
		int current_combo = ludum_game->GetCurrentComboMultiplier();
		if (current_combo != cached_combo_value)
		{
			if (current_combo < 2)
				UnregisterParticles(death::GameHUDKeys::COMBO_ID);
			else
				RegisterParticles(death::GameHUDKeys::COMBO_ID, game->CreateScoringText("Combo : %d x", current_combo, 60.0f, particle_manager.get()));

			cached_combo_value = current_combo;
		}
	}
	return true;
}
