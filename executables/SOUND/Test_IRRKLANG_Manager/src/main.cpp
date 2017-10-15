#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/Buffer.h> 
#include <chaos/LogTools.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>
#include <chaos/IrrklangTools.h>
#include <chaos/MathTools.h>

static int const BLEND_NONE = 0;
static int const BLEND_IN = 1;
static int const BLEND_OUT = 2;

// ================================================================================

class SoundBaseObject : public chaos::ReferencedObject
{
protected:

  /** protected constructor */
  SoundBaseObject(class SoundManager * in_sound_manager);

public:

  /** get the name of the object */
  char const * GetName() const { return name.c_str(); }

protected:

  /* the name */
  std::string name;
  /** the manager */
  class SoundManager * sound_manager = nullptr;
};


// ================================================================================

class SoundVolumeObject : public SoundBaseObject
{
  friend class SoundManager;

protected:

  /** protected constructor */
  SoundVolumeObject(class SoundManager * in_sound_manager);

public:



  /** get the own object volume */
  float GetVolume() const;
  /** change the volume of the object */
  void SetVolume(float in_volume);

  /** get the final volume for the object (category and blendings taken into account) */
  virtual float GetEffectiveVolume() const;
  /** tick the object */
  virtual void Tick(float delta_time);
  /** whether the object is finished */
  virtual bool IsFinished() const;


  /** start a blend effect to 1.0f */
  void StartBlendIn(float blend_time, bool fullrange_blend_time = true);
  /** start a blend effect to 0.0f */
  void StartBlendOut(float blend_time, bool fullrange_blend_time = true);
  /** stop current blend effects */
  void StopBlending();

  /** require the stop (destroy the object) with or without a blend time */
  void StopAndKill(float in_blendout_time);

protected:

  void UpdateBlendFactor(float delta_time);

protected:

  /* the name */
  std::string name;

  /** the volume */
  float volume = 1.0f;
  /** the blend factor (whether the object is blending) */
  int   blend_type = BLEND_NONE;
  float blend_factor = 1.0f;
  float blend_time = 0.0f;
  /** whether the object is to be killed */
  bool should_kill = false;

};

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

public:




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

public:

  /** tick the sounds */
  void Tick(float delta_time);

  /** returns true if the sound is finished */
  bool IsFinished() const;
  /** returns true whether the sound should be removed from the list at the end */
  bool ShouldKillAtEnd() const;
  /** get the final volume for the sound (category and blendings taken into account) */
  virtual float GetEffectiveVolume() const override;

  /** stop the sound */
  void Stop(double in_blendout_factor);
  /** pause the sound */


  bool IsLooping() const;

  /** change the position */
  void SetPosition(glm::vec3 const & in_position);
  /** get the position */
  glm::vec3 GetPosition() const;
  /** change the speed */
  void SetSpeed(glm::vec3 const & in_speed);
  /** get the speed */
  glm::vec3 GetSpeed() const;



protected:

  /** the category */
  SoundCategory * category = nullptr;

  /** position */
  bool is_3D_sound   = false;
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 speed    = glm::vec3(0.0f, 0.0f, 0.0f);

  /** the looping information */
  float loop_start = -1.0f;
  float loop_end = -1.0f;
  float loop_blend_time = 0.5f;
  /** the sound */
  boost::intrusive_ptr<irrklang::ISound> irrklang_sound;
  boost::intrusive_ptr<irrklang::ISound> irrklang_loop_sound;
  /** the source */
  SoundSource * source = nullptr;
};

// ================================================================================

class PlaySoundDesc
{
public:

  /** the name of the category */
  std::string category_name;
  /** or a pointer on the category */
  SoundCategory * category = nullptr;

  
  
  /** whether we want to loop */
  bool looping = false;

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

public:

  /** play a sound */
  Sound * PlaySound(PlaySoundDesc const & desc);

protected:

  /** the irrklang source */
  boost::intrusive_ptr<irrklang::ISoundSource> irrklang_source;
  /** the loop information */
  float loop_start      = -1.0f;
  float loop_end        = -1.0f;
  float loop_blend_time =  0.5f;
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

  void Tick(float delta_time);


  /** find a sound by its name */
  Sound * FindSound(char const * name);
  /** find a category by its name */
  SoundCategory * FindSoundCategory(char const * name);
  /** find a source by its name */
  SoundSource * FindSoundSource(char const * name);

