#include "SoundTest.h"

#include <chaos/FileTools.h>
#include <chaos/Buffer.h>

// ==============================================================
// DESC
// ==============================================================

bool PlaySoundDesc::IsSound3D() const
{
  return is_3D_sound;
}

void PlaySoundDesc::Enable3D(bool enable)
{
  is_3D_sound = enable;
}

void PlaySoundDesc::SetPosition(glm::vec3 const & in_position, bool set_3D_sound)
{
  position = in_position;
  if (set_3D_sound)
    is_3D_sound = true;  
}

void PlaySoundDesc::SetVelocity(glm::vec3 const & in_velocity, bool set_3D_sound)
{
  velocity = in_velocity;
  if (set_3D_sound)
    is_3D_sound = true;
}

// ==============================================================
// CALLBACKS
// ==============================================================
  
void SoundCallbacks::OnSoundFinished(SoundBase * sound) 
{
  assert(sound != nullptr);
}

SoundCallbackIrrklangWrapper::SoundCallbackIrrklangWrapper(SoundSimple * in_sound):
  sound(in_sound)
{
  assert(in_sound != nullptr);
}

void SoundCallbackIrrklangWrapper::OnSoundStopped(irrklang::ISound* irrklang_sound, irrklang::E_STOP_EVENT_CAUSE reason, void* userData)
{
  sound->OnSoundFinished();
}

SoundCallbackIrrklangWrapper::~SoundCallbackIrrklangWrapper()
{
  assert(0);
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

/** a generic function to find an object in a list by its name */
template<typename T>
static T * FindSoundObject(char const * name, std::vector<boost::intrusive_ptr<T>> & objects)
{
  if (name == nullptr)
    return nullptr;

  size_t count = objects.size();
  for (size_t i = 0; i < count; ++i)
  {
    T * object = objects[i].get();
    if (object == nullptr)
      continue;
    if (object->GetName() == name)
      return object;
  }
  return nullptr;
}

SoundBase * SoundManager::FindSound(char const * name)
{
  return FindSoundObject<SoundBase>(name, sounds);
}

SoundCategory * SoundManager::FindSoundCategory(char const * name)
{
  return FindSoundObject<SoundCategory>(name, categories);
}

SoundSourceBase * SoundManager::FindSoundSource(char const * name)
{
  return FindSoundObject<SoundSourceBase>(name, sources);
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
#if 0
  DetachAllObjectsFromList(sounds); // destroy sounds first to make other list destructions faster
  sounds.clear();

  DetachAllObjectsFromList(categories);
  categories.clear();

  DetachAllObjectsFromList(sources);
  sources.clear();
#endif

  irrklang_devices = nullptr;
  irrklang_engine = nullptr;

  return true;
}

bool SoundManager::AddSource(SoundSourceBase * in_source)
{
  assert(in_source != nullptr);

  // manager initialized ?
  if (!IsManagerStarted())
    return false;
  // already attached to a manager
  if (in_source->IsAttachedToManager())
    return false;

  // insert and the new source
  sources.push_back(in_source);

  return true;
}


SoundSourceSimple * SoundManager::AddSourceSimple(char const * in_filename, char const * in_name)
{
  assert(in_filename != nullptr);

  // not initialized engine ?
  if (!IsManagerStarted())
    return nullptr;

  // by default the name of a source is its filepath
  if (in_name == nullptr) 
    in_name = in_filename;

  // name already existing ?
  if (FindSoundSource(in_name) != nullptr) 
    return nullptr;

  // load the file
  chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(in_filename, false); 
  if (buffer == nullptr)
    return nullptr;


  // create the source on irrklang side
  // XXX : we give filename even if the file is already loaded because it helps irrklangs to find the data format
  irrklang::ISoundSource * irrklang_source = irrklang_engine->addSoundSourceFromMemory(buffer.data, (irrklang::ik_s32)buffer.bufsize, in_filename, true);
  if (irrklang_source == nullptr)
    return nullptr;

  SoundSourceSimple * result = new SoundSourceSimple();
  if (result == nullptr)
    return nullptr;



#if 0
  SoundSource * result = new SoundSource(this);


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

#endif

  return nullptr;
}



// ==============================================================
// MANAGED OBJECT
// ==============================================================

SoundManager * SoundManagedObject::GetManager()
{
  return sound_manager.get();
}
SoundManager const * SoundManagedObject::GetManager() const
{
  return sound_manager.get();
}

bool SoundManagedObject::IsAttachedToManager() const
{
  return (sound_manager != nullptr);
}

irrklang::ISoundEngine * SoundManagedObject::GetIrrklangEngine()
{
  if (sound_manager != nullptr)
    return sound_manager->GetIrrklangEngine();
  return nullptr;
}

// ==============================================================
// VOLUME
// ==============================================================

float SoundManagedVolumeObject::GetVolume() const
{
  if (!IsAttachedToManager())
    return 0.0f;
  return volume;
}

float SoundManagedVolumeObject::GetEffectiveVolume() const
{
  return GetVolume();
}

// ==============================================================
// SOUND
// ==============================================================

void SoundBase::OnSoundFinished()
{
  if (callbacks != nullptr)
    callbacks->OnSoundFinished(this);
}

bool SoundBase::DoPlaySound()
{
  return true; // immediatly finished
}

bool SoundBase::PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks, bool enable_callbacks)
{
  // copy the data
  is_3D_sound = desc.IsSound3D();
  position    = desc.position;
  velocity    = desc.velocity;
  paused      = desc.paused;
  looping     = desc.looping;

  callbacks = in_callbacks;

  // start the sound
  bool completed = DoPlaySound();
  // raise the 'completion event' if necessary
  if (completed && enable_callbacks)
    OnSoundFinished();
  // returns
  return completed;
}

