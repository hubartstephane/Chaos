#include "Ludum45HUD.h"
#include "Ludum45Game.h"
#include "Ludum45Level.h"
#include "Ludum45Player.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45GameInstance.h"

#include <death/GameHUDComponent.h>

DEATH_GAMEFRAMEWORK_IMPLEMENT_HUD(Ludum);


namespace death
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(UPGRADE_ID);
		CHAOS_DECLARE_TAG(SHROUDLIFE_ID);
	};
};

// ====================================================================
// LudumPlayingHUD
// ====================================================================

bool LudumPlayingHUD::FillHUDContent()
{
	if (!death::PlayingHUD::FillHUDContent())
		return false;	

	RegisterComponent(death::GameHUDKeys::SHROUDLIFE_ID, new GameHUDShroudLifeComponent(), "LifeGrid_5x2");
	RegisterComponent(death::GameHUDKeys::LIFE_ID, new death::GameHUDLifeComponent());
	RegisterComponent(death::GameHUDKeys::UPGRADE_ID, new GameHUDUpgradeComponent());
	RegisterComponent(death::GameHUDKeys::LEVEL_TITLE_ID, new death::GameHUDLevelTitleComponent());
	
	//RegisterComponent(death::GameHUDKeys::NOTIFICATION_ID, new death::GameHUDNotificationComponent());

	return true;
}


bool LudumPlayingHUD::CreateHUDLayers()
{
	// call super method
	if (!death::PlayingHUD::CreateHUDLayers())
		return false;


	LudumGame * ludum_game = GetLudumGame();
	if (ludum_game != nullptr)
	{
		// create a layer for the life bar
		int render_order = 30;
		particle_manager->AddLayer<ParticleLifeTrait>(++render_order, death::GameHUDKeys::LIFE_LAYER_ID, "gameobject");
		// create a layer for the shroudlife bar
		ParticleShroudLifeTrait::LayerTrait shroud_trait;
		shroud_trait.game = ludum_game;
		particle_manager->AddLayer<ParticleShroudLifeTrait>(++render_order, death::GameHUDKeys::SHROUDLIFE_ID, "gameobject", shroud_trait);
	}

	return true;
}

// ====================================================================
// GameHUDLevelTitleComponent
// ====================================================================

GameHUDUpgradeComponent::GameHUDUpgradeComponent(chaos::TagType in_layer_id) :
	death::GameHUDCacheValueComponent<std::string>("%s", std::string(), in_layer_id) 
{
	generator_params.line_height = 60.0f;
	generator_params.font_info_name = "normal";
	generator_params.position = glm::vec2(-40.0f, 100.0f);
	generator_params.hotpoint_type = chaos::Hotpoint::BOTTOM_RIGHT;
}

bool GameHUDUpgradeComponent::UpdateCachedValue(bool & destroy_allocation) 
{
	LudumPlayingHUD const * playing_hud = auto_cast(hud);
	if (playing_hud != nullptr)
	{
		LudumPlayer const * ludum_player = playing_hud->GetLudumPlayer(0);
		if (ludum_player != nullptr)
		{
			std::string upgrade_string = ludum_player->GetPlayerUpgradeString();
			if (upgrade_string.length() > 0)
			{
				if (cached_value == upgrade_string)
					return false;
				cached_value = upgrade_string;
				return true;
			}

		}
	}

	destroy_allocation = true;
	cached_value = std::string();
	return true;
}

std::string GameHUDUpgradeComponent::FormatText() const 
{
	return cached_value;	
}



// ====================================================================
// GameHUDShroudLifeComponent
// ====================================================================

void GameHUDShroudLifeComponent::OnInsertedInHUD(char const * bitmap_name)
{
	if (allocations == nullptr)
	{
		allocations = hud->GetGameParticleCreator().CreateParticles(bitmap_name, 1, death::GameHUDKeys::SHROUDLIFE_ID);
		if (allocations == nullptr)
			return;
	}


	chaos::TagType in_layer_id = death::GameHUDKeys::SHROUDLIFE_ID;
	bitmap_name = bitmap_name;

}

bool GameHUDShroudLifeComponent::InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
{
	if (!GameHUDSingleAllocationComponent::InitializeFromConfiguration(json, config_path))
		return true;


	chaos::JSONTools::GetEnumAttribute(json, "hotpoint_type", chaos::Hotpoint::hotpoint_encoding, hotpoint_type);
	chaos::JSONTools::GetAttribute(json, "position", position);
	chaos::JSONTools::GetAttribute(json, "particle_size", particle_size);


	return true;
}