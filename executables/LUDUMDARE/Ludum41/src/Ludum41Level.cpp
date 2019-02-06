#include "Ludum41Level.h"
#include "Ludum41Game.h"
#include "Ludum41GameInstance.h"
#include "Ludum41Player.h"



size_t LudumLevel::GetBrickCount() const
{
	size_t result = 0;
	for (size_t i = 0; i < bricks.size(); ++i)
	{
		std::vector<int> const & line = bricks[i];
		for (int b : line)
			if (b != LudumLevel::NONE)
				++result;
	}
	return result;
}

death::GameLevelInstance * LudumLevel::DoCreateLevelInstance(death::Game * in_game)
{
	return new LudumLevelInstance;
}


DEATH_GAMEFRAMEWORK_IMPLEMENT_LEVELINSTANCE(Ludum);

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