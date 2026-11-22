#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	Sound * SoundSource::GenerateSound()
	{
		return new Sound;
	}

	Sound * SoundSource::Play(PlaySoundDesc const & play_desc, SoundCallbacks * in_callbacks)
	{
		// ensure we have access to manager
		if (!IsAttachedToManager())
			return nullptr;

		// ensure there are no name collision
		if (!sound_manager->CanAddSound((play_desc.sound_name.length() > 0) ? play_desc.sound_name.c_str() : nullptr))
			return nullptr;

		// compute required categories
		std::vector<SoundCategory *> categories = default_categories;

		for (std::string const & category_name : play_desc.category_names)
		{
			if (category_name.length() > 0)
			{
				SoundCategory * category = sound_manager->FindCategory(category_name.c_str());
				if (category != nullptr)
					if (std::find(categories.begin(), categories.end(), category) == categories.end())
						categories.push_back(category);
			}
		}

		for (SoundCategory * category : play_desc.categories)
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

			result->is_3D_sound = play_desc.IsSound3D();
			result->position = play_desc.position;
			result->velocity = play_desc.velocity;
			result->paused = play_desc.paused;
			result->looping = play_desc.looping;
			result->volume = std::clamp(play_desc.volume, 0.0f, 1.0f); ;
			result->callbacks = in_callbacks;

			if (play_desc.sound_name.length() > 0)
				result->name = play_desc.sound_name;

			// store the sound
			sound_manager->sounds.push_back(result);

			// play the sound
			result->DoPlaySound(play_desc);
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
		SoundObject::DoUpdateEffectivePause(effective_pause);
		if (!IsAttachedToManager())
			return;
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

	bool SoundSource::InitializeFromJSON(nlohmann::json const * json)
	{
		if (!SoundObject::InitializeFromJSON(json))
			return false;
		// update the default category
		std::vector<std::string> category_names;
		if (JSONTools::GetAttribute(json, "categories", category_names))
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

}; // namespace chaos