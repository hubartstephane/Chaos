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
  /** the name of the sound object to create */
  std::string sound_name;
  /** the name of the category ... */
  std::string category_name;
  /** ... or a pointer on the category */
  class SoundCategory * category = nullptr;

protected:

  /** returns true whether the sound is in 3D */
  bool is_3D_sound = false;
};

class TickDesc
{
public:

  /** the delta time */
  float delta_time = 0.0f;
  /** the cumulated volume */

};

// ==============================================================
// CALLBACKS
// ==============================================================

class SoundCallbacks : public chaos::ReferencedObject
{
  friend class Sound;
  friend class SoundSimple;

protected:

  /** called whenever a sound is finished */
  virtual void OnSoundFinished(Sound * in_sound);
  /** called whenever an object is removed from manager */
  virtual void OnRemovedFromManager(Sound * in_sound);
};

class SoundAutoCallbacks : public SoundCallbacks
{
public:

  /** default constructor */
  SoundAutoCallbacks() = default;
  /** assignation constructor */
  template<typename U, typename V>
  SoundAutoCallbacks(U & in_finished, V & in_removed) :
    sound_finished_func(in_finished),
    removed_from_manager_func(in_removed)
  {
  }

protected:

  /** called whenever a sound is finished */
  virtual void OnSoundFinished(Sound * in_sound) override
  {
    if (sound_finished_func)
      sound_finished_func(in_sound);
  }
  /** called whenever an object is removed from manager */
  virtual void OnRemovedFromManager(Sound * in_sound) override
  {
    if (removed_from_manager_func)
      removed_from_manager_func(in_sound);
  }

public:

  /** the callbacks function */
  std::function<void(Sound *)> sound_finished_func;
  /** the callbacks function */
  std::function<void(Sound *)> removed_from_manager_func;
};

// ==============================================================
// MANAGER
// ==============================================================

class SoundManager : public chaos::ReferencedObject
{
  friend class Sound;
  friend class SoundSource;
  friend class SoundCategory;

public:

  /** getter on the irrklang engine */
  irrklang::ISoundEngine * GetIrrklangEngine();

  /** start the manager */
  bool StartManager();
  /** stop the manager */
  bool StopManager();
  /** returns whether the manager is correctly started */
  bool IsManagerStarted() const;

  /** tick the manager */
  virtual void Tick(float delta_time);

  /** find a sound by its name */
  class Sound * FindSound(char const * name);
  /** find a category by its name */
  class SoundCategory * FindSoundCategory(char const * name);
  /** find a source by its name */
  class SoundSource * FindSoundSource(char const * name);

  /** find a sound by its name */
  class Sound const * FindSound(char const * name) const;
  /** find a category by its name */
  class SoundCategory const * FindSoundCategory(char const * name) const;
  /** find a source by its name */
  class SoundSource const * FindSoundSource(char const * name) const;

  /** find a simple source by its path */
  class SoundSourceSimple * FindSourceSimple(boost::filesystem::path const & in_path);
  /** find a simple source by its path */
  class SoundSourceSimple const * FindSourceSimple(boost::filesystem::path const & in_path) const;

  /** load and add a simple source inside the manager (name is a copy of filename) */
  SoundSourceSimple * AddSourceSimple(boost::filesystem::path const & in_path);
  /** load and add a simple source inside the manager */
  class SoundSourceSimple * AddSourceSimple(boost::filesystem::path const & in_path, char const * in_name);
  /** add a sequence inside the manager */
  class SoundSourceSequence * AddSourceSequence(char const * in_name);
  /** add a random inside the manager */
  class SoundSourceRandom * AddSourceRandom(char const * in_name);

  /** add a category inside the manager */
  class SoundCategory * AddSourceCategory(char const * in_name);

protected:

  /** test whether a sound with given name could be inserted in the manager */
  bool CanAddSound(char const * in_name) const;
  /** test whether a source with given name could be inserted in the manager */
  bool CanAddSource(char const * in_name) const;
  /** test whether a category with given name could be inserted in the manager */
  bool CanAddCategory(char const * in_name) const;
  /** simple method to initialize and insert a source */
  template<typename T> 
  T * DoAddSources(T * in_source, char const * in_name)
  {
    if (in_source == nullptr)
      return nullptr;
    if (in_name != nullptr)
      in_source->name = in_name;
    in_source->sound_manager = this;
    sources.push_back(in_source);
    return in_source;
  }

  /** remove a category from the list */
  void RemoveSoundCategory(class SoundCategory * in_category);
  /** remove a sound from the list */
  void RemoveSound(class Sound * in_sound);
  /** remove a sound source from the list */
  void RemoveSoundSource(class SoundSource * in_source);

  /** remove a category from the list */
  void RemoveSoundCategory(size_t index);
  /** remove a sound from the list */
  void RemoveSound(size_t index);
  /** remove a sound source from the list */
  void RemoveSoundSource(size_t index);

  /** utility function to remove a sound object from a list */
  template<typename T>
  void DoRemoveSoundObject(size_t index, T & vector)
  {
    size_t count = vector.size();
    if (index >= count)
      return;

    vector[index]->DetachFromManager(); // order is important because next operation could destroy the object

    if (index != count - 1)
      vector[index] = vector[count - 1];
    vector.pop_back();
  }

