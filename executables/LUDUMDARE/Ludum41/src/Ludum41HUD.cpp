#include "Ludum41HUD.h"
#include "Ludum41Game.h"

bool LudumPlayingHUD::DoTick(double delta_time)
{
	// call super method
	PlayingHUD::DoTick(delta_time);
	
	LudumGame const * ludum_game = dynamic_cast<LudumGame const *>(game);
	if (ludum_game != nullptr)
	{
		UpdateComboParticles(ludum_game);
		UpdateLifeParticles(ludum_game);
	}
	return true;
}

void LudumPlayingHUD::UpdateComboParticles(LudumGame const * ludum_game)
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

void LudumPlayingHUD::UpdateLifeParticles(LudumGame const * ludum_game)
{
	int current_life = ludum_game->GetCurrentLife();
	if (current_life != cached_life_value)
	{
		if (current_life < 0)
			UnregisterParticles(death::GameHUDKeys::LIFE_ID);
		else
		{


		}
		cached_life_value = current_life;
	}
}

bool LudumPlayingHUD::InitializeHUD()
{
	// call super method
	if (!death::PlayingHUD::InitializeHUD())
		return false;
	// create a layer for the life bar
	LudumGame * ludum_game = dynamic_cast<LudumGame *>(game);
	if (ludum_game != nullptr)
	{
		int render_order = -1;
		ParticleLifeObjectTrait life_trait;
		life_trait.game = ludum_game;
		particle_manager->AddLayer<ParticleLifeObjectTrait>(render_order, death::GameHUDKeys::LIFE_LAYER_ID, "gameobject", life_trait);
	}
	return true;
}
