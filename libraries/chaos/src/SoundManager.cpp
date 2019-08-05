#include <chaos/SoundManager.h>
#include <chaos/MathTools.h>
#include <chaos/Buffer.h>
#include <chaos/FileTools.h>
#include <chaos/JSONTools.h>
#include <chaos/BoostTools.h>

namespace chaos
{

	// ==============================================================
	// PLAY SOUND DESC
	// ==============================================================

	bool PlaySoundDesc::IsSound3D() const
	{
		return is_3D_sound;
	}

	void PlaySoundDesc::Enable3D(bool enable)
	{
		is_3D_sound = enable;
	}

	void PlaySoundDesc::SetPosition(glm::vec3 const & in_position, bool update_3D_sound)
	{
		position = in_position;
		if (update_3D_sound)
			is_3D_sound = true;
	}

	void PlaySoundDesc::SetVelocity(glm::vec3 const & in_velocity, bool update_3D_sound)
	{
		velocity = in_velocity;
		if (update_3D_sound)
			is_3D_sound = true;
	}

	// ==============================================================
	// CALLBACKS
	// ==============================================================

	void SoundCallbacks::OnFinished(SoundObject * object)
	{
		assert(object != nullptr);
	}

	void SoundCallbacks::OnRemovedFromManager(SoundObject * object)
	{
		assert(object != nullptr);
	}

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

	// ==============================================================
	// BLEND VOLUME DESC
	// ==============================================================

	BlendVolumeDesc BlendVolumeDesc::BlendIn(float blend_time)
	{
		BlendVolumeDesc result;
		result.blend_type = BLEND_IN;
		result.blend_time = blend_time;
		return result;
	}

	BlendVolumeDesc BlendVolumeDesc::BlendOut(float blend_time, bool pause_at_end, bool kill_at_end)
	{
		BlendVolumeDesc result;
		result.blend_type = BLEND_OUT;
		result.blend_time = blend_time;
		result.pause_at_end = pause_at_end;
		result.kill_at_end = kill_at_end;
		return result;
	}

	// ==============================================================
	// SOUND OBJECT
	// ==============================================================

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
		// early exit
		if (!HasVolumeBlending())
			return;

		float speed = 1.0f / blend_desc.blend_time;
		float delta_blend = delta_time * speed;

