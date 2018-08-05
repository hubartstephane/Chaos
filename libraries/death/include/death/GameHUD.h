#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/ParticleManager.h>

namespace death
{
	class GameHUD : public chaos::ReferencedObject
	{
	public:

		static int const TITLE       = 0;
		static int const BEST_SCORE  = 1;
		static int const LAST_KEY    = 1;

		/** insert some particles inside the HUD */
		void AddParticles(int key, chaos::ParticleAllocation * allocation, bool remove_previous = true);
		/** remove some particles from the HUD */
		void RemoveParticles(int key);
		/** clear all particles from the HUD */
		void Clear();

	protected:

		std::multimap<int, boost::intrusive_ptr<chaos::ParticleAllocation>> particle_allocations;
	};

	class MainMenuHUD : public GameHUD
	{
	public:

	};

	class PauseMenuHUD : public GameHUD
	{
	public:

	};

	class GameOverHUD : public GameHUD
	{
	public:

	};

	class PlayingHUD : public GameHUD
	{

	public:

		void SetScoreValue(class Game * game, int new_score);

	protected:

		void CacheAndCreateScoreAllocation(class Game * game, int value, char const * format, float Y, int & cached_value, int key);

	protected:

		int cached_score_value = -1;
	};

}; // namespace death
