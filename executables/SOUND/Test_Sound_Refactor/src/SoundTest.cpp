#include "SoundTest.h"

#include <chaos/FileTools.h>
#include <chaos/MathTools.h>
#include <chaos/Buffer.h>




Sound::~Sound()
{
  int i = 0;
  ++i;
}

SoundCategory::~SoundCategory()
{
  int i = 0;
  ++i;
}

SoundSource::~SoundSource()
{
  int i = 0;
  ++i;
}

SoundCallbacks::~SoundCallbacks()
{
  int i = 0;
  ++i;
}


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

}

bool SoundObject::IsAttachedToManager() const
{
  return (GetManager() != nullptr);
}

void SoundObject::OnRemovedFromManager()
{
  assert(IsAttachedToManager());
  if (callbacks != nullptr)
    callbacks->OnRemovedFromManager(this);
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

void SoundObject::OnFinished()
{
  if (callbacks != nullptr)
    callbacks->OnFinished(this);
}

// ==============================================================
// SOURCES
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
  if (!sound_manager->CanAddSound((desc.sound_name.length() > 0)? desc.sound_name.c_str() : nullptr))
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

// ==============================================================
// VOLUME
// ==============================================================

void SoundVolumeObject::Pause(bool in_pause)
{
  paused = in_pause;
}

bool SoundVolumeObject::IsPaused() const
{
  if (!IsAttachedToManager())
    return false;
  return paused;
}

bool SoundVolumeObject::IsEffectivePaused() const
{
  return IsPaused();
}

float SoundVolumeObject::GetVolume() const
{
  if (!IsAttachedToManager())
    return 0.0f;
  return volume;
}

float SoundVolumeObject::GetEffectiveVolume() const
{
  return GetVolume();
}

void SoundVolumeObject::TickObject(float delta_time)
{
  SoundObject::TickObject(delta_time);




}

bool SoundVolumeObject::ComputeFinishedState()
{



  return SoundObject::ComputeFinishedState();
}

void SoundVolumeObject::SetVolume(float in_volume)
{
  volume = chaos::MathTools::Clamp(in_volume, 0.0f, 1.0f);
}

// ==============================================================
// CATEGORY
// ==============================================================

void SoundCategory::OnRemovedFromManager()
{
  sound_manager->DestroyAllSoundPerCategory(this);
  SoundVolumeObject::OnRemovedFromManager();
}

void SoundCategory::RemoveFromManager()
{
  assert(IsAttachedToManager());
  sound_manager->RemoveCategory(this);
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
  if (is_3D_sound && irrklang_sound != nullptr)
    irrklang_sound->setPosition(chaos::IrrklangTools::ToIrrklangVector(in_position));
}

void Sound::SetVelocity(glm::vec3 const & in_velocity)
{
  velocity = in_velocity;
  if (is_3D_sound && irrklang_sound != nullptr)
    irrklang_sound->setVelocity(chaos::IrrklangTools::ToIrrklangVector(in_velocity));
}

float Sound::GetEffectiveVolume() const
{
  float result = SoundVolumeObject::GetEffectiveVolume();
  if (category != nullptr)
    result *= category->GetEffectiveVolume();
  return result;
}

bool Sound::IsEffectivePaused() const
{
  if (SoundVolumeObject::IsEffectivePaused())
    return true;
  if (category != nullptr && category->IsEffectivePaused())
    return true;
  return false;
}

void Sound::Pause(bool in_pause)
{
  if (irrklang_sound != nullptr)
    if (IsEffectivePaused() != in_pause)
      irrklang_sound->setIsPaused(in_pause);
  SoundVolumeObject::Pause(in_pause);
}

void Sound::Stop()
{
  if (IsAttachedToManager()) // irrklang resource will be destroyed later
    RemoveFromManager();
}

bool Sound::ComputeFinishedState()
{
  if (irrklang_sound == nullptr)
    return true;
  if (SoundVolumeObject::ComputeFinishedState()) // parent call
    return true;
  return irrklang_sound->isFinished();
}

void Sound::SetVolume(float in_volume)
{
  SoundVolumeObject::SetVolume(in_volume);
  if (irrklang_sound != nullptr)
    irrklang_sound->setVolume((irrklang::ik_f32)GetEffectiveVolume()); // XXX : cannot wait for next tick because sound could be paused just before volume change
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
  SoundVolumeObject::OnRemovedFromManager();
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
  DoPlaySound();
}

bool Sound::DoPlaySound()
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
  return (irrklang_sound == nullptr); 
}

void Sound::TickObject(float delta_time)
{
  // update the volume
  SoundVolumeObject::TickObject(delta_time);
  // apply volume on the irrklang object
  if (irrklang_sound != nullptr)
    irrklang_sound->setVolume((irrklang::ik_f32)GetEffectiveVolume());
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
  //DoTickObjects(delta_time, sources, &SoundManager::RemoveSource);
  // tick all categories
  DoTickObjects(delta_time, categories, &SoundManager::RemoveCategory);
  // tick all sounds
  DoTickObjects(delta_time, sounds, &SoundManager::RemoveSound);
}

template<typename T, typename U>
static size_t FindObjectIndexInVector(T * object, U const & vector)
{
  assert(object != nullptr);
  size_t count = vector.size();
  for (size_t i = 0; i < count; ++i)
    if (vector[i].get() == object)
      return i;
  return count;
}
/** a generic function to find an object in a list by its name */
template<typename T, typename U>
static T * FindObjectByName(char const * name, U & objects)
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

/** a generic function to find an object in a list by its path */
template<typename T, typename U>
static T * FindObjectByPath(boost::filesystem::path const & in_path, U & objects)
{
  size_t count = objects.size();
  for (size_t i = 0; i < count; ++i)
  {
    T * obj = objects[i].get();
    if (obj == nullptr)
      continue;
    if (obj->GetPath() == in_path)
      return obj;
  }
  return nullptr;
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

SoundSource * SoundManager::FindSourceByPath(boost::filesystem::path const & in_path)
{
  return FindObjectByPath<SoundSource>(in_path, sources);
}

SoundSource const * SoundManager::FindSourceByPath(boost::filesystem::path const & in_path) const
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
    chaos::IrrklangTools::ToIrrklangVector(position), 
    chaos::IrrklangTools::ToIrrklangVector(lookdir), 
    chaos::IrrklangTools::ToIrrklangVector(speed), 
    chaos::IrrklangTools::ToIrrklangVector(up));

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

SoundSource * SoundManager::AddSource(boost::filesystem::path const & in_path)
{
  return AddSource(in_path, in_path.string().c_str());
}

SoundSource * SoundManager::AddSource(boost::filesystem::path const & in_path, char const * in_name)
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
  chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(in_path, false);
  if (buffer == nullptr)
    return nullptr;
  // create the source on irrklang side
  // XXX : we give filename even if the file is already loaded because it helps irrklangs to find the data format
  boost::intrusive_ptr<irrklang::ISoundSource> irrklang_source = engine->addSoundSourceFromMemory(buffer.data, (irrklang::ik_s32)buffer.bufsize, in_path.string().c_str(), true);
  if (irrklang_source == nullptr)
    return nullptr;
  // insert the result
  SoundSource * result = DoAddSource(new SoundSource(), in_name);
  if (result == nullptr)
    return nullptr;
  // last initializations
  result->irrklang_source = irrklang_source;
  result->path = in_path;

  return result;
}
