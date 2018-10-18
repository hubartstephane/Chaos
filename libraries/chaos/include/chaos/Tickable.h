#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/NamedObject.h>
#include <chaos/ReferencedObject.h>

namespace chaos
{

	// ========================================================
	// Tickable : an object that deserve to be ticked
	// ========================================================	

	class Tickable : public virtual ReferencedObject, public virtual NamedObject
	{
	public:

		/** change the pause state */
		void SetPause(bool in_pause = true);
		/** get the pause state */
		bool IsPaused() const;
		/** the tick entry point */
		bool Tick(double delta_time);

	protected:

		/** the tick user function */
		virtual bool DoTick(double delta_time);
		/** called whenever object pause state has been changed */
		virtual void OnPauseStateChanged(bool in_pause);

	protected:

		/** the pause state */
		bool paused = false;
	};

}; // namespace chaos
