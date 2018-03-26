#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/IrrklangTools.h>
#include <chaos/FilePath.h>
#include <chaos/Manager.h>


// ==============================================================
// FORWARD DECLARATION / FRIENDSHIP MACROS
// ==============================================================

namespace chaos
{

  // all classes in this file
#define CHAOS_SOUND_CLASSES (PlaySoundDesc) (Sound) (SoundManager) (SoundCallbacks) (SoundAutoCallbacks) (SoundObject) (SoundSource) (SoundCategory)

  // forward declaration
#define CHAOS_SOUND_FORWARD_DECL(r, data, elem) class elem;
  BOOST_PP_SEQ_FOR_EACH(CHAOS_SOUND_FORWARD_DECL, _, CHAOS_SOUND_CLASSES)

    // friendship macro
#define CHAOS_SOUND_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_SOUND_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_SOUND_FRIEND_DECL, _, CHAOS_SOUND_CLASSES)

    // ==============================================================
    // CALLBACKS
    // ==============================================================

  class SoundCallbacks : public ReferencedObject
  {
    CHAOS_SOUND_ALL_FRIENDS

  protected:

    /** called whenever an object is finished */
    virtual void OnFinished(SoundObject * in_object);
    /** called whenever an object is removed from manager */
    virtual void OnRemovedFromManager(SoundObject * in_object);
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
    virtual void OnFinished(SoundObject * in_object) override;
    /** called whenever an object is removed from manager */
    virtual void OnRemovedFromManager(SoundObject * in_object) override;

  public:

    /** the callbacks function */
    std::function<void(SoundObject *)> finished_func;
    /** the callbacks function */
    std::function<void(SoundObject *)> removed_func;
  };

  // ==============================================================
  // BLEND VOLUME DESC
  // ==============================================================

  class BlendVolumeDesc
  {
  public:

    static int const BLEND_NONE = 0;
    static int const BLEND_IN = 1;
    static int const BLEND_OUT = 2;

    /** the kind of blending */
    int   blend_type = BLEND_NONE;
    /** the time to blend from [0 to 1] or [1 to 0] => if current blend value is between, the time is renormalized */
    float blend_time = 1.0f;
    /** whether the object should be paused at the end of blend */
    bool  pause_at_end = false;
    /** whether the object should be killed at the end of blend */
    bool  kill_at_end = false;
    /** some callbacks */
    boost::intrusive_ptr<SoundCallbacks> callbacks;
  };

  // ==============================================================
  // PLAY SOUND DESC
  // =============================================================

  class PlaySoundDesc
  {
    CHAOS_SOUND_ALL_FRIENDS

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

    /** the initial volume of the object */
    float volume = 1.0f;
    /** the blend in time of the object */
    float blend_in_time = 0.0f;

    /** the position of the sound in 3D */
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    /** the velocity of the sound in 3D */
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    /** true whether the sound is in 3D */
    bool is_3D_sound = false;

    /** the name of the sound object to create */
    std::string sound_name;
    /** the name of the category ... */
    std::string category_name;
    /** ... or a pointer on the category */
    SoundCategory * category = nullptr;
  };

  // ==============================================================
  // SOUND OBJECT
  // ==============================================================

  class SoundObject : public ReferencedObject
  {
    CHAOS_SOUND_ALL_FRIENDS

  public:

    /** getter on the irrklang engine */
    irrklang::ISoundEngine * GetIrrklangEngine();

    /** getter on the manager object */
    SoundManager * GetManager();
    /** getter on the manager object */
    SoundManager const * GetManager() const;

    /** blend the volume */
    bool StartBlend(BlendVolumeDesc const & desc, bool replace_older = false);

    /** stop the object */
    void Stop();

    /** returns whether the object is attached to a manager */
    bool IsAttachedToManager() const;
    /** get the name of the object */
    char const * GetName() const { return name.c_str(); }
    /** change the callbacks associated to this object */
    void SetCallbacks(SoundCallbacks * in_callbacks);
    /** get whether the sound is finished */
    bool IsFinished() const { return is_finished; }

    /** pause the object */
    virtual void Pause(bool in_pause = true);
    /** get the final pause status for the object */
    virtual bool IsEffectivePaused() const;
    /** get whether the object is paused */
    bool IsPaused() const;

    /** change the object volume */
    virtual void SetVolume(float in_volume);
    /** get the final volume for the sound (category and blendings taken into account) */
    virtual float GetEffectiveVolume() const;
    /** get the own object volume */
    float GetVolume() const;

    /** returns true whether there is a blendout and waiting stop */
    bool IsPendingKill() const;
    /** returns true whether there is a blending */
    bool HasVolumeBlending() const;

  protected:

    /** internal tick the object */
    virtual void TickObject(float delta_time);
    /** unbind from manager */
    virtual void OnRemovedFromManager();
    /** remove element from manager list and detach it */
    virtual void RemoveFromManager();
    /** get whether the sound is finished */
    virtual bool ComputeFinishedState();

