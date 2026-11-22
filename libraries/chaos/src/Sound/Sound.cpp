#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool Sound::IsOfCategory(SoundCategory const * category) const
	{
		assert(category != nullptr);

		return (std::find(categories.begin(), categories.end(), category) != categories.end());
	}

	bool Sound::IsSound3D() const
	{
		return is_3D_sound;
	}

	glm::vec3 Sound::GetPosition() const
	{
		return position;
	}

	glm::vec3 Sound::GetVelocity() const
	{
		return velocity;
	}

	bool Sound::IsLooping() const
	{
		return looping;
	}

	void Sound::SetPosition(glm::vec3 const & in_position)
	{
		position = in_position;
		if (is_3D_sound)
		{
			if (irrklang_sound != nullptr)
				irrklang_sound->setPosition(ToIrrklangVector(in_position));
			DoUpdateEffectiveVolume(GetEffectiveVolume());
		}
	}

	void Sound::SetVelocity(glm::vec3 const & in_velocity)
	{
		velocity = in_velocity;
		if (is_3D_sound)
			if (irrklang_sound != nullptr)
				irrklang_sound->setVelocity(ToIrrklangVector(in_velocity));
	}

	float Sound::Get3DVolumeModifier() const
	{
		if (is_3D_sound && min_distance > 0.0f && max_distance >= min_distance)
		{
			glm::vec3 listener_position = sound_manager->listener_transform[3];

			float distance = glm::distance(position, listener_position);

			float distance_volume = 1.0f;
			if (distance < min_distance)
				distance_volume = 1.0f;
			else if (distance > max_distance)
				distance_volume = 0.0f;
			else
				distance_volume = 1.0f - ((distance - min_distance) / (max_distance - min_distance));

			return distance_volume;
		}
		return 1.0f;
	}

	float Sound::GetEffectiveVolume() const
	{
		float result = SoundObject::GetEffectiveVolume();
		// volume for all categories
		if (result > 0.0f)
			for (SoundCategory * category : categories)
				if (category != nullptr)
					result *= category->GetEffectiveVolume();
		// volume for the sound
		if (result > 0.0f)
			if (source != nullptr)
				result *= source->GetEffectiveVolume();
		// 3D volume affect
		if (result > 0.0f)
			result *= Get3DVolumeModifier();
		return result;
	}

	bool Sound::IsEffectivePaused() const
	{
		// standard pause
		if (SoundObject::IsEffectivePaused())
			return true;
		// from categories
		for (SoundCategory * category : categories)
			if (category != nullptr && category->IsEffectivePaused())
				return true;
		// from sources
		if (source != nullptr && source->IsEffectivePaused())
			return true;
		// from a too far 3D object
		if (pause_timer_when_too_far >= 0 && pause_timer_value >= pause_timer_when_too_far && Get3DVolumeModifier() == 0.0f)
			return true;
		return false;
	}

	bool Sound::ComputeFinishedState()
	{
		if (irrklang_sound == nullptr)
			return true;
		if (SoundObject::ComputeFinishedState()) // parent call
			return true;
		return irrklang_sound->isFinished();
	}

	void Sound::RemoveFromManager()
	{
		assert(IsAttachedToManager());
		sound_manager->RemoveSound(this);
	}

	void Sound::OnRemovedFromManager()
	{
		assert(IsAttachedToManager());
		if (irrklang_sound != nullptr)
		{
			irrklang_sound->stop();
			irrklang_sound = nullptr;
		}
		SoundObject::OnRemovedFromManager();
	}

	bool Sound::DoPlaySound(PlaySoundDesc const & play_desc)
	{
		// test whether the sound may be played
		// error => immediatly finished
		if (source == nullptr || source->irrklang_source == nullptr)
			return true;

		irrklang::ISoundEngine * irrklang_engine = GetIrrklangEngine();
		if (irrklang_engine == nullptr)
			return true;

		// copy blend data
		if (play_desc.blend_in_time > 0.0f)
		{
			blend_desc.blend_type = SoundBlendType::BlendIn;
			blend_desc.blend_time = play_desc.blend_in_time;
			blend_value = 0.0f;
		}

		// copy some 3D sound data before computing effective colume
		min_distance = play_desc.min_distance;
		max_distance = play_desc.max_distance;
		pause_timer_when_too_far = play_desc.pause_timer_when_too_far;
		position = play_desc.position;
		velocity = play_desc.velocity;

		// compute effective expected values
		bool  effective_pause  = IsEffectivePaused();
		float effective_volume = GetEffectiveVolume();

		// play sound
		bool track = true;
		bool sound_effect = true;

		// if we have some additionnal initialization to do, we start the sound paused so we do not have sound volume artifact
		bool some_initializations = (is_3D_sound) || (effective_volume != 1.0f);

		bool start_paused = some_initializations || effective_pause;

		if (is_3D_sound)
		{
			irrklang_sound = irrklang_engine->play3D(
				source->irrklang_source.get(),
				ToIrrklangVector(position),
				looping,
				start_paused,
				track,
				sound_effect);
		}
		else
		{
			irrklang_sound = irrklang_engine->play2D(
				source->irrklang_source.get(),
				looping,
				start_paused,
				track,
				sound_effect);
		}

		if (irrklang_sound == nullptr)
			return false;

		// update volume
		if (effective_volume != 1.0f)
			DoUpdateEffectiveVolume(effective_volume);

		// resume sound
		if (some_initializations && !effective_pause)
			DoUpdateIrrklangPause(effective_pause);

		return true;
	}

	void Sound::DoUpdateEffectivePause(bool effective_pause)
	{
		SoundObject::DoUpdateEffectivePause(effective_pause);
		if (!IsAttachedToManager())
			return;
		DoUpdateIrrklangPause(effective_pause);
	}

	void Sound::DoUpdateEffectiveVolume(float effective_volume)
	{
		if (cached_effective_volume != effective_volume)
		{
			cached_effective_volume = effective_volume;
			if (irrklang_sound != nullptr)
				irrklang_sound->setVolume((irrklang::ik_f32)effective_volume);
		}
	}

	void Sound::DoUpdateIrrklangPause(bool effective_pause)
	{
		if (irrklang_sound != nullptr)
			irrklang_sound->setIsPaused(effective_pause);
	}

	void Sound::SetSoundTrackPosition(int position)
	{
		if (irrklang_sound != nullptr)
			irrklang_sound->setPlayPosition((irrklang::ik_u32)position);
	}

	void Sound::TickObject(float delta_time)
	{
		SoundObject::TickObject(delta_time);

		// 3D object that wants to be paused
		if (IsAttachedToManager())
		{
			if (is_3D_sound && pause_timer_when_too_far >= 0.0f)
			{
				if (Get3DVolumeModifier() == 0.0f) // a 3D object too far to be listened
				{
					if (pause_timer_value < pause_timer_when_too_far)
					{
						pause_timer_value += delta_time;
						if (pause_timer_value >= pause_timer_when_too_far) // timer reached the limit
						{
							pause_timer_value = pause_timer_when_too_far;
							DoUpdateEffectivePause(IsEffectivePaused()); // update pause state
						}
					}
				}
				else
				{
					// was in pause due to distance ?
					if (pause_timer_value >= pause_timer_when_too_far)
						DoUpdateEffectivePause(IsEffectivePaused());  // update pause state (resume if all other conditions are present)
					pause_timer_value = 0.0f;
				}
			}
		}
	}


#if 0
	void Sound::SetMaxDistance(float distance)
	{
		if (irrklang_sound != nullptr)
			irrklang_sound->setMaxDistance((irrklang::ik_f32)distance);
	}
	void Sound::SetMinDistance(float distance)
	{
		if (irrklang_sound != nullptr)
			irrklang_sound->setMinDistance((irrklang::ik_f32)distance);
	}
#endif

}; // namespace chaos