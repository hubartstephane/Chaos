#include <chaos/Chaos.h>

#include "Ludum41IsolationHUD.h"
#include "Ludum41IsolationGame.h"
#include "Ludum41IsolationGameInstance.h"
#include "Ludum41IsolationLevel.h"
#include "Ludum41IsolationLevelInstance.h"
#include "Ludum41IsolationPlayer.h"

// ====================================================================
// GameHUDComboComponent
// ====================================================================

GameHUDComboComponent::GameHUDComboComponent() :
	chaos::GameHUDCacheValueComponent<int>("Combo: %d x")
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
	RegisterComponent(chaos::GameHUDKeys::COMBO_ID, new GameHUDComboComponent());
	RegisterComponent(chaos::GameHUDKeys::LIFE_ID, new chaos::GameHUDLifeComponent());
	return true;
}