    /** called at terminaison of the object */
    void OnObjectFinished();
    /** update the flag finished and return it */
    bool UpdateFinishedState();
    /** called at blend terminaison */
    void OnBlendFinished();

    /** loading from a JSON object */
    virtual bool InitializeFromJSON(nlohmann::json const & json, boost::filesystem::path const & config_path);

  protected:

    /** whether the object is finished */
    bool is_finished = false;
    /** whether the sound is paused */
    bool paused = false;
    /** the volume */
    float volume = 1.0f;
    /* the name */
    std::string name;
    /** the manager */
    SoundManager * sound_manager = nullptr;
    /** the blending description */
    BlendVolumeDesc blend_desc;
    /** the blend value */
    float           blend_value = 1.0f;
    /** the callbacks */
    boost::intrusive_ptr<SoundCallbacks> callbacks;
  };

  // ==============================================================
  // SOURCE
  // ==============================================================

  class SoundSource : public SoundObject
  {
    CHAOS_SOUND_ALL_FRIENDS

  public:

    /** generating and playing a sound */
    Sound * PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks = nullptr);
    /** get the path of the resource */
    boost::filesystem::path const & GetPath() const { return path; }

    /** pause the object */
    virtual void Pause(bool in_pause = true) override;
    /** change the object volume */
    virtual void SetVolume(float in_volume) override;

    /** get the category */
    SoundCategory * GetDefaultCategory() { return default_category; }
    /** get the category */
    SoundCategory const * GetDefaultCategory() const { return default_category; }

    /** set the category */
    bool SetDefaultCategory(SoundCategory * in_category);

  protected:

    /** unbind from manager */
    virtual void OnRemovedFromManager() override;
    /** remove element from manager list and detach it */
    virtual void RemoveFromManager() override;
    /** the sound generation method */
    virtual Sound * GenerateSound();

    /** loading from a JSON object */
    virtual bool InitializeFromJSON(nlohmann::json const & json, boost::filesystem::path const & config_path) override;

  protected:

