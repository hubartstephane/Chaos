#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/Buffer.h> 
#include <chaos/LogTools.h> 
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


