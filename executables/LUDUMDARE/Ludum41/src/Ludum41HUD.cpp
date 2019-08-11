#include "Ludum41HUD.h"
#include "Ludum41Game.h"
#include "Ludum41GameInstance.h"
#include "Ludum41Level.h"
#include "Ludum41LevelInstance.h"
#include "Ludum41Player.h"

#include <death/GameHUD.h>
#include <death/GameHUDComponent.h>

DEATH_GAMEFRAMEWORK_IMPLEMENT_HUD(Ludum);


// ====================================================================
// GameHUDComboComponent
// ====================================================================

bool GameHUDComboComponent::UpdateCachedValue(bool & destroy_allocation)
{
	LudumPlayingHUD const * playing_hud = auto_cast(hud);
	if (playing_hud != nullptr)
	{
		LudumGameInstance const * ludum_game_instance = playing_hud->GetLudumGameInstance();
		if (ludum_game_instance != nullptr)
		{
			int current_combo = ludum_game_instance->GetCurrentComboMultiplier();
			if (current_combo < 2)
				destroy_allocation = true;
			if (current_combo != cached_value)
			{
				cached_value = current_combo;
				return true;
			}
		}
	}
	return false;
}

// ====================================================================
// LudumPlayingHUD
// ====================================================================

bool LudumPlayingHUD::FillHUDContent()
{

	// shuwww


	if (!death::PlayingHUD::FillHUDContent())
		return false;
	RegisterComponent(death::GameHUDKeys::COMBO_ID, new GameHUDComboComponent("normal", 60.0f, glm::vec2(20.0, -80.0f), chaos::Hotpoint::TOP_LEFT, death::GameHUDKeys::TEXT_LAYER_ID));
	RegisterComponent(death::GameHUDKeys::LIFE_ID, new death::GameHUDLifeComponent());
	return true;
}

bool LudumPlayingHUD::CreateHUDLayers()
{


	// shuwww


	// call super method
	if (!death::PlayingHUD::CreateHUDLayers())
		return false;
	// create a layer for the life bar
	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game != nullptr)
	{
		int render_order = -1;
		ParticleLifeObjectTrait::LayerTrait life_trait;
		life_trait.game = ludum_game;
		particle_manager->AddLayer<ParticleLifeObjectTrait>(++render_order, death::GameHUDKeys::LIFE_LAYER_ID, "gameobject", life_trait);
	}
	return true;
}



