#include "chaos/Chaos.h"

#include "Ludum50GameInstance.h"
#include "Ludum50Game.h"
#include "Ludum50Level.h"
#include "Ludum50LevelInstance.h"
#include "Ludum50Player.h"

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

