#include "Ludum41Level.h"
#include "Ludum41LevelInstance.h"
#include "Ludum41Game.h"
#include "Ludum41GameInstance.h"
#include "Ludum41Player.h"

DEATH_GAMEFRAMEWORK_IMPLEMENT_LEVELINSTANCE(Ludum);

bool LudumLevelInstance::Initialize(death::Game * in_game, death::GameLevel * in_level)
{
	if (!death::GameLevelInstance::Initialize(in_game, in_level))
		return false;

	return true;
}

bool LudumLevelInstance::IsLevelCompleted() const
{
	LudumGame const * ludum_game = dynamic_cast<LudumGame const *>(GetGame());
	if (ludum_game == nullptr)
		return false;

	LudumLevel const * ludum_level = dynamic_cast<LudumLevel const *>(GetLevel());
	if (ludum_level == nullptr)
		return false;

	// the only bricks remaining are the one that cannot be destroyed
	if (ludum_game->GetBrickCount() == ludum_level->indestructible_brick_count)
		return true;

	return false;
}

bool LudumLevelInstance::CanCompleteLevel() const
{
	LudumGame const * ludum_game = dynamic_cast<LudumGame const *>(GetGame());
	if (ludum_game == nullptr)
		return true;

	if (ludum_game->CanStartChallengeBallIndex(true) != std::numeric_limits<size_t>::max())
		return true;

	return false;
}

bool LudumLevelInstance::DoTick(double delta_time)
{
	death::GameLevelInstance::DoTick(delta_time);
	RestrictPlayerToWorld(0);
	return true;
}


#if 0


	// no more destructible
	size_t brick_count = GetBrickCount();
	if (brick_count == level->indestructible_brick_count)
	{
		if (CanStartChallengeBallIndex(true) != std::numeric_limits<size_t>::max())
		{
			SetNextLevel(true);
		}
	}
}
#endif