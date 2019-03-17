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
	LudumPlayingHUD const * playing_hud = dynamic_cast<LudumPlayingHUD const*>(hud);
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

void GameHUDComboComponent::TweakTextGeneratorParams(chaos::ParticleTextGenerator::GeneratorParams & params, chaos::box2 const & view_box)
{
	int hotpoint = chaos::Hotpoint::TOP_LEFT;

	glm::vec2 corner = GetViewBoxCorner(view_box, hotpoint);
	params.hotpoint_type = hotpoint;
	params.position.x = corner.x + 20.0f;
	params.position.y = corner.y - 60.0f;
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

bool LudumPlayingHUD::CreateHUDLayers()
{
	// call super method
	if (!death::PlayingHUD::CreateHUDLayers())
		return false;
	// create a layer for the life bar
	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game != nullptr)
	{
		int render_order = -1;
		ParticleLifeObjectTrait life_trait;
		life_trait.game = ludum_game;
		particle_manager->AddLayer<ParticleLifeObjectTrait>(++render_order, death::GameHUDKeys::LIFE_LAYER_ID, "gameobject", life_trait);
	}
	return true;
}



