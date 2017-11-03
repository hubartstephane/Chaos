#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/IrrklangTools.h>


namespace chaos
{

  /**
  * SoundObjectCallbacks : an object to store some callbacks
  */

  class SoundObjectCallbacks : public ReferencedObject
  {
  public:

    /** destructor */
    virtual ~SoundObjectCallbacks() = default;
    /** called whenever an object is finished */
    virtual void OnFinished(class SoundBaseObject * sound_object) {}
    /** called whenever an object is removed from manager */
    virtual void OnRemovedFromManager(class SoundBaseObject * sound_object) {}
  };

  /**
   * SoundBaseObject : base class for sound objects. They are object with a name that helps accessing them
   */

  class SoundBaseObject : public ReferencedObject
  {
    friend class SoundManager;

  protected:

    /** protected constructor */
    SoundBaseObject(class SoundManager * in_sound_manager);
    /** unbind from manager */
    virtual void DetachFromManager();
    /** returns whether the object is attached to a manager */
    bool IsAttachedToManager() const;
    /** remove the object from the manager */
    virtual void RemoveFromManager() {}

  public:

    /** get the name of the object */
    char const * GetName() const { return name.c_str(); }
    /** give a callback object to the gamepad */
    void SetCallbacks(SoundObjectCallbacks * in_callbacks);

  protected:

    /* the name */
    std::string name;
    /** the manager */
    class SoundManager * sound_manager = nullptr;
    /** the callbacks */
    boost::intrusive_ptr<SoundObjectCallbacks> callbacks;
  };

  /**
  * SoundVolumeObject : base class for sound objects that have a volume. the volume may be blended in or out
  */

  static int const BLEND_VOLUME_NONE = 0;
  static int const BLEND_VOLUME_IN   = 1;
  static int const BLEND_VOLUME_OUT  = 2;

  class SoundVolumeObject : public SoundBaseObject
  {
    friend class SoundManager;

  protected:

    /** protected constructor */
    SoundVolumeObject(class SoundManager * in_sound_manager);

    /** get the final volume for the object (category and blendings taken into account) */
    virtual float GetEffectiveVolume() const;

  public:

    /** returns true whether there is a pending kill */
    bool IsPendingKill() const;
    /** get the own object volume */
    float GetVolume() const;
    /** change the volume of the object */
    void SetVolume(float in_volume);
    /** start a blend effect to 1.0f */
    bool StartBlendIn(float in_blend_volume_time, bool in_fullrange_blend_volume_time = true);
    /** start a blend effect to 0.0f */
    bool StartBlendOut(float in_blend_volume_time, bool in_fullrange_blend_volume_time = true);
    /** require the stop (destroy the object) with or without a blend time */
    bool StopAndKill(float in_blendout_time, bool in_fullrange_blend_volume_time);

  protected:

    /** tick the object */
    virtual void Tick(float delta_time);
    /** whether the object is finished */
    virtual bool IsFinished() const;
    /** internal method to update the blend factor */
    void UpdateBlendFactor(float delta_time);

  protected:

    /** the volume */
    float volume = 1.0f;
    /** the blend factor (whether the object is blending) */
    int   blend_volume_type = BLEND_VOLUME_NONE;
    float blend_volume_factor = 1.0f;
    float blend_volume_time = 0.0f;
    /** whether the object is to be killed */
    bool pending_kill = false;
  };

  /**
  * SoundCategory : an object used to share some volume settings 
  */

  class SoundCategory : public SoundVolumeObject
  {
    friend class Sound;
    friend class SoundSource;
    friend class SoundManager;

  protected:

    /** constructor is protected */
    SoundCategory(class SoundManager * in_sound_manager);
    /** destructor too */
    virtual ~SoundCategory();
    /** unbind from manager */
    virtual void DetachFromManager() override;
    /** remove the object from the manager */
    virtual void RemoveFromManager() override;

  public:

