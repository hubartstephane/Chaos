#include <chaos/Chaos.h>

#include "Ludum45HUD.h"
#include "Ludum45Game.h"
#include "Ludum45Level.h"
#include "Ludum45Player.h"
#include "Ludum45LevelInstance.h"
#include "Ludum45GameInstance.h"

namespace chaos
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
	if (!chaos::PlayingHUD::FillHUDContent())
		return false;	

	RegisterComponent(chaos::GameHUDKeys::SHROUDLIFE_ID, new GameHUDShroudLifeComponent(), "LifeGrid");
	RegisterComponent(chaos::GameHUDKeys::LIFE_ID, new chaos::GameHUDLifeComponent());
	RegisterComponent(chaos::GameHUDKeys::UPGRADE_ID, new GameHUDUpgradeComponent());
	RegisterComponent(chaos::GameHUDKeys::LEVEL_TITLE_ID, new chaos::GameHUDLevelTitleComponent());
	
	//RegisterComponent(chaos::GameHUDKeys::NOTIFICATION_ID, new chaos::GameHUDNotificationComponent());

	return true;
}

// ====================================================================
// GameHUDLevelTitleComponent
// ====================================================================

GameHUDUpgradeComponent::GameHUDUpgradeComponent(chaos::TagType in_layer_id) :
	chaos::GameHUDCacheValueComponent<std::string>("%s", std::string(), in_layer_id) 
{
	generator_params.line_height = 60.0f;
	generator_params.font_info_name = "normal";
	generator_params.position = glm::vec2(-40.0f, 100.0f);
	generator_params.hotpoint = chaos::Hotpoint::BOTTOM_RIGHT;
}

bool GameHUDUpgradeComponent::UpdateCachedValue(bool & destroy_allocation) 
{
	LudumPlayingHUD const * playing_hud = auto_cast(hud);
	if (playing_hud != nullptr)
	{
		LudumPlayer const * ludum_player = playing_hud->GetPlayer(0);
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
#if 0

	chaos::BitmapAtlas::BitmapInfo const * bitmap_info = hud->GetGameParticleCreator().FindBitmapInfo(bitmap_name);
	if (bitmap_info == nullptr)
		return;

	if (allocations == nullptr)
	{
		allocations = hud->GetGameParticleCreator().SpawnParticles(chaos::GameHUDKeys::SHROUDLIFE_ID, bitmap_name, 1, true);
		if (allocations == nullptr)
			return;
	}




// shuludum : raw copy from GameHUDLifeComponent. Something better has to be found

	// shuludum    XXX : bitmap_info->   devrait avoir une fonction qui retourne la VRAIE taille d'une animation et pas la taille de la grille complete => GetAnimationLayout(0, ...)

	chaos::BitmapAtlas::BitmapLayout layout = bitmap_info->GetAnimationLayout(0, chaos::WrapMode::CLAMP);

	
	float bw = (float)layout.width;
	float bh = (float)layout.height;


	glm::vec2 particle_final_size = particle_size;
	if (particle_final_size.x <= 0.0f || particle_final_size.y <= 0.0f)
	{
		if (particle_final_size.x <= 0.0f && particle_final_size.y <= 0.0f) // both are invalid
			particle_final_size = glm::vec2(bw, bh);
		else if (particle_final_size.x <= 0.0f)
			particle_final_size.x = particle_final_size.y * bw / bh;
		else
			particle_final_size.y = particle_final_size.x * bh / bw;
	}


	glm::vec2 screen_ref = GetCanvasBoxCorner(GetGame()->GetCanvasBox(), hotpoint);
	glm::vec2 particle_position = chaos::ConvertHotpoint(screen_ref + position, particle_final_size, hotpoint, chaos::Hotpoint::CENTER);


	//glm::vec2 particle_position = whole_particle_ref;

	// update the particles members
	chaos::ParticleAccessor<ParticleShroudLife> particles = allocations->GetParticleAccessor();
	for (size_t i = 0 ; i < particles.GetDataCount() ; ++i)
	{
		ParticleShroudLife & p = particles[i];

		p.bitmap_info = bitmap_info;
		p.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		p.bounding_box.position = particle_position;
		p.bounding_box.half_size = 0.5f * particle_final_size;
	
	}
#endif
	
}

bool GameHUDShroudLifeComponent::InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
{
	if (!GameHUDSingleAllocationComponent::InitializeFromConfiguration(json, config_path))
		return true;

	chaos::JSONTools::GetAttribute(json, "hotpoint", hotpoint);
	chaos::JSONTools::GetAttribute(json, "position", position);
	chaos::JSONTools::GetAttribute(json, "particle_size", particle_size);

	return true;
}