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

  }

  virtual bool Tick(double delta_time) override
  {

    return false; // no redraw
  }

  virtual void OnMouseButton(int button, int action, int modifier) override
  {




  }

  void Test1()
  {
    int i = 0;

    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return;

    boost::filesystem::path configuration_path = application->GetResourcesPath() / "config.json";

    nlohmann::json c1 = chaos::JSONTools::LoadJSONFileRecursive(configuration_path);
    nlohmann::json c2 = chaos::JSONTools::LoadJSONFileRecursive(configuration_path);

    for (nlohmann::json::const_iterator it = c1.begin(); it != c1.end(); ++it)
    {
      std::string c = it.key();
      if (it->is_boolean())
      {
        bool b = it->get<bool>();
        b = b;
      }
      if (it->is_number_integer())
      {
        int i = it->get<int>();
        i = i;
      }
      if (it->is_number_float())
      {
        float f = it->get<float>();
        f = f;
      }

      if (it->is_string())
      {
        std::string s = it->get<std::string>();
        s = s;
      }
      ++i;
    }

    c1["MONCHOIX"] = c2;

    boost::filesystem::path result_dir;
    if (chaos::FileTools::CreateTemporaryDirectory("MON_REP_%d", result_dir))
    {
      std::ofstream stream((result_dir / "mon_fichier.json").string().c_str());
      stream << c1.dump(4);

      chaos::WinTools::ShowFile(result_dir);
    }

    application = application;
  }






  virtual bool Initialize(nlohmann::json const & configuration) override
  {
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    Test1();

    return true;
  }

  virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);
    hints.toplevel = 0;
    hints.decorated = 1;
  }

protected:


};

// ===============================================








// ===============================================


int _tmain(int argc, char ** argv, char ** env)
{











  boost::filesystem::path ref = "mareference/bidule/fichier.txt";

  chaos::FilePathParam P1("truc1.xxx.yyy");
  chaos::FilePathParam P2(std::string("truc2.yyy"));
  chaos::FilePathParam P3(boost::filesystem::path("truc3.zzz"));

  chaos::FilePathParam P4("truc1.xxx.yyy", ref);
  chaos::FilePathParam P5(std::string("truc2.yyy"), ref);
  chaos::FilePathParam P6(boost::filesystem::path("truc3.zzz"), ref);




  auto b1 = chaos::FileTools::IsTypedFile(P1, ".xxx");
  auto b2 = chaos::FileTools::IsTypedFile(P1, ".yyy");

  auto R1 = P1.GetResolvedPath();
  auto R2 = P2.GetResolvedPath();
  auto R3 = P3.GetResolvedPath();
  auto R4 = P4.GetResolvedPath();
  auto R5 = P5.GetResolvedPath();
  auto R6 = P6.GetResolvedPath();


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


