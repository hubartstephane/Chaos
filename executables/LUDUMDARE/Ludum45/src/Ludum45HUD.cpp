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

	chaos::BitmapAtlas::BitmapInfo const * bitmap_info = hud->GetGameParticleCreator().FindBitmapInfo(bitmap_name);
	if (bitmap_info == nullptr)
		return;

	if (allocations == nullptr)
	{
		allocations = hud->GetGameParticleCreator().CreateParticles(bitmap_name, 1, death::GameHUDKeys::SHROUDLIFE_ID);
		if (allocations == nullptr)
			return;
	}




// shuludum : raw copy from GameHUDLifeComponent. Something better has to be found



	glm::vec2 particle_final_size = particle_size;
	if (particle_final_size.x <= 0.0f || particle_final_size.y <= 0.0f)
	{
		if (particle_final_size.x <= 0.0f && particle_final_size.y <= 0.0f) // both are invalid
			particle_final_size = glm::vec2(bitmap_info->width, bitmap_info->height);
		else if (particle_final_size.x <= 0.0f)
			particle_final_size.x = particle_final_size.y * bitmap_info->width / bitmap_info->height;
		else
			particle_final_size.y = particle_final_size.x * bitmap_info->height / bitmap_info->width;
	}


	glm::vec2 screen_ref = GetCanvasBoxCorner(GetGame()->GetCanvasBox(), hotpoint_type);
	glm::vec2 whole_particle_ref = chaos::Hotpoint::Convert(screen_ref + position, particle_final_size, hotpoint_type, chaos::Hotpoint::BOTTOM_LEFT);


	glm::vec2 particle_position = whole_particle_ref;

	// update the particles members
	chaos::ParticleAccessor<ParticleShroudLife> particles = allocations->GetParticleAccessor<ParticleShroudLife>();
	for (size_t i = 0 ; i < particles.GetCount() ; ++i)
	{
		ParticleShroudLife & p = particles[i];

		p.bitmap_info = bitmap_info;
		p.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		p.bounding_box.position = chaos::Hotpoint::Convert(particle_position, particle_final_size, chaos::Hotpoint::BOTTOM_LEFT, chaos::Hotpoint::CENTER);
		p.bounding_box.half_size = 0.5f * particle_final_size;
	
	}
	
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