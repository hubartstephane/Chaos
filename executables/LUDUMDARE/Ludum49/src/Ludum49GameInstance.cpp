#include "Ludum49PCH.h"
#include "Ludum49GameInstance.h"
#include "Ludum49Game.h"
#include "Ludum49Level.h"
#include "Ludum49LevelInstance.h"
#include "Ludum49Player.h"

LudumGameInstance::LudumGameInstance()
{
	player_class = LudumPlayer::GetStaticClass();
}

void LudumGameInstance::OnLevelChanged(Level * new_level, Level * old_level, LevelInstance * new_level_instance)
{
	GameInstance::OnLevelChanged(new_level, old_level, new_level_instance);
	// play a sound
	if (new_level != nullptr && old_level != nullptr)
		game->PlaySound("next_level", false, false, 0.0f, SoundContext::GAME);
}

