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
    assert(sound_manager != nullptr);
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

    return volume * blend_factor;
  }

  void SoundVolumeObject::UpdateBlendFactor(float delta_time)
  {
    assert(IsAttachedToManager()); // should never be called elsewhere

    if (blend_type == BLEND_VOLUME_IN)
    {
      if (blend_time == 0.0f) // immediate blending
        blend_factor = 1.0f;
      else
        blend_factor = chaos::MathTools::Clamp(blend_factor + delta_time / blend_time, 0.0f, 1.0f);
    }
    else if (blend_type == BLEND_VOLUME_OUT)
    {
      if (blend_time == 0.0f) // immediate blending
        blend_factor = 0.0f;
      else
        blend_factor = chaos::MathTools::Clamp(blend_factor - delta_time / blend_time, 0.0f, 1.0f);
    }
  }

  // fullrange_blend_time == true
  // ----------------------------
  //    blend_time = time to go from 0 to 1
  //
  //
  // fullrange_blend_time == false
  // -----------------------------
  //    blend_time = time to go from current_value to 1

  void SoundVolumeObject::StartBlendIn(float in_blend_time, bool fullrange_blend_time)
  {
    if (!IsAttachedToManager())
      return;

    blend_type = BLEND_VOLUME_IN;
    if (fullrange_blend_time)
      blend_time = in_blend_time;
    else
    {
      if (blend_factor == 1.0f)
        blend_time = 1.0f; // well, a non null value for division. It is useless because the wanted final value is already reached
      else
        blend_time = in_blend_time / (1.0f - blend_factor);
    }
  }

  void SoundVolumeObject::StartBlendOut(float in_blend_time, bool fullrange_blend_time) // blend_time = 0 for immediate change at next tick
  {
    if (!IsAttachedToManager())
      return;

    blend_type = BLEND_VOLUME_OUT;
    if (fullrange_blend_time)
      blend_time = in_blend_time;
    else
    {
      if (blend_factor == 0.0f)
        blend_time = 1.0f; // well, a non null value for division. It is useless because the wanted final value is already reached
      else
        blend_time = in_blend_time / (blend_factor - 0.0f);
    }
  }

  void SoundVolumeObject::StopBlending(bool set_blend_to_final_value)
  {
    if (sound_manager == nullptr) // detached from manager
      return;

    if (set_blend_to_final_value)
    {
      if (blend_type == BLEND_VOLUME_IN)
        blend_factor = 1.0f;
      else if (blend_type == BLEND_VOLUME_OUT)
        blend_factor = 0.0f;
    }
    blend_type = BLEND_VOLUME_NONE;
  }

  void SoundVolumeObject::Tick(float delta_time)
  {
    assert(IsAttachedToManager()); // should never be called elsewhere

    UpdateBlendFactor(delta_time);
  }

  bool SoundVolumeObject::IsFinished() const
  {
    assert(IsAttachedToManager()); // should never be called elsewhere

    if (blend_type == BLEND_VOLUME_OUT && blend_factor <= 0.0f) // blending out is finished
      return true;
    return false;
  }

  void SoundVolumeObject::StopAndKill(float in_blendout_time) // the object will only be destroyed after the tick
  {

#if 0
    blendout_time = in_blendout_time;
    if (blendout_time <= 0.0f) // if this is the first call
    {
      blendout_factor = 1.0f;
      should_kill = true;
    }
#endif
  }

















