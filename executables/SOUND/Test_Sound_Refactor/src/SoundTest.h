#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/IrrklangTools.h>

// ==============================================================
// FORWARD DECLARATION / FRIENDSHIP MACROS
// ==============================================================

// all classes in this file
#define CHAOS_SOUND_CLASSES (PlaySoundDesc) (Sound) (SoundManager) (SoundCallbacks) (SoundAutoCallbacks) (SoundObject) (SoundObjectOwner) (SoundComposite) (SoundSource) (SoundSourceSimple) (SoundSourceComposite) (SoundSourceSequence) (SoundSourceRandom) (SoundCategory) (SoundSimple) (SoundComposite) (SoundSequence) (SoundRandom) (SoundManagedObject)

// forward declaration
#define CHAOS_SOUND_FORWARD_DECL(r, data, elem) class elem;
BOOST_PP_SEQ_FOR_EACH(CHAOS_SOUND_FORWARD_DECL, _, CHAOS_SOUND_CLASSES)

// friendship macro
#define CHAOS_SOUND_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_SOUND_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_SOUND_FRIEND_DECL, _, CHAOS_SOUND_CLASSES)

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
  void SetPosition(glm::vec3 const & in_position, bool update_3D_sound = true);
  /** set the velocity of the sound (this enables the 3D feature) */
  void SetVelocity(glm::vec3 const & in_velocity, bool update_3D_sound = true);

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
  SoundCategory * category = nullptr;

protected:

  /** returns true whether the sound is in 3D */
  bool is_3D_sound = false;
};

// ==============================================================
// CALLBACKS
// ==============================================================

class SoundCallbacks : public chaos::ReferencedObject
{
  CHAOS_SOUND_ALL_FRIENDS

protected:

  /** called whenever an object is finished */
  virtual void OnFinished(SoundManagedObject * in_object);
  /** called whenever an object is removed from manager */
  virtual void OnRemovedFromOwner(SoundManagedObject * in_object);
};

class SoundAutoCallbacks : public SoundCallbacks
{
  CHAOS_SOUND_ALL_FRIENDS

public:

  /** default constructor */
  SoundAutoCallbacks() = default;
  /** assignation constructor */
  template<typename U, typename V>
  SoundAutoCallbacks(U & in_finished, V & in_removed) :
    finished_func(in_finished),
    removed_func(in_removed)
  {
  }

protected:

  /** called whenever a sound is finished */
  virtual void OnFinished(SoundManagedObject * in_object) override;
  /** called whenever an object is removed from manager */
  virtual void OnRemovedFromOwner(SoundManagedObject * in_object) override;

public:

  /** the callbacks function */
  std::function<void(SoundManagedObject *)> finished_func;
  /** the callbacks function */
  std::function<void(SoundManagedObject *)> removed_func;
};

// ==============================================================
// SOUND OBJECT
// ==============================================================

class SoundObject : public chaos::ReferencedObject
{
  CHAOS_SOUND_ALL_FRIENDS

public:

  /** getter on the irrklang engine */
  irrklang::ISoundEngine * GetIrrklangEngine();

  /** getter on the manager object */
  virtual SoundManager * GetManager();
  /** getter on the manager object */
  virtual SoundManager const * GetManager() const;

  /** returns whether the object is attached to a manager */
  bool IsAttachedToManager() const;

protected:

  /** internal tick the object */
  virtual void TickObject(float delta_time);
};



//---------------------------------------------------------------------



#if 0
public:

/** find a category by its name */
SoundCategory * FindCategory(char const * name);
/** find a category by its name */
SoundCategory const * FindCategory(char const * name) const;


/** add a category inside the manager */
SoundCategory * AddSourceCategory(char const * in_name);

protected:

/** test whether a category with given name could be inserted in the manager */
bool CanAddCategory(char const * in_name) const;

protected:

/** the categories */
std::vector<boost::intrusive_ptr<SoundCategory>> categories;

#endif

// ==============================================================
// SOUND SOURCE OWNER
// ==============================================================

class SoundObjectOwner : public SoundObject
{
  CHAOS_SOUND_ALL_FRIENDS

public:

  /** find a source by its name */
  SoundSource * FindSource(char const * name);
  /** find a source by its name */
  SoundSource const * FindSource(char const * name) const;

  /** find a simple source by its path */
  SoundSourceSimple * FindSourceSimple(boost::filesystem::path const & in_path);
  /** find a simple source by its path */
  SoundSourceSimple const * FindSourceSimple(boost::filesystem::path const & in_path) const;