  /** add a source */
  SoundSource * AddSource(char const * in_name, char const * in_filename, float in_loop_start = -1.0f, float in_loop_end = -1.0f, float in_loop_blend_time = 0.5f);





  void StopCategory(char const * selector, double blend_time);


  void StopSound(char const * selector, double fadeout_time = 0.0);


  /** add some source */



protected:

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




// ================================================================================

SoundBaseObject::SoundBaseObject(class SoundManager * in_sound_manager) :
  sound_manager(in_sound_manager)
{
  assert(sound_manager != nullptr);
}

// ================================================================================


SoundVolumeObject::SoundVolumeObject(class SoundManager * in_sound_manager) :
  SoundBaseObject(in_sound_manager)
{
  assert(sound_manager != nullptr);
}

float SoundVolumeObject::GetVolume() const
{
  return volume;
}

void SoundVolumeObject::SetVolume(float in_volume)
{
  volume = in_volume;
}

float SoundVolumeObject::GetEffectiveVolume() const
{
  return volume * blend_factor;
}

void SoundVolumeObject::UpdateBlendFactor(float delta_time)
{
  if (blend_type == BLEND_IN)
  {
    if (blend_time == 0.0f) // immediate blending
      blend_factor = 1.0f;
    else
      blend_factor = chaos::MathTools::Clamp(blend_factor + delta_time / blend_time, 0.0f, 1.0f);
  }
  else if (blend_type == BLEND_OUT)
  {
    if (blend_time == 0.0f) // immediate blending
      blend_factor = 0.0f;
    else
      blend_factor = chaos::MathTools::Clamp(blend_factor - delta_time / blend_time, 0.0f, 1.0f);
  }
}

bool SoundVolumeObject::IsFinished() const
{
  if (blend_type == BLEND_OUT && blend_factor <= 0.0f) // blending out is finished
    return true;
  return false;
}

void SoundVolumeObject::Tick(float delta_time)
{
  UpdateBlendFactor(delta_time);
}

// fullrange_blend_time == true
// ----------------------------
//    blend_time = time to go from 0 to 1
//
//
// fullrange_blend_time == false
// -----------------------------
//    blend_time = time to go from current_value to 1

void SoundVolumeObject::StartBlendIn(float in_blend_time, bool fullrange_blend_time)
{
  blend_type = BLEND_IN;
  if (fullrange_blend_time)
    blend_time = in_blend_time;
  else
  {
    if (blend_factor == 1.0f)
      blend_time = 1.0f; // well, a non null value for division. It is useless because the wanted final value is already reached
    else
      blend_time = in_blend_time / (1.0f - blend_factor);
  }
}

void SoundVolumeObject::StartBlendOut(float in_blend_time, bool fullrange_blend_time) // blend_time = 0 for immediate change at next tick
{
  blend_type = BLEND_OUT;
  if (fullrange_blend_time)
    blend_time = in_blend_time;
  else
  {
    if (blend_factor == 0.0f)
      blend_time = 1.0f; // well, a non null value for division. It is useless because the wanted final value is already reached
    else
      blend_time = in_blend_time / (blend_factor - 0.0f);
  }
}

void SoundVolumeObject::StopBlending()
{
  blend_type = BLEND_NONE;
}




void SoundVolumeObject::StopAndKill(float in_blendout_time) // the object will only be destroyed after the tick
{

#if 0
  blendout_time = in_blendout_time;
  if (blendout_time <= 0.0f) // if this is the first call
  {
    blendout_factor = 1.0f;
    should_kill = true;
  }
#endif
}

// ================================================================================

SoundSource::SoundSource(class SoundManager * in_sound_manager) :
  SoundBaseObject(in_sound_manager)
{
  assert(sound_manager != nullptr);
}


SoundSource::~SoundSource()
{


}

Sound * SoundSource::PlaySound(PlaySoundDesc const & desc)
{
  Sound * result = nullptr;

  // get the engine
  irrklang::ISoundEngine * irrklang_engine = sound_manager->irrklang_engine.get();
  if (irrklang_engine == nullptr)
    return nullptr;

  // search the category
  SoundCategory * category = desc.category;
  if (category == nullptr)
    category = sound_manager->FindSoundCategory(desc.category_name.c_str());






  irrklang_engine->play3D(irrklang_source, position, looping, paused, track, sound_effect);
  

 // irrklang::ISound * sound = engine->play2D(sound_source1.get(), false /* looped */, false /*  start paused */, true /* track */, true /* enable FX */);


  return result;
}


// ================================================================================

SoundCategory::SoundCategory(class SoundManager * in_sound_manager) :
  SoundVolumeObject(in_sound_manager)
{
  assert(sound_manager != nullptr);
}

SoundCategory::~SoundCategory()
{


}


// ================================================================================

Sound::Sound(class SoundManager * in_sound_manager) :
  SoundVolumeObject(in_sound_manager)
{
  assert(sound_manager != nullptr);
}


Sound::~Sound()
{


}


void Sound::SetPosition(glm::vec3 const & in_position)
{
  if (!is_3D_sound)
    return;
  position = in_position;
}

glm::vec3 Sound::GetPosition() const
{
  if (!is_3D_sound)
    return glm::vec3(0.0f, 0.0f, 0.0f);
  return position;
}

void Sound::SetSpeed(glm::vec3 const & in_speed)
{
  if (!is_3D_sound)
    return;
  speed = in_speed;
}

glm::vec3 Sound::GetSpeed() const
{
  if (!is_3D_sound)
    return glm::vec3(0.0f, 0.0f, 0.0f);
  return speed;
}

float Sound::GetEffectiveVolume() const
{
  float result = SoundVolumeObject::GetEffectiveVolume();
  if (category != nullptr)
    result *= category->GetEffectiveVolume();
  return result;
}

bool Sound::IsLooping() const
{
  // irrklang native looping
  if (irrklang_sound->isLooped())
    return true;
  // our manual looping
  if (loop_start >= 0.0f && loop_start < loop_end)
  {


  }
  return false;
}

bool Sound::IsFinished() const
{
  if (SoundVolumeObject::IsFinished())
    return true;
  if (irrklang_sound->isFinished() && !IsLooping())
    return true;
  return false;
}

bool Sound::ShouldKillAtEnd() const
{
  return false;
}

void Sound::Tick(float delta_time)
{
  SoundVolumeObject::Tick(delta_time);
  if (IsFinished())
    return;

  float current_time = 1000.0f * (float)irrklang_sound->getPlayPosition();

}


// ================================================================================



// ================================================================================

void SoundManager::Tick(float delta_time)
{

  // tick all sounds
  for (int i = sounds.size() - 1; i >= 0; --i)
  {
    Sound * sound = sounds[i].get();
    if (sound == nullptr)
      continue;
    sound->Tick(delta_time);
    if (sound->IsFinished() && sound->ShouldKillAtEnd())
    {
      if (i != sounds.size() - 1)
        sounds[i] = sounds[sounds.size() - 1]; // destroy sound that is to be destroyed
      sounds.pop_back();
    }
  }

  // tick all categories
  for (int i = categories.size() - 1; i >= 0; --i)
  {
    SoundCategory * category = categories[i].get();
    if (category == nullptr)
      continue;
    category->Tick(delta_time);
    if (category->IsFinished() && category->ShouldKillAtEnd())
    {
      if (i != categories.size() - 1)
        categories[i] = categories[categories.size() - 1]; // destroy sound that is to be destroyed
      categories.pop_back();
    }
  }




}

Sound * SoundManager::FindSound(char const * name)
{
  return FindSoundObject<Sound>(name, sounds);
}

SoundCategory * SoundManager::FindSoundCategory(char const * name)
{
  return FindSoundObject<SoundCategory>(name, categories);
}

SoundSource * SoundManager::FindSoundSource(char const * name)
{
  return FindSoundObject<SoundSource>(name, sources);
}

SoundSource * SoundManager::AddSource(char const * in_name, char const * in_filename, float in_loop_start, float in_loop_end, float in_loop_blend_time)
{
  assert(in_filename != nullptr);
  assert(irrklang_engine.get() != nullptr);

  if (in_name == nullptr)
    in_name = in_filename;

  if (FindSoundSource(in_name) != nullptr) // name already existing
    return nullptr;

  chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(in_filename, false); // load the file
  if (buffer == nullptr)
    return nullptr;

  // create the source on irrklang side
  // XXX : we give filename even if the file is already loaded because it helps irrklangs to find the data format
  irrklang::ISoundSource * irrklang_source = irrklang_engine->addSoundSourceFromMemory(buffer.data, buffer.bufsize, in_filename, true);
  if (irrklang_source == nullptr)
    return nullptr;

  SoundSource * result = new SoundSource(this);
  if (result == nullptr)
    return nullptr;

  result->name = in_name;
  result->irrklang_source = irrklang_source;
  result->loop_start = in_loop_start;
  result->loop_end = in_loop_end;
  result->loop_blend_time = in_loop_blend_time;

  irrklang_source->drop(); // now that we have a reference on the resource, we can unref it

  sources.push_back(result);

  return result;
}

bool SoundManager::StartManager()
{
  StopManager(); // destroy previous references

                 // get the list of all devices
  irrklang_devices = irrklang::createSoundDeviceList();
  if (irrklang_devices == nullptr)
    return false;
  irrklang_devices->drop();

  // create the engine
  irrklang_engine = irrklang::createIrrKlangDevice();
  if (irrklang_engine == nullptr)
    return false;
  irrklang_engine->drop(); // suppress the extra reference

  return true;
}

bool SoundManager::StopManager()
{
  irrklang_devices = nullptr;
  irrklang_engine = nullptr;

  return true;
}















class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

