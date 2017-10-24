#include <chaos/SoundManager.h>
#include <chaos/MathTools.h>
#include <chaos/Buffer.h>
#include <chaos/FileTools.h>

namespace chaos
{

  //
  // SoundBaseObject methods
  //

  SoundBaseObject::SoundBaseObject(class SoundManager * in_sound_manager) :
    sound_manager(in_sound_manager)
  {
    assert(sound_manager != nullptr);
  }

  void SoundBaseObject::DetachFromManager()
  {
    assert(IsAttachedToManager());
    sound_manager = nullptr;
  }

  bool SoundBaseObject::IsAttachedToManager() const
  {
    return (sound_manager != nullptr);
  }

  //
  // SoundVolumeObject methods
  //

  SoundVolumeObject::SoundVolumeObject(class SoundManager * in_sound_manager) :
    SoundBaseObject(in_sound_manager)
  {

  }

  bool SoundVolumeObject::IsPendingKill() const
  {
    return pending_kill;
  }

  float SoundVolumeObject::GetVolume() const
  {
    if (!IsAttachedToManager())
      return 0.0f;
    return volume;
  }

  void SoundVolumeObject::SetVolume(float in_volume)
  {
    if (!IsAttachedToManager())
      return;
    volume = chaos::MathTools::Clamp(in_volume, 0.0f, 1.0f);
  }

  float SoundVolumeObject::GetEffectiveVolume() const
  {
    assert(IsAttachedToManager()); // should never be called elsewhere

    return volume * blend_volume_factor;
  }

  void SoundVolumeObject::UpdateBlendFactor(float delta_time)
  {
    assert(IsAttachedToManager()); // should never be called elsewhere

    if (blend_volume_type == BLEND_VOLUME_IN)
    {
      if (blend_volume_time <= 0.0f) // immediate blending
        blend_volume_factor = 1.0f;
      else
        blend_volume_factor = chaos::MathTools::Clamp(blend_volume_factor + delta_time / blend_volume_time, 0.0f, 1.0f);
    }
    else if (blend_volume_type == BLEND_VOLUME_OUT)
    {
      if (blend_volume_time <= 0.0f) // immediate blending
        blend_volume_factor = 0.0f;
      else
        blend_volume_factor = chaos::MathTools::Clamp(blend_volume_factor - delta_time / blend_volume_time, 0.0f, 1.0f);
    }
  }

  // fullrange_blend_volume_time == true
  // -----------------------------------
  //    blend_volume_time = time to go from 0 to 1
  //
  //
  // fullrange_blend_volume_time == false
  // ------------------------------------
  //    blend_volume_time = time to go from current_value to 1

  bool SoundVolumeObject::StartBlendIn(float in_blend_volume_time, bool in_fullrange_blend_volume_time)
  {
    if (!IsAttachedToManager())
      return false;
    if (IsPendingKill()) // do not start a blend if there is a pending kill => the object must continue its blending out effect if any
      return false;

    blend_volume_type = BLEND_VOLUME_IN;
    if (in_fullrange_blend_volume_time)
      blend_volume_time = in_blend_volume_time;
    else
    {
      if (blend_volume_factor == 1.0f)
        blend_volume_time = 1.0f; // well, a non null value for division. It is useless because the wanted final value is already reached
      else
        blend_volume_time = in_blend_volume_time / (1.0f - blend_volume_factor);
    }
    return true;
  }

  bool SoundVolumeObject::StartBlendOut(float in_blend_volume_time, bool in_fullrange_blend_volume_time) // blend_time = 0 for immediate change at next tick
  {
    if (!IsAttachedToManager())
      return false;
    if (IsPendingKill()) // do not start a blend if there is a pending kill => the object must continue its blending out effect if any
      return false;

    blend_volume_type = BLEND_VOLUME_OUT;
    if (in_fullrange_blend_volume_time)
      blend_volume_time = in_blend_volume_time;
    else
    {
      if (blend_volume_factor == 0.0f)
        blend_volume_time = 1.0f; // well, a non null value for division. It is useless because the wanted final value is already reached
      else
        blend_volume_time = in_blend_volume_time / (blend_volume_factor - 0.0f);
    }
    return true;
  }

  void SoundVolumeObject::Tick(float delta_time)
  {
    assert(IsAttachedToManager()); // should never be called elsewhere

    UpdateBlendFactor(delta_time);
  }

