#include "SoundTest.h"

// ==============================================================
// CALLBACKS
// ==============================================================
  
void SoundCallbacks::OnSoundFinished(SoundBase * sound) 
{
  assert(sound != nullptr);
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


}

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

