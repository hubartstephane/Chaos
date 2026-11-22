#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void SoundAutoCallbacks::OnFinished(SoundObject * object)
	{
		if (finished_func)
			finished_func(object);
	}

	void SoundAutoCallbacks::OnRemovedFromManager(SoundObject * object)
	{
		if (removed_func)
			removed_func(object);
	}

}; // namespace chaos