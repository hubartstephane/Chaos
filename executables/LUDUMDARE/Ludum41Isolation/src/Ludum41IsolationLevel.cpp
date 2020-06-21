#include "Ludum41IsolationLevel.h"
#include "Ludum41IsolationLevelInstance.h"
#include "Ludum41IsolationGame.h"
#include "Ludum41IsolationGameInstance.h"
#include "Ludum41IsolationPlayer.h"

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

death::LevelInstance * LudumLevel::DoCreateLevelInstance()
{
	return new LudumLevelInstance;
}
