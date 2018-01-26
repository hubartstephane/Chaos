#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/Buffer.h> 
#include <chaos/LogTools.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>
#include <chaos/IrrklangTools.h>
#include <chaos/MathTools.h>
#include <chaos/SoundManager.h>


class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

  virtual bool OnDraw(glm::ivec2 size) override
  {
    glClearColor(0.0f, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return true;
  }

  virtual void Finalize() override
  {   
    sound1    = nullptr;
    category1 = nullptr;
    source1   = nullptr;

    if (sound_manager != nullptr)
    {
      sound_manager->StopManager();
      sound_manager = nullptr;
    }
  }

  virtual bool Tick(double delta_time) override
  {
    sound_manager->Tick((float)delta_time);

    return false; // no redraw
  }

  virtual void OnMouseButton(int button, int action, int modifier) override
  {
    if (button == 0 && action == GLFW_PRESS)
    {
      chaos::PlaySoundDesc desc;
      desc.category = category1.get();
      desc.looping = true;
      sound1 = source1->PlaySound(desc);

    }
    else if (button == 1 && action == GLFW_PRESS)
    {
      if (sound1 != nullptr)
        sound1->StopAndKill(2.0f, true);
    }
    else if (button == 2 && action == GLFW_PRESS)
    {
      if (category1 != nullptr)
      {
        if ((modifier & GLFW_MOD_SHIFT) != 0)
        {
          category1->CloneCategoryAndStop(3.0f, true);

          chaos::PlaySoundDesc desc;
          desc.category = category1.get();
          desc.looping = true;
          sound1 = source1->PlaySound(desc);
        }
        else
          category1->StopAndKill(2.0f, true);

      }
    }
  }

  virtual bool Initialize(nlohmann::json const & configuration) override
  {
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    // create the sound manager
    sound_manager = new chaos::SoundManager;
    if (sound_manager == nullptr)
      return false;

    sound_manager->StartManager();

    // create the sound
    boost::filesystem::path resources_path = application->GetApplicationPath() / "resources";
    boost::filesystem::path src1_path = resources_path / "The Pretender.ogg";

    chaos::SoundLoopInfo loop_info;
    loop_info.start = 3.0f;
    loop_info.end   = 6.0f;
    loop_info.blend_time = 1.0f;

    source1 = sound_manager->AddSource(src1_path.string().c_str(), nullptr, loop_info);

    category1 = sound_manager->AddCategory();

    return true;
  }

  virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);
    hints.toplevel = 0;
    hints.decorated = 1;
  }

protected:

  boost::intrusive_ptr<chaos::SoundManager> sound_manager;

  boost::intrusive_ptr<chaos::SoundSource> source1;

  boost::intrusive_ptr<chaos::Sound> sound1;

  boost::intrusive_ptr<chaos::SoundCategory> category1;

  
};

// ================================================================

namespace chaos
{
  namespace SoundWork
  {

    class PlaySoundDesc
    {
    public:

      /** whether the sound must start paused */
      bool start_paused = false;
      /** whether the sound must loop */
      bool looping = false;

    public:

      bool IsSound3D() const { return false; }

      
    };



    class SoundFinishedCallback : public chaos::ReferencedObject
    {
    public:

      virtual void OnFinished(SoundBase * sound);
    };

    class SoundStopEventReceiver : public irrklang::ISoundStopEventReceiver
    {
    public:

      virtual void OnSoundStopped(ISound * sound, E_STOP_EVENT_CAUSE reason, void * userData) override
      {

      }


    };


    class SoundBase
    {
    public:

      virtual void Pause();
      virtual void Resume();

    protected:


      virtual bool StartSound(PlaySoundDesc const & desc);
      
      void SetCallbacks(SoundFinishedCallback * in_callbacks)
      {
        callbacks = in_callbacks;
      }

    protected:

      boost::intrusive_ptr<SoundFinishedCallback> callbacks;
    };

    class Sound : public SoundBase
    {
    public:

      Sound(SoundSource * in_source) : 
        source(in_source)
      {
        assert(in_source != nullptr);
      }

      virtual bool StartSound(PlaySoundDesc const & desc) override
      {
        irrklang::ISound * result = nullptr;

        bool looping = in_looping;
        bool paused = in_paused;
        bool track = true;
        bool sound_effect = true;

        if (desc.IsSound3D())
        {
          irrklang_sound = sound_manager->irrklang_engine->play2D(
            irrklang_source.get(),
            looping,
            paused,
            track,
            sound_effect);

        }
        else
        {
          irrklang_sound = sound_manager->irrklang_engine->play3D(
            irrklang_source.get(),
            SoundManager::ToIrrklangVector(desc.position),
            looping,
            paused,
            track,
            sound_effect);
        }

        irrklang::ISoundStopEventReceiver receiver;
        receiver.OnSoundStopped()

        irrklang_sound->setSoundStopEventReceiver()

      }

    protected:

      /** reference to the source */
      SoundSource * source = nullptr;
      /** reference to the irrklang object */
      boost::intrusive_ptr<irrklang::ISound> irrklang_sound;

      /** whether the sound is in 3D */
      bool is_3D_sound = false;
      /** the sound position */
      glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
      /** the sound velocity */
      glm::vec3 speed = glm::vec3(0.0f, 0.0f, 0.0f);
    };




    class SoundSourceBase
    {
    protected:

      SoundBase * GenerateSound()
      {
        return nullptr;
      }

    public:

      virtual SoundBase * Play(PlaySoundDesc const & desc, SoundFinishedCallback * in_callbacks)
      {
        // generate the sound object
        SoundBase * result = GenerateSound();
        if (result == nullptr)
          return nullptr;
        // assign terminaison callbacks
        if (in_callbacks != nullptr)
          result->SetCallbacks(in_callbacks);
        // start to play
        result->StartSound(desc);
        
        return result;
      }
    };


    class SoundRandomSource : public SoundSourceBase
    {
      friend class SoundRandom;

    public:

      SoundBase * GenerateSound()
      {
        return new SoundRandom(this);
      }

    protected:

      std::vector<boost::intrusive_ptr<SoundSourceBase>> child_sources;

    };

    class SoundRandom : public SoundBase
    {
      friend class SoundRandomSource;

    public:

      SoundRandom(SoundRandomSource * in_source) : 
        source(in_source) 
      {
        assert(in_source != nullptr);
      }


      bool Play(Desc & desc)
      {
        // no children, immediatly ends
        if (source->child_sources.size() == 0) 
          return true;

        // find a

        size_t r = (size_t)(rand() % (int)source->child_sources.size());
          
        int i = chaos::MathTools::Random()


      }

    protected:

      SoundRandomSource * source = nullptr;

    };







    // any object in a sound manager that have a name
    class SoundObjectBase
    {


    protected:

      std::string name;

      SoundManager * manager = nullptr;
    };

    // any object that have a volume
    class SoundVolumeObject : public SoundObjectBase
    {


    };

    // this is just a container for sound
    class SoundCategory : public SoundVolumeObject
    {


    };

    // 
    class SoundSourceBase : public SoundVolumeObject
    {
    public:

      virtual SoundBase * PlaySound();


    };

    class RandomSoundSource : public SoundSourceBase
    {
    public:




    };

    class SequenceSoundSource : public SoundSourceBase
    {



    };

    //





















  };

};













// ================================================================



int _tmain(int argc, char ** argv, char ** env)
{
  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 500;
  params.height = 500;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

  return 0;
}


