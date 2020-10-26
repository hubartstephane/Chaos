#include <chaos/Chaos.h>

namespace chaos
{
	// ========================================================
	// Tickable implementation
	// ========================================================	
	
	void Tickable::SetPause(bool in_pause)
	{
		if (paused == in_pause)
			return;
		paused = in_pause;
		OnPauseStateChanged(in_pause);
	}
		
	bool Tickable::IsPaused() const
	{
		return paused;
	}

	bool Tickable::Tick(float delta_time)
	{
		if (!CanTick())
			return false;
		return DoTick(delta_time);
	}

	bool Tickable::CanTick()
	{
		return !IsPaused();
	}

	bool Tickable::DoTick(float delta_time)
	{
		return true;
	}

	void Tickable::OnPauseStateChanged(bool in_pause)
	{

	}

}; // namespace chaos
