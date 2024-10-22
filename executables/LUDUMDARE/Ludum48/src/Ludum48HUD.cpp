#include "Ludum48PCH.h"
#include "Ludum48HUD.h"
#include "Ludum48Game.h"
#include "Ludum48Level.h"
#include "Ludum48Player.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48GameInstance.h"

namespace chaos
{
	namespace GameHUDKeys
	{
		CHAOS_DECLARE_TAG(DIAMOND_ID);
	};
};

// ====================================================================
// LudumPlayingHUD
// ====================================================================

bool LudumPlayingHUD::FillHUDContent()
{
	if (!PlayingHUD::FillHUDContent())
		return false;
	RegisterComponent(CreateHUDComponent<GameHUDTimeoutComponent>(GameHUDKeys::LEVEL_TIMEOUT_ID));
	RegisterComponent(CreateHUDComponent<LudumHUDDiamondComponent>(GameHUDKeys::DIAMOND_ID));
	RegisterComponent(CreateHUDComponent<LudumHUDLifeComponent>(GameHUDKeys::LIFE_ID));
	RegisterComponent(CreateHUDComponent<GameHUDLevelTitleComponent>(GameHUDKeys::LEVEL_TITLE_ID));
	return true;
}

// ====================================================================
// LudumHUDDiamondComponent
// ====================================================================

LudumHUDDiamondComponent::LudumHUDDiamondComponent(char const* in_text) :
	GameHUDCacheValueTextComponent<std::pair<int, int>>(in_text)
{
	generator_params.line_height = 130.0f;
	generator_params.font_info_name = "normal";
	generator_params.position = glm::vec2(-20.0f, -80.0f);
	generator_params.hotpoint = Hotpoint::TOP_RIGHT;
}

bool LudumHUDDiamondComponent::QueryValue(std::pair<int, int>& result) const
{
	LudumLevelInstance const * li = GetLevelInstance();
	if (li == nullptr)
		return false;
	result = { li->GetDiamondCount(), li->GetRequiredDiamondCount() };
	return true;
}
void LudumHUDDiamondComponent::UpdateMesh()
{
	SetText(StringTools::Printf(text.c_str(), cached_value.first, cached_value.second).c_str());
}

// ====================================================================
// LudumHUDLifeComponent
// ====================================================================

LudumHUDLifeComponent::LudumHUDLifeComponent(char const* in_text) :
	GameHUDCacheValueTextComponent<int>(in_text)
{
	generator_params.line_height = 130.0f;
	generator_params.font_info_name = "normal";
	generator_params.position = glm::vec2(-20.0f, -80.0f);
	generator_params.hotpoint = Hotpoint::TOP_RIGHT;
}

bool LudumHUDLifeComponent::QueryValue(int & result) const
{
	Player const * player = GetPlayer(0);
	if (player == nullptr)
		return false;
	result = player->GetLifeCount();
	return true;
}
void LudumHUDLifeComponent::UpdateMesh()
{
	SetText(StringTools::Printf(text.c_str(), cached_value).c_str());
}

