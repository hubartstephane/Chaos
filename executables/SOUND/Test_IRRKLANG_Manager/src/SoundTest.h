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
  virtual void OnSoundFinished(SoundBase * sound);

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

  /** accessibility function */
  virtual void OnSoundFinished(SoundCallbacks * callbacks);

  /** the sound method */
  virtual void PlaySound(PlaySoundDesc const & desc);

public:


protected:

  /** the callbacks that are being called at the end of the sound */
  boost::intrusive_ptr<SoundCallbacks> callbacks;
};

                /* ---------------- */

class SoundSimple : public SoundBase
{
  friend class SoundSourceSimple;

protected:

  /** protected constructor */
  SoundSimple(class SoundSourceSimple * in_source);
  /** the sound method */
  virtual void PlaySound(PlaySoundDesc const & desc) override;
 
protected:

  /** the source that generated this object */
  boost::intrusive_ptr<SoundSourceSimple> source;
};



// ==============================================================
// SOURCES
// ==============================================================

class SoundSourceBase : public chaos::ReferencedObject
{
  /** the accessibility function */
  virtual void PlaySound(SoundBase * sound, PlaySoundDesc const & desc);

public:

  /** the sound generation method */
  virtual SoundBase * GenerateSound();
   


};

                /* ---------------- */

class SoundSourceSimple : public SoundSourceBase
{
public:

  /** generating a source object */
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