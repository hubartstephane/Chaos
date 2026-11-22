#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void SoundCategory::OnRemovedFromManager()
	{
		sound_manager->DestroyAllSoundPerCategory(this);
		sound_manager->UpdateAllSourcesPerCategory(this);
		SoundObject::OnRemovedFromManager();
	}

	void SoundCategory::RemoveFromManager()
	{
		assert(IsAttachedToManager());
		sound_manager->RemoveCategory(this);
	}

	void SoundCategory::DoUpdateEffectivePause(bool effective_pause)
	{
		SoundObject::DoUpdateEffectivePause(effective_pause);
		if (!IsAttachedToManager())
			return;
		sound_manager->UpdateAllSoundPausePerCategory(this);
	}

	void SoundCategory::DoUpdateEffectiveVolume(float effective_volume)
	{
		sound_manager->UpdateAllSoundVolumePerCategory(this);
	}

}; // namespace chaos