  virtual bool OnDraw(int width, int height) override
  {
    glClearColor(0.0f, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return true;
  }

  void DropAllSounds()
  {
    for (auto sound : playing_sounds)
      if (sound != nullptr)
        sound->stop();

    playing_sounds.clear();
  }

  virtual void Finalize() override
  {
    DropAllSounds();
    engine = nullptr;
    sound_source1 = nullptr;
    sound_source2 = nullptr;
  }

  virtual bool Tick(double delta_time) override
  {
    for (size_t i = 0; i < playing_sounds.size(); ++i)
    {
      if (playing_sounds[i] == nullptr || playing_sounds[i]->isFinished())
      {
        if (i != playing_sounds.size() - 1)
          playing_sounds[i] = playing_sounds[playing_sounds.size() - 1];
        playing_sounds.pop_back();
        i--;
      }
    }
    return false; // no redraw
  }

  virtual void OnMouseButton(int button, int action, int modifier) override
  {
    if (button == 0 && action == GLFW_PRESS)
    {
      irrklang::ISound * sound = engine->play2D(sound_source1.get(), false /* looped */, false /*  start paused */, true /* track */, true /* enable FX */);
      playing_sounds.push_back(sound);
    }
    else if (button == 1 && action == GLFW_PRESS)
    {
      irrklang::ISound * sound = engine->play2D(sound_source2.get(), false /* looped */, false /*  start paused */, true /* track */, true /* enable FX */);
      playing_sounds.push_back(sound);
    }
    else if (button == 2 && action == GLFW_PRESS)
      DropAllSounds();
  }

  virtual bool Initialize() override
  {
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    // list of sound devices
    boost::intrusive_ptr<irrklang::ISoundDeviceList> deviceList = irrklang::createSoundDeviceList();
    if (deviceList != nullptr)
    {
      for (int i = 0; i < deviceList->getDeviceCount(); ++i)
        printf("DEVICE %d: [%s]  [%s]\n", i, deviceList->getDeviceDescription(i), deviceList->getDeviceID(i));
      deviceList = nullptr;
    }

    // create the engine
    engine = irrklang::createIrrKlangDevice();
    if (engine == nullptr)
      return false;
    engine->drop(); // XXX : because we add our own reference to an object already having one

                    // create the sound
    boost::filesystem::path resources_path = application->GetApplicationPath() / "resources";
    boost::filesystem::path src1_path = resources_path / "70_Stir_HiHatOpen.wav";
    boost::filesystem::path src2_path = resources_path / "70_Stir_Kick.wav";

    sound_source1 = engine->addSoundSourceFromFile(src1_path.string().c_str(), irrklang::ESM_NO_STREAMING, true);
    if (sound_source1 == nullptr)
      return false;

    sound_source2 = engine->addSoundSourceFromFile(src2_path.string().c_str(), irrklang::ESM_NO_STREAMING, true);
    if (sound_source2 == nullptr)
      return false;

    return true;
  }

  virtual void TweakSingleWindowApplicationHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFW::Window::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);
    hints.toplevel = 0;
    hints.decorated = 1;
  }

protected:

  boost::intrusive_ptr<irrklang::ISoundEngine> engine;
  boost::intrusive_ptr<irrklang::ISoundSource> sound_source1;
  boost::intrusive_ptr<irrklang::ISoundSource> sound_source2;

  std::vector<boost::intrusive_ptr<irrklang::ISound>> playing_sounds;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 500;
  params.height = 500;
  params.monitor_index = 0;
  chaos::MyGLFW::Window::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

  return 0;
}


