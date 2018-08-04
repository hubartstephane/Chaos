#include <death/GameHUD.h>
#include <death/Game.h>

namespace death
{
	void PlayingHUD::SetScoreValue(class Game * game, int new_score)
	{
		CacheAndCreateScoreAllocation(game, new_score, "Score : %d", 20.0f, cached_score_value, score_allocations);
	}

	void PlayingHUD::CacheAndCreateScoreAllocation(class Game * game, int value, char const * format, float Y,int & cached_value, boost::intrusive_ptr<chaos::ParticleAllocation> & allocation)
	{
		if (value < 0)
			allocation = nullptr;
		else if (cached_value != value)
			allocation = game->CreateScoringText(format, value, Y);
		cached_value = value;
	}

}; // namespace death

