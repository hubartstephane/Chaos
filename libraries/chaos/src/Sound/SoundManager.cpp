#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	irrklang::vec3df ToIrrklangVector(glm::vec3 const& src)
	{
		irrklang::vec3df result;
		result.X = (float)src.x;
		result.Y = (float)src.y;
		result.Z = (float)src.z;
		return result;
	}

	irrklang::ISoundEngine * SoundManager::GetIrrklangEngine()
	{
		return irrklang_engine.get();
	}

	bool SoundManager::DoStartManager()
	{
		// super method
		if (!ResourceManager::DoStartManager())
			return false;
		// read the properties
		if (!ReadConfigurableProperties(ReadConfigurablePropertiesContext::Initialization, false))
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
		// XXX : note on 3D sounds
		//       3D sounds for irrklang have no finite limit (where the volume becomes 0.0f)
		//       instead you have a MinDistance & MaxDistance
		//       after MaxDistance the volume does not decrease any more and is greater than 0
		//       for our purpose, we do not want to have infinite distance sound
		//
		//       we can use   setRolloffFactor(...) to indicate to irrklang how volume decrease with distance
		//       (0 means that volume does not decrease at all => we can compute ourselves how to fixe the volume,
		//       and so we can choose a finite limite after which sound can not be heard anymore)
		//
		//       So, why still use 3D sounds ???
		//       => because irrklang use your output devices to spacialize the sound
		//          sounds on the left are played on the left device :) and so on
		//
		//       It's a shame but you cannot set a    setRolloffFactor(...) factor     per sound
		//       It is for the whole engine !
		irrklang_engine->setRolloffFactor(0.0f);
		// suppress the extra reference
		irrklang_engine->drop();

		// other initializations
		return OnInitialize(GetJSONReadConfiguration());
	}

	bool SoundManager::DoStopManager()
	{
		// empty the managed objects list (destroy sounds first to make other list destructions faster)
		RemoveAllObjectsFromList(sounds, &SoundManager::OnObjectRemovedFromManager);
		RemoveAllObjectsFromList(categories, &SoundManager::OnObjectRemovedFromManager);
		RemoveAllObjectsFromList(sources, &SoundManager::OnObjectRemovedFromManager);

		// clean irrklang resources
		irrklang_devices = nullptr;
		irrklang_devices = nullptr;
		irrklang_engine = nullptr;

		// super method
		ResourceManager::DoStopManager();

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

	SoundSource * SoundManager::FindSource(ObjectRequest request)
	{
		return request.FindObject(sources);
	}
	SoundSource const * SoundManager::FindSource(ObjectRequest request) const
	{
		return request.FindObject(sources);
	}

	Sound * SoundManager::FindSound(ObjectRequest request)
	{
		return request.FindObject(sounds);
	}

	Sound const * SoundManager::FindSound(ObjectRequest request) const
	{
		return request.FindObject(sounds);
	}

	SoundSource * SoundManager::FindSourceByPath(FilePathParam const & path)
	{
		return FindObjectByPath(path, sources);
	}

	SoundSource const * SoundManager::FindSourceByPath(FilePathParam const & path) const
	{
		return FindObjectByPath(path, sources);
	}

	SoundCategory * SoundManager::FindCategory(ObjectRequest request)
	{
		return request.FindObject(categories);
	}

	SoundCategory const * SoundManager::FindCategory(ObjectRequest request) const
	{
		return request.FindObject(categories);
	}

	bool SoundManager::CanAddCategory(ObjectRequest request) const
	{
		return CanAddObject(request, [this](ObjectRequest r){return (FindCategory(r) == nullptr);} );
	}

	bool SoundManager::CanAddSource(ObjectRequest request) const
	{
		return CanAddObject(request, [this](ObjectRequest r) {return (FindSource(r) == nullptr); } );
	}

	bool SoundManager::CanAddSound(ObjectRequest request) const
	{
		return CanAddObject(request, [this](ObjectRequest r) {return (FindSound(r) == nullptr); } );
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

	glm::vec3 SoundManager::GetListenerPosition() const
	{
		return listener_transform[3];
	}

	glm::vec3 SoundManager::GetListenerVelocity() const
	{
		return listener_velocity;
	}

	bool SoundManager::SetListenerPosition(glm::vec3 const & position, glm::vec3 const & velocity)
	{
		if (!IsManagerStarted())
			return false;

		if (position != GetListenerPosition() || velocity != GetListenerVelocity())
		{
			// update the internal values
			listener_transform = glm::translate(position);
			listener_velocity = velocity;

			glm::vec3 pos     = listener_transform[3];
			glm::vec3 lookdir = listener_transform[2];
			glm::vec3 up      = listener_transform[1];

			irrklang_engine->setListenerPosition(
				ToIrrklangVector(pos),
				ToIrrklangVector(lookdir),
				ToIrrklangVector(velocity),
				ToIrrklangVector(up));

			// update all 3D sounds volume
			size_t count = sounds.size();
			for (size_t i = 0; i < count; ++i)
			{
				Sound * sound = sounds[i].get();
				if (sound != nullptr && !sound->IsPendingKill() && sound->is_3D_sound)
					sound->DoUpdateEffectiveVolume(sound->GetEffectiveVolume());
			}
		}
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
			sound->DoUpdateEffectiveVolume(sound->GetEffectiveVolume());
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
			sound->DoUpdateEffectiveVolume(sound->GetEffectiveVolume());
		}
	}

	bool SoundManager::OnConfigurationChanged(JSONReadConfiguration config)
	{
		return true; // do not call super method
	}

	bool SoundManager::OnInitialize(JSONReadConfiguration config)
	{
		// initialize the categories
		if (!LoadCategories(config.default_config))
			return false;
		// Initialize the sources
		if (!LoadSources(config.default_config))
			return false;
		return true;
	}

	bool SoundManager::LoadCategories(nlohmann::json const * config)
	{
		return LoadObjects<false>( // no [recurse] reading
			"categories",
			config,
			SoundCategoryLoader(this));
	}

	bool SoundManager::LoadSources(nlohmann::json const * config)
	{
		return LoadObjects<true>(
			"sources",
			config,
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

}; // namespace chaos