    /** a category may be cloned and faded out (returns the cloned temp category) */
    SoundCategory * CloneCategoryAndStop(float in_blend_volume_time, bool in_fullrange_blend_volume_time = true);

  };

  /**
   * PlaySoundDesc : used as parameters for playing 2D sounds
   */

  class PlaySoundDesc
  {
  public:

    /** the name of the sound object to create*/
    std::string sound_name;
    /** the name of the category ... */
    std::string category_name;
    /** ... or a pointer on the category */
    SoundCategory * category = nullptr;
    /** whether we want to loop */
    bool looping = false;
  };

  /**
  * Play3DSoundDesc : used as parameters for playing 3D sounds
  */

  class Play3DSoundDesc : public PlaySoundDesc
  {
  public:

    /** the position of the sound */
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    /** the speed of the sound */
    glm::vec3 speed = glm::vec3(0.0f, 0.0f, 0.0f);
  };

  /**
   * SoundLoopInfo : use to describe how looping source should be handled
   */

  class SoundLoopInfo
  {
  public:
    /** when the loop starts on the time line (negative for the very beginning of the sound) */
    float start = -1.0f;
    /** when the loop ends on the time line (negative for the very end of the sound) */
    float end = -1.0f;
    /** the time for blending (0 => no blend, < 0 => blend before start, > 0 => blend after end) */
    float blend_time = -1.0f;
  };

  /**
  * SoundLoopInfoExt : SoundLoopInfo enriched with the track length
  */

  class SoundLoopInfoExt : public SoundLoopInfo
  {
  public:
    /** the length of the source */
    float play_length = -1.0f;
  };

  /**
   * SoundSource : used to describe some source of sound for irrklang
   */

  class SoundSource : public SoundBaseObject
  {
    friend class Sound;
    friend class SoundCategory;
    friend class SoundManager;

  protected:

    /** constructor is protected */
    SoundSource(class SoundManager * in_sound_manager);
    /** destructor too */
    virtual ~SoundSource();
    /** unbind from manager */
    virtual void DetachFromManager() override;
    /** remove the object from the manager */
    virtual void RemoveFromManager() override;

  public:

    /** play a sound */
    Sound * PlaySound(PlaySoundDesc const & desc);
    /** play a 3D sound */
    Sound * Play3DSound(Play3DSoundDesc const & desc);

    /** get the length in seconds of the source */
    float GetPlayLength() const;

  protected:

    /** returns true whether the source require a non conventionnal loop */
    static bool IsManualLoopRequired(SoundLoopInfoExt const & in_loop_info);
    /** get the length in seconds of the source (uncached value) */
    float DoGetPlayLength() const;
    /** Get clamped SoundLoopInfoExt */
    SoundLoopInfoExt GetClampedLoopInfoExt(SoundLoopInfo const & in_loop_info) const;

    /** generate irrklang sound for a 2D sound */
    irrklang::ISound * DoPlayIrrklangSound(PlaySoundDesc const & desc, bool in_looping, bool in_paused);
    /** generate irrklang sound for a 3D sound */
    irrklang::ISound * DoPlayIrrklangSound(Play3DSoundDesc const & desc, bool in_looping, bool in_paused);

    /** generate pair of sounds (for manual looping) */
    template<typename T>
    std::pair<irrklang::ISound *, irrklang::ISound *> DoPlayIrrklangSoundPair(T const & desc)
    {
      assert(IsAttachedToManager());

      std::pair<irrklang::ISound *, irrklang::ISound *> result;
      result.first = result.second = nullptr;

      bool manual_looping = desc.looping && IsManualLoopRequired(loop_info_ext);

      irrklang::ISound * first = DoPlayIrrklangSound(desc, desc.looping && !manual_looping, false); // generate first sound
      if (first != nullptr)
      {
        if (manual_looping && loop_info_ext.blend_time != 0.0f) // use blending => require a second track
        {
          irrklang::ISound * second = DoPlayIrrklangSound(desc, desc.looping && !manual_looping, true); // generate second sound if required (paused at beginning)
          if (second == nullptr)
          {
            first->drop(); // in case of failure of this second creation, destroy the first => failure of the whole function
            return result;
          }
          result.second = second;
        }
        result.first = first;
      }
      return result;
    }