		// keep trace of the effective volume before blending is done
		float old_effective_volume = GetEffectiveVolume();
		// update the blend data
		if (blend_desc.blend_type == BlendVolumeDesc::BLEND_IN)
		{
			blend_value = MathTools::Clamp(blend_value + delta_blend, 0.0f, 1.0f);
			if (blend_value >= 1.0f)
				OnBlendFinished();
		}
		else if (blend_desc.blend_type == BlendVolumeDesc::BLEND_OUT)
		{
			blend_value = MathTools::Clamp(blend_value - delta_blend, 0.0f, 1.0f);
			if (blend_value <= 0.0f)
				OnBlendFinished();
		}
		// at this point the object may not be in manager anymore (due to DoTickObjects(...) its destruction should be prevented
		if (!IsAttachedToManager())
			return;
		// compare effective volume change
		float new_effective_volume = GetEffectiveVolume();
		if (old_effective_volume != new_effective_volume)
			DoUpdateEffectiveVolume(new_effective_volume);
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
		in_volume = MathTools::Clamp(in_volume, 0.0f, 1.0f);
		// early exit
		if (!IsAttachedToManager())
			return;		
		if (in_volume == volume)
			return;
		// change the volume state
		float old_effective_volume = GetEffectiveVolume();
		volume = in_volume;
		float new_effective_volume = GetEffectiveVolume();
		// trigger changes
		if (old_effective_volume != new_effective_volume)
			DoUpdateEffectiveVolume(new_effective_volume);
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
		float result = SoundObject::GetVolume();
		result *= blend_value;
		return result;
	}

	void SoundObject::Stop()
	{
		if (IsAttachedToManager()) // irrklang resources will be destroyed later
			RemoveFromManager();
	}

	bool SoundObject::FadeOut(float blend_time, bool kill)
	{
		// already a fade out -> do nothing
		if (IsPendingKill())
			return false;

		chaos::BlendVolumeDesc desc;
		desc.blend_time = blend_time;
		desc.blend_type = chaos::BlendVolumeDesc::BLEND_OUT;

		if (kill)
			desc.kill_at_end = true;
		else
			desc.pause_at_end = true;

		return StartBlend(desc, true); // always replace previous : maybe there is a FADE-IN, we want to FADE-OUT
	}

	bool SoundObject::StartBlend(BlendVolumeDesc const & desc, bool replace_older, bool update_blend_value)
	{
		// only if attached
		if (!IsAttachedToManager())
			return false;
		// do not override a kill
		if (IsPendingKill())
			return false;
		// do not start a blend if there is a blend
		if (HasVolumeBlending() && !replace_older)
			return false;
		// ensure validity
		if (desc.blend_time < 0.0f)
			return false;
		if (desc.blend_type != BlendVolumeDesc::BLEND_IN && desc.blend_type != BlendVolumeDesc::BLEND_OUT)
			return false;
		// copy the blend
		blend_desc = desc;
		// immediate blending (special case)		
		if (desc.blend_time == 0.0f)
		{
			if (desc.blend_type == BlendVolumeDesc::BLEND_IN)
				blend_value = 1.0f;
			else if (desc.blend_type == BlendVolumeDesc::BLEND_OUT)
				blend_value = 0.0f;
			if (desc.callbacks != nullptr)
				desc.callbacks->OnFinished(this);
		} 
		// update blend value
		if (update_blend_value)
		{
			if (desc.blend_type == BlendVolumeDesc::BLEND_IN)
				blend_value = 0.0f;
			else if (desc.blend_type == BlendVolumeDesc::BLEND_OUT)
				blend_value = 1.0f;
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
		return (blend_desc.blend_type != BlendVolumeDesc::BLEND_NONE);
	}

	bool SoundObject::InitializeFromJSON(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		assert(IsAttachedToManager());
		float json_volume = 1.0f;
		JSONTools::GetAttribute(json, "volume", json_volume, 1.0f);
		SetVolume(json_volume);
		return true;
	}

	// ==============================================================
	// SOURCE
	// ==============================================================

	Sound * SoundSource::GenerateSound()
	{
		return new Sound;
	}

	Sound * SoundSource::Play(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks)
	{
		// ensure we have access to manager
		if (!IsAttachedToManager())
			return nullptr;

		// ensure there are no name collision
		if (!sound_manager->CanAddSound((desc.sound_name.length() > 0) ? desc.sound_name.c_str() : nullptr))
			return nullptr;

		// compute required categories
		std::vector<SoundCategory *> categories = default_categories;
		
		for (std::string const & category_name : desc.category_names)
		{
			if (category_name.length() > 0)
			{
				SoundCategory * category = sound_manager->FindCategory(category_name.c_str());
				if (category != nullptr)
					if (std::find(categories.begin(), categories.end(), category) == categories.end())
						categories.push_back(category);
			}
		}
		
		for (SoundCategory * category : desc.categories)
			if (category != nullptr && category->sound_manager == sound_manager)
				if (std::find(categories.begin(), categories.end(), category) == categories.end())
					categories.push_back(category);

		// create the sound and initialize it
		Sound * result = GenerateSound();
		if (result != nullptr)
		{
			// initialize the newly created object
			result->categories = std::move(categories);
			result->sound_manager = sound_manager;
			result->source = this;
			
			result->is_3D_sound = desc.IsSound3D();
			result->position = desc.position;
			result->velocity = desc.velocity;
			result->paused = desc.paused;
			result->looping = desc.looping;
			result->volume = MathTools::Clamp(desc.volume, 0.0f, 1.0f); ;
			result->callbacks = in_callbacks;

			if (desc.sound_name.length() > 0)
				result->name = desc.sound_name;

			// store the sound
			sound_manager->sounds.push_back(result);
			
			// play the sound
			result->DoPlaySound(desc);
		}
		return result;
	}

	void SoundSource::RemoveFromManager()
	{
		assert(IsAttachedToManager());
		sound_manager->RemoveSource(this);
	}

	void SoundSource::OnRemovedFromManager()
	{
		// destroy all sound using this source
		sound_manager->DestroyAllSoundPerSource(this);
		// destroy irrklang resource
		if (irrklang_source != nullptr)
		{
			irrklang::ISoundEngine * irrklang_engine = GetIrrklangEngine();
			if (irrklang_engine != nullptr)
				irrklang_engine->removeSoundSource(irrklang_source.get());
			irrklang_source = nullptr;
		}
		// parent call
		SoundObject::OnRemovedFromManager();
	}

	void SoundSource::DoUpdateEffectivePause(bool effective_pause)
	{
		sound_manager->UpdateAllSoundPausePerSource(this);
	}

	void SoundSource::DoUpdateEffectiveVolume(float effective_volume)
	{
		sound_manager->UpdateAllSoundVolumePerSource(this);
	}

	bool SoundSource::SetDefaultCategories(std::vector<SoundCategory *> const & categories)
	{
		// a detached source cannot have a category
		if (!IsAttachedToManager())
			return false;
		// a detached category cannot be used
		for (SoundCategory * category : categories)
		{
			if (category != nullptr)
			{
				if (!category->IsAttachedToManager())
					return false;
				if (sound_manager != category->sound_manager) // source and category must have the same manager
					return false;
			}				
		}
		default_categories = categories;
		return true;
	}

	bool SoundSource::InitializeFromJSON(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		if (!SoundObject::InitializeFromJSON(json, config_path))
			return false;
		// update the default category
		std::vector<std::string> category_names;
		if (JSONTools::GetAttributeArray(json, "categories", category_names))
		{
			std::vector<SoundCategory *> categories;
			for (std::string const & category_name : category_names)
			{
				SoundCategory * category = sound_manager->FindCategory(category_name.c_str());
				if (category != nullptr)
				{
					if (std::find(categories.begin(), categories.end(), category) == categories.end()) // no duplicate
						categories.push_back(category);					
				}
			}
			default_categories = std::move(categories);
		}
		return true;
	}

	// ==============================================================
	// CATEGORY
	// ==============================================================

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
		sound_manager->UpdateAllSoundPausePerCategory(this);
	}

	void SoundCategory::DoUpdateEffectiveVolume(float effective_volume)
	{
		sound_manager->UpdateAllSoundVolumePerCategory(this);
	}

	// ==============================================================
	// SOUND
	// ==============================================================
	
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
			if (irrklang_sound != nullptr)
				irrklang_sound->setPosition(IrrklangTools::ToIrrklangVector(in_position));
	}

	void Sound::SetVelocity(glm::vec3 const & in_velocity)
	{
		velocity = in_velocity;
		if (is_3D_sound)
			if (irrklang_sound != nullptr)
				irrklang_sound->setVelocity(IrrklangTools::ToIrrklangVector(in_velocity));
	}

	float Sound::GetEffectiveVolume() const
	{
		float result = SoundObject::GetEffectiveVolume();
		for (SoundCategory * category : categories)
			if (category != nullptr)
				result *= category->GetEffectiveVolume();
		if (source != nullptr)
			result *= source->GetEffectiveVolume();
		return result;
	}

	bool Sound::IsEffectivePaused() const
	{
		if (SoundObject::IsEffectivePaused())
			return true;
		for (SoundCategory * category : categories)
			if (category != nullptr && category->IsEffectivePaused())
				return true;
		if (source != nullptr && source->IsEffectivePaused())
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

	bool Sound::DoPlaySound(PlaySoundDesc const & desc)
	{
		// test whether the sound may be played
		// error => immediatly finished
		if (source == nullptr || source->irrklang_source == nullptr)
			return true;

		irrklang::ISoundEngine * irrklang_engine = GetIrrklangEngine();
		if (irrklang_engine == nullptr)
			return true;

		// copy blend data
		if (desc.blend_time > 0.0f)
		{
			blend_desc.blend_type = BlendVolumeDesc::BLEND_IN;
			blend_desc.blend_time = desc.blend_time;
			blend_value = 0.0f;
		}

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
				IrrklangTools::ToIrrklangVector(position),
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
			DoUpdateIrrklangVolume(effective_volume);

		// update 3D data
		if (is_3D_sound)
		{
			SetVelocity(velocity);
			SetMinDistance(desc.min_distance);
			SetMaxDistance(desc.max_distance);
		}
		// resume sound
		if (some_initializations && !effective_pause)
			DoUpdateIrrklangPause(effective_pause);

		return true;
	}

	void Sound::DoUpdateEffectivePause(bool effective_pause)
	{
		DoUpdateIrrklangPause(effective_pause);
	}

	void Sound::DoUpdateEffectiveVolume(float effective_volume)
	{
		DoUpdateIrrklangVolume(effective_volume);
	}

	void Sound::DoUpdateIrrklangPause(bool effective_pause)
	{
		if (irrklang_sound != nullptr)
			irrklang_sound->setIsPaused(effective_pause);
	}

	void Sound::DoUpdateIrrklangVolume(float effective_volume)
	{
		if (irrklang_sound != nullptr)
			irrklang_sound->setVolume((irrklang::ik_f32)effective_volume);
	}









	void Sound::SetSoundTrackPosition(int position)
	{
		if (irrklang_sound != nullptr)
			irrklang_sound->setPlayPosition((irrklang::ik_u32)position);
	}

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

	// ==============================================================
	// MANAGER
	// ==============================================================

	irrklang::ISoundEngine * SoundManager::GetIrrklangEngine()
	{
		return irrklang_engine.get();
	}

	bool SoundManager::DoStartManager()
	{
		// super method
		if (!Manager::DoStartManager())
			return false;
		// get the list of all devices
		irrklang_devices = irrklang::createSoundDeviceList();
		if (irrklang_devices == nullptr)
			return false;
		irrklang_devices->drop();
		// create the engine
		irrklang_engine = irrklang::createIrrKlangDevice();
		if (irrklang_engine == nullptr)
			return false;
		irrklang_engine->drop(); // suppress the extra reference

		return true;
	}

	bool SoundManager::DoStopManager()
	{
		// super method
		Manager::DoStopManager();

		// empty the managed objects list (destroy sounds first to make other list destructions faster)
		RemoveAllObjectsFromList(sounds, &SoundManager::OnObjectRemovedFromManager);
		RemoveAllObjectsFromList(categories, &SoundManager::OnObjectRemovedFromManager);
		RemoveAllObjectsFromList(sources, &SoundManager::OnObjectRemovedFromManager);

		// clean irrklang resources
		irrklang_devices = nullptr;
		irrklang_engine = nullptr;

		return true;
	}

	void SoundManager::Tick(float delta_time)
	{
		if (!IsManagerStarted())
			return;
		// tick all sources
		DoTickObjects(delta_time, sources, &SoundManager::RemoveSource);
		// tick all categories
		DoTickObjects(delta_time, categories, &SoundManager::RemoveCategory);
		// tick all sounds
		DoTickObjects(delta_time, sounds, &SoundManager::RemoveSound);
	}

	void SoundManager::OnObjectRemovedFromManager(SoundObject * object)
	{
		assert(object != nullptr);
		object->OnRemovedFromManager();
	}

	void SoundManager::RemoveCategory(SoundCategory * category)
	{
		RemoveCategoryByIndex(FindObjectIndexInVector(category, categories));
	}

	void SoundManager::RemoveCategoryByIndex(size_t index)
	{
		DoRemoveObject(index, categories, &SoundManager::OnObjectRemovedFromManager);
	}

	void SoundManager::RemoveSound(Sound * sound)
	{
		RemoveSoundByIndex(FindObjectIndexInVector(sound, sounds));
	}

	void SoundManager::RemoveSoundByIndex(size_t index)
	{
		DoRemoveObject(index, sounds, &SoundManager::OnObjectRemovedFromManager);
	}

	void SoundManager::RemoveSource(SoundSource * source)
	{
		RemoveSourceByIndex(FindObjectIndexInVector(source, sources));
	}

	void SoundManager::RemoveSourceByIndex(size_t index)
	{
		DoRemoveObject(index, sources, &SoundManager::OnObjectRemovedFromManager);
	}

	SoundSource * SoundManager::FindSource(char const * name)
	{
		return FindObjectByName(name, sources);
	}
	SoundSource const * SoundManager::FindSource(char const * name) const
	{
		return FindObjectByName(name, sources);
	}

	Sound * SoundManager::FindSound(char const * name)
	{
		return FindObjectByName(name, sounds);
	}

	Sound const * SoundManager::FindSound(char const * name) const
	{
		return FindObjectByName(name, sounds);
	}

	SoundSource * SoundManager::FindSourceByPath(FilePathParam const & path)
	{
		return FindObjectByPath(path, sources);
	}

	SoundSource const * SoundManager::FindSourceByPath(FilePathParam const & path) const
	{
		return FindObjectByPath(path, sources);
	}

	SoundCategory * SoundManager::FindCategory(char const * name)
	{
		return FindObjectByName(name, categories);
	}

	SoundCategory const * SoundManager::FindCategory(char const * name) const
	{
		return FindObjectByName(name, categories);
	}

	bool SoundManager::CanAddCategory(char const * name) const
	{
		return CanAddObject(name, [this](char const * n){return FindCategory(n);} );
	}

	bool SoundManager::CanAddSource(char const * name) const
	{
		return CanAddObject(name, [this](char const * n) {return FindSource(n); } );
	}

	bool SoundManager::CanAddSound(char const * name) const
	{
		return CanAddObject(name, [this](char const * n) {return FindSound(n); } );
	}

	void SoundManager::UpdateAllSourcesPerCategory(SoundCategory * category)
	{
		assert(category != nullptr);

		size_t count = sources.size();
		for (size_t i = 0; i < count; ++i)
		{
			SoundSource * source = sources[i].get();
			if (source == nullptr)
				continue;
				
			// notify the source that its default category is being destroyed
			auto it = std::find(source->default_categories.begin(), source->default_categories.end(), category);
			if (it != source->default_categories.end())
				source->default_categories.erase(it);
		}
	}

	void SoundManager::DestroyAllSoundPerCategory(SoundCategory * category)
	{
		assert(category != nullptr);

		for (size_t i = sounds.size(); i > 0; --i) // from back to beginning because we are about to suppress elements
		{
			size_t index = i - 1;

			Sound * sound = sounds[index].get();
			if (sound == nullptr)
				continue;
			if (!sound->IsOfCategory(category))
				continue;
			RemoveSoundByIndex(index);
		}
	}

	void SoundManager::DestroyAllSoundPerSource(SoundSource * source)
	{
		assert(source != nullptr);

		for (size_t i = sounds.size(); i > 0; --i) // from back to beginning because we are about to suppress elements
		{
			size_t index = i - 1;

			Sound * sound = sounds[index].get();
			if (sound == nullptr)
				continue;
			if (sound->source != source)
				continue;
			RemoveSoundByIndex(index);
		}
	}

	bool SoundManager::SetListenerPosition(glm::mat4 const & view, glm::vec3 const & speed)
	{
		if (!IsManagerStarted())
			return false;

		glm::vec3 position = view[3];
		glm::vec3 lookdir = view[2];
		glm::vec3 up = view[1];

		irrklang_engine->setListenerPosition(
			IrrklangTools::ToIrrklangVector(position),
			IrrklangTools::ToIrrklangVector(lookdir),
			IrrklangTools::ToIrrklangVector(speed),
			IrrklangTools::ToIrrklangVector(up));

		return true;
	}

	SoundCategory * SoundManager::AddCategory(char const * name)
	{
		// test whether a category with the given name could be inserted
		if (!CanAddCategory(name))
			return nullptr;
		// create the category
		SoundCategory * result = new SoundCategory();
		if (result == nullptr)
			return nullptr;
		// initialize the object
		result->sound_manager = this;
		if (name != nullptr)
			result->name = name;
		categories.push_back(result);

		return result;
	}

	SoundSource * SoundManager::AddSource(FilePathParam const & path, char const * name)
	{
		return SoundSourceLoader(this).LoadObject(path, name);
	}

	void SoundManager::UpdateAllSoundPausePerCategory(SoundCategory * category)
	{			
		size_t count = sounds.size();
		for (size_t i = 0; i < count; ++i)
		{
			Sound * sound = sounds[i].get();
			if (sound == nullptr)
				continue;
			if (category != nullptr && !sound->IsOfCategory(category))
				continue;
			sound->DoUpdateEffectivePause(sound->IsEffectivePaused());
		}
	}

	void SoundManager::UpdateAllSoundVolumePerCategory(SoundCategory * category)
	{
		size_t count = sounds.size();
		for (size_t i = 0; i < count; ++i)
		{
			Sound * sound = sounds[i].get();
			if (sound == nullptr)
				continue;
			if (category != nullptr && !sound->IsOfCategory(category))
				continue;
			sound->DoUpdateIrrklangVolume(sound->GetEffectiveVolume());
		}
	}

	void SoundManager::UpdateAllSoundPausePerSource(SoundSource * source)
	{
		size_t count = sounds.size();
		for (size_t i = 0; i < count; ++i)
		{
			Sound * sound = sounds[i].get();
			if (sound == nullptr)
				continue;
			if (source != nullptr && source != sound->source)
				continue;
			sound->DoUpdateIrrklangPause(sound->IsEffectivePaused());
		}
	}

	void SoundManager::UpdateAllSoundVolumePerSource(SoundSource * source)
	{
		size_t count = sounds.size();
		for (size_t i = 0; i < count; ++i)
		{
			Sound * sound = sounds[i].get();
			if (sound == nullptr)
				continue;
			if (source != nullptr && source != sound->source)
				continue;
			sound->DoUpdateIrrklangVolume(sound->GetEffectiveVolume());
		}
	}

	// JSON name policy
	//
	// "key": {}                  => anonymous object = error
	// "key": {"name":"myname"}   => name given by the member 'name'
	// "@key" : {}                => the key begins with @ => the name is the key (with @removed)

	SoundCategory * SoundManager::AddJSONCategory(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		// no anonymous category
		if (name == nullptr)
			return nullptr;
		// create the object
		SoundCategory * category = AddCategory(name);
		// initialize the new object
		if (category != nullptr)
			category->InitializeFromJSON(json, config_path);
		return category;
	}

	bool SoundManager::InitializeFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		// initialize the categories
		if (!LoadCategoriesFromConfiguration(json, config_path))
			return false;
		// Initialize the sources
		if (!LoadSourcesFromConfiguration(json, config_path))
			return false;
		return true;
	}

	bool SoundManager::LoadCategoriesFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		return LoadObjectsFromConfiguration(
			"categories",
			json,
			config_path,
			boost::mpl::false_(), // no [recurse] reading
			SoundCategoryLoader(this));
	}

	bool SoundManager::LoadSourcesFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path)
	{
		return LoadObjectsFromConfiguration(
			"sources",
			json,
			config_path,
			boost::mpl::true_(),
			SoundSourceLoader(this));
	}

	size_t SoundManager::GetSoundCount() const 
	{ 
		return sounds.size(); 
	}

	Sound * SoundManager::GetSound(size_t index)
	{
		if (index >= sounds.size())
			return nullptr;
		return sounds[index].get();
	}

	Sound const * SoundManager::GetSound(size_t index) const
	{
		if (index >= sounds.size())
			return nullptr;
		return sounds[index].get();
	}

	size_t SoundManager::GetCategoryCount() const 
	{ 
		return categories.size(); 
	}

	SoundCategory * SoundManager::GetCategory(size_t index)
	{
		if (index >= categories.size())
			return nullptr;
		return categories[index].get();
	}

	SoundCategory const * SoundManager::GetCategory(size_t index) const
	{
		if (index >= categories.size())
			return nullptr;
		return categories[index].get();
	}

	size_t SoundManager::GetSourceCount() const 
	{ 
		return sources.size(); 
	}

	SoundSource * SoundManager::GetSource(size_t index)
	{
		if (index >= sources.size())
			return nullptr;
		return sources[index].get();
	}

	SoundSource const * SoundManager::GetSource(size_t index) const
	{
		if (index >= sources.size())
			return nullptr;
		return sources[index].get();
	}

	// ==============================================================
	// SOUND SOURCE LOADER
	// ==============================================================

	SoundSource * SoundSourceLoader::LoadObject(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path) const
	{
		SoundSource * result = nullptr;
		// the entry has a reference to another file => recursive call
		std::string p;
		if (JSONTools::GetAttribute(json, "path", p))
		{
			FilePathParam path(p, config_path);

			result = LoadObject(path, name);
			if (result != nullptr)
				result->InitializeFromJSON(json, config_path);			
		}
		return result;
	}

	SoundSource * SoundSourceLoader::LoadObject(FilePathParam const & path, char const * name) const
	{
		// check for path
		if (!CheckResourcePath(path))
			return nullptr;
		// check for name
		if (!CheckResourceName(&path.GetResolvedPath(), name, nullptr))
			return nullptr;
		// create the source
		SoundSource * result = GenSourceObject(path, name);
		if (result != nullptr)
		{
			ApplyNameToLoadedResource(result);
			ApplyPathToLoadedResource(result);
			if (manager != nullptr)
				manager->sources.push_back(result);
		}
		return result;
	}

	SoundSource * SoundSourceLoader::GenSourceObject(FilePathParam const & path, char const * name) const
	{
		// get the irrklang engine
		irrklang::ISoundEngine * engine = manager->GetIrrklangEngine();
		if (engine == nullptr)
			return nullptr;
		// load the file
		Buffer<char> buffer = FileTools::LoadFile(path, false);
		if (buffer == nullptr)
			return nullptr;
		// create the source on irrklang side
		// XXX : we give filename even if the file is already loaded because it helps irrklangs to find the data format
		boost::filesystem::path const & resolved_path = path.GetResolvedPath();

		shared_ptr<irrklang::ISoundSource> irrklang_source = engine->addSoundSourceFromMemory(buffer.data, (irrklang::ik_s32)buffer.bufsize, resolved_path.string().c_str(), true);
		if (irrklang_source == nullptr)
			return nullptr;
		// insert the result
		SoundSource * result = new SoundSource();
		if (result == nullptr)
			return nullptr;
		// last initializations
		result->sound_manager = manager;
		result->irrklang_source = irrklang_source;
		
		return result;
	}

	bool SoundSourceLoader::IsPathAlreadyUsedInManager(FilePathParam const & path) const
	{
		return (manager->FindSourceByPath(path) != nullptr);
	}

	bool SoundSourceLoader::IsNameAlreadyUsedInManager(char const * in_name) const
	{
		return (manager->FindSource(in_name) != nullptr);
	}

	// ==============================================================
	// SOUND CATEGORY LOADER
	// ==============================================================

	SoundCategory * SoundCategoryLoader::LoadObject(char const * name, nlohmann::json const & json, boost::filesystem::path const & config_path) const
	{
		// no anonymous category
		if (name == nullptr)
			return nullptr;
		// create the object
		SoundCategory * result = manager->AddCategory(name);
		// initialize the new object
		if (result != nullptr)
			result->InitializeFromJSON(json, config_path);
		return result;
	}

	bool SoundCategoryLoader::IsNameAlreadyUsedInManager(char const * in_name) const
	{
		return (manager->FindCategory(in_name) != nullptr);
	}

}; // namespace chaos