bool SoundBase::IsSound3D() const
{
  return is_3D_sound;
}

glm::vec3 SoundBase::GetPosition() const
{
  return position;
}

glm::vec3 SoundBase::GetVelocity() const
{
  return velocity;
}

bool SoundBase::IsPaused() const
{
  return paused;
}

bool SoundBase::IsLooping() const
{
  return looping;
}

void SoundBase::SetPosition(glm::vec3 const & in_position, bool set_3D_sound)
{
  position = in_position;
  if (set_3D_sound)
    is_3D_sound = true;
}

void SoundBase::SetVelocity(glm::vec3 const & in_velocity, bool set_3D_sound)
{
  velocity = in_velocity;
  if (set_3D_sound)
    is_3D_sound = true;
}

void SoundBase::Pause()
{
  paused = true;
}

void SoundBase::Resume()
{
  paused = false;
}

void SoundBase::Stop()
{

}
                /* ---------------- */

SoundSimple::SoundSimple(class SoundSourceSimple * in_source) : 
  source(in_source)
{
  assert(in_source != nullptr);
}

bool SoundSimple::DoPlaySound()
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
      chaos::IrrklangTools::ToIrrklangVector(position),
      looping,
      paused,
      track,
      sound_effect);

    if (irrklang_sound != nullptr)
      irrklang_sound->setVelocity(chaos::IrrklangTools::ToIrrklangVector(velocity));
  }
  else
  {      
    irrklang_sound = irrklang_engine->play2D(
      source->irrklang_source.get(),
      looping,
      paused,
      track,
      sound_effect);
  }

  if (callbacks != nullptr && irrklang_sound != nullptr)
    irrklang_sound->setSoundStopEventReceiver(new SoundCallbackIrrklangWrapper(this), nullptr);

  return (irrklang_sound == nullptr); // error => immediatly finished
}

bool SoundSimple::IsSound3D() const
{
  return is_3D_sound;
}

void SoundSimple::SetPosition(glm::vec3 const & in_position, bool set_3D_sound)
{
  SoundBase::SetPosition(in_position, set_3D_sound);
  if (irrklang_sound != nullptr && is_3D_sound)
    irrklang_sound->setPosition(chaos::IrrklangTools::ToIrrklangVector(in_position));
}

void SoundSimple::SetVelocity(glm::vec3 const & in_velocity, bool set_3D_sound)
{
  SoundBase::SetVelocity(in_velocity, set_3D_sound);
  if (irrklang_sound != nullptr && is_3D_sound)
    irrklang_sound->setVelocity(chaos::IrrklangTools::ToIrrklangVector(in_velocity));
}