    /** general function to play a sound */
    template<typename T>
    Sound * DoPlaySound(T const & desc)
    {
      assert(IsAttachedToManager());

      // get the engine
      irrklang::ISoundEngine * irrklang_engine = sound_manager->irrklang_engine.get();
      if (irrklang_engine == nullptr)
        return nullptr;

      // search the category
      SoundCategory * category = desc.category;
      if (category == nullptr)
        category = sound_manager->FindSoundCategory(desc.category_name.c_str());

      // generate the irrklang sound
      std::pair<irrklang::ISound *, irrklang::ISound *> irrklang_sound_pair = DoPlayIrrklangSoundPair(desc);
      if (irrklang_sound_pair.first == nullptr) // if the call fails, both first and second are garanted to be null
        return nullptr;

      // create the chaos side object
      Sound * result = new Sound(sound_manager);
      if (result != nullptr)
      {
        result->name = desc.sound_name;
        result->category = category;
        result->source = this;
        result->looping = desc.looping;
        result->irrklang_sound = irrklang_sound_pair.first; // keep our own reference on irrklang objects
        result->irrklang_loop_sound = irrklang_sound_pair.second;

        sound_manager->sounds.push_back(result);
      }

      // independant of success of failure, we have a copy of irrklang interface or we don't need them anymore
      // destroy the reference
      irrklang_sound_pair.first->drop();
      if (irrklang_sound_pair.second != nullptr)
        irrklang_sound_pair.second->drop();

      return result;
    }

  protected:

    /** the irrklang source */
    boost::intrusive_ptr<irrklang::ISoundSource> irrklang_source;
    /** the loop information */
    SoundLoopInfoExt loop_info_ext;
  };

  /**
   * Sound : a sound (may use several irrklang objects for looping) 
   */

  class Sound : public SoundVolumeObject
  {
    friend class SoundCategory;
    friend class SoundSource;
    friend class SoundManager;

  protected:

    /** constructor is protected */
    Sound(class SoundManager * in_sound_manager);
    /** destructor too */
    virtual ~Sound();
    /** unbind from manager */
    virtual void DetachFromManager() override;
    /** remove the object from the manager */
    virtual void RemoveFromManager() override;
    /** get the final volume for the sound (category and blendings taken into account) */
    virtual float GetEffectiveVolume() const override;
    /** tick the sounds */
    virtual void Tick(float delta_time) override;
    /** returns true if the sound is finished */
    virtual bool IsFinished() const override;

    /** internal method to tick a sound with no blending but with manual loop (returns volume of track 1) */
    float DoTickManualLoopNoBlend(float delta_time);
    /** internal method to tick a sound with blending and a manual loop (returns volume of track 1) */
    float DoTickManualLoopWithBlend(float delta_time);

  public:

    /** change the position */
    void SetPosition(glm::vec3 const & in_position);
    /** get the position */
    glm::vec3 GetPosition() const;
    /** change the speed */
    void SetSpeed(glm::vec3 const & in_speed);
    /** get the speed */
    glm::vec3 GetSpeed() const;

    /** get the category of the sound */
    SoundCategory const * GetCategory() const { return category; }
    /** get the category of the sound */
    SoundCategory * GetCategory(){ return category; }

    /** get the source of the sound */
    SoundSource const * GetSource() const { return source; }
    /** get the source of the sound */
    SoundSource * GetSource() { return source; }

    /** returns true whether the sound is in 3D */
    bool Is3DSound() const;
    /** returns true whether the sound is looping */
    bool IsLooping() const;

    /** get the position in seconds of the source */
    float GetPlayPosition() const;

  protected:

