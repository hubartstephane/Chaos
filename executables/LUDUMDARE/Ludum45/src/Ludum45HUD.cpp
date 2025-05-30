#include "Ludum45PCH.h"
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
	RegisterComponent(CreateHUDComponent<GameHUDShroudLifeComponent>(chaos::GameHUDKeys::SHROUDLIFE_ID, "LifeGrid"));
	RegisterComponent(CreateHUDComponent<chaos::GameHUDLifeComponent>(chaos::GameHUDKeys::LIFE_ID));
	RegisterComponent(CreateHUDComponent<GameHUDUpgradeComponent>(chaos::GameHUDKeys::UPGRADE_ID));
	RegisterComponent(CreateHUDComponent<chaos::GameHUDLevelTitleComponent>(chaos::GameHUDKeys::LEVEL_TITLE_ID));
	return true;
}

// ====================================================================
// GameHUDLevelTitleComponent
// ====================================================================

GameHUDUpgradeComponent::GameHUDUpgradeComponent() :
	chaos::GameHUDCacheValueTextComponent<std::string>("%s")
{
	generator_params.line_height = 60.0f;
	generator_params.font_info_name = "normal";
	generator_params.position = glm::vec2(-40.0f, 100.0f);
	generator_params.hotpoint = chaos::Hotpoint::BOTTOM_RIGHT;
}

bool GameHUDUpgradeComponent::QueryValue(std::string & result) const
{
	LudumPlayer const* ludum_player = GetPlayer(0);
	if (ludum_player == nullptr)
		return false;
	result = ludum_player->GetPlayerUpgradeString();
	return true;
}

void GameHUDUpgradeComponent::UpdateMesh()
{
	SetText(chaos::StringTools::Printf(text.c_str(), cached_value.c_str()).c_str());
}

// ====================================================================
// GameHUDShroudLifeComponent
// ====================================================================

bool GameHUDShroudLifeComponent::QueryValue(GameHUDHealthInfo & result) const
{
	LudumPlayer const* ludum_player = GetPlayer(0);
	if (ludum_player == nullptr)
		return false;
	result = { ludum_player->GetHealth(), ludum_player->GetMaxHealth() };
	return true;
}

void GameHUDShroudLifeComponent::UpdateMesh()
{
	chaos::GPUDrawInterface<chaos::VertexDefault> DI(GetGPUDevice(), nullptr);

	chaos::AtlasBitmapInfo const* bitmap_info = DI.FindBitmapInfo(bitmap_name);
	if (bitmap_info == nullptr)
		return;

	float image_count = (float)bitmap_info->GetAnimationImageCount();
	int index = (int)(image_count * (1.0 - (cached_value.health / cached_value.max_health)));

	chaos::AtlasBitmapLayout layout = bitmap_info->GetAnimationLayout(index, chaos::WrapMode::CLAMP);

	glm::vec2 final_particle_size = layout.ApplyRatioToSize(particle_size);

	glm::vec2 screen_ref = GetCanvasBoxCorner(GetGame()->GetCanvasBox(), hotpoint);
	glm::vec2 particle_position = chaos::ConvertHotpoint(screen_ref + position, final_particle_size, hotpoint, chaos::Hotpoint::CENTER);

	chaos::ParticleDefault particle;

	particle.texcoords = layout.GetTexcoords();
	particle.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	particle.bounding_box.position = particle_position;
	particle.bounding_box.half_size = 0.5f * final_particle_size;

	ParticleToPrimitives(particle, DI);

	mesh = DI.GetDynamicMesh();
}

bool GameHUDShroudLifeComponent::OnInitialize(chaos::JSONReadConfiguration config)
{
	if (!GameHUDMeshComponent::OnInitialize(config))
		return true;

	chaos::JSONTools::GetAttribute(config, "hotpoint", hotpoint);
	chaos::JSONTools::GetAttribute(config, "position", position);
	chaos::JSONTools::GetAttribute(config, "particle_size", particle_size);

	return true;
}