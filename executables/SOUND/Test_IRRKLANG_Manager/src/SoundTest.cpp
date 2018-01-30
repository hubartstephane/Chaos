#include "SoundTest.h"

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

void PlaySoundDesc::SetPosition(glm::vec3 const & in_position)
{
  is_3D_sound = true;
  position = in_position;
}

void PlaySoundDesc::SetVelocity(glm::vec3 const & in_velocity)
{
  is_3D_sound = true;
  velocity = in_velocity;
}

// ==============================================================
// CALLBACKS
// ==============================================================
  
void SoundCallbacks::OnSoundFinished(SoundBase * sound) 
{
  assert(sound != nullptr);
}

SoundCallbackIrrklangWrapper::SoundCallbackIrrklangWrapper(SoundSimple * in_sound, SoundCallbacks * in_callbacks):
  sound(in_sound),
  callbacks(in_callbacks)
{
  assert(in_sound != nullptr);
  assert(in_callbacks != nullptr);
}

void SoundCallbackIrrklangWrapper::OnSoundStopped(irrklang::ISound* irrklang_sound, irrklang::E_STOP_EVENT_CAUSE reason, void* userData)
{
  callbacks->OnSoundFinished(sound.get());
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

irrklang::ISoundEngine * SoundManagedObject::GetIrrklangEngine()
{
  if (sound_manager != nullptr)
    return sound_manager->GetIrrklangEngine();
  return nullptr;
}

// ==============================================================
// SOUND
// ==============================================================

void SoundBase::OnSoundFinished()
{
  if (callbacks != nullptr)
    callbacks->OnSoundFinished(this);
}

bool SoundBase::DoPlaySound(PlaySoundDesc const & desc)
{
  return true; // immediatly finished
}

bool SoundBase::PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks)
{
  // copy the data
  is_3D_sound = desc.IsSound3D();
  position    = desc.position;
  velocity    = desc.velocity;
  paused      = desc.paused;
  looping     = desc.looping;

  callbacks = in_callbacks;

  // start the sound
  bool completed = DoPlaySound(desc);
  // raise the 'completion event' if necessary
  if (completed)
    OnSoundFinished();
  // returns
  return completed;
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

void SoundBase::SetPosition(glm::vec3 const & in_position)
{
  is_3D_sound = true;
  position = in_position;
}

void SoundBase::SetVelocity(glm::vec3 const & in_velocity)
{
  is_3D_sound = true;
  velocity = in_velocity;
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

bool SoundSimple::DoPlaySound(PlaySoundDesc const & desc)
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
      chaos::IrrklangTools::ToIrrklangVector(desc.position),
      looping,
      paused,
      track,
      sound_effect);

    if (irrklang_sound != nullptr)
      irrklang_sound->setVelocity(chaos::IrrklangTools::ToIrrklangVector(desc.velocity));
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
    irrklang_sound->setSoundStopEventReceiver(new SoundCallbackIrrklangWrapper(this, callbacks.get()), nullptr);

  return (irrklang_sound == nullptr); // error => immediatly finished
}

bool SoundSimple::IsSound3D() const
{
  return is_3D_sound;
}

void SoundSimple::SetPosition(glm::vec3 const & in_position)
{
  SoundBase::SetPosition(in_position);
  if (irrklang_sound != nullptr && is_3D_sound)
    irrklang_sound->setPosition(chaos::IrrklangTools::ToIrrklangVector(in_position));
}

void SoundSimple::SetVelocity(glm::vec3 const & in_velocity)
{
  SoundBase::SetVelocity(in_velocity);
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

SoundSequence::SoundSequence(class SoundSourceSequence * in_source) :
  source(in_source)
{
  assert(in_source != nullptr);
}

bool SoundSequence::DoPlaySound(PlaySoundDesc const & desc)
{
  if (source == nullptr)
    return false;

  for (; index < source->child_sources.size(); ++index)  
  {
    SoundSourceBase * child_source = source->child_sources[index].get();
    if (child_source == nullptr)
      continue;

    current_sound = child_source->GenerateSound();
    if (current_sound == nullptr)
      continue;

    PlaySoundDesc other_desc = sound_desc;
    other_desc.looping = false;
    other_desc.paused  = false;

    if (!current_sound->PlaySound(other_desc))
      continue;

  }

 // if (index >= source->child_sources.size())
 //   return true; // finished

  

  //source->child_sources





  return true;
}

void SoundSequence::SetPosition(glm::vec3 const & in_position)
{
  SoundBase::SetPosition(in_position);
  if (current_sound != nullptr)
    current_sound->SetPosition(in_position);
}

void SoundSequence::SetVelocity(glm::vec3 const & in_velocity)
{
  SoundBase::SetVelocity(in_velocity);
  if (current_sound != nullptr)
    current_sound->SetVelocity(in_velocity);
}

void SoundSequence::Pause()
{
  SoundBase::Pause();
  if (current_sound != nullptr)
    current_sound->Pause();
}

void SoundSequence::Resume()
{
  SoundBase::Resume();
  if (current_sound != nullptr)
    current_sound->Resume();
}

void SoundSequence::Stop()
{
  SoundBase::Stop();
  if (current_sound != nullptr)
    current_sound->Stop();
}


// ==============================================================
// SOURCES
// ==============================================================

bool SoundSourceBase::PlaySound(SoundBase * sound, PlaySoundDesc const & desc, SoundCallbacks * in_callbacks)
{
  assert(sound != nullptr);
  return sound->PlaySound(desc, in_callbacks);
}

SoundBase * SoundSourceBase::GenerateSound() 
{ 
  return nullptr; 
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
