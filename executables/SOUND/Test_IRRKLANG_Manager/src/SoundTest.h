#pragma once

#include "chaos/ReferencedObject.h"

// ==============================================================
// CALLBACKS
// ==============================================================

class SoundCallbacks : public chaos::ReferencedObject
{
  friend class SoundBase;

protected:

  /** called whenever a sound is finished */
  virtual void OnSoundFinished(SoundBase * sound) {}

public:



};

// ==============================================================
// DESC
// ==============================================================

class PlaySoundDesc
{
public:

  /** whether the sound is paused at the beginning */
  bool start_paused = false;
  /** whether the sound is looping */
  bool looping = false;
};

// ==============================================================
// SOUND
// ==============================================================

class SoundBase : public chaos::ReferencedObject
{
  friend class SoundSourceBase;

protected:

  /** the accessibility function */
  virtual void OnSoundFinished(SoundCallbacks * callbacks) 
  {
    assert(callbacks != nullptr);
    callbacks->OnSoundFinished(this);
  }

  /** the sound method */
  virtual void PlaySound(PlaySoundDesc const & desc) {}

public:



};



// ==============================================================
// SOURCES
// ==============================================================

class SoundSourceBase : public chaos::ReferencedObject
{
  /** the accessibility function */
  virtual void PlaySound(SoundBase * sound, PlaySoundDesc const & desc) 
  {
    assert(sound != nullptr);
    sound->PlaySound(desc);
  }

public:

  virtual SoundBase * GenerateSound() { return nullptr; }
   


};

                /* ---------------- */

class SoundSourceSimple : public SoundSourceBase
{
public:

  virtual SoundBase * GenerateSound() override;

protected:

  /** the irrklang source */
  boost::intrusive_ptr<irrklang::ISoundSource> irrklang_source;
};


                /* ---------------- */

class SoundSourceComposite : public SoundSourceBase
{
public:

protected:

  /** child sources */
  std::vector<boost::intrusive_ptr<SoundSourceBase>> child_sources;
};

                /* ---------------- */

class SoundSourceSequence : public SoundSourceComposite
{
public:

};

                /* ---------------- */

class SoundSourceRandom : public SoundSourceComposite
{
public:

};