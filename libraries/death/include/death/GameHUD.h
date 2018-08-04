#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/ParticleManager.h>

namespace death
{
	class GameHUD : public chaos::ReferencedObject
	{

	};

	class MainMenuHUD : public GameHUD
	{
	public:

		boost::intrusive_ptr<chaos::ParticleAllocation> title_allocations;
		boost::intrusive_ptr<chaos::ParticleAllocation> best_score_allocations;
	};

	class PauseMenuHUD : public GameHUD
	{
	public:

		boost::intrusive_ptr<chaos::ParticleAllocation> title_allocations;
	};

	class GameOverHUD : public GameHUD
	{
	public:

		boost::intrusive_ptr<chaos::ParticleAllocation> title_allocations;
	};

	class PlayingHUD : public GameHUD
	{

	public:

		void SetScoreValue(class Game * game, int new_score);

	protected:

		void CacheAndCreateScoreAllocation(class Game * game, int value, char const * format, float Y, int & cached_value, boost::intrusive_ptr<chaos::ParticleAllocation> & allocation);

	protected:

		boost::intrusive_ptr<chaos::ParticleAllocation> score_allocations;

		int cached_score_value = -1;
	};

}; // namespace death
