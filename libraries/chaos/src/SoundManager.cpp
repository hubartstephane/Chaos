#include <chaos/SoundManager.h>
#include <chaos/MathTools.h>
#include <chaos/Buffer.h>
#include <chaos/FileTools.h>

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

  void SoundCallbacks::OnFinished(SoundObject * in_object)
  {
    assert(in_object != nullptr);
  }

  void SoundCallbacks::OnRemovedFromManager(SoundObject * in_object)
  {
    assert(in_object != nullptr);
  }

  void SoundAutoCallbacks::OnFinished(SoundObject * in_object)
  {
    if (finished_func)
      finished_func(in_object);
  }

  void SoundAutoCallbacks::OnRemovedFromManager(SoundObject * in_object)
  {
    if (removed_func)
      removed_func(in_object);
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
    if (HasVolumeBlending())
    {
      float speed = 1.0f / blend_desc.blend_time;
      float delta_blend = delta_time * speed;

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
    }
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
    if (blend_desc.callbacks)
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
    paused = in_pause;
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

  void SoundObject::SetVolume(float in_volume)
  {
    volume = MathTools::Clamp(in_volume, 0.0f, 1.0f);
  }

  void SoundObject::Stop()
  {
    if (IsAttachedToManager()) // irrklang resources will be destroyed later
      RemoveFromManager();
  }

  bool SoundObject::StartBlend(BlendVolumeDesc const & desc, bool replace_older)
  {
    // only if attached
    if (!IsAttachedToManager())
      return false;
    // do not start a blend if there is a blend
    if (HasVolumeBlending() && !replace_older)
      return false;
    // ensure validity
    if (desc.blend_time < 0.0f)
      return false;
    if (desc.blend_type != BlendVolumeDesc::BLEND_IN && desc.blend_type != BlendVolumeDesc::BLEND_OUT)
      return false;
    // immediate blending (special case)
    if (desc.blend_time == 0.0f)
    {
      if (desc.blend_type == BlendVolumeDesc::BLEND_IN)
        blend_value = 1.0f;
      else if (desc.blend_type == BlendVolumeDesc::BLEND_OUT)
        blend_value = 0.0f;
      if (desc.callbacks)
        desc.callbacks->OnFinished(this);
    }
    else
      blend_desc = desc;

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

  // ==============================================================
  // SOURCE
  // ==============================================================

  Sound * SoundSource::GenerateSound()
  {
    return new Sound;
  }

  Sound * SoundSource::PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks)
  {
    // ensure we have access to manager
    if (!IsAttachedToManager())
      return nullptr;

    // ensure there are no name collision
    if (!sound_manager->CanAddSound((desc.sound_name.length() > 0) ? desc.sound_name.c_str() : nullptr))
      return nullptr;

    // ensure (if a category is required) that is correct
    SoundCategory * sound_category = desc.category;
    if (sound_category == nullptr)
    {
      if (desc.category_name.length() > 0)
      {
        sound_category = sound_manager->FindCategory(desc.category_name.c_str());
        if (sound_category == nullptr) // there is a category requirement by name that does not exist
          return nullptr;
      }
    }

    // test whether the category has the same manager as the source
    if (sound_category != nullptr && sound_category->sound_manager != sound_manager)
      return nullptr;

    // create the sound and initialize it
    Sound * result = GenerateSound();
    if (result != nullptr)
    {
      // initialize the newly created object (other values will be initialized in Sound::PlaySound(...)
      result->category = sound_category;
      result->sound_manager = sound_manager;
      result->source = this;
      sound_manager->sounds.push_back(result);
      // play the sound
      result->PlaySound(desc, in_callbacks);
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

  void SoundSource::Pause(bool in_pause)
  {
    if (IsPaused() == in_pause)
      return;
    SoundObject::Pause(in_pause);
    if (IsAttachedToManager())
      GetManager()->UpdateAllSoundPausePerSource(this);
  }

  void SoundSource::SetVolume(float in_volume)
  {
    if (GetVolume() == in_volume)
      return;
    SoundObject::SetVolume(in_volume);
    if (IsAttachedToManager())
      GetManager()->UpdateAllSoundVolumePerSource(this);
  }

  // ==============================================================
  // CATEGORY
  // ==============================================================

  void SoundCategory::OnRemovedFromManager()
  {
    sound_manager->DestroyAllSoundPerCategory(this);
    SoundObject::OnRemovedFromManager();
  }

  void SoundCategory::RemoveFromManager()
  {
    assert(IsAttachedToManager());
    sound_manager->RemoveCategory(this);
  }

  void SoundCategory::Pause(bool in_pause)
  {
    if (IsPaused() == in_pause)
      return;
    SoundObject::Pause(in_pause);
    if (IsAttachedToManager())
      GetManager()->UpdateAllSoundPausePerCategory(this);
  }

  void SoundCategory::SetVolume(float in_volume)
  {
    if (GetVolume() == in_volume)
      return;
    SoundObject::SetVolume(in_volume);
    if (IsAttachedToManager())
      GetManager()->UpdateAllSoundVolumePerCategory(this);
  }

  // ==============================================================
  // SOUND
  // ==============================================================

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

  void Sound::PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks)
  {
    // copy the data
    is_3D_sound = desc.IsSound3D();
    position = desc.position;
    velocity = desc.velocity;
    paused = desc.paused;
    looping = desc.looping;

    if (desc.sound_name.length() > 0)
      name = desc.sound_name;

    callbacks = in_callbacks;

    // start the sound
    DoPlaySound(desc);
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

    // play sound
    bool track = true;
    bool sound_effect = true;

    if (is_3D_sound)
    {
      irrklang_sound = irrklang_engine->play3D(
        source->irrklang_source.get(),
        IrrklangTools::ToIrrklangVector(position),
        looping,
        IsEffectivePaused(),
        track,
        sound_effect);

      if (irrklang_sound != nullptr)
        irrklang_sound->setVelocity(IrrklangTools::ToIrrklangVector(velocity));
    }
    else
    {
      irrklang_sound = irrklang_engine->play2D(
        source->irrklang_source.get(),
        looping,
        IsEffectivePaused(),
        track,
        sound_effect);
    }

    if (desc.blend_in_time > 0.0f)
    {
      BlendVolumeDesc blend_desc;
      blend_desc.blend_type = BlendVolumeDesc::BLEND_IN;
      blend_desc.blend_time = desc.blend_in_time;
      blend_value = 0.0f;
      StartBlend(blend_desc);
    }
    DoUpdateVolume();

    return (irrklang_sound == nullptr);
  }

  void Sound::TickObject(float delta_time)
  {
    // update the volume
    SoundObject::TickObject(delta_time);
    // early exit
    if (!IsAttachedToManager())
      return;
    // apply volume on the irrklang object
    DoUpdateVolume();
  }

  void Sound::SetVolume(float in_volume)
  {
    if (GetVolume() == in_volume)
      return;
    SoundObject::SetVolume(in_volume);
    DoUpdateVolume();
  }

  void Sound::Pause(bool in_pause)
  {
    if (IsPaused() == in_pause)
      return;
    SoundObject::Pause(in_pause);
    DoUpdatePause();
  }

  void Sound::DoUpdateVolume()
  {
    if (irrklang_sound != nullptr)
      irrklang_sound->setVolume((irrklang::ik_f32)GetEffectiveVolume());
  }

  void Sound::DoUpdatePause()
  {
    if (irrklang_sound != nullptr)
      irrklang_sound->setIsPaused(IsEffectivePaused());
  }

  // ==============================================================
  // MANAGER
  // ==============================================================

  irrklang::ISoundEngine * SoundManager::GetIrrklangEngine()
  {
    return irrklang_engine.get();
  }

  bool SoundManager::IsManagerStarted() const
  {
    return (irrklang_engine != nullptr);
  }

  bool SoundManager::StartManager()
  {
    // exit if manager is already started
    if (IsManagerStarted())
      return true;
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

  bool SoundManager::StopManager()
  {
    // exit if manager is already stopped
    if (!IsManagerStarted())
      return false;

    // empty the managed objects list
    DetachAllObjectsFromList(sounds); // destroy sounds first to make other list destructions faster
    DetachAllObjectsFromList(categories);
    DetachAllObjectsFromList(sources);

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

  void SoundManager::RemoveCategory(SoundCategory * in_category)
  {
    RemoveCategory(FindObjectIndexInVector(in_category, categories));
  }

  void SoundManager::RemoveCategory(size_t index)
  {
    DoRemoveObject(index, categories);
  }

  void SoundManager::RemoveSound(Sound * in_sound)
  {
    RemoveSound(FindObjectIndexInVector(in_sound, sounds));
  }

  void SoundManager::RemoveSound(size_t index)
  {
    DoRemoveObject(index, sounds);
  }

  void SoundManager::RemoveSource(SoundSource * in_source)
  {
    RemoveSource(FindObjectIndexInVector(in_source, sources));
  }

  void SoundManager::RemoveSource(size_t index)
  {
    DoRemoveObject(index, sources);
  }

  SoundSource * SoundManager::FindSource(char const * name)
  {
    return FindObjectByName<SoundSource>(name, sources);
  }
  SoundSource const * SoundManager::FindSource(char const * name) const
  {
    return FindObjectByName<SoundSource>(name, sources);
  }

  Sound * SoundManager::FindSound(char const * name)
  {
    return FindObjectByName<Sound>(name, sounds);
  }

  Sound const * SoundManager::FindSound(char const * name) const
  {
    return FindObjectByName<Sound>(name, sounds);
  }

  SoundSource * SoundManager::FindSourceByPath(FilePathParam const & in_path)
  {
    return FindObjectByPath<SoundSource>(in_path, sources);
  }

  SoundSource const * SoundManager::FindSourceByPath(FilePathParam const & in_path) const
  {
    return FindObjectByPath<SoundSource>(in_path, sources);
  }

  SoundCategory * SoundManager::FindCategory(char const * name)
  {
    return FindObjectByName<SoundCategory>(name, categories);
  }

  SoundCategory const * SoundManager::FindCategory(char const * name) const
  {
    return FindObjectByName<SoundCategory>(name, categories);
  }

  bool SoundManager::CanAddCategory(char const * in_name) const
  {
    return CanAddObject<SoundCategory>(in_name, &SoundManager::FindCategory);
  }

  bool SoundManager::CanAddSource(char const * in_name) const
  {
    return CanAddObject<SoundSource>(in_name, &SoundManager::FindSource);
  }

  bool SoundManager::CanAddSound(char const * in_name) const
  {
    return CanAddObject<Sound>(in_name, &SoundManager::FindSound);
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
      if (sound->category != category)
        continue;
      RemoveSound(index);
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
      RemoveSound(index);
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

  SoundCategory * SoundManager::AddCategory(char const * in_name)
  {
    // test whether a category with the given name could be inserted
    if (!CanAddCategory(in_name))
      return nullptr;
    // create the category
    SoundCategory * result = new SoundCategory();
    if (result == nullptr)
      return nullptr;
    // initialize the object
    result->sound_manager = this;
    if (in_name != nullptr)
      result->name = in_name;
    categories.push_back(result);

    return result;
  }

  SoundSource * SoundManager::AddSource(FilePathParam const & in_path)
  {
    boost::filesystem::path const resolved_path = in_path.GetResolvedPath();
    return AddSource(in_path, resolved_path.string().c_str());
  }

  SoundSource * SoundManager::AddSource(FilePathParam const & in_path, char const * in_name)
  {
    // test whether a source with the given name could be inserted
    if (!CanAddSource(in_name))
      return nullptr;
    // find a simple source with the given path
    if (FindSourceByPath(in_path) != nullptr)
      return nullptr;
    // get the irrklang engine
    irrklang::ISoundEngine * engine = GetIrrklangEngine();
    if (engine == nullptr)
      return nullptr;
    // load the file
    Buffer<char> buffer = FileTools::LoadFile(in_path, false);
    if (buffer == nullptr)
      return nullptr;
    // create the source on irrklang side
    // XXX : we give filename even if the file is already loaded because it helps irrklangs to find the data format
    boost::filesystem::path const & resolved_path = in_path.GetResolvedPath();

    boost::intrusive_ptr<irrklang::ISoundSource> irrklang_source = engine->addSoundSourceFromMemory(buffer.data, (irrklang::ik_s32)buffer.bufsize, resolved_path.string().c_str(), true);
    if (irrklang_source == nullptr)
      return nullptr;
    // insert the result
    SoundSource * result = DoAddSource(new SoundSource(), in_name);
    if (result == nullptr)
      return nullptr;
    // last initializations
    result->irrklang_source = irrklang_source;
    result->path = resolved_path;

    return result;
  }

  void SoundManager::UpdateAllSoundPausePerCategory(SoundCategory * category)
  {
    size_t count = sounds.size();
    for (size_t i = 0; i < count; ++i)
    {
      Sound * sound = sounds[i].get();
      if (sound == nullptr)
        continue;
      if (category != nullptr && category != sound->category)
        continue;
      sound->DoUpdatePause();
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
      if (category != nullptr && category != sound->category)
        continue;
      sound->DoUpdateVolume();
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
      sound->DoUpdatePause();
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
      sound->DoUpdateVolume();
    }
  }

  bool SoundManager::InitializeFromConfiguration(nlohmann::json const & configuration)
  {

    return true;
  }

}; // namespace chaos