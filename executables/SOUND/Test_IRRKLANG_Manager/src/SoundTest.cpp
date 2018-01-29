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


#if 0

virtual void setSoundStopEventReceiver(ISoundStopEventReceiver* reciever, void* userData = 0) = 0;

#endif

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

void SoundBase::OnSoundFinished(SoundCallbacks * callbacks)
{
  if (callbacks != nullptr)
    callbacks->OnSoundFinished(this);
}


bool SoundBase::PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks)
{
  return true; // immediatly finished
}

                /* ---------------- */

SoundSimple::SoundSimple(class SoundSourceSimple * in_source) : 
  source(in_source)
{
  assert(in_source != nullptr);
}

bool SoundSimple::PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks)
{
  // test whether the sound may be played
  // error => immediatly finished
  if (source == nullptr || source->irrklang_source == nullptr)
    return true;

  irrklang::ISoundEngine * irrklang_engine = GetIrrklangEngine();
  if (irrklang_engine == nullptr)
    return true; 

  // play sound
  bool looping = desc.looping;
  bool paused = desc.start_paused;
  bool track = true;
  bool sound_effect = true;

  is_3D_sound = desc.IsSound3D();

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

  if (in_callbacks != nullptr && irrklang_sound != nullptr)
    irrklang_sound->setSoundStopEventReceiver(new SoundCallbackIrrklangWrapper(this, in_callbacks), nullptr);

  return (irrklang_sound == nullptr); // error => immediatly finished
}

bool SoundSimple::IsSound3D() const
{
  return is_3D_sound;
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