  bool SoundVolumeObject::IsFinished() const
  {
    assert(IsAttachedToManager()); // should never be called elsewhere

    if (IsPendingKill())
    {
      if (blend_volume_type == BLEND_VOLUME_NONE)
        return true;
      assert(blend_volume_type == BLEND_VOLUME_OUT); // cannot have BLEND IN and pending kill
      if (blend_volume_factor <= 0.0f)
        return true;
    }
    return false;
  }

  bool SoundVolumeObject::StopAndKill(float in_blendout_time, bool in_fullrange_blend_volume_time) // the object will only be destroyed after the tick
  {
    if (!IsAttachedToManager())
      return false;
    if (IsPendingKill()) // already pending kill
      return false;

    if (in_blendout_time > 0.0f)
    {
      StartBlendOut(in_blendout_time, in_fullrange_blend_volume_time); // XXX : order is important
      pending_kill = true;
    }
    else
      RemoveFromManager(); // immediate removal  
  }

  //
  // SoundCategory methods
  //

  SoundCategory::SoundCategory(class SoundManager * in_sound_manager) :
    SoundVolumeObject(in_sound_manager)
  {

  }

  SoundCategory::~SoundCategory()
  {

  }

  void SoundCategory::DetachFromManager()
  {
    sound_manager->DestroyAllSoundPerCategory(this);
    SoundBaseObject::DetachFromManager();
  }

  
  void SoundCategory::RemoveFromManager()
  {
    assert(IsAttachedToManager());
    sound_manager->RemoveSoundCategory(this);
  }

  //
  // SoundSource methods
  // 

  SoundSource::SoundSource(class SoundManager * in_sound_manager) :
    SoundBaseObject(in_sound_manager)
  {

  }

  SoundSource::~SoundSource()
  {

  }

  void SoundSource::RemoveFromManager()
  {
    assert(IsAttachedToManager());
    sound_manager->RemoveSoundSource(this);
  }

  void SoundSource::DetachFromManager()
  {
    sound_manager->DestroyAllSoundPerSource(this);
    irrklang_source = nullptr;
    SoundBaseObject::DetachFromManager();
  }

  float SoundSource::GetPlayLength() const
  {
    if (!IsAttachedToManager())
      return 0.0f;

	assert(irrklang_source != nullptr);

    irrklang::ik_u32 milliseconds = irrklang_source->getPlayLength();
    if (milliseconds < 0)
      return -1.0f;
    return 1000.0f * (float)milliseconds;
  }

  bool SoundSource::IsManualLoopRequired() const
  {
    assert(IsAttachedToManager()); // should never be called elsewhere

    if (loop_info.blend_time <= 0.0f) // no blend => no manual looping 
      return false;

    float length = GetPlayLength();
    if (length < 0.0f)
      return false;    // if we cannot determine the length (streaming), we cannot use manual looping

    return true;
  }

  irrklang::ISound * SoundSource::DoPlayIrrklangSound(PlaySoundDesc const & desc, bool in_looping, bool in_paused)
  {
    assert(IsAttachedToManager()); // should never be called elsewhere

    bool looping = in_looping;
    bool paused = in_paused;
    bool track = true;
    bool sound_effect = true;

    return sound_manager->irrklang_engine->play2D(
      irrklang_source.get(),
      looping,
      paused,
      track,
      sound_effect);
  }

  irrklang::ISound * SoundSource::DoPlayIrrklangSound(Play3DSoundDesc const & desc, bool in_looping, bool in_paused)
  {
    assert(IsAttachedToManager()); // should never be called elsewhere

    bool looping = in_looping;
    bool paused = in_paused;
    bool track = true;
    bool sound_effect = true;

    irrklang::ISound * result = sound_manager->irrklang_engine->play3D(
      irrklang_source.get(),
      Sound::ConvertVectorToIrrklang(desc.position),
      looping,
      paused,
      track,
      sound_effect);

    if (result != nullptr)
      result->setVelocity(Sound::ConvertVectorToIrrklang(desc.speed));

    return result;
  }

  Sound * SoundSource::PlaySound(PlaySoundDesc const & desc)
  {
    if (!IsAttachedToManager())
      return nullptr;
    return DoPlaySound(desc);
  }