  /** find a sound by its name */
  Sound * FindSound(char const * name);
  /** find a sound by its name */
  Sound const * FindSound(char const * name) const;

  /** load and add a simple source inside the manager (name is a copy of filename) */
  SoundSourceSimple * AddSourceSimple(boost::filesystem::path const & in_path);
  /** load and add a simple source inside the manager */
  SoundSourceSimple * AddSourceSimple(boost::filesystem::path const & in_path, char const * in_name);

  /** add a sequence inside the manager */
  SoundSourceSequence * AddSourceSequence(char const * in_name);
  /** add a random inside the manager */
  SoundSourceRandom * AddSourceRandom(char const * in_name);

protected:

  /** test whether a source with given name could be inserted in the manager */
  bool CanAddSource(char const * in_name) const;
  /** test whether a sound with given name could be inserted in the manager */
  bool CanAddSound(char const * in_name) const;

  /** utility function to test whether an object can be inserted */
  template<typename T>
  bool CanAddObject(char const * in_name, T const * (SoundManager::*find_func)(char const *) const) const
  {
    // manager initialized ?
    SoundManager * manager = GetManager();
    if (manager == nullptr || !manager->IsManagerStarted())
      return false;
    // name already existing ?
    if (in_name != nullptr && (this->*find_func)(in_name) != nullptr)
      return false;
    return true;
  }

  /** simple method to initialize and insert a source */
  template<typename T>
  T * DoAddSources(T * in_source, char const * in_name)
  {
    if (in_source == nullptr)
      return nullptr;
    if (in_name != nullptr)
      in_source->name = in_name;
    in_source->owner = this;
    sources.push_back(in_source);
    return in_source;
  }

protected:

  /** the sources */
  std::vector<boost::intrusive_ptr<SoundSource>> sources;
  /** the sounds */
  std::vector<boost::intrusive_ptr<Sound>> sounds;
};















// ==============================================================
// MANAGER
// ==============================================================

class SoundManager : public SoundObjectOwner
{
  CHAOS_SOUND_ALL_FRIENDS

public:

  /** start the manager */
  bool StartManager();
  /** stop the manager */
  bool StopManager();
  /** returns whether the manager is correctly started */
  bool IsManagerStarted() const;

  /** getter on the manager object */
  virtual SoundManager * GetManager() override;
  /** getter on the manager object */
  virtual SoundManager const * GetManager() const override;

  /** public method to tick the manager */
  void Tick(float delta_time);

  /** update the listener position */
  bool SetListenerPosition(glm::mat4 const & view, glm::vec3 const & speed = glm::vec3(0.0f, 0.0f, 0.0f));

protected:

  /** internal tick the manager */
  virtual void TickObject(float delta_time) override;

  /** remove a category from the list */
  void RemoveCategory(SoundCategory * in_category);
  /** remove a sound from the list */
  void RemoveSound(Sound * in_sound);
  /** remove a sound source from the list */
  void RemoveSource(SoundSource * in_source);

  /** remove a category from the list */
  void RemoveCategory(size_t index);
  /** remove a sound from the list */
  void RemoveSound(size_t index);
  /** remove a sound source from the list */
  void RemoveSource(size_t index);

  /** utility function to remove a sound object from a list */
  template<typename T>
  void DoRemoveObject(size_t index, T & vector)
  {
    // ensure the index is valid
    size_t count = vector.size();
    if (index >= count)
      return;
    // copy the intrusive_ptr to prevent the destruction
    auto object = vector[index]; 
    // remove the object from the array
    if (index != count - 1)
      vector[index] = vector[count - 1];
    vector.pop_back();
    // callback then let the unreferencement manage the object lifetime
    object->OnRemovedFromOwner();
  }

  /** detach all elements from a list */
  template<typename T>
  void DetachAllObjectsFromList(T & vector)
  {
    while (vector.size() > 0)
    {
      // copy the intrusive_ptr to prevent the destruction
      auto object = vector.back();
      // remove the object from the array
      vector.pop_back();
      // callback then let the unreferencement manage the object lifetime
      object->OnRemovedFromOwner();
    }
  }

  /** destroy all sounds in a category */
  void DestroyAllSoundPerCategory(SoundCategory * category);
  /** destroy all sounds with a given source */
  void DestroyAllSoundPerSource(SoundSource * source);

