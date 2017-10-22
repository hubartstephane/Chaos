#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>


namespace chaos
{

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

  public:

    /** get the name of the object */
    char const * GetName() const { return name.c_str(); }

  protected:

    /* the name */
    std::string name;
    /** the manager */
    class SoundManager * sound_manager = nullptr;
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

    /** get the own object volume */
    float GetVolume() const;
    /** change the volume of the object */
    void SetVolume(float in_volume);
    /** start a blend effect to 1.0f */
    void StartBlendIn(float blend_time, bool fullrange_blend_time = true);
    /** start a blend effect to 0.0f */
    void StartBlendOut(float blend_time, bool fullrange_blend_time = true);


    /** stop current blend effects */
    void StopBlending(bool set_blend_to_final_value = false);
    /** require the stop (destroy the object) with or without a blend time */
    void StopAndKill(float in_blendout_time);

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
    int   blend_type = BLEND_VOLUME_NONE;
    float blend_factor = 1.0f;
    float blend_time = 0.0f;
    /** whether the object is to be killed */
    bool should_kill = false;
  };










#if 0



  // ================================================================================



  // ================================================================================



  // ================================================================================

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

  public:




  };

  // ================================================================================

  class SoundLoopInfo
  {
  public:
    /** when the loop starts on the time line (negative for the very beginning of the sound) */
    float start = -1.0f;
    /** when the loop ends on the time line (negative for the very end of the sound) */
    float end = -1.0f;
    /** the time for blending (negative for no blending) */
    float blend_time = -1.0f;
  };

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

  class Play3DSoundDesc : public PlaySoundDesc
  {
  public:

    /** the position of the sound */
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    /** the speed of the sound */
    glm::vec3 speed = glm::vec3(0.0f, 0.0f, 0.0f);
  };

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

  public:

    /** play a sound */
    Sound * PlaySound(PlaySoundDesc const & desc);
    /** play a 3D sound */
    Sound * Play3DSound(Play3DSoundDesc const & desc);

    /** get the length in seconds of the source */
    float GetPlayLength() const;

  protected:

    /** returns true whether the source require a non conventionnal loop */
    bool IsManualLoopRequired(PlaySoundDesc const & desc) const;

    /** generate irrklang sound for a 2D sound */
    irrklang::ISound * DoPlayIrrklangSound(PlaySoundDesc const & desc, bool in_looping, bool in_paused);
    /** generate irrklang sound for a 3D sound */
    irrklang::ISound * DoPlayIrrklangSound(Play3DSoundDesc const & desc, bool in_looping, bool in_paused);

    /** generate pair of sounds (for manual looping) */
    template<typename T>
    std::pair<irrklang::ISound *, irrklang::ISound *> DoPlayIrrklangSoundPair(T const & desc)
    {
      assert(sound_manager != nullptr);

      std::pair<irrklang::ISound *, irrklang::ISound *> result;
      result.first = result.second = nullptr;

      bool manual_looping = IsManualLoopRequired(desc);

      irrklang::ISound * first = DoPlayIrrklangSound(desc, desc.looping && !manual_looping, false); // generate first sound
      if (first != nullptr)
      {
        if (manual_looping)
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
      assert(sound_manager != nullptr);

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
      if (irrklang_sound_pair.first == nullptr)
        return nullptr;

      // create the chaos side object
      Sound * result = new Sound(sound_manager);
      if (result != nullptr)
      {
        result->name = desc.sound_name;
        result->category = category;
        result->source = this;
        result->irrklang_sound = irrklang_sound_pair.first; // keep our own reference on irrklang objects
        result->irrklang_loop_sound = irrklang_sound_pair.second;
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
    SoundLoopInfo loop_info;
  };

  // ================================================================================

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
    /** get the final volume for the sound (category and blendings taken into account) */
    virtual float GetEffectiveVolume() const override;

  public:



    /** change the position */
    void SetPosition(glm::vec3 const & in_position);
    /** get the position */
    glm::vec3 GetPosition() const;
    /** change the speed */
    void SetSpeed(glm::vec3 const & in_speed);
    /** get the speed */
    glm::vec3 GetSpeed() const;







    /** tick the sounds */
    void Tick(float delta_time);
    /** returns true if the sound is finished */
    bool IsFinished() const;
    /** returns true whether the sound should be removed from the list at the end */
    bool ShouldKillAtEnd() const;
    /** stop the sound */
    void Stop(double in_blendout_factor);
    /** pause the sound */
    bool IsLooping() const;






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

    /** the sound */
    boost::intrusive_ptr<irrklang::ISound> irrklang_sound;
    boost::intrusive_ptr<irrklang::ISound> irrklang_loop_sound; // for looping special sound
                                                                /** the source */
    SoundSource * source = nullptr;
  };




  // ================================================================================


  class SoundManager : public chaos::ReferencedObject
  {
    friend class Sound;
    friend class SoundCategory;
    friend class SoundSource;

  public:

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




    void Tick(float delta_time);








  protected:

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
    void DetroyAllSoundPerCategory(SoundCategory * category);

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

#endif

}; // namespace chaos
