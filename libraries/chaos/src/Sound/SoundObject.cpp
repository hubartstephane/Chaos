#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace GlobalVariables
	{
		CHAOS_GLOBAL_VARIABLE(bool, Mute, false)
	};

	irrklang::ISoundEngine * SoundObject::GetIrrklangEngine()
	{
		SoundManager * manager = GetManager();
		if (manager == nullptr)
			return nullptr;
		return manager->irrklang_engine.get();
	}

	SoundManager * SoundObject::GetManager()
	{
		return sound_manager;
	}

	SoundManager const * SoundObject::GetManager() const
	{
		return sound_manager;
	}

	void SoundObject::TickObject(float delta_time)
	{
		// blend effects
		if (HasVolumeBlending())
		{
			float speed = 1.0f / blend_desc.blend_time;
			float delta_blend = delta_time * speed;

			// update the blend data
			if (blend_desc.blend_type == SoundBlendType::BlendIn)
			{
				blend_value = std::clamp(blend_value + delta_blend, 0.0f, 1.0f);
				if (blend_value >= 1.0f)
					OnBlendFinished();
			}
			else if (blend_desc.blend_type == SoundBlendType::BlendOut)
			{
				blend_value = std::clamp(blend_value - delta_blend, 0.0f, 1.0f);
				if (blend_value <= 0.0f)
					OnBlendFinished();
			}
			// at this point the object may not be in manager anymore (due to DoTickObjects(...) its destruction should be prevented
			if (!IsAttachedToManager())
				return;
		}
		// compare effective volume change
		DoUpdateEffectiveVolume(GetEffectiveVolume());
	}

	void SoundObject::OnBlendFinished()
	{
		// capture the blending
		BlendVolumeDesc old_blend_desc = blend_desc;
		blend_desc = BlendVolumeDesc();
		// call the callbacks
		if (old_blend_desc.callbacks != nullptr)
			old_blend_desc.callbacks->OnFinished(this);
		// stop or pause the object
		if (old_blend_desc.kill_at_end)
			Stop();
		else if (old_blend_desc.pause_at_end)
			Pause();
	}


	bool SoundObject::IsAttachedToManager() const
	{
		return (GetManager() != nullptr);
	}

	void SoundObject::OnRemovedFromManager()
	{
		assert(IsAttachedToManager());
		// the callbacks
		if (callbacks != nullptr)
			callbacks->OnRemovedFromManager(this);
		if (blend_desc.callbacks != nullptr)
			blend_desc.callbacks->OnRemovedFromManager(this);
		// reset some data
		blend_desc = BlendVolumeDesc();
		sound_manager = nullptr;
	}

	void SoundObject::RemoveFromManager()
	{
	}

	bool SoundObject::UpdateFinishedState()
	{
		if (!is_finished)
			is_finished = ComputeFinishedState();
		return is_finished;
	}

	bool SoundObject::ComputeFinishedState()
	{
		return false;
	}

	void SoundObject::SetCallbacks(SoundCallbacks * in_callbacks)
	{
		callbacks = in_callbacks;
	}

	void SoundObject::OnObjectFinished()
	{
		if (callbacks != nullptr)
			callbacks->OnFinished(this);
	}

	void SoundObject::Pause(bool in_pause)
	{
		// early exit
		if (!IsAttachedToManager())
			return;
		if (in_pause == paused)
			return;
		// change the pause state
		bool old_effective_pause = IsEffectivePaused();
		paused = in_pause;
		bool new_effective_pause = IsEffectivePaused();
		// trigger changes
		if (old_effective_pause != new_effective_pause)
			DoUpdateEffectivePause(new_effective_pause);
	}

	void SoundObject::DoUpdateEffectivePause(bool effective_pause)
	{
		if (effective_pause && HasVolumeBlending() && blend_desc.kill_when_paused)
			Stop();
	}

	bool SoundObject::IsPaused() const
	{
		if (!IsAttachedToManager())
			return false;
		return paused;
	}

	bool SoundObject::IsEffectivePaused() const
	{
		return IsPaused();
	}

	void SoundObject::SetVolume(float in_volume)
	{
		in_volume = std::clamp(in_volume, 0.0f, 1.0f);
		// early exit
		if (!IsAttachedToManager())
			return;
		if (in_volume == volume)
			return;
		// change the volume state
		volume = in_volume;
		DoUpdateEffectiveVolume(GetEffectiveVolume());
	}

	void SoundObject::DoUpdateEffectiveVolume(float effective_volume)
	{
	}

	float SoundObject::GetVolume() const
	{
		if (!IsAttachedToManager())
			return 0.0f;
		return volume;
	}

	float SoundObject::GetEffectiveVolume() const
	{
		if (GlobalVariables::Mute.Get())
			return 0.0f;

		float result = SoundObject::GetVolume();
		result *= blend_value;
		return result;
	}

	void SoundObject::Stop()
	{
		if (IsAttachedToManager()) // irrklang resources will be destroyed later
			RemoveFromManager();
	}

	bool SoundObject::FadeOut(float blend_time, bool kill_at_end, bool kill_when_paused)
	{
		BlendVolumeDesc desc = BlendVolumeDesc::BlendOut(blend_time, !kill_at_end, kill_at_end, kill_when_paused);

		return StartBlend(desc, true); // always replace previous : maybe there is a FADE-IN, we want to FADE-OUT
	}

	bool SoundObject::StartBlend(BlendVolumeDesc const & desc, bool replace_older)
	{
		// only if attached
		if (!IsAttachedToManager())
			return false;
		// do not override a kill
		if (IsPendingKill())
		{
			blend_desc.kill_when_paused |= desc.kill_when_paused;
			if (desc.kill_when_paused && IsEffectivePaused())
				Stop();
			return false;
		}
		// do not start a blend if there is a blend
		if (HasVolumeBlending() && !replace_older)
			return false;
		// ensure validity
		if (desc.blend_time < 0.0f)
			return false;
		if (desc.blend_type != SoundBlendType::BlendIn && desc.blend_type != SoundBlendType::BlendOut)
			return false;
		// copy the blend
		blend_desc = desc;
		// immediate blending (special case)
		if (desc.blend_time == 0.0f)
		{
			if (desc.blend_type == SoundBlendType::BlendIn)
				blend_value = 1.0f;
			else if (desc.blend_type == SoundBlendType::BlendOut)
				blend_value = 0.0f;
			if (desc.callbacks != nullptr)
				desc.callbacks->OnFinished(this);
		}

		// update the sound volume (before the tick is processed to avoid sound artifact)
		DoUpdateEffectiveVolume(GetEffectiveVolume());

		return true;
	}

	bool SoundObject::IsPendingKill() const
	{
		if (HasVolumeBlending())
			return blend_desc.kill_at_end;
		return false;
	}

	bool SoundObject::HasVolumeBlending() const
	{
		return (blend_desc.blend_type != SoundBlendType::None);
	}

	bool SoundObject::InitializeFromJSON(nlohmann::json const * json)
	{
		assert(IsAttachedToManager());
		float json_volume = 1.0f;
		JSONTools::GetAttribute(json, "volume", json_volume, 1.0f);
		SetVolume(json_volume);
		return true;
	}

}; // namespace chaos