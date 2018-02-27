#include "SoundTest.h"

#include <chaos/FileTools.h>
#include <chaos/Buffer.h>


// XXX: RemoveFromOwner useless ??










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
  
void SoundCallbacks::OnFinished(class SoundManagedObject * in_object)
{
  assert(in_object != nullptr);
}

void SoundCallbacks::OnRemovedFromOwner(class SoundManagedObject * in_object)
{
  assert(in_object != nullptr);
}

void SoundAutoCallbacks::OnFinished(class SoundManagedObject * in_object)
{
  if (finished_func)
    finished_func(in_object);
}
void SoundAutoCallbacks::OnRemovedFromOwner(class SoundManagedObject * in_object)
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
  return nullptr;
}

SoundManager const * SoundObject::GetManager() const
{
  return nullptr;
}

void SoundObject::Tick(float delta_time)
{

}

bool SoundObject::IsAttachedToManager() const
{
  return (GetManager() != nullptr);
}

// ==============================================================
// SOUND SOURCE OWNER
// ==============================================================

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

SoundSource * SoundObjectOwner::FindSoundSource(char const * name)
{
  return FindSoundObject<SoundSource>(name, sources);
}
SoundSource const * SoundObjectOwner::FindSoundSource(char const * name) const
{
  return FindSoundObject<SoundSource>(name, sources);
}

Sound * SoundObjectOwner::FindSound(char const * name)
{
  return FindSoundObject<Sound>(name, sounds);
}

Sound const * SoundObjectOwner::FindSound(char const * name) const
{
  return FindSoundObject<Sound>(name, sounds);
}

SoundCategory * SoundObjectOwner::FindSoundCategory(char const * name)
{
  return FindSoundObject<SoundCategory>(name, categories);
}