  Sound * SoundSource::Play3DSound(Play3DSoundDesc const & desc)
  {
    if (sound_manager == nullptr) // detached from manager
      return nullptr;

    Sound * result = DoPlaySound(desc);
    if (result != nullptr)
    {
      result->is_3D_sound = true;
      result->position = desc.position;
      result->speed = desc.speed;
    }
    return result;
  }

  //
  // Sound methods
  //

  Sound::Sound(class SoundManager * in_sound_manager) :
    SoundVolumeObject(in_sound_manager)
  {

  }

  Sound::~Sound()
  {
  }

  void Sound::RemoveFromManager()
  {
    assert(IsAttachedToManager());
    sound_manager->RemoveSound(this);
  }

  void Sound::DetachFromManager()
  {
    irrklang_sound = nullptr;
    irrklang_loop_sound = nullptr;
    SoundBaseObject::DetachFromManager();
  }

  irrklang::vec3df Sound::ConvertVectorToIrrklang(glm::vec3 const & src)
  {
    irrklang::vec3df result;
    result.X = (float)src.x;
    result.Y = (float)src.y;
    result.Z = (float)src.z;
    return result;
  }

  void Sound::SetPosition(glm::vec3 const & in_position)
  {
    if (!IsAttachedToManager())
      return;
    if (!is_3D_sound)
      return;

    position = in_position;
    if (irrklang_sound != nullptr)
      irrklang_sound->setPosition(ConvertVectorToIrrklang(in_position));
    if (irrklang_loop_sound != nullptr)
      irrklang_loop_sound->setPosition(ConvertVectorToIrrklang(in_position));
  }

  glm::vec3 Sound::GetPosition() const
  {
    if (!IsAttachedToManager())
      return glm::vec3(0.0f, 0.0f, 0.0f);
    if (!is_3D_sound)
      return glm::vec3(0.0f, 0.0f, 0.0f);
    return position;
  }

  void Sound::SetSpeed(glm::vec3 const & in_speed)
  {
    if (!IsAttachedToManager())
      return;
    if (!is_3D_sound)
      return;

    speed = in_speed;
    if (irrklang_sound != nullptr)
      irrklang_sound->setVelocity(ConvertVectorToIrrklang(in_speed));
    if (irrklang_loop_sound != nullptr)
      irrklang_loop_sound->setVelocity(ConvertVectorToIrrklang(in_speed));
  }

  glm::vec3 Sound::GetSpeed() const
  {
    if (!IsAttachedToManager())
      return glm::vec3(0.0f, 0.0f, 0.0f);
    if (!is_3D_sound)
      return glm::vec3(0.0f, 0.0f, 0.0f);
    return speed;
  }

  bool Sound::Is3DSound() const
  {
    return is_3D_sound;
  }

  bool Sound::IsLooping() const
  {
    return looping;
  }

  float Sound::GetEffectiveVolume() const
  {
    assert(IsAttachedToManager()); // should never be called elsewhere

    float result = SoundVolumeObject::GetEffectiveVolume();
    if (category != nullptr)
      result *= category->GetEffectiveVolume();
    return result;
  }

  bool Sound::IsFinished() const
  {
    if (SoundVolumeObject::IsFinished()) // finished due to blend out
      return true;
    if (IsLooping()) // a looping sound is never finished
      return false;
    return irrklang_sound->isFinished(); // ignore manual looping system (it is looping so never finishing)
  }

  void Sound::Tick(float delta_time)
  {
    SoundVolumeObject::Tick(delta_time); // update blend volumes
    if (IsFinished())
      return;

    if (source == nullptr) // whatever happens next, we cannot due anything with that sound
      return;

    if (!IsLooping() || !source->IsManualLoopRequired())
      return;



  }


  //
  // SoundManager methods
  //

  void SoundManager::Tick(float delta_time)
  {
    // tick all categories
    for (int i = categories.size() - 1; i >= 0; ++i)
    {
      SoundCategory * category = categories[i].get();
      if (category == nullptr)
        continue;
      category->Tick(delta_time);

      if (category->IsFinished())
        RemoveSoundCategory(i);
    }

    // tick all sounds
    for (int i = sounds.size() - 1; i >= 0; ++i)
    {
      Sound * sound = sounds[i].get();
      if (sound == nullptr)
        continue;
      sound->Tick(delta_time);

      if (sound->IsFinished())
        RemoveSound(i);
    }
  }

