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
  
void SoundCallbacks::OnSoundFinished(Sound * in_sound)
{
  assert(in_sound != nullptr);
}

void SoundCallbacks::OnRemovedFromManager(Sound * in_sound)
{
  assert(in_sound != nullptr);
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
template<typename T, typename U>
static T * FindSoundObject(char const * name, U & objects)
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

Sound const * SoundManager::FindSound(char const * name) const
{
  return FindSoundObject<Sound>(name, sounds);
}

SoundCategory const * SoundManager::FindSoundCategory(char const * name) const
{
  return FindSoundObject<SoundCategory>(name, categories);
}

SoundSource const * SoundManager::FindSoundSource(char const * name) const
{
  return FindSoundObject<SoundSource>(name, sources);
}

/** a generic function to find an object in a list by its path */
template<typename T, typename U>
static T * FindSoundObjectByPath(boost::filesystem::path const & in_path, U & objects)
{
  size_t count = objects.size();
  for (size_t i = 0; i < count; ++i)
  {
    T * obj = dynamic_cast<SoundSourceSimple *>(objects[i].get());
    if (obj == nullptr)
      continue;
    if (obj->GetPath() == in_path)
      return obj;
  }
  return nullptr;
}

SoundSourceSimple * SoundManager::FindSourceSimple(boost::filesystem::path const & in_path)
{
  return FindSoundObjectByPath<SoundSourceSimple>(in_path, sources);
}

SoundSourceSimple const * SoundManager::FindSourceSimple(boost::filesystem::path const & in_path) const
{
  return FindSoundObjectByPath<SoundSourceSimple>(in_path, sources);
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

#if 0

  // tick all categories
  for (size_t i = categories.size(); i > 0; --i)
  {
    size_t index = i - 1;

    SoundCategory * category = categories[index].get();
    if (category == nullptr)
      continue;
    category->Tick(delta_time);

    if (category->IsFinished())
    {
      if (category->callbacks != nullptr)
        category->callbacks->OnFinished(category);
      RemoveSoundCategory(index);
    }
  }

  // tick all sounds
  for (size_t i = sounds.size(); i > 0; --i)
  {
    size_t index = i - 1;

    Sound * sound = sounds[index].get();
    if (sound == nullptr)
      continue;
    sound->Tick(delta_time);

    if (sound->IsFinished())
    {
      if (sound->callbacks != nullptr)
        sound->callbacks->OnFinished(sound);
      RemoveSound(index);
    }
  }

#endif








}

bool SoundManager::CanAddSound(char const * in_name) const
{
  // manager initialized ?
  if (!IsManagerStarted())
    return false;
  // name already existing ?
  if (in_name != nullptr && FindSound(in_name) != nullptr)
    return false;
  return true;
}

bool SoundManager::CanAddCategory(char const * in_name) const
{
  // manager initialized ?
  if (!IsManagerStarted())
    return false;
  // name already existing ?
  if (in_name != nullptr && FindSoundCategory(in_name) != nullptr)
    return false;
  return true;
}

bool SoundManager::CanAddSource(char const * in_name) const
{
  // manager initialized ?
  if (!IsManagerStarted())
    return false;
  // name already existing ?
  if (in_name != nullptr && FindSoundSource(in_name) != nullptr)
    return false;
  return true;
}

SoundSourceSequence * SoundManager::AddSourceSequence(char const * in_name)
{
  // test whether a source with the given name could be inserted
  if (!CanAddSource(in_name))
    return nullptr;
  // create the source
  return DoAddSources(new SoundSourceSequence(), in_name);
}

SoundSourceRandom * SoundManager::AddSourceRandom(char const * in_name)
{
  // test whether a source with the given name could be inserted
  if (!CanAddSource(in_name))
    return nullptr;
  // create the source
  return DoAddSources(new SoundSourceRandom(), in_name);
}

SoundSourceSimple * SoundManager::AddSourceSimple(boost::filesystem::path const & in_path)
{
  return AddSourceSimple(in_path, in_path.string().c_str());
}

SoundSourceSimple * SoundManager::AddSourceSimple(boost::filesystem::path const & in_path, char const * in_name)
{
  // test whether a source with the given name could be inserted
  if (!CanAddSource(in_name))
    return nullptr;
  // find a simple source with the given path
  if (FindSourceSimple(in_path) != nullptr)
    return nullptr;
  // load the file
  chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(in_path, false);
  if (buffer == nullptr)
    return nullptr;
  // create the source on irrklang side
  // XXX : we give filename even if the file is already loaded because it helps irrklangs to find the data format
  boost::intrusive_ptr<irrklang::ISoundSource> irrklang_source = irrklang_engine->addSoundSourceFromMemory(buffer.data, (irrklang::ik_s32)buffer.bufsize, in_path.string().c_str(), true);
  if (irrklang_source == nullptr)
    return nullptr;
  // insert the result
  SoundSourceSimple * result = DoAddSources(new SoundSourceSimple(), in_name);
  if (result == nullptr)
    return nullptr;
  // last initializations
  result->irrklang_source = irrklang_source;
  result->path = in_path;

  return result;
}

class SoundCategory * SoundManager::AddSourceCategory(char const * in_name)
{


  return nullptr;
}









template<typename T, typename U>
static size_t GetObjectIndexInVector(T * object, U const & vector)
{
  assert(object != nullptr);
  size_t count = vector.size();
  for (size_t i = 0; i < count; ++i)
    if (vector[i].get() == object)
      return i;
  return count;
}

void SoundManager::RemoveSoundCategory(SoundCategory * in_category)
{
  RemoveSoundCategory(GetObjectIndexInVector(in_category, categories));
}

void SoundManager::RemoveSoundCategory(size_t index)
{
  DoRemoveSoundObject(index, categories);
}

void SoundManager::RemoveSound(Sound * in_sound)
{
  RemoveSound(GetObjectIndexInVector(in_sound, sounds));
}

void SoundManager::RemoveSound(size_t index)
{
  DoRemoveSoundObject(index, sounds);
}

void SoundManager::RemoveSoundSource(SoundSource * in_source)
{
  RemoveSoundSource(GetObjectIndexInVector(in_source, sources));
}

void SoundManager::RemoveSoundSource(size_t index)
{
  DoRemoveSoundObject(index, sources);
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

void SoundManagedObject::DetachFromManager()
{
  assert(IsAttachedToManager());
  sound_manager = nullptr;
}

void SoundManagedObject::RemoveFromManager()
{
}

void SoundManagedObject::Tick(float delta_time)
{
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

void SoundManagedVolumeObject::Tick(float delta_time)
{
  SoundManagedObject::Tick(delta_time);







}

// ==============================================================
// SOUND
// ==============================================================

void SoundCategory::DetachFromManager()
{
 // sound_manager->DestroyAllSoundPerCategory(this);
  SoundManagedVolumeObject::DetachFromManager();
}

void SoundCategory::RemoveFromManager()
{
  assert(IsAttachedToManager());
  sound_manager->RemoveSoundCategory(this);
}

void SoundCategory::Tick(float delta_time)
{
  SoundManagedVolumeObject::Tick(delta_time);




}

// ==============================================================
// SOUND
// ==============================================================

void Sound::OnSoundFinished(bool enable_callbacks)
{
  finished = true;
  if (enable_callbacks && callbacks != nullptr)
    callbacks->OnSoundFinished(this);
}

void Sound::Tick(float delta_time)
{
  SoundManagedVolumeObject::Tick(delta_time);







}

bool Sound::DoPlaySound()
{
  return true; // immediatly finished
}

bool Sound::PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks, bool enable_callbacks)
{
  // copy the data
  is_3D_sound = desc.IsSound3D();
  position    = desc.position;
  velocity    = desc.velocity;
  paused      = desc.paused;
  looping     = desc.looping;

  if (desc.sound_name.length() > 0)
    name = desc.sound_name;

  callbacks = in_callbacks;

  // start the sound
  bool completed = DoPlaySound();
  // raise the 'completion event' if necessary
  if (completed)
    OnSoundFinished(enable_callbacks);
  // returns
  return completed;
}

void Sound::RemoveFromManager()
{
  assert(IsAttachedToManager());
  sound_manager->RemoveSound(this);
}

void Sound::DetachFromManager()
{
  assert(IsAttachedToManager());
  if (callbacks != nullptr)
    callbacks->OnRemovedFromManager(this);
  SoundManagedObject::DetachFromManager();
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

bool Sound::IsPaused() const
{
  return paused;
}

bool Sound::IsLooping() const
{
  return looping;
}

bool Sound::IsFinished() const
{
  return finished;
}

void Sound::SetPosition(glm::vec3 const & in_position, bool set_3D_sound)
{
  position = in_position;
  if (set_3D_sound)
    is_3D_sound = true;
}

void Sound::SetVelocity(glm::vec3 const & in_velocity, bool set_3D_sound)
{
  velocity = in_velocity;
  if (set_3D_sound)
    is_3D_sound = true;
}

void Sound::Pause()
{
  paused = true;
}

void Sound::Resume()
{
  paused = false;
}

void Sound::Stop()
{

}

                /* ---------------- */

SoundSimple::SoundSimple(class SoundSourceSimple * in_source) : 
  source(in_source)
{
  assert(in_source != nullptr);
}

SoundSimple::~SoundSimple()
{

}

// XXX : Do not use 
//         irrklang_sound->setSoundStopEventReceiver(...)
//       The callback is called in another thread !!!
//       Instead use the Tick(...) fuction.
//       Less complication.

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
  return (irrklang_sound == nullptr); // error => immediatly finished
}

bool SoundSimple::IsSound3D() const
{
  return is_3D_sound;
}

void SoundSimple::SetPosition(glm::vec3 const & in_position, bool set_3D_sound)
{
  Sound::SetPosition(in_position, set_3D_sound);
  if (irrklang_sound != nullptr && is_3D_sound)
    irrklang_sound->setPosition(chaos::IrrklangTools::ToIrrklangVector(in_position));
}

void SoundSimple::SetVelocity(glm::vec3 const & in_velocity, bool set_3D_sound)
{
  Sound::SetVelocity(in_velocity, set_3D_sound);
  if (irrklang_sound != nullptr && is_3D_sound)
    irrklang_sound->setVelocity(chaos::IrrklangTools::ToIrrklangVector(in_velocity));
}

void SoundSimple::Pause()
{
  Sound::Pause();
  if (irrklang_sound != nullptr)
    irrklang_sound->setIsPaused(true);
}

void SoundSimple::Resume()
{
  Sound::Resume();
  if (irrklang_sound != nullptr)
    irrklang_sound->setIsPaused(false);
}

void SoundSimple::Stop()
{
  Sound::Stop();
  if (irrklang_sound != nullptr)
    irrklang_sound->stop();
}

void SoundSimple::DetachFromManager()
{
  if (irrklang_sound != nullptr) // destroy the sound
  {
    irrklang_sound->stop();
    irrklang_sound = nullptr;
  }
  Sound::DetachFromManager();
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
  Sound::SetPosition(in_position, set_3D_sound);
  if (current_sound != nullptr)
    current_sound->SetPosition(in_position, set_3D_sound);
}

void SoundComposite::SetVelocity(glm::vec3 const & in_velocity, bool set_3D_sound)
{
  Sound::SetVelocity(in_velocity, set_3D_sound);
  if (current_sound != nullptr)
    current_sound->SetVelocity(in_velocity, set_3D_sound);
}

void SoundComposite::Pause()
{
  Sound::Pause();
  if (current_sound != nullptr)
    current_sound->Pause();
}

void SoundComposite::Resume()
{
  Sound::Resume();
  if (current_sound != nullptr)
    current_sound->Resume();
}

void SoundComposite::Stop()
{
  Sound::Stop();
  if (current_sound != nullptr)
    current_sound->Stop();
}

                        /* ---------------- */

SoundCompositeCallbacks::SoundCompositeCallbacks(SoundComposite * in_sound_composite):
  sound_composite(in_sound_composite)
{
  assert(in_sound_composite != nullptr);
}

void SoundCompositeCallbacks::OnSoundFinished(Sound * sound)
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

Sound * SoundSource::GenerateSound()
{ 
  return nullptr; 
}

Sound * SoundSource::PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks, bool enable_callbacks)
{
  // ensure we have access to manager
  if (!IsAttachedToManager())
    return nullptr;

  // ensure there are no name collision
  if (!sound_manager->CanAddSound((desc.sound_name.length() > 0)? desc.sound_name.c_str() : nullptr))
    return nullptr;

  // ensure (if a category is required) that is correct
  SoundCategory * sound_category = desc.category;
  if (sound_category == nullptr && desc.category_name.length() > 0)
  {
    sound_category = sound_manager->FindSoundCategory(desc.category_name.c_str());
    if (sound_category == nullptr) // there is a category requirement by name that does not exist
      return nullptr;
  }

  // a category is required and it does not belong to the same valid manager => failure
  if (sound_category != nullptr && sound_category->GetManager() != sound_manager)
    return nullptr;
   
  Sound * result = GenerateSound();
  if (result != nullptr)
  {
    // initialize the newly created object (other values will be initialized in Sound::PlaySound(...)
    result->category = sound_category; 
    result->sound_manager = sound_manager;    
    sound_manager->sounds.push_back(result);
    // play the sound
    result->PlaySound(desc, in_callbacks, enable_callbacks);
  }
  return result;
}

void SoundSource::DetachFromManager()
{
 // sound_manager->DestroyAllSoundPerSource(this);

  SoundManagedObject::DetachFromManager();
}

void SoundSource::RemoveFromManager()
{
  assert(IsAttachedToManager());
  sound_manager->RemoveSoundSource(this);
}

                /* ---------------- */

Sound * SoundSourceSimple::GenerateSound()
{
  return new SoundSimple(this);
}

void SoundSourceSimple::DetachFromManager()
{
  // sound_manager->DestroyAllSoundPerSource(this);

  if (irrklang_source != nullptr)
  {
    irrklang::ISoundEngine * irrklang_engine = GetIrrklangEngine();
    if (irrklang_engine != nullptr)
      irrklang_engine->removeSoundSource(irrklang_source.get());
    irrklang_source = nullptr;
  }

  SoundSource::DetachFromManager();
}

                /* ---------------- */

Sound * SoundSourceSequence::GenerateSound()
{
  return new SoundSequence(this);
}