SoundCategory const * SoundObjectOwner::FindSoundCategory(char const * name) const
{
  return FindSoundObject<SoundCategory>(name, categories);
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

SoundSourceSimple * SoundObjectOwner::FindSourceSimple(boost::filesystem::path const & in_path)
{
  return FindSoundObjectByPath<SoundSourceSimple>(in_path, sources);
}

SoundSourceSimple const * SoundObjectOwner::FindSourceSimple(boost::filesystem::path const & in_path) const
{
  return FindSoundObjectByPath<SoundSourceSimple>(in_path, sources);
}

bool SoundObjectOwner::CanAddSource(char const * in_name) const
{
  return CanAddObject<SoundSource>(in_name, &SoundObjectOwner::FindSoundSource);
}

bool SoundObjectOwner::CanAddSound(char const * in_name) const
{
  return CanAddObject<Sound>(in_name, &SoundManager::FindSound);
}

bool SoundObjectOwner::CanAddCategory(char const * in_name) const
{
  return CanAddObject<SoundCategory>(in_name, &SoundManager::FindSoundCategory);
}

SoundSourceSequence * SoundObjectOwner::AddSourceSequence(char const * in_name)
{
  // test whether a source with the given name could be inserted
  if (!CanAddSource(in_name))
    return nullptr;
  // create the source
  return DoAddSources(new SoundSourceSequence(), in_name);
}

SoundSourceRandom * SoundObjectOwner::AddSourceRandom(char const * in_name)
{
  // test whether a source with the given name could be inserted
  if (!CanAddSource(in_name))
    return nullptr;
  // create the source
  return DoAddSources(new SoundSourceRandom(), in_name);
}

SoundSourceSimple * SoundObjectOwner::AddSourceSimple(boost::filesystem::path const & in_path)
{
  return AddSourceSimple(in_path, in_path.string().c_str());
}

SoundSourceSimple * SoundObjectOwner::AddSourceSimple(boost::filesystem::path const & in_path, char const * in_name)
{
  // test whether a source with the given name could be inserted
  if (!CanAddSource(in_name))
    return nullptr;
  // find a simple source with the given path
  if (FindSourceSimple(in_path) != nullptr)
    return nullptr;
  // get the irrklang engine
  irrklang::ISoundEngine * engine = SoundObject::GetIrrklangEngine();
  if (engine == nullptr)
    return nullptr;
  // load the file
  chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(in_path, false);
  if (buffer == nullptr)
    return nullptr;
  // create the source on irrklang side
  // XXX : we give filename even if the file is already loaded because it helps irrklangs to find the data format
  boost::intrusive_ptr<irrklang::ISoundSource> irrklang_source = engine->addSoundSourceFromMemory(buffer.data, (irrklang::ik_s32)buffer.bufsize, in_path.string().c_str(), true);
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

class SoundCategory * SoundObjectOwner::AddSourceCategory(char const * in_name)
{
  // test whether a category with the given name could be inserted
  if (!CanAddCategory(in_name))
    return nullptr;
  // create the category
  SoundCategory * result = new SoundCategory();
  if (result == nullptr)
    return nullptr;
  // initialize the object
  result->owner = this;
  if (in_name != nullptr)
    result->name = in_name;
  categories.push_back(result);

  return result;
}
















// ==============================================================
// MANAGER
// ==============================================================

SoundManager * SoundManager::GetManager()
{
  return this;
}

SoundManager const * SoundManager::GetManager() const
{
  return this;
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
  // tick all categories
  DoTick(delta_time, categories, &SoundManager::RemoveSoundCategory);
  // tick all sounds
  DoTick(delta_time, sounds, &SoundManager::RemoveSound);
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
  DoRemoveObject(index, categories);
}

void SoundManager::RemoveSound(Sound * in_sound)
{
  RemoveSound(GetObjectIndexInVector(in_sound, sounds));
}

void SoundManager::RemoveSound(size_t index)
{
  DoRemoveObject(index, sounds);
}

void SoundManager::RemoveSoundSource(SoundSource * in_source)
{
  RemoveSoundSource(GetObjectIndexInVector(in_source, sources));
}

void SoundManager::RemoveSoundSource(size_t index)
{
  DoRemoveObject(index, sources);
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
    chaos::IrrklangTools::ToIrrklangVector(position), 
    chaos::IrrklangTools::ToIrrklangVector(lookdir), 
    chaos::IrrklangTools::ToIrrklangVector(speed), 
    chaos::IrrklangTools::ToIrrklangVector(up));

  return true;
}







// ==============================================================
// MANAGED OBJECT
// ==============================================================

SoundManager * SoundManagedObject::GetManager()
{
  if (owner == nullptr)
    return nullptr;
  SoundManager * result = dynamic_cast<SoundManager *>(owner.get());
  if (result != nullptr)
    return result;
  return result->GetManager();
}

SoundManager const * SoundManagedObject::GetManager() const
{
  if (owner == nullptr)
    return nullptr;
  SoundManager const * result = dynamic_cast<SoundManager const *>(owner.get());
  if (result != nullptr)
    return result;
  return result->GetManager();
}

void SoundManagedObject::OnRemovedFromOwner()
{
  assert(IsAttachedToManager());
  owner = nullptr;
}

void SoundManagedObject::RemoveFromOwner()
{
}

void SoundManagedObject::Tick(float delta_time)
{
}

bool SoundManagedObject::IsFinished() const
{
  return true;
}

void SoundManagedObject::SetCallbacks(SoundCallbacks * in_callbacks)
{
  callbacks = in_callbacks;
}

void SoundManagedObject::OnFinished()
{
  if (callbacks != nullptr)
    callbacks->OnFinished(this);
}


// ==============================================================
// VOLUME
// ==============================================================

void SoundManagedVolumeObject::Pause()
{
  paused = true;
}

void SoundManagedVolumeObject::Resume()
{
  paused = false;
}

bool SoundManagedVolumeObject::IsPaused() const
{
  if (!IsAttachedToManager())
    return false;
  return paused;
}

bool SoundManagedVolumeObject::IsEffectivePaused() const
{
  return IsPaused();
}

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

bool SoundManagedVolumeObject::IsFinished() const
{


  return false;
}

// ==============================================================
// SOUND
// ==============================================================

void SoundCategory::OnRemovedFromOwner()
{
  owner->DestroyAllSoundPerCategory(this);
  SoundManagedVolumeObject::OnRemovedFromOwner();
}

void SoundCategory::RemoveFromOwner()
{
  assert(IsAttachedToManager());
  owner->RemoveSoundCategory(this);
}

void SoundCategory::Tick(float delta_time)
{
  SoundManagedVolumeObject::Tick(delta_time);




}

// ==============================================================
// SOUND
// ==============================================================

void Sound::Tick(float delta_time)
{
  SoundManagedVolumeObject::Tick(delta_time); // update the volume
}

float Sound::GetEffectiveVolume() const
{
  float result = SoundManagedVolumeObject::GetEffectiveVolume();
  if (category != nullptr)
    result *= category->GetEffectiveVolume();
  return result;
}

bool Sound::IsEffectivePaused() const
{
  if (category != nullptr && category->IsEffectivePaused())
    return true;
  return SoundManagedVolumeObject::IsEffectivePaused();
}

bool Sound::DoPlaySound()
{
  return true; // immediatly finished
}

void Sound::PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks)
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
  DoPlaySound();
}

