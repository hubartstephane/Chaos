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
#include <chaos/JSONTools.h>
#include <chaos/FileTools.h>
#include <chaos/GLTextureTools.h>


#include "ParticleManagerRefactor.h"

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
    texture1 = nullptr;
    texture2 = nullptr;
    texture3 = nullptr;
    texture4 = nullptr;
    texture5 = nullptr;
    texture6 = nullptr;
  }

  virtual bool Tick(double delta_time) override
  {

    return false; // no redraw
  }

  virtual void OnMouseButton(int button, int action, int modifier) override
  {




  }

  virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
  {
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    chaos::JSONTools::ShowConfigFile(config);

    boost::filesystem::path const & resource_path = application->GetResourcesPath();

    texture1 = chaos::GLTextureTools::GenTextureObject(resource_path / "textures" / "texture1.json");
    texture2 = chaos::GLTextureTools::GenTextureObject(resource_path / "textures" / "texture2.json");
    texture3 = chaos::GLTextureTools::GenTextureObject(resource_path / "textures" / "texture3.json");
    texture4 = chaos::GLTextureTools::GenTextureObject(resource_path / "textures" / "texture4.json");
    texture5 = chaos::GLTextureTools::GenTextureObject(resource_path / "textures" / "texture5.json");
    texture6 = chaos::GLTextureTools::GenTextureObject(resource_path / "textures" / "space.png");

    return true;
  }

  virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);
    hints.toplevel = 0;
    hints.decorated = 1;
  }

protected:

  boost::intrusive_ptr<chaos::Texture> texture1;
  boost::intrusive_ptr<chaos::Texture> texture2;
  boost::intrusive_ptr<chaos::Texture> texture3;
  boost::intrusive_ptr<chaos::Texture> texture4;
  boost::intrusive_ptr<chaos::Texture> texture5;
  boost::intrusive_ptr<chaos::Texture> texture6;


};

// ===============================================








// ===============================================


int _tmain(int argc, char ** argv, char ** env)
{






  ParticleLayer sl(new ParticleLayerDescExample());

  ParticleRange r = sl.SpawnParticles(17);

  boost::intrusive_ptr<ParticleRangeAllocation> p1 = sl.SpawnParticlesAndKeepRange(5);
  boost::intrusive_ptr<ParticleRangeAllocation> p2 = sl.SpawnParticlesAndKeepRange(10);
  boost::intrusive_ptr<ParticleRangeAllocation> p3 = sl.SpawnParticlesAndKeepRange(20);


  p2 = nullptr;
  sl.TickParticles(0.0f);

  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 500;
  params.height = 500;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

  return 0;
}


