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
  assert(callbacks != nullptr);
  callbacks->OnSoundFinished(this);
}


void SoundBase::PlaySound(PlaySoundDesc const & desc)
{

}

                /* ---------------- */

SoundSimple::SoundSimple(class SoundSourceSimple * in_source) : 
  source(in_source)
{
  assert(in_source != nullptr);
}

void SoundSimple::PlaySound(PlaySoundDesc const & desc)
{
  if (desc.IsSound3D())
  {

    /*
    irrklang::ISound * result = sound_manager->irrklang_engine->play3D(
      irrklang_source.get(),
      SoundManager::ToIrrklangVector(desc.position),
      looping,
      paused,
      track,
      sound_effect);

    if (result != nullptr)
      result->setVelocity(SoundManager::ToIrrklangVector(desc.speed));

    */

  }
  else
  {

  }


}

#if 0

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

irrklang::ISound * result = sound_manager->irrklang_engine->play3D(
  irrklang_source.get(),
  SoundManager::ToIrrklangVector(desc.position),
  looping,
  paused,
  track,
  sound_effect);

if (result != nullptr)
result->setVelocity(SoundManager::ToIrrklangVector(desc.speed));

#endif

// ==============================================================
// SOURCES
// ==============================================================

void SoundSourceBase::PlaySound(SoundBase * sound, PlaySoundDesc const & desc)
{
  assert(sound != nullptr);
  sound->PlaySound(desc);
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