  /** internal tick list of objects */
  template<typename T>
  void DoTickObjects(float delta_time, T & vector, void (SoundManager::*remove_func)(size_t))
  {
    for (size_t i = vector.size(); i > 0; --i)
    {
      size_t index = i - 1;

      auto * object = vector[index].get();
      if (object == nullptr)
        continue;

      // test whether object was already finished before ticking
      bool finished = object->IsFinished();
      bool paused = object->IsEffectivePaused();
      bool should_remove = finished;

      // call tick if required 
      if (!finished && !paused)
      {
        object->TickObject(delta_time);
        should_remove = object->UpdateFinishedState();
      }
      // remove the object if needed
      if (should_remove)
      {
        object->OnFinished();
        (this->*remove_func)(index);
      }
    }
  }

protected:

  /** all detected devices */
  boost::intrusive_ptr<irrklang::ISoundDeviceList> irrklang_devices;
  /** the irrklank engine */
  boost::intrusive_ptr<irrklang::ISoundEngine> irrklang_engine;
};









// ==============================================================
// MANAGED OBJECT
// ==============================================================

class SoundManagedObject : public SoundObject
{
  CHAOS_SOUND_ALL_FRIENDS

public:

  /** getter on the manager object */
  virtual SoundManager * GetManager() override;
  /** getter on the manager object */
  virtual SoundManager const * GetManager() const override;

  /** get the name of the object */
  char const * GetName() const { return name.c_str(); }

  /** change the callbacks associated to this object */
  void SetCallbacks(SoundCallbacks * in_callbacks);

  /** get whether the sound is finished */
  bool IsFinished() const { return is_finished; }

protected:

  /** unbind from manager */
  virtual void OnRemovedFromOwner();
  /** remove element from manager list and detach it */
  virtual void RemoveFromOwner();
  /** get whether the sound is finished */
  virtual bool ComputeFinishedState();

  /** internal tick the sounds */
  virtual void TickObject(float delta_time) override;

  /** called at terminaison of the object */
  void OnFinished();
  /** update the flag finished and return it */
  bool UpdateFinishedState();

protected:

  /** the name */
  std::string name;
  /** whether the object is finished */
  bool is_finished = false;
  /** the irrklank engine */
  boost::intrusive_ptr<SoundObjectOwner> owner;
  /** the callbacks that are being called at the end of the object */
  boost::intrusive_ptr<SoundCallbacks> callbacks;
};








// ==============================================================
// VOLUME
// ==============================================================

class SoundManagedVolumeObject : public SoundManagedObject
{
  CHAOS_SOUND_ALL_FRIENDS

public:

  /** pause the object */
  virtual void Pause();
  /** resume the object */
  virtual void Resume();

  /** get whether the object is paused */
  bool IsPaused() const;
  /** get the final pause status for the object */
  virtual bool IsEffectivePaused() const;

  /** get the own object volume */
  float GetVolume() const;
  /** get the final volume for the sound (category and blendings taken into account) */
  virtual float GetEffectiveVolume() const;

  /** get whether the sound is finished */
  virtual bool ComputeFinishedState() override;

protected:

  /** internal tick the sounds */
  virtual void TickObject(float delta_time) override;

protected:

  /** whether the sound is paused */
  bool paused = false;
  /** the volume */
  float volume = 1.0f;
};

// ==============================================================
// CATEGORY
// ==============================================================

class SoundCategory : public SoundManagedVolumeObject
{
  CHAOS_SOUND_ALL_FRIENDS

protected:

  /** unbind from manager */
  virtual void OnRemovedFromOwner() override;
  /** remove element from manager list and detach it */
  virtual void RemoveFromOwner() override;
  /** internal tick the sounds */
  virtual void TickObject(float delta_time) override;
};










// ==============================================================
// SOUND
// ==============================================================

class Sound : public SoundManagedVolumeObject
{
  CHAOS_SOUND_ALL_FRIENDS

public:

  /** set the position of the sound (this enables the 3D feature) */
  virtual void SetPosition(glm::vec3 const & in_position, bool update_3D_sound = true);
  /** set the velocity of the sound (this enables the 3D feature) */
  virtual void SetVelocity(glm::vec3 const & in_velocity, bool update_3D_sound = true);
  /** stop the sound */
  virtual void Stop();

  /** returns whether the sound is effectively paused */
  virtual bool IsEffectivePaused() const override;
  /** returns the effective volume of the sound */
  virtual float GetEffectiveVolume() const override;

  /** returns whether the sound is in 3D dimension */
  bool IsSound3D() const;
  /** get the position of the sound */
  glm::vec3 GetPosition() const;
  /** get the velocity of the sound */
  glm::vec3 GetVelocity() const;

  /** get whether the sound is looping */
  bool IsLooping() const;

protected:

