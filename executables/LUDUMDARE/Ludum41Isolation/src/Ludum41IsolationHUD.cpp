#include "Ludum41IsolationHUD.h"
#include "Ludum41IsolationGame.h"
#include "Ludum41IsolationGameInstance.h"
#include "Ludum41IsolationLevel.h"
#include "Ludum41IsolationLevelInstance.h"
#include "Ludum41IsolationPlayer.h"

#include <death/GameHUD.h>
#include <death/GameHUDComponent.h>

// ====================================================================
// GameHUDComboComponent
// ====================================================================

GameHUDComboComponent::GameHUDComboComponent(chaos::TagType in_layer_id) :
	death::GameHUDCacheValueComponent<int>("Combo: %d x", -1, in_layer_id) 
{
	generator_params.line_height = 60.0f;
	generator_params.font_info_name = "normal";
	generator_params.position = glm::vec2(20.0f, -80.0f);
	generator_params.hotpoint = chaos::Hotpoint::TOP_LEFT;
}

bool GameHUDComboComponent::UpdateCachedValue(bool & destroy_allocation)
{
	LudumPlayingHUD const * playing_hud = auto_cast(hud);
	if (playing_hud != nullptr)
	{
		LudumGameInstance const * ludum_game_instance = playing_hud->GetGameInstance();
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
	if (!death::PlayingHUD::FillHUDContent())
		return false;
	RegisterComponent(death::GameHUDKeys::COMBO_ID, new GameHUDComboComponent());
	RegisterComponent(death::GameHUDKeys::LIFE_ID, new death::GameHUDLifeComponent());
	return true;
}

int LudumPlayingHUD::CreateHUDLayers()
{
	// call super method
	int render_order = death::PlayingHUD::CreateHUDLayers();
	if (render_order < 0)
		return render_order;
	// create a layer for the life bar
	LudumGame * ludum_game = GetGame();
	if (ludum_game != nullptr)
	{
		ParticleLifeObjectTrait life_trait;
		life_trait.game = ludum_game;
		particle_manager->AddLayer<ParticleLifeObjectTrait>(render_order++, death::GameHUDKeys::LIFE_LAYER_ID, "gameobject", life_trait);
	}
	return render_order;
}



