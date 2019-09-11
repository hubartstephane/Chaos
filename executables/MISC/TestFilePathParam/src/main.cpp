#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/Buffer.h> 
#include <chaos/LogTools.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>
#include <chaos/MathTools.h>
#include <chaos/JSONTools.h>
#include <chaos/FileTools.h>
#include <chaos/FilePath.h>



class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

  virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size) override
  {
    glClearColor(0.0f, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return true;
  }


  virtual bool Tick(double delta_time) override
  {

    return false; // no redraw
  }

  virtual bool OnMouseButton(int button, int action, int modifier) override
  {
		return false;
  }

  virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
  {
		if (!chaos::MyGLFW::Window::InitializeFromConfiguration(config, config_path))
			return false;

    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;


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


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{

  boost::filesystem::path ref = "mareference/bidule/../fichier.txt";

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

  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 500;
  params.height = 500;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

  return 0;
}