  /** the sound method (returns true whether it is immediatly finished) */
  virtual bool DoPlaySound();
  /** unbind from manager */
  virtual void OnRemovedFromOwner() override;
  /** remove element from manager list and detach it */
  virtual void RemoveFromOwner() override;
  /** internal tick the sounds */
  virtual void TickObject(float delta_time) override;
  /** the method being called from exterior */
  void PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks = nullptr);

protected:

  /** returns true whether the sound is in 3D */
  bool is_3D_sound = false;
  /** the position of the sound in 3D */
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
  /** the velocity of the sound in 3D */
  glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
  /** whether the sound is looping */
  bool looping = false;

  /** the category of the sound */
  SoundCategory * category = nullptr;
  /** the source that generated this object */
  SoundSource * source = nullptr;
};

                /* ---------------- */

class SoundSimple : public Sound
{
  CHAOS_SOUND_ALL_FRIENDS

protected:

  /** unbind from manager */
  virtual void OnRemovedFromOwner() override;
  /** the sound method (returns true whether it is immediatly finished) */
  virtual bool DoPlaySound() override;
  /** internal tick the object */
  virtual void TickObject(float delta_time);
  /** get whether the sound is finished */
  virtual bool ComputeFinishedState();
 
public:

  /** returns whether the sound is in 3D dimension */
  bool IsSound3D() const;
  /** overriding some methods */
  virtual void SetPosition(glm::vec3 const & in_position, bool update_3D_sound = true) override;
  virtual void SetVelocity(glm::vec3 const & in_velocity, bool update_3D_sound = true) override;
  virtual void Pause() override;
  virtual void Resume() override;
  virtual void Stop() override;

protected:

  /** returns true whether the sound is in 3D */
  bool is_3D_sound = false;
  /** the irrklang sound */
  boost::intrusive_ptr<irrklang::ISound> irrklang_sound;
};

                    /* ---------------- */

class SoundComposite : public Sound
{
  CHAOS_SOUND_ALL_FRIENDS

public:

  /** overriding some methods */
  virtual void SetPosition(glm::vec3 const & in_position, bool update_3D_sound = true) override;
  virtual void SetVelocity(glm::vec3 const & in_velocity, bool update_3D_sound = true) override;
  virtual void Pause() override;
  virtual void Resume() override;
  virtual void Stop() override;

protected:

  /** protected constructor */
  SoundComposite(SoundSourceComposite * in_source);
  /** internal tick the sounds */
  virtual void TickObject(float delta_time) override;
  /** called whenever a child element is finished (returns true when completed) */
  virtual bool DoPlayNextSound();

protected:

  /** the sound that is currently being played */
  boost::intrusive_ptr<Sound> current_sound;
  /** the source composite that generated this object */
  boost::intrusive_ptr<SoundSourceComposite> source;
};

                    /* ---------------- */

class SoundSequence : public SoundComposite
{
  CHAOS_SOUND_ALL_FRIENDS

protected:

  /** protected constructor */
  SoundSequence(SoundSourceSequence * in_source);
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
  CHAOS_SOUND_ALL_FRIENDS

protected:

  /** the sound generation method */
  virtual Sound * GenerateSound();

public:

  /** generating and playing a sound */
  Sound * PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks = nullptr);

protected:

  /** unbind from manager */
  virtual void OnRemovedFromOwner() override;
  /** remove element from manager list and detach it */
  virtual void RemoveFromOwner() override;

};

                /* ---------------- */

class SoundSourceSimple : public SoundSource
{
  CHAOS_SOUND_ALL_FRIENDS

public:

  /** get the path of the resource */
  boost::filesystem::path const & GetPath() const { return path; }

protected:

  /** generating a source object */
  virtual Sound * GenerateSound() override;
  /** unbind from manager */
  virtual void OnRemovedFromOwner() override;

protected:

  /** the irrklang source */
  boost::intrusive_ptr<irrklang::ISoundSource> irrklang_source;
  /** the resource path */
  boost::filesystem::path path;
};


                /* ---------------- */

class SoundSourceComposite : public SoundSource
{
  CHAOS_SOUND_ALL_FRIENDS

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
  CHAOS_SOUND_ALL_FRIENDS

protected:

  /** generating a source object */
  virtual Sound * GenerateSound() override;
};



                /* ---------------- */

class SoundSourceRandom : public SoundSourceComposite
{
  CHAOS_SOUND_ALL_FRIENDS

protected:

};




// undefine macros
#undef CHAOS_SOUND_CLASSES
#undef CHAOS_SOUND_FORWARD_DECL
#undef CHAOS_SOUND_FRIEND_DECL
#undef CHAOS_SOUND_ALL_FRIENDS