void Sound::RemoveFromOwner()
{
  assert(IsAttachedToManager());
  owner->RemoveSound(this);
}

void Sound::OnRemovedFromOwner()
{
  assert(IsAttachedToManager());
  if (callbacks != nullptr)
    callbacks->OnRemovedFromOwner(this);
  SoundManagedObject::OnRemovedFromOwner();
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

void Sound::Stop()
{

  // XXX :




}

                /* ---------------- */

void SoundSimple::Tick(float delta_time)
{
  // update the volume
  Sound::Tick(delta_time); 
  // whatever happens next, we cannot due anything with that sound ?
  if (irrklang_sound == nullptr || source == nullptr) 
    return;
  // apply volume on the irrklang object
  float current_volume = GetEffectiveVolume();
  irrklang_sound->setVolume((irrklang::ik_f32)current_volume);
}

bool SoundSimple::IsFinished() const
{
  if (irrklang_sound == nullptr)
    return true;
  if (Sound::IsFinished()) // parent call
    return true;
  if (IsLooping()) // a looping sound is never finished
    return false;
  return irrklang_sound->isFinished();
}


// XXX : Do not use 
//         irrklang_sound->setSoundStopEventReceiver(...)
//       The callback is called in another thread !!!
//       Instead use the Tick(...) fuction.
//       Less complication.

bool SoundSimple::DoPlaySound()
{
  SoundSourceSimple * source_simple = dynamic_cast<SoundSourceSimple*>(source);

  // test whether the sound may be played
  // error => immediatly finished
  if (source_simple == nullptr || source_simple->irrklang_source == nullptr)
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
      source_simple->irrklang_source.get(),
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
      source_simple->irrklang_source.get(),
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

void SoundSimple::OnRemovedFromOwner()
{
  if (irrklang_sound != nullptr) // destroy the sound
  {
    irrklang_sound->stop();
    irrklang_sound = nullptr;
  }
  Sound::OnRemovedFromOwner();
}
                        /* ---------------- */

SoundComposite::SoundComposite(class SoundSourceComposite * in_source) :
  source(in_source)
{
  assert(in_source != nullptr);
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

void SoundComposite::Tick(float delta_time)
{
  Sound::Tick(delta_time);
  if (current_sound != nullptr)
    current_sound->Tick(delta_time);


}

bool SoundComposite::DoPlayNextSound()
{
  return true; // finished
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

    current_sound = source->child_sources[index++]->PlaySound(desc, nullptr);
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

Sound * SoundSource::PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks)
{
  // ensure we have access to manager
  if (!IsAttachedToManager())
    return nullptr;

  // ensure there are no name collision
  if (!owner->CanAddSound((desc.sound_name.length() > 0)? desc.sound_name.c_str() : nullptr))
    return nullptr;

  // ensure (if a category is required) that is correct
  SoundCategory * sound_category = desc.category;
  if (sound_category == nullptr && desc.category_name.length() > 0)
  {
    sound_category = owner->FindSoundCategory(desc.category_name.c_str());
    if (sound_category == nullptr) // there is a category requirement by name that does not exist
      return nullptr;
  }

  // a category is required and it does not belong to the same valid manager => failure
  if (sound_category != nullptr && sound_category->GetManager() != owner)
    return nullptr;
   
  Sound * result = GenerateSound();
  if (result != nullptr)
  {
    // initialize the newly created object (other values will be initialized in Sound::PlaySound(...)
    result->category = sound_category; 
    result->owner = owner;
    result->source = this;
    owner->sounds.push_back(result);
    // play the sound
    result->PlaySound(desc, in_callbacks);
  }
  return result;
}

void SoundSource::OnRemovedFromOwner()
{
  owner->DestroyAllSoundPerSource(this);
  SoundManagedObject::OnRemovedFromOwner();
}

void SoundSource::RemoveFromOwner()
{
  assert(IsAttachedToManager());
  owner->RemoveSoundSource(this);
}

                /* ---------------- */

Sound * SoundSourceSimple::GenerateSound()
{
  return new SoundSimple();
}

void SoundSourceSimple::OnRemovedFromOwner()
{
  // destroy irrklang resource
  if (irrklang_source != nullptr)
  {
    irrklang::ISoundEngine * irrklang_engine = GetIrrklangEngine();
    if (irrklang_engine != nullptr)
      irrklang_engine->removeSoundSource(irrklang_source.get());
    irrklang_source = nullptr;
  }
  // parent call
  SoundSource::OnRemovedFromOwner();
}

                /* ---------------- */

Sound * SoundSourceSequence::GenerateSound()
{
  return new SoundSequence(this);
}