#if 0

  // ================================================================================



  // ================================================================================



  // ================================================================================

  SoundSource::SoundSource(class SoundManager * in_sound_manager) :
    SoundBaseObject(in_sound_manager)
  {
    assert(sound_manager != nullptr);
  }


  SoundSource::~SoundSource()
  {


  }

  void SoundSource::DetachFromManager()
  {
    irrklang_source = nullptr;
    SoundBaseObject::DetachFromManager();
  }

  float SoundSource::GetPlayLength() const
  {
    if (sound_manager == nullptr) // detached source
      return 0.0f;

    irrklang::ik_u32 milliseconds = irrklang_source->getPlayLength();
    if (milliseconds < 0)
      return -1.0f;
    return 1000.0f * (float)milliseconds;
  }

  bool SoundSource::IsManualLoopRequired(PlaySoundDesc const & desc) const
  {
    assert(sound_manager != nullptr);

    if (!desc.looping)
      return false;
    if (loop_info.blend_time <= 0.0f)
      return false;

    float length = GetPlayLength();
    if (length < 0.0f)
      return false;    // if we cannot determine the length (streaming), we cannot use manual looping

    return true;
  }

  irrklang::ISound * SoundSource::DoPlayIrrklangSound(PlaySoundDesc const & desc, bool in_looping, bool in_paused)
  {
    assert(sound_manager != nullptr);

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
    assert(sound_manager != nullptr);

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
    if (sound_manager == nullptr) // detached from manager
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




  // ================================================================================

  SoundCategory::SoundCategory(class SoundManager * in_sound_manager) :
    SoundVolumeObject(in_sound_manager)
  {
    assert(sound_manager != nullptr);
  }

  SoundCategory::~SoundCategory()
  {


  }

  void SoundCategory::DetachFromManager()
  {
    sound_manager->DetroyAllSoundPerCategory(this);
    SoundBaseObject::DetachFromManager();
  }

  // ================================================================================

  Sound::Sound(class SoundManager * in_sound_manager) :
    SoundVolumeObject(in_sound_manager)
  {
    assert(sound_manager != nullptr);
  }

  Sound::~Sound()
  {
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
    if (sound_manager == nullptr) // detached from manager
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
    if (sound_manager == nullptr) // detached from manager
      return glm::vec3(0.0f, 0.0f, 0.0f);
    if (!is_3D_sound)
      return glm::vec3(0.0f, 0.0f, 0.0f);
    return position;
  }

  void Sound::SetSpeed(glm::vec3 const & in_speed)
  {
    if (sound_manager == nullptr) // detached from manager
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
    if (sound_manager == nullptr) // detached from manager
      return glm::vec3(0.0f, 0.0f, 0.0f);
    if (!is_3D_sound)
      return glm::vec3(0.0f, 0.0f, 0.0f);
    return speed;
  }

  float Sound::GetEffectiveVolume() const
  {
    assert(sound_manager != nullptr);

    float result = SoundVolumeObject::GetEffectiveVolume();
    if (category != nullptr)
      result *= category->GetEffectiveVolume();
    return result;
  }
















  bool Sound::IsLooping() const
  {
    // irrklang native looping
    if (irrklang_sound->isLooped())
      return true;
#if 0
    // our manual looping
    if (loop_start >= 0.0f && loop_start < loop_end)
    {


    }
#endif
    return false;
  }

  bool Sound::IsFinished() const
  {
    if (SoundVolumeObject::IsFinished())
      return true;
    if (irrklang_sound->isFinished() && !IsLooping())
      return true;
    return false;
  }

  bool Sound::ShouldKillAtEnd() const
  {
    return false;
  }

  void Sound::Tick(float delta_time)
  {
    SoundVolumeObject::Tick(delta_time);
    if (IsFinished())
      return;

    float current_time = 1000.0f * (float)irrklang_sound->getPlayPosition();

  }


  // ================================================================================



  // ================================================================================

  void SoundManager::Tick(float delta_time)
  {
#if 0

    // tick all sounds
    for (int i = sounds.size() - 1; i >= 0; --i)
    {
      Sound * sound = sounds[i].get();
      if (sound == nullptr)
        continue;
      sound->Tick(delta_time);
      if (sound->IsFinished() && sound->ShouldKillAtEnd())
      {
        if (i != sounds.size() - 1)
          sounds[i] = sounds[sounds.size() - 1]; // destroy sound that is to be destroyed
        sounds.pop_back();
      }
    }

    // tick all categories
    for (int i = categories.size() - 1; i >= 0; --i)
    {
      SoundCategory * category = categories[i].get();
      if (category == nullptr)
        continue;
      category->Tick(delta_time);
      if (category->IsFinished() && category->ShouldKillAtEnd())
      {
        if (i != categories.size() - 1)
          categories[i] = categories[categories.size() - 1]; // destroy sound that is to be destroyed
        categories.pop_back();
      }
    }
#endif



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

    if (in_name == nullptr)
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
    DetachAllObjectsFromList(sounds);
    DetachAllObjectsFromList(categories);
    DetachAllObjectsFromList(sources);

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
      if (sound->category == old_category)
        sound->category = new_category;
    }
  }

  void SoundManager::DetroyAllSoundPerCategory(SoundCategory * category)
  {
    int count = sounds.size();
    for (int i = count - 1; i >= 0; --i)
    {
      Sound * sound = sounds[i].get();
      if (sound == nullptr)
        continue;
      if (sound->category != category)
        continue;


    }

  }
#endif








}; // namespace chaos