  Sound * SoundManager::FindSound(char const * name)
  {
    return FindSoundObject<Sound>(name, sounds);
  }

  SoundCategory * SoundManager::FindSoundCategory(char const * name)
  {
    return FindSoundObject<SoundCategory>(name, categories);
  }

  SoundSource * SoundManager::FindSoundSource(char const * name)
  {
    return FindSoundObject<SoundSource>(name, sources);
  }

  SoundSource * SoundManager::AddSource(char const * in_filename, char const * in_name, SoundLoopInfo in_loop_info)
  {
    assert(in_filename != nullptr);

    if (irrklang_engine != nullptr) // not initialized engine
      return nullptr;

    if (in_name == nullptr) // by default the name of a source is its filepath
      in_name = in_filename;

    if (FindSoundSource(in_name) != nullptr) // name already existing
      return nullptr;

    chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(in_filename, false); // load the file
    if (buffer == nullptr)
      return nullptr;

    // create the source on irrklang side
    // XXX : we give filename even if the file is already loaded because it helps irrklangs to find the data format
    irrklang::ISoundSource * irrklang_source = irrklang_engine->addSoundSourceFromMemory(buffer.data, buffer.bufsize, in_filename, true);
    if (irrklang_source == nullptr)
      return nullptr;

    SoundSource * result = new SoundSource(this);
    if (result == nullptr)
      return nullptr;

    result->name = in_name;
    result->irrklang_source = irrklang_source;
    result->loop_info = in_loop_info;

    irrklang_source->drop(); // now that we have a reference on the resource, we can unref it

    sources.push_back(result);

    return result;
  }

  bool SoundManager::StartManager()
  {
    StopManager(); // destroy previous references

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
    DetachAllObjectsFromList(sounds); // destroy sounds first to make other list destructions faster
    sounds.clear();

    DetachAllObjectsFromList(categories);
    categories.clear();

    DetachAllObjectsFromList(sources);
    sources.clear();

    irrklang_devices = nullptr;
    irrklang_engine = nullptr;

    return true;
  }

  void SoundManager::ReplaceSoundCategory(SoundCategory * new_category, SoundCategory * old_category)
  {
    int count = sounds.size();
    for (int i = 0; i < count; ++i)
    {
      Sound * sound = sounds[i].get();
      if (sound == nullptr)
        continue;
      if (sound->category == old_category) // replace the category
        sound->category = new_category;
    }
  }

  void SoundManager::DestroyAllSoundPerCategory(SoundCategory * category)
  {
    assert(category != nullptr);

    for (int i = sounds.size() - 1; i >= 0; --i) // from back to beginning because we are about to suppress elements
    {
      Sound * sound = sounds[i].get();
      if (sound == nullptr)
        continue;
      if (sound->category != category)
        continue;
	  RemoveSound(i);
    }
  }

  void SoundManager::DestroyAllSoundPerSource(SoundSource * source)
  {
    assert(source != nullptr);

    for (int i = sounds.size() - 1; i >= 0; --i) // from back to beginning because we are about to suppress elements
    {
      Sound * sound = sounds[i].get();
      if (sound == nullptr)
        continue;
      if (sound->source != source)
        continue;
	  RemoveSound(i);
    }
  }

  template<typename T, typename U>
  static int GetObjectIndexInVector(T * object, U const & vector)
  {
    assert(object != nullptr);
    int count = vector.size();
    for (int i = 0; i < count; ++i)
      if (vector[i].get() == object)
        return i;
    return -1;
  }

  void SoundManager::RemoveSoundCategory(SoundCategory * sound_category)
  {
    RemoveSoundCategory(GetObjectIndexInVector(sound_category, categories));
  }

  void SoundManager::RemoveSoundCategory(int index)
  {
    DoRemoveSoundObject(index, categories);
  }

  void SoundManager::RemoveSound(Sound * sound)
  {
	RemoveSound(GetObjectIndexInVector(sound, sounds));
  }

  void SoundManager::RemoveSound(int index)
  {
    DoRemoveSoundObject(index, sounds);
  }

  void SoundManager::RemoveSoundSource(SoundSource * source)
  {
	  RemoveSoundSource(GetObjectIndexInVector(source, sources));
  }

  void SoundManager::RemoveSoundSource(int index)
  {
    DoRemoveSoundObject(index, sources);
  }

}; // namespace chaos
