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

#include "SoundTest.h"

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
    source2   = nullptr;
    source3   = nullptr;

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
      desc.looping = false;
      desc.paused = false;
      desc.category = category1.get();
      desc.sound_name = "toto";

	  chaos::SoundAutoCallbacks * cb = new chaos::SoundAutoCallbacks;
      cb->finished_func = [](chaos::SoundObject * ob)
      {
        ob = ob;
      };
      cb->removed_func = [](chaos::SoundObject * ob)
      {
        ob = ob;
      };

      sound1 = source1->PlaySound(desc, cb);
    }

    if (button == 1 && action == GLFW_PRESS)
    {
      chaos::Sound * s = sound_manager->FindSound("toto");
      if (s != nullptr)
      {
        chaos::BlendVolumeDesc desc;
        desc.blend_type = chaos::BlendVolumeDesc::BLEND_OUT;
        desc.kill_at_end = true;
        desc.blend_time = 2.0f;
        s->StartBlend(desc);
      }
        //s->Stop();

    //  if (category1 != nullptr)
    //    category1->SetVolume(category1->GetVolume() * 0.5f);

      //if (source1 != nullptr)
      //  source1->SetVolume(source1->GetVolume() * 0.5f);

      //if (category1 != nullptr)
      //  category1->Pause(!category1->IsPaused());
      //if (source1 != nullptr)
      //  source1->Pause(!source1->IsPaused());

     // sound_manager->StopManager();

      if (source1 != nullptr)
      {
       // source1->Stop();

        auto x = category1.get();
        x = x;
      }


      if (sound1 != nullptr)
      {
        //sound1->Pause(!sound1->IsPaused());
       // sound1->Stop();
       // sound1 = nullptr;
      }
    }

#if 0
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
#endif
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
    boost::filesystem::path src1_path = resources_path / "70_Stir_RideBell.wav";
    boost::filesystem::path src2_path = resources_path / "70_Stir_SnareOff3.wav";

    source1 = sound_manager->AddSource(src1_path, nullptr);
    source2 = sound_manager->AddSource(src2_path, nullptr);
    source3 = sound_manager->AddSource(src2_path, nullptr);

    category1 = sound_manager->AddCategory(nullptr);

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

  boost::intrusive_ptr<chaos::SoundSource> source2;

  boost::intrusive_ptr<chaos::SoundSource> source3;

  boost::intrusive_ptr<chaos::Sound> sound1;

  boost::intrusive_ptr<chaos::SoundCategory> category1;
};

int _tmain(int argc, char ** argv, char ** env)
{
  ParticleLayer<ParticleLayerDescExample> sl;

  std::vector<int> iii = { 10, 20, 30, 40, 50, 60 };

  //auto it = std::lower_bound(iii.begin(), iii.end(), 35);

  auto it = std::lower_bound(iii.begin(), iii.end(), 35, [](int k, int u) {return (k < u); });

  int h = *it;

  iii.insert(it, 35);


 // auto it = std::remove_if(iii.begin(), iii.end(), [](int k) {return (k % 2) == 1; });

//  iii.erase(it, iii.end());

  //auto it2 = std::binary_search((iii.begin(), iii.end(), [](int k) {return (k == 3); });

  //int u = *it2;

  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 500;
  params.height = 500;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

  return 0;
}