  /** detach all elements from a list */
  template<typename T>
  void DetachAllObjectsFromList(T & v)
  {
    size_t count = v.size();
    for (size_t i = 0; i < count; ++i)
    {
      auto obj = v[i].get();
      if (obj == nullptr)
        continue;
      obj->DetachFromManager();
    }
    v.clear();
  }

protected:

  /** all detected devices */
  boost::intrusive_ptr<irrklang::ISoundDeviceList> irrklang_devices;
  /** the irrklank engine */
  boost::intrusive_ptr<irrklang::ISoundEngine> irrklang_engine;

  /** the categories */
  std::vector<boost::intrusive_ptr<class SoundCategory>> categories;
  /** the sounds */
  std::vector<boost::intrusive_ptr<class Sound>> sounds;
  /** the sources */
  std::vector<boost::intrusive_ptr<class SoundSource>> sources;
};

// ==============================================================
// MANAGED OBJECT
// ==============================================================

class SoundManagedObject : public chaos::ReferencedObject
{
  friend class SoundManager;

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

  /** unbind from manager */
  virtual void DetachFromManager();
  /** remove element from manager list and detach it */
  virtual void RemoveFromManager();
  /** get whether the sound is finished */
  bool IsFinished() const;
  /** tick the sounds */
  virtual void Tick(float delta_time);

protected:

  /* the name */
  std::string name;
  /** the irrklank engine */
  boost::intrusive_ptr<SoundManager> sound_manager;
  /** whether the sound is finished */
  bool finished = false;
};

// ==============================================================
// VOLUME
// ==============================================================

class SoundManagedVolumeObject : public SoundManagedObject
{
  friend class SoundManager;

public:

  /** get the own object volume */
  float GetVolume() const;
  /** get the final volume for the sound (category and blendings taken into account) */
  virtual float GetEffectiveVolume() const;

protected:

  /** tick the sounds */
  virtual void Tick(float delta_time) override;

protected:

  /** the volume */
  float volume = 1.0f;
};

// ==============================================================
// CATEGORY
// ==============================================================

class SoundCategory : public SoundManagedVolumeObject
{
  friend class SoundManager;

protected:

  /** unbind from manager */
  virtual void DetachFromManager() override;
  /** remove element from manager list and detach it */
  virtual void RemoveFromManager() override;
  /** tick the sounds */
  virtual void Tick(float delta_time) override;
};

// ==============================================================
// SOUND
// ==============================================================

class Sound : public SoundManagedVolumeObject
{

  friend class SoundSource;
  friend class SoundSequence;
  friend class SoundSourceRandom;
  friend class SoundManager;

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
  virtual void OnSoundFinished(bool enable_callbacks);
  /** the sound method (returns true whether it is immediatly finished) */
  virtual bool DoPlaySound();
  /** unbind from manager */
  virtual void DetachFromManager() override;
  /** remove element from manager list and detach it */
  virtual void RemoveFromManager() override;
  /** tick the sounds */
  virtual void Tick(float delta_time) override;
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

  /** the category of the sound */
  class SoundCategory * category = nullptr;
};

                /* ---------------- */

class SoundSimple : public Sound
{
  friend class SoundSourceSimple;

protected:

  virtual ~SoundSimple();

  /** protected constructor */
  SoundSimple(class SoundSourceSimple * in_source);
  /** unbind from manager */
  virtual void DetachFromManager() override;
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

class SoundComposite : public Sound
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
  boost::intrusive_ptr<Sound> current_sound;
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
  virtual void OnSoundFinished(Sound * sound) override;

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

class SoundSource : public SoundManagedObject
{
  friend class SoundManager;

protected:

  /** the sound generation method */
  virtual Sound * GenerateSound();

public:

  /** generating and playing a sound */
  Sound * PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks = nullptr, bool enable_callbacks = true);

protected:

  /** unbind from manager */
  virtual void DetachFromManager() override;
  /** remove element from manager list and detach it */
  virtual void RemoveFromManager() override;

};

                /* ---------------- */

class SoundSourceSimple : public SoundSource
{
  friend class SoundSimple;
  friend class SoundManager;

public:

  /** get the path of the resource */
  boost::filesystem::path const & GetPath() const { return path; }

protected:

  /** generating a source object */
  virtual Sound * GenerateSound() override;
  /** unbind from manager */
  virtual void DetachFromManager() override;

protected:

  /** the irrklang source */
  boost::intrusive_ptr<irrklang::ISoundSource> irrklang_source;
  /** the resource path */
  boost::filesystem::path path;
};


                /* ---------------- */

class SoundSourceComposite : public SoundSource
{
  friend class SoundSequence;

public:

  /** add some source */
  void AddChildSource(SoundSource * in_source);

protected:

  /** child sources */
  std::vector<boost::intrusive_ptr<SoundSource>> child_sources;
};



                /* ---------------- */

class SoundSourceSequence : public SoundSourceComposite
{
  friend class SoundSequence;

protected:

  /** generating a source object */
  virtual Sound * GenerateSound() override;
};



                /* ---------------- */

class SoundSourceRandom : public SoundSourceComposite
{
protected:

};