#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void SoundCallbacks::OnFinished(SoundObject * object)
	{
		assert(object != nullptr);
	}

	void SoundCallbacks::OnRemovedFromManager(SoundObject * object)
	{
		assert(object != nullptr);
	}

}; // namespace chaos