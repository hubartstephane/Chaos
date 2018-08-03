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

	class PlayingHUD : public GameHUD
	{

	public:

		void SetScoreValue(int new_score);

	public:

		boost::intrusive_ptr<chaos::ParticleAllocation> score_allocations;

	protected:

		int cached_score_value = -1;
	};

}; // namespace death