    /** an utility conversion method */
    static irrklang::vec3df ConvertVectorToIrrklang(glm::vec3 const & src);

  protected:

    /** the category */
    SoundCategory * category = nullptr;

    /** position */
    bool is_3D_sound = false;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 speed = glm::vec3(0.0f, 0.0f, 0.0f);

    /** whether we want to loop */
    bool looping = false;

    /** the sound */
    boost::intrusive_ptr<irrklang::ISound> irrklang_sound;
    boost::intrusive_ptr<irrklang::ISound> irrklang_loop_sound; // for looping special sound
    /** the source */
    SoundSource * source = nullptr;
  };

  /**
   * SoundManager : the main class to handle sound
   */

  class SoundManager : public ReferencedObject
  {
    friend class Sound;
    friend class SoundCategory;
    friend class SoundSource;

  public:

    /** destructor */
    virtual ~SoundManager();

    /** start the manager */
    bool StartManager();
    /** stop the manager */
    bool StopManager();

    /** find a sound by its name */
    Sound * FindSound(char const * name);
    /** find a category by its name */
    SoundCategory * FindSoundCategory(char const * name);
    /** find a source by its name */
    SoundSource * FindSoundSource(char const * name);

    /** add a source */
    SoundSource * AddSource(char const * in_filename, char const * in_name = nullptr, SoundLoopInfo in_loop_info = SoundLoopInfo());
    /** add a category */
    SoundCategory * AddCategory(char const * in_name = nullptr);

    /** the tick function of the manager */
    void Tick(float delta_time);

  protected:

    /** add a category internal method. If parent_category is not null, this means we want a temp object for fadeout and kill purpose */
    SoundCategory * DoAddCategory(char const * in_name, SoundCategory * parent_category);

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

    /** destroy all sounds in a category */
    void DestroyAllSoundPerCategory(SoundCategory * category);
    /** destroy all sounds with a given source */
    void DestroyAllSoundPerSource(SoundSource * source);
    /** replace some category by another in sound */
    void ReplaceSoundCategory(SoundCategory * new_category, SoundCategory * old_category);

    /** a generic function to find an object in a list by its name */
    template<typename T>
    T * FindSoundObject(char const * name, std::vector<boost::intrusive_ptr<T>> & objects)
    {
      if (name == nullptr)
        return nullptr;

      size_t count = objects.size();
      for (size_t i = 0; i < count; ++i)
      {
        T * object = objects[i].get();
        if (object == nullptr)
          continue;
        if (object->name == name)
          return object;
      }
      return nullptr;
    }

    /** remove a category from the list */
    void RemoveSoundCategory(SoundCategory * sound_category);
    /** remove a sound from the list */
    void RemoveSound(Sound * sound);
    /** remove a sound source from the list */
    void RemoveSoundSource(SoundSource * source);

    /** remove a category from the list */
    void RemoveSoundCategory(int index);
    /** remove a sound from the list */
    void RemoveSound(int index);
    /** remove a sound source from the list */
    void RemoveSoundSource(int index);


    /** utility function to remove a sound object from a list */
    template<typename T>
    void DoRemoveSoundObject(int index, T & vector)
    {
      assert(index >= 0 && index < (int)vector.size());

      vector[index]->DetachFromManager(); // order is important because next operation could destroy the object

      if (index != vector.size() - 1)
        vector[index] = vector[vector.size() - 1];
      vector.pop_back();
    }

  protected:

    /** all detected devices */
    boost::intrusive_ptr<irrklang::ISoundDeviceList> irrklang_devices;
    /** the irrklank engine */
    boost::intrusive_ptr<irrklang::ISoundEngine> irrklang_engine;

    /** the categories */
    std::vector<boost::intrusive_ptr<SoundCategory>> categories;
    /** the sounds */
    std::vector<boost::intrusive_ptr<Sound>> sounds;
    /** the sources */
    std::vector<boost::intrusive_ptr<SoundSource>> sources;
  };

}; // namespace chaos
