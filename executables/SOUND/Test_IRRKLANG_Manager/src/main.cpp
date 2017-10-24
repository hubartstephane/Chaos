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

  virtual bool OnDraw(int width, int height) override
  {
    glClearColor(0.0f, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return true;
  }

  void DropAllSounds()
  {

  }

  virtual void Finalize() override
  {
    if (sound_manager != nullptr)
    {
      sound_manager->StopManager();
      sound_manager = nullptr;
    }

    source1 = nullptr;
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

    }
    else if (button == 1 && action == GLFW_PRESS)
    {

    }

  }

  virtual bool Initialize() override
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

    source1 = sound_manager->AddSource(src1_path.string().c_str(), nullptr);
#if 0


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
#endif
    return true;
  }

  virtual void TweakSingleWindowApplicationHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFW::Window::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);
    hints.toplevel = 0;
    hints.decorated = 1;
  }

protected:

  boost::intrusive_ptr<chaos::SoundManager> sound_manager;

  boost::intrusive_ptr<chaos::SoundSource> source1;
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