    /** the resource path */
    boost::filesystem::path path;
    /** the irrklang source */
    boost::intrusive_ptr<irrklang::ISoundSource> irrklang_source;
    /** the default category */
    SoundCategory * default_category = nullptr;
  };

  // ==============================================================
  // CATEGORY
  // ==============================================================

  class SoundCategory : public SoundObject
  {
    CHAOS_SOUND_ALL_FRIENDS

  public:

    /** pause the object */
    virtual void Pause(bool in_pause = true) override;
    /** change the object volume */
    virtual void SetVolume(float in_volume) override;

  protected:

    /** unbind from manager */
    virtual void OnRemovedFromManager() override;
    /** remove element from manager list and detach it */
    virtual void RemoveFromManager() override;
  };

  // ==============================================================
  // SOUND
  // ==============================================================

  class Sound : public SoundObject
  {
    CHAOS_SOUND_ALL_FRIENDS

  public:

    /** set the position of the sound */
    void SetPosition(glm::vec3 const & in_position);
    /** set the velocity of the sound */
    void SetVelocity(glm::vec3 const & in_velocity);

    /** pause the object */
    virtual void Pause(bool in_pause = true) override;
    /** change the object volume */
    virtual void SetVolume(float in_volume) override;

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
    virtual bool DoPlaySound(PlaySoundDesc const & desc);
    /** unbind from manager */
    virtual void OnRemovedFromManager() override;
    /** remove element from manager list and detach it */
    virtual void RemoveFromManager() override;
    /** internal tick the sounds */
    virtual void TickObject(float delta_time) override;
    /** the method being called from exterior */
    void PlaySound(PlaySoundDesc const & desc, SoundCallbacks * in_callbacks = nullptr);

    /** get whether the sound is finished */
    virtual bool ComputeFinishedState() override;

    /** internal method to force the sound to update its volume */
    void DoUpdateVolume();
    /** internal method to force the sound to update its pause state */
    void DoUpdatePause();

  protected:

    /** returns true whether the sound is in 3D */
    bool is_3D_sound = false;
    /** whether the sound is looping */
    bool looping = false;

    /** the position of the sound in 3D */
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    /** the velocity of the sound in 3D */
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

    /** the category of the sound */
    SoundCategory * category = nullptr;
    /** the source that generated this object */
    SoundSource * source = nullptr;

    /** the irrklang sound */
    boost::intrusive_ptr<irrklang::ISound> irrklang_sound;
  };

  // ==============================================================
  // MANAGER
  // ==============================================================

  class SoundManager : public Manager
  {
    CHAOS_SOUND_ALL_FRIENDS

  public:

    /** getter on the irrklang engine */
    irrklang::ISoundEngine * GetIrrklangEngine();

    /** start the manager */
    bool StartManager();
    /** stop the manager */
    bool StopManager();
    /** returns whether the manager is correctly started */
    bool IsManagerStarted() const;

    /** public method to tick the manager */
    void Tick(float delta_time);

    /** find a source by its name */
    SoundSource * FindSource(char const * name);
    /** find a source by its name */
    SoundSource const * FindSource(char const * name) const;

    /** find a simple source by its path */
    SoundSource * FindSourceByPath(FilePathParam const & in_path);
    /** find a simple source by its path */
    SoundSource const * FindSourceByPath(FilePathParam const & in_path) const;

    /** find a category by its name */
    SoundCategory * FindCategory(char const * name);
    /** find a category by its name */
    SoundCategory const * FindCategory(char const * name) const;

    /** find a sound by its name */
    Sound * FindSound(char const * name);
    /** find a sound by its name */
    Sound const * FindSound(char const * name) const;

    /** add a category inside the manager */
    SoundCategory * AddCategory(char const * in_name);

    /** load and add a simple source inside the manager (name is a copy of filename) */
    SoundSource * AddSource(FilePathParam const & in_path);
    /** load and add a simple source inside the manager */
    SoundSource * AddSource(FilePathParam const & in_path, char const * in_name);

    /** update the listener position */
    bool SetListenerPosition(glm::mat4 const & view, glm::vec3 const & speed = glm::vec3(0.0f, 0.0f, 0.0f));

    /** initialize the manager from a configuration file */
    virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override;

  protected:

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

    /** called whenever an object is being removed */
    static void OnObjectRemovedFromManager(SoundObject * object);

    /** destroy all sounds in a category */
    void DestroyAllSoundPerCategory(SoundCategory * category);
    /** destroy all sounds with a given source */
    void DestroyAllSoundPerSource(SoundSource * source);
    /** remove the category of all sources using given argument */
    void UpdateAllSourcesPerCategory(SoundCategory * category);

    /** internal tick list of objects */
    template<typename T>
    void DoTickObjects(float delta_time, T & vector, void (SoundManager::*remove_func)(size_t))
    {
      for (size_t i = vector.size(); i > 0; --i) // in reverse order
      {
        size_t index = i - 1;

        auto object = vector[index]; // copy the intrusive_ptr to prevent the destruction 
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
          object->OnObjectFinished();
          (this->*remove_func)(index);
        }
      }
    }

    /** test whether a category with given name could be inserted in the manager */
    bool CanAddCategory(char const * in_name) const;
    /** test whether a source with given name could be inserted in the manager */
    bool CanAddSource(char const * in_name) const;
    /** test whether a sound with given name could be inserted in the manager */
    bool CanAddSound(char const * in_name) const;

    /** utility function to test whether an object can be inserted */
    template<typename T>
    bool CanAddObject(char const * in_name, T const * (SoundManager::*find_func)(char const *) const) const
    {
      // manager initialized ?
      if (!IsManagerStarted())
        return false;
      // name already existing ?
      if (in_name != nullptr && (this->*find_func)(in_name) != nullptr)
        return false;
      return true;
    }

    /** simple method to initialize and insert a source */
    SoundSource * DoAddSource(SoundSource * in_source, char const * in_name);

    /** update all sounds pause per category */
    void UpdateAllSoundPausePerCategory(SoundCategory * category);
    /** update all sounds volume per category */
    void UpdateAllSoundVolumePerCategory(SoundCategory * category);
    /** update all sounds pause per source */
    void UpdateAllSoundPausePerSource(SoundSource * source);
    /** update all sounds volume per source */
    void UpdateAllSoundVolumePerSource(SoundSource * source);

    /** add a category from a JSON object */
    SoundCategory * AddJSONCategory(char const * keyname, nlohmann::json const & json, boost::filesystem::path const & config_path);
    /** add a source from a JSON object */
    SoundSource * AddJSONSource(char const * keyname, nlohmann::json const & json, boost::filesystem::path const & config_path);

    /** load the categories from configuration */
    bool InitializeCategoriesFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);
    /** load the sources from configuration */
    bool InitializeSourcesFromConfiguration(nlohmann::json const & json, boost::filesystem::path const & config_path);

  protected:

    /** all detected devices */
    boost::intrusive_ptr<irrklang::ISoundDeviceList> irrklang_devices;
    /** the irrklank engine */
    boost::intrusive_ptr<irrklang::ISoundEngine> irrklang_engine;

    /** the sources */
    std::vector<boost::intrusive_ptr<SoundSource>> sources;
    /** the sounds */
    std::vector<boost::intrusive_ptr<Sound>> sounds;
    /** the categories */
    std::vector<boost::intrusive_ptr<SoundCategory>> categories;
  };

}; // namespace chaos

   // undefine macros
#undef CHAOS_SOUND_CLASSES
#undef CHAOS_SOUND_FORWARD_DECL
#undef CHAOS_SOUND_FRIEND_DECL
#undef CHAOS_SOUND_ALL_FRIENDS

