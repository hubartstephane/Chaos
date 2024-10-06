#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ========================================================
	// Tickable implementation
	// ========================================================

	void TickableInterface::SetPause(bool in_pause)
	{
		if (paused == in_pause)
			return;
		paused = in_pause;
		OnPauseStateChanged(in_pause);
	}

	bool TickableInterface::IsPaused() const
	{
		return paused;
	}

	bool TickableInterface::Tick(float delta_time)
	{
		if (!CanTick())
			return false;
		return DoTick(delta_time);
	}

	bool TickableInterface::CanTick()
	{
		return !IsPaused();
	}

	bool TickableInterface::DoTick(float delta_time)
	{
		return true;
	}

	void TickableInterface::OnPauseStateChanged(bool in_pause)
	{

	}

}; // namespace chaos
