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

	RegisterComponent(chaos::GameHUDKeys::SHROUDLIFE_ID, new GameHUDShroudLifeComponent("LifeGrid"));
	RegisterComponent(chaos::GameHUDKeys::LIFE_ID, new chaos::GameHUDLifeComponent());
	RegisterComponent(chaos::GameHUDKeys::UPGRADE_ID, new GameHUDUpgradeComponent());
	RegisterComponent(chaos::GameHUDKeys::LEVEL_TITLE_ID, new chaos::GameHUDLevelTitleComponent());
	
	//RegisterComponent(chaos::GameHUDKeys::NOTIFICATION_ID, new chaos::GameHUDNotificationComponent());

	return true;
}

// ====================================================================
// GameHUDLevelTitleComponent
// ====================================================================

GameHUDUpgradeComponent::GameHUDUpgradeComponent() :
	chaos::GameHUDCacheValueComponent<std::string>("%s", std::string()) 
{
	generator_params.line_height = 60.0f;
	generator_params.font_info_name = "normal";
	generator_params.position = glm::vec2(-40.0f, 100.0f);
	generator_params.hotpoint = chaos::Hotpoint::BOTTOM_RIGHT;
}

bool GameHUDUpgradeComponent::UpdateCachedValue(bool & destroy_mesh)
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

	destroy_mesh = true;
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

bool GameHUDShroudLifeComponent::DoUpdateGPUResources(chaos::GPURenderer* renderer)
{
	LudumPlayer* ludum_player = GetPlayer(0);
	if (ludum_player == nullptr)
		return false;


	float health = ludum_player->GetHealth();
	float max_health = ludum_player->GetMaxHealth();

	chaos::GPUDrawInterface<chaos::VertexDefault> DI(nullptr);

	chaos::BitmapAtlas::BitmapInfo const* bitmap_info = DI.FindBitmapInfo(bitmap_name);
	if (bitmap_info == nullptr)
		return false;

	float image_count = (float)bitmap_info->GetAnimationImageCount();
	int index = (int)(image_count * (1.0 - (health / max_health)));

	if (index == cached_value)
		return true;

	chaos::BitmapAtlas::BitmapLayout layout = bitmap_info->GetAnimationLayout(index, chaos::WrapMode::CLAMP);

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

	chaos::ParticleDefault particle;

	particle.texcoords = layout.GetTexcoords();
	particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	particle.bounding_box.position = particle_position;
	particle.bounding_box.half_size = 0.5f * particle_final_size;

	ParticleToPrimitives(particle, DI);

	mesh = DI.ExtractMesh();

	cached_value = index;

	return true;
}

bool GameHUDShroudLifeComponent::InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
{
	if (!GameHUDMeshComponent::InitializeFromConfiguration(json, config_path))
		return true;

	chaos::JSONTools::GetAttribute(json, "hotpoint", hotpoint);
	chaos::JSONTools::GetAttribute(json, "position", position);
	chaos::JSONTools::GetAttribute(json, "particle_size", particle_size);

	return true;
}