#include <death/GameHUD.h>
#include <death/Game.h>

namespace death
{

	void GameHUD::AddParticles(int key, chaos::ParticleAllocation * allocation, bool remove_previous)
	{
		if (remove_previous)
			RemoveParticles(key);
		particle_allocations.insert(std::make_pair(key, allocation));
	}
	
	void GameHUD::RemoveParticles(int key)
	{
		particle_allocations.erase(key);
	}
		
	void GameHUD::Clear()
	{
		particle_allocations.clear();
	}

	void PlayingHUD::SetScoreValue(class Game * game, int new_score)
	{
		CacheAndCreateScoreAllocation(game, new_score, "Score : %d", 20.0f, cached_score_value, BEST_SCORE);
	}

	void PlayingHUD::CacheAndCreateScoreAllocation(class Game * game, int value, char const * format, float Y,int & cached_value, int key)
	{
		if (value < 0)
			RemoveParticles(key);
		else if (cached_value != value)
			AddParticles(key, game->CreateScoringText(format, value, Y));
		cached_value = value;
	}

}; // namespace death

