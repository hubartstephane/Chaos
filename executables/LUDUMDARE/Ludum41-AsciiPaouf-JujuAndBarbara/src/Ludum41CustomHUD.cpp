#include "Ludum41CustomPCH.h"
#include "Ludum41CustomHUD.h"
#include "Ludum41CustomGame.h"
#include "Ludum41CustomGameInstance.h"
#include "Ludum41CustomLevel.h"
#include "Ludum41CustomLevelInstance.h"
#include "Ludum41CustomPlayer.h"

// ====================================================================
// GameHUDComboComponent
// ====================================================================

GameHUDComboComponent::GameHUDComboComponent() :
	chaos::GameHUDCacheValueTextComponent<int>("Combo: %d x")
{
	generator_params.line_height = 60.0f;
	generator_params.font_info_name = "normal";
	generator_params.position = glm::vec2(20.0f, -80.0f);
	generator_params.hotpoint = chaos::Hotpoint::TOP_LEFT;
}

bool GameHUDComboComponent::QueryValue(int & result) const
{
	LudumGameInstance const* ludum_game_instance = GetGameInstance();
	if (ludum_game_instance == nullptr)
		return false;
	result = ludum_game_instance->GetCurrentComboMultiplier();
	return (result > 0);
}

// ====================================================================
// LudumPlayingHUD
// ====================================================================

bool LudumPlayingHUD::FillHUDContent()
{
	if (!chaos::PlayingHUD::FillHUDContent())
		return false;
	RegisterComponent(CreateHUDComponent<GameHUDComboComponent>(chaos::GameHUDKeys::COMBO_ID));
	RegisterComponent(CreateHUDComponent<chaos::GameHUDLifeComponent>(chaos::GameHUDKeys::LIFE_ID));
	return true;
}