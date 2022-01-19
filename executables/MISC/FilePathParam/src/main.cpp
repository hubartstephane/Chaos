#include <chaos/Chaos.h>

class WindowOpenGLTest : public chaos::Window
{
    CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

  virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size, chaos::GPUProgramProviderBase const* uniform_provider) override
  {
    glClearColor(0.0f, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return true;
  }


  virtual bool Tick(float delta_time) override
  {

    return false; // no redraw
  }

  virtual bool OnMouseButtonImpl(int button, int action, int modifier) override
  {
		return false;
  }

  virtual bool InitializeFromConfiguration(nlohmann::json const & config) override
  {
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;


    return true;
  }

  virtual void TweakHints(chaos::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::Window::TweakHints(hints, monitor, pseudo_fullscreen);
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

    chaos::WindowParams params;
    params.monitor = nullptr;
    params.width = 500;
    params.height = 500;
    params.monitor_index = 0;

    chaos::WindowHints hints;

    return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, params, hints);
}


