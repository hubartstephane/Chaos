#include "Ludum41HUD.h"

void LudumPlayingHUD::SetComboValue(death::Game * game, int new_combo)
{
	if (new_combo < 2)
	{
		cached_combo_value = new_combo;
		UnregisterParticles(death::GameHUDKeys::COMBO_ID);
		return;
	}
	CacheAndCreateScoreAllocation(game, new_combo, "Combo : %d x", 60.0f, cached_combo_value, death::GameHUDKeys::COMBO_ID);
}