void SoundSimple::Pause()
{
  SoundBase::Pause();
  if (irrklang_sound != nullptr)
    irrklang_sound->setIsPaused(true);
}

void SoundSimple::Resume()
{
  SoundBase::Resume();
  if (irrklang_sound != nullptr)
    irrklang_sound->setIsPaused(false);
}

void SoundSimple::Stop()
{
  SoundBase::Stop();
  if (irrklang_sound != nullptr)
    irrklang_sound->stop();
}
                        /* ---------------- */

SoundComposite::SoundComposite(class SoundSourceComposite * in_source) :
  source(in_source)
{
  assert(in_source != nullptr);
}

bool SoundComposite::DoPlayNextSound()
{
  return true; // finished
}

void SoundComposite::SetPosition(glm::vec3 const & in_position, bool set_3D_sound)
{
  SoundBase::SetPosition(in_position, set_3D_sound);
  if (current_sound != nullptr)
    current_sound->SetPosition(in_position, set_3D_sound);
}

void SoundComposite::SetVelocity(glm::vec3 const & in_velocity, bool set_3D_sound)
{
  SoundBase::SetVelocity(in_velocity, set_3D_sound);
  if (current_sound != nullptr)
    current_sound->SetVelocity(in_velocity, set_3D_sound);
}

void SoundComposite::Pause()
{
  SoundBase::Pause();
  if (current_sound != nullptr)
    current_sound->Pause();
}

void SoundComposite::Resume()
{
  SoundBase::Resume();
  if (current_sound != nullptr)
    current_sound->Resume();
}

void SoundComposite::Stop()
{
  SoundBase::Stop();
  if (current_sound != nullptr)
    current_sound->Stop();
}

                        /* ---------------- */

SoundCompositeCallbacks::SoundCompositeCallbacks(SoundComposite * in_sound_composite):
  sound_composite(in_sound_composite)
{
  assert(in_sound_composite != nullptr);
}

void SoundCompositeCallbacks::OnSoundFinished(SoundBase * sound)
{
  sound_composite->DoPlayNextSound();
}

                        /* ---------------- */

SoundSequence::SoundSequence(class SoundSourceSequence * in_source) :
  SoundComposite(in_source)
{

}

bool SoundSequence::DoPlaySound()
{
  return DoPlayNextSound();
}

bool SoundSequence::DoPlayNextSound()
{
  if (source == nullptr) // if no source, cannot do anything more
    return true;

  size_t child_source_count = source->child_sources.size();

  for (size_t count = 0 ; count < child_source_count ; ++count) // at most, one try per child source
  {
    // end is reached ?
    if (index == child_source_count)
    {
      if (!looping)
        return true;
      else
        index = 0;
    }
    // play the child sound
    PlaySoundDesc desc;
    desc.looping = false;
    desc.paused = paused;
    desc.SetPosition(position, false);
    desc.SetVelocity(velocity, false);
    desc.Enable3D(is_3D_sound);

    bool enable_child_callbacks = false;

    current_sound = source->child_sources[index++]->PlaySound(desc, new SoundCompositeCallbacks(this), enable_child_callbacks);
    if (current_sound != nullptr)
      return false; // un finished yet
  }
  return true; // finished
}


                        /* ---------------- */


// ==============================================================
// SOURCES
// ==============================================================

SoundBase * SoundSourceBase::GenerateSound() 
{ 
  return nullptr; 
}

SoundBase * SoundSourceBase::PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks, bool enable_callbacks)
{
  SoundBase * result = GenerateSound();
  if (result != nullptr)
  {
    bool completed = result->PlaySound(desc, in_callbacks, enable_callbacks);
    if (completed)
    {
      delete (result); // if finished, no need to return the sound object
      return nullptr;
    }
  }
  return result;
}

                /* ---------------- */

SoundBase * SoundSourceSimple::GenerateSound()
{
  return new SoundSimple(this);
}

                /* ---------------- */

SoundBase * SoundSourceSequence::GenerateSound()
{
  return new SoundSequence(this);
}
