#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/ParticleManager.h>
#include <chaos/Renderable.h>
#include <chaos/NamedObject.h>

namespace death
{
	// Create a gamespace for unique idenfiers
	namespace GameHUDKeys
	{		
		CHAOS_DECLARE_TAG(TITLE_ID);
		CHAOS_DECLARE_TAG(BEST_SCORE_ID);
	};

	class GameHUD : public chaos::Renderable
	{
	public:




		/** insert some particles inside the HUD */
		void RegisterParticles(chaos::TagType key, chaos::ParticleAllocation * allocation, bool remove_previous = true);
		/** remove some particles from the HUD */
		void UnregisterParticles(chaos::TagType key);
		/** clear all particles from the HUD */
		void Clear();

	protected:

		std::multimap<chaos::TagType, boost::intrusive_ptr<chaos::ParticleAllocation>> particle_allocations;
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

		void CacheAndCreateScoreAllocation(class Game * game, int value, char const * format, float Y, int & cached_value, chaos::TagType key);

	protected:

		int cached_score_value = -1;
	};

}; // namespace death
