#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/IrrklangTools.h>




// ==============================================================
// DESC
// ==============================================================

class PlaySoundDesc
{
public:

  /** returns whether the sound is in 3D dimension */
  bool IsSound3D() const;
  /** set o runset the 3D flag */
  void Enable3D(bool enable);

  /** set the position of the sound (this enables the 3D feature) */
  void SetPosition(glm::vec3 const & in_position);
  /** set the velocity of the sound (this enables the 3D feature) */
  void SetVelocity(glm::vec3 const & in_velocity);

public:

  /** whether the sound is paused at the beginning */
  bool start_paused = false;
  /** whether the sound is looping */
  bool looping = false;

  /** returns true whether the sound is in 3D */
  bool is_3D_sound = false;
  /** the position of the sound in 3D */
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
  /** the velocity of the sound in 3D */
  glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
};

// ==============================================================
// CALLBACKS
// ==============================================================

class SoundCallbacks : public chaos::ReferencedObject
{
  friend class SoundBase;
  friend class SoundSimple;
  friend class SoundCallbackIrrklangWrapper;

protected:

  /** called whenever a sound is finished */
  virtual void OnSoundFinished(SoundBase * sound);
};

class SoundCallbackIrrklangWrapper : public irrklang::ISoundStopEventReceiver
{
  friend class SoundSimple;

protected:

  /** protected constructor */
  SoundCallbackIrrklangWrapper(SoundSimple * in_sound, SoundCallbacks * in_callbacks);

public:

  /** the destructor */
  virtual ~SoundCallbackIrrklangWrapper();

  /** the method to override */
  virtual void OnSoundStopped(irrklang::ISound* irrklang_sound, irrklang::E_STOP_EVENT_CAUSE reason, void* userData) override;

protected:

  /** the sound object concerned */
  boost::intrusive_ptr<SoundSimple> sound;
  /** the callback object */
  boost::intrusive_ptr<SoundCallbacks> callbacks;
};

// ==============================================================
// MANAGER
// ==============================================================

class SoundManager : public chaos::ReferencedObject
{
public:

  /** getter on the irrklang engine */
  irrklang::ISoundEngine * GetIrrklangEngine();

protected:

  /** the irrklank engine */
  boost::intrusive_ptr<irrklang::ISoundEngine> irrklang_engine;
};

// ==============================================================
// MANAGED OBJECT
// ==============================================================

class SoundManagedObject : public chaos::ReferencedObject
{
public:

  /** getter on the irrklang engine */
  irrklang::ISoundEngine * GetIrrklangEngine();

  /** getter on the manager object */
  SoundManager * GetManager();
  /** getter on the manager object */
  SoundManager const * GetManager() const;

protected:

  /** the irrklank engine */
  boost::intrusive_ptr<SoundManager> sound_manager;
};



// ==============================================================
// SOUND
// ==============================================================

class SoundBase : public SoundManagedObject
{
  friend class SoundSourceBase;

protected:

  /** accessibility function */
  virtual void OnSoundFinished(SoundCallbacks * callbacks);

  /** the sound method (returns true whether it is immediatly finished) */
  virtual bool PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks = nullptr);

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
  /** the sound method (returns true whether it is immediatly finished) */
  virtual bool PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks = nullptr) override;
 
public:

  /** returns whether the sound is in 3D dimension */
  bool IsSound3D() const;

protected:

  /** returns true whether the sound is in 3D */
  bool is_3D_sound = false;
  /** the irrklang sound */
  boost::intrusive_ptr<irrklang::ISound> irrklang_sound;
  /** the source that generated this object */
  boost::intrusive_ptr<SoundSourceSimple> source;
};

                    /* ---------------- */

class SoundSequence : public SoundBase
{
  friend SoundSourceSequence;

protected:

  /** protected constructor */
  SoundSequence(class SoundSourceSequence * in_source);
  /** the sound method (returns true whether it is immediatly finished) */
  virtual bool PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks = nullptr) override;

protected:

  /** the index of next element to play */
  size_t index = 0;
  /** the source sequence thzat generated this object */
  boost::intrusive_ptr<SoundSourceSequence> source;
};



// ==============================================================
// SOURCES
// ==============================================================

class SoundSourceBase : public SoundManagedObject
{

protected:

  /** the accessibility function */
  virtual bool PlaySound(SoundBase * sound, PlaySoundDesc const & desc, SoundCallbacks * in_callbacks = nullptr);

public:

  /** the sound generation method */
  virtual SoundBase * GenerateSound();
   


};

                /* ---------------- */

class SoundSourceSimple : public SoundSourceBase
{
  friend class SoundSimple;

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

  /** generating a source object */
  virtual SoundBase * GenerateSound() override;
};

                /* ---------------- */

class SoundSourceRandom : public SoundSourceComposite
{
public:

};