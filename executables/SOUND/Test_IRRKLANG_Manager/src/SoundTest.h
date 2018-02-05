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
  /** set the 3D flag */
  void Enable3D(bool enable);

  /** set the position of the sound (this enables the 3D feature) */
  void SetPosition(glm::vec3 const & in_position, bool set_3D_sound = true);
  /** set the velocity of the sound (this enables the 3D feature) */
  void SetVelocity(glm::vec3 const & in_velocity, bool set_3D_sound = true);

public:

  /** whether the sound is paused at the beginning */
  bool paused = false;
  /** whether the sound is looping */
  bool looping = false;
  /** the position of the sound in 3D */
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
  /** the velocity of the sound in 3D */
  glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

protected:

  /** returns true whether the sound is in 3D */
  bool is_3D_sound = false;
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
  SoundCallbackIrrklangWrapper(SoundSimple * in_sound);

public:

  /** the destructor */
  virtual ~SoundCallbackIrrklangWrapper();

  /** the method to override */
  virtual void OnSoundStopped(irrklang::ISound* irrklang_sound, irrklang::E_STOP_EVENT_CAUSE reason, void* userData) override;

protected:

  /** the sound object concerned */
  boost::intrusive_ptr<SoundSimple> sound;
};

// ==============================================================
// MANAGER
// ==============================================================

class SoundManager : public chaos::ReferencedObject
{
public:

  /** getter on the irrklang engine */
  irrklang::ISoundEngine * GetIrrklangEngine();

  /** start the manager */
  bool StartManager();
  /** stop the manager */
  bool StopManager();
  /** returns whether the manager is correctly started */
  bool IsManagerStarted() const;

  /** find a sound by its name */
  class SoundBase * FindSound(char const * name);
  /** find a category by its name */
  class SoundCategory * FindSoundCategory(char const * name);
  /** find a source by its name */
  class SoundSourceBase * FindSoundSource(char const * name);

  /** load and add a simple source inside the manager */
  class SoundSourceSimple * AddSourceSimple(char const * in_filename, char const * in_name);
  /** add a source inside the manager */
  bool AddSource(class SoundSourceBase * in_source);

protected:

  /** all detected devices */
  boost::intrusive_ptr<irrklang::ISoundDeviceList> irrklang_devices;
  /** the irrklank engine */
  boost::intrusive_ptr<irrklang::ISoundEngine> irrklang_engine;

  /** the categories */
  std::vector<boost::intrusive_ptr<class SoundCategory>> categories;
  /** the sounds */
  std::vector<boost::intrusive_ptr<class SoundBase>> sounds;
  /** the sources */
  std::vector<boost::intrusive_ptr<class SoundSourceBase>> sources;
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

  /** returns whether the object is attached to a manager */
  bool IsAttachedToManager() const;

  /** get the name of the object */
  char const * GetName() const { return name.c_str(); }

protected:

  /* the name */
  std::string name;
  /** the irrklank engine */
  boost::intrusive_ptr<SoundManager> sound_manager;
};

// ==============================================================
// VOLUME
// ==============================================================

class SoundManagedVolumeObject : public SoundManagedObject
{

public:

  /** get the own object volume */
  float GetVolume() const;
  /** get the final volume for the sound (category and blendings taken into account) */
  virtual float GetEffectiveVolume() const;

protected:

  /** the volume */
  float volume;
};

// ==============================================================
// CATEGORY
// ==============================================================

class SoundCategory : public SoundManagedVolumeObject
{


};

// ==============================================================
// SOUND
// ==============================================================

class SoundBase : public SoundManagedVolumeObject
{
  friend class SoundSourceBase;
  friend class SoundSequence;
  friend class SoundSourceRandom;
  friend class SoundCallbackIrrklangWrapper;

public:

  /** set the position of the sound (this enables the 3D feature) */
  virtual void SetPosition(glm::vec3 const & in_position, bool set_3D_sound = true);
  /** set the velocity of the sound (this enables the 3D feature) */
  virtual void SetVelocity(glm::vec3 const & in_velocity, bool set_3D_sound = true);
  /** pause the sound */
  virtual void Pause();
  /** resume the sound */
  virtual void Resume();
  /** stop the sound */
  virtual void Stop();

  /** returns whether the sound is in 3D dimension */
  bool IsSound3D() const;
  /** get the position of the sound */
  glm::vec3 GetPosition() const;
  /** get the velocity of the sound */
  glm::vec3 GetVelocity() const;
  /** get whether the sound is paused */
  bool IsPaused() const;
  /** get whether the sound is looping */
  bool IsLooping() const;

protected:

  /** accessibility function */
  virtual void OnSoundFinished();
  /** the sound method (returns true whether it is immediatly finished) */
  virtual bool DoPlaySound();
  /** the method being called from exterior */
  bool PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks = nullptr, bool enable_callbacks = true);

protected:

  /** returns true whether the sound is in 3D */
  bool is_3D_sound = false;
  /** the position of the sound in 3D */
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
  /** the velocity of the sound in 3D */
  glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
  /** whether the sound is paused */
  bool paused = false;
  /** whether the sound is looping */
  bool looping = false;

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
  virtual bool DoPlaySound() override;
 
public:

  /** returns whether the sound is in 3D dimension */
  bool IsSound3D() const;
  /** overriding some methods */
  virtual void SetPosition(glm::vec3 const & in_position, bool set_3D_sound = true) override;
  virtual void SetVelocity(glm::vec3 const & in_velocity, bool set_3D_sound = true) override;
  virtual void Pause() override;
  virtual void Resume() override;
  virtual void Stop() override;

protected:

  /** returns true whether the sound is in 3D */
  bool is_3D_sound = false;
  /** the irrklang sound */
  boost::intrusive_ptr<irrklang::ISound> irrklang_sound;
  /** the source that generated this object */
  boost::intrusive_ptr<SoundSourceSimple> source;
};

                    /* ---------------- */

class SoundComposite : public SoundBase
{
  friend class SoundSourceSequence;
  friend class SoundCompositeCallbacks;

public:

  /** overriding some methods */
  virtual void SetPosition(glm::vec3 const & in_position, bool set_3D_sound = true) override;
  virtual void SetVelocity(glm::vec3 const & in_velocity, bool set_3D_sound = true) override;
  virtual void Pause() override;
  virtual void Resume() override;
  virtual void Stop() override;

protected:

  /** protected constructor */
  SoundComposite(class SoundSourceComposite * in_source);
  /** called whenever a child element is finished (returns true when completed) */
  virtual bool DoPlayNextSound();

protected:

  /** the sound that is currently being played */
  boost::intrusive_ptr<SoundBase> current_sound;
  /** the source composite that generated this object */
  boost::intrusive_ptr<SoundSourceComposite> source;
};

class SoundCompositeCallbacks : public SoundCallbacks
{
  friend class SoundComposite;
  friend class SoundSequence;
  
protected:

  /** protected constructor */
  SoundCompositeCallbacks(SoundComposite * in_sound);
  /** called whenever a sound is finished */
  virtual void OnSoundFinished(SoundBase * sound) override;

protected:

  /** the composite */
  boost::intrusive_ptr<SoundComposite> sound_composite;
};

                    /* ---------------- */

class SoundSequence : public SoundComposite
{
  friend class SoundSourceSequence;

protected:

  /** protected constructor */
  SoundSequence(class SoundSourceSequence * in_source);
  /** the sound method (returns true whether it is immediatly finished) */
  virtual bool DoPlaySound() override;
  /** called whenever a child element is finished (returns true when completed) */
  virtual bool DoPlayNextSound() override;

protected:

  /** the index of next element to play */
  size_t index = 0;
};



// ==============================================================
// SOURCES
// ==============================================================

class SoundSourceBase : public SoundManagedObject
{

protected:

  /** the sound generation method */
  virtual SoundBase * GenerateSound();

public:

  /** generating and playing a sound */
  SoundBase * PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks = nullptr, bool enable_callbacks = true);
};

                /* ---------------- */

class SoundSourceSimple : public SoundSourceBase
{
  friend class SoundSimple;

protected:

  /** generating a source object */
  virtual SoundBase * GenerateSound() override;

protected:

  /** the irrklang source */
  boost::intrusive_ptr<irrklang::ISoundSource> irrklang_source;
};


                /* ---------------- */

class SoundSourceComposite : public SoundSourceBase
{
  friend class SoundSequence;

public:

  /** add some source */
  void AddChildSource(SoundSourceBase * in_source);

protected:

  /** child sources */
  std::vector<boost::intrusive_ptr<SoundSourceBase>> child_sources;
};



                /* ---------------- */

class SoundSourceSequence : public SoundSourceComposite
{
  friend class SoundSequence;

protected:

  /** generating a source object */
  virtual SoundBase * GenerateSound() override;
};



                /* ---------------- */

class SoundSourceRandom : public SoundSourceComposite
{
protected:

};