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
    if (callbacks != nullptr)
      callbacks->OnRemovedFromManager(this);
  }

  bool SoundBaseObject::IsAttachedToManager() const
  {
    return (sound_manager != nullptr);
  }

  void SoundBaseObject::SetCallbacks(SoundObjectCallbacks * in_callbacks)
  {
    callbacks = in_callbacks;
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

    if (blend_volume_factor == 1.0f) // max value already reached. Do nothing
      return true;

    blend_volume_type = BLEND_VOLUME_IN;
    if (in_fullrange_blend_volume_time)
      blend_volume_time = in_blend_volume_time;
    else
      blend_volume_time = in_blend_volume_time / (1.0f - blend_volume_factor);

    return true;
  }

  bool SoundVolumeObject::StartBlendOut(float in_blend_volume_time, bool in_fullrange_blend_volume_time) // blend_time = 0 for immediate change at next tick
  {
    if (!IsAttachedToManager())
      return false;
    if (IsPendingKill()) // do not start a blend if there is a pending kill => the object must continue its blending out effect if any
      return false;

    if (blend_volume_factor == 0.0f) // min value already reached. Do nothing
      return true;

    blend_volume_type = BLEND_VOLUME_OUT;
    if (in_fullrange_blend_volume_time)
      blend_volume_time = in_blend_volume_time;
    else
      blend_volume_time = in_blend_volume_time / (blend_volume_factor - 0.0f);
    return true;
  }

  bool SoundVolumeObject::StopAndKill(float in_blendout_time, bool in_fullrange_blend_volume_time) // the object will only be destroyed after the tick
  {
    if (!IsAttachedToManager())
      return false;
    if (IsPendingKill()) // already pending kill
      return false;

    if (in_blendout_time > 0.0f && blend_volume_factor > 0.0f) // do not want immediate blending. Min blend value still not reached
    {
      StartBlendOut(in_blendout_time, in_fullrange_blend_volume_time); // XXX : order is important
      pending_kill = true;
    }
    else
    {
      if (callbacks != nullptr)
        callbacks->OnFinished(this);
      RemoveFromManager(); // immediate removal  
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

  SoundCategory * SoundCategory::CloneCategoryAndStop(float in_blend_volume_time, bool in_fullrange_blend_volume_time)
  {
    if (!IsAttachedToManager())
      return nullptr;
    if (IsPendingKill())
      return nullptr;

    SoundCategory * Result = sound_manager->DoAddCategory(nullptr, this);
    if (Result != nullptr)
    {
      sound_manager->ReplaceSoundCategory(Result, this);
      Result->StopAndKill(in_blend_volume_time, in_fullrange_blend_volume_time);
    }
    return Result;
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

  SoundLoopInfoExt SoundSource::GetClampedLoopInfoExt(SoundLoopInfo const & in_loop_info) const
  {
    SoundLoopInfoExt result;
    result.start      = in_loop_info.start;
    result.end        = in_loop_info.end;
    result.blend_time = in_loop_info.blend_time;

    // length unknown : no manual blending possible
    result.play_length = DoGetPlayLength();
    if (result.play_length < 0.0f)
      return result;

    // clamp the result according to the play length
    if (result.start < 0.0f)
      result.start = 0.0f;
    else if (result.start > result.play_length)
      result.start = result.play_length;

    if (result.end < 0.0f || result.end > result.play_length)
      result.end = result.play_length;

    // start and end equals => no blending possible
    if (result.start == result.end)
    {
      result.blend_time = 0.0f;
      return result;
    }
    
    // sort start and end
    if (result.start > result.end)
      std::swap(result.start, result.end);

    // clamp blend time
    if (result.blend_time > 0.0f) // blend after end
    {
      float clamped_end = min(result.end + result.blend_time, result.play_length);
      result.blend_time = (clamped_end - result.end);
    }
    else if (result.blend_time < 0.0f) // blend before start
    {
      float clamped_start = max(result.start + result.blend_time, 0.0f);
      result.blend_time = (result.start - clamped_start);
    }

    return result;
  }

  void SoundSource::RemoveFromManager()
  {
    assert(IsAttachedToManager());
    sound_manager->RemoveSoundSource(this);
  }

  void SoundSource::DetachFromManager()
  {
    sound_manager->DestroyAllSoundPerSource(this);

    if (irrklang_source != nullptr)
    {
      if (sound_manager->irrklang_engine != nullptr)
        sound_manager->irrklang_engine->removeSoundSource(irrklang_source.get());
      irrklang_source = nullptr;
    }

    SoundBaseObject::DetachFromManager();
  }

  float SoundSource::GetPlayLength() const
  {
    if (!IsAttachedToManager())
      return 0.0f;
    if (irrklang_source == nullptr) // no sound, no duration
      return -1.0f;
    return loop_info_ext.play_length;
  }

  float SoundSource::DoGetPlayLength() const
  {
    if (!IsAttachedToManager())
      return 0.0f;
    if (irrklang_source == nullptr) // no sound, no duration
      return -1.0f;

    irrklang::ik_u32 milliseconds = irrklang_source->getPlayLength();
    if (milliseconds < 0)
      return -1.0f;
    return ((float)milliseconds) / 1000.0f;
  }

  bool SoundSource::IsManualLoopRequired(SoundLoopInfoExt const & in_loop_info)
  {
    if (in_loop_info.blend_time > 0.0f) // blending => manual blend required
      return true;

    if (in_loop_info.start > 0.0f) // start not aligned on track start => manual required
      return true;

    if (in_loop_info.end < in_loop_info.play_length) // end not aligned on track end => manual required
      return true;

    return false;
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
      SoundManager::ToIrrklangVector(desc.position),
      looping,
      paused,
      track,
      sound_effect);

    if (result != nullptr)
      result->setVelocity(SoundManager::ToIrrklangVector(desc.speed));

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
    if (!IsAttachedToManager())
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

  float Sound::GetPlayPosition() const
  {
    if (!IsAttachedToManager())
      return 0.0f;

    if (irrklang_sound == nullptr)
      return 0.0f;

    irrklang::ik_u32 milliseconds = irrklang_sound->getPlayPosition();
    if (milliseconds < 0)
      return 0.0f;
    return ((float)milliseconds) / 1000.0f;
  }

  void Sound::RemoveFromManager()
  {
    assert(IsAttachedToManager());
    sound_manager->RemoveSound(this);
  }

  void Sound::DetachFromManager()
  {
    if (irrklang_sound != nullptr)
    {
      irrklang_sound->stop();
      irrklang_sound = nullptr;
    }
    if (irrklang_loop_sound != nullptr)
    {
      irrklang_loop_sound->stop();
      irrklang_loop_sound = nullptr;
    }
    SoundBaseObject::DetachFromManager();
  }



  void Sound::SetPosition(glm::vec3 const & in_position)
  {
    if (!IsAttachedToManager())
      return;
    if (!is_3D_sound)
      return;

    position = in_position;
    if (irrklang_sound != nullptr)
      irrklang_sound->setPosition(SoundManager::ToIrrklangVector(in_position));
    if (irrklang_loop_sound != nullptr)
      irrklang_loop_sound->setPosition(SoundManager::ToIrrklangVector(in_position));
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
      irrklang_sound->setVelocity(SoundManager::ToIrrklangVector(in_speed));
    if (irrklang_loop_sound != nullptr)
      irrklang_loop_sound->setVelocity(SoundManager::ToIrrklangVector(in_speed));
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


  float Sound::DoTickManualLoopNoBlend(float delta_time)
  {
    assert(irrklang_loop_sound == nullptr);

    float v1 = 1.0f; // the volume of track 1

    SoundLoopInfoExt loop_info_ext = source->loop_info_ext;

    // get position of current main track
    float p1 = ((float)irrklang_sound->getPlayPosition()) / 1000.0f;

    // compute s1 & s2 the blending ranges start
    float s1 = loop_info_ext.start;
    float s2 = loop_info_ext.end;

    // end reached ?
    if (p1 >= s2)
    {
      p1 = s1 + (p1 - s2);

      irrklang_sound->setPlayPosition((irrklang::ik_u32)(p1 * 1000.0f));
    }
    return v1;
  }

  float Sound::DoTickManualLoopWithBlend(float delta_time)
  {
    assert(irrklang_loop_sound != nullptr);

    float v1 = 1.0f; // the volume of track 1

    SoundLoopInfoExt loop_info_ext = source->loop_info_ext;

    float positive_blend_time = (loop_info_ext.blend_time < 0.0f) ?
      -loop_info_ext.blend_time :
      +loop_info_ext.blend_time;

    // compute s1 & s2 the blending ranges start
    float s1 = loop_info_ext.start;
    float s2 = loop_info_ext.end;
    if (loop_info_ext.blend_time < 0.0f) // blend before start
    {
      s1 -= positive_blend_time;
      s2 -= positive_blend_time;
    }

    // get position of current main track
    float p1 = ((float)irrklang_sound->getPlayPosition()) / 1000.0f;
    float p2 = 0.0f;

    bool playing1 = true;
    bool playing2 = false;

    if (p1 < s2)
    {
      v1 = 1.0f;
      playing1 = true;
      playing2 = false;
    }
    else if (p1 < s2 + positive_blend_time) // inside blend range
    {
      v1 = 1.0f - (p1 - s2) / positive_blend_time;
      p2 = (s1 + (p1 - s2)); // position of second track depends on the first

      playing1 = true;
      playing2 = true;
    }
    else // over blend range
    {
      v1 = 0.0f;
      p2 = (s1 + (p1 - s2)); // position of second track depends on the first
      playing1 = false;
      playing2 = true;
    }

    if (playing1 != !irrklang_sound->getIsPaused()) // change state => this is the good moment to set track position
    {
      irrklang_sound->setIsPaused(!playing1);
      if (playing1)
        irrklang_sound->setPlayPosition((irrklang::ik_u32)(p1 * 1000.0f));
    }

    if (playing2 != !irrklang_loop_sound->getIsPaused()) // change state => this is the good moment to set track position
    {
      irrklang_loop_sound->setIsPaused(!playing2);
      if (playing2)
        irrklang_loop_sound->setPlayPosition((irrklang::ik_u32)(p2 * 1000.0f));
    }

    if (!playing1 && playing2) // swap tracks if necessary
    {
      v1 = 1.0f - v1;
      std::swap(irrklang_sound, irrklang_loop_sound);
    }

    return v1;
  }

  void Sound::Tick(float delta_time)
  {
    SoundVolumeObject::Tick(delta_time); // update blend volumes
    if (IsFinished())
      return;

    if (irrklang_sound == nullptr || source == nullptr) // whatever happens next, we cannot due anything with that sound
      return;

    // get current volume

    float v1 = 1.0f;
    if (IsLooping() && SoundSource::IsManualLoopRequired(source->loop_info_ext))
    {   
      if (source->loop_info_ext.blend_time == 0.0f)
        v1 = DoTickManualLoopNoBlend(delta_time);
      else
        v1 = DoTickManualLoopWithBlend(delta_time);
    }

    // apply volumes
    float current_volume = GetEffectiveVolume();

    irrklang_sound->setVolume((irrklang::ik_f32)current_volume * v1);

    if (irrklang_loop_sound != nullptr)
    {
      float v2 = 1.0f - v1;
      irrklang_loop_sound->setVolume((irrklang::ik_f32)current_volume * v2);
    }
  }


  //
  // SoundManager methods
  //

  SoundManager::~SoundManager()
  {
    StopManager();
  }

  void SoundManager::Tick(float delta_time)
  {
    // tick all categories
    for (int i = categories.size() - 1; i >= 0; --i)
    {
      SoundCategory * category = categories[i].get();
      if (category == nullptr)
        continue;
      category->Tick(delta_time);

      if (category->IsFinished())
      {
        if (category->callbacks != nullptr)
          category->callbacks->OnFinished(category);
        RemoveSoundCategory(i);
      }
    }

    // tick all sounds
    for (int i = sounds.size() - 1; i >= 0; --i)
    {
      Sound * sound = sounds[i].get();
      if (sound == nullptr)
        continue;
      sound->Tick(delta_time);

      if (sound->IsFinished())
      {
        if (sound->callbacks != nullptr)
          sound->callbacks->OnFinished(sound);
        RemoveSound(i);
      }
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

  SoundCategory * SoundManager::AddCategory(char const * in_name)
  {
    return DoAddCategory(in_name, nullptr);
  }

  SoundCategory * SoundManager::DoAddCategory(char const * in_name, SoundCategory * parent_category)
  {
    if (in_name != nullptr && parent_category == nullptr)
      if (FindSoundCategory(in_name) != nullptr) // category already existing
        return nullptr;

    SoundCategory * result = new SoundCategory(this);
    if (result != nullptr)
    {
      if (in_name != nullptr)
        result->name = in_name;
      categories.push_back(result);
    }
    return result;
  }

  SoundSource * SoundManager::AddSource(char const * in_filename, char const * in_name, SoundLoopInfo in_loop_info)
  {
    assert(in_filename != nullptr);

    if (irrklang_engine == nullptr) // not initialized engine
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

    if (in_name != nullptr)
      result->name = in_name;
    result->irrklang_source = irrklang_source;

    result->loop_info_ext = result->GetClampedLoopInfoExt(in_loop_info); // clamp the loop info
   
    // XXX : for unknown reasons, irrklang sound_source must not be drop() 
    //       (except for additionnal reference counter)
    //       see comments in headers
    //
    // irrklang_source->drop(); 

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

  irrklang::vec3df SoundManager::ToIrrklangVector(glm::vec3 const & src)
  {
    irrklang::vec3df result;
    result.X = (float)src.x;
    result.Y = (float)src.y;
    result.Z = (float)src.z;
    return result;
  }

  void SoundManager::SetListenerPosition(glm::mat4 const & view, glm::vec3 const & speed)
  {
    if (irrklang_engine == nullptr)
      return;

    glm::vec3 position = view[3];
    glm::vec3 lookdir  = view[2];
    glm::vec3 up       = view[1];

    irrklang_engine->setListenerPosition(ToIrrklangVector(position), ToIrrklangVector(lookdir), ToIrrklangVector(speed), ToIrrklangVector(up));
  }

}; // namespace chaos
