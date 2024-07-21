#include "Ludum48PCH.h"
#include "Ludum48GameInstance.h"
#include "Ludum48Game.h"
#include "Ludum48Level.h"
#include "Ludum48LevelInstance.h"
#include "Ludum48Player.h"

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

