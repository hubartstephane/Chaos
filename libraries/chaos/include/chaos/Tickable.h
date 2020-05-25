#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/NamedObject.h>
#include <chaos/Object.h>

namespace chaos
{

	// ========================================================
	// Tickable : an object that deserve to be ticked
	// ========================================================	

	class Tickable : public Object, public NamedObject
	{
	public:

		/** change the pause state */
		void SetPause(bool in_pause = true);
		/** get the pause state */
		bool IsPaused() const;
		/** the tick entry point */
		bool Tick(float delta_time);

	protected:

		/** the tick user function */
		virtual bool DoTick(float delta_time);
		/** whether the object should be ticked or not */
		virtual bool CanTick();
		/** called whenever object pause state has been changed */
		virtual void OnPauseStateChanged(bool in_pause);

	protected:

		/** the pause state */
		bool paused = false;
	};

}; // namespace chaos
