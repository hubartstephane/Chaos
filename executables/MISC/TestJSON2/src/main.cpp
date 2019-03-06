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


class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

  virtual bool OnDraw(chaos::Renderer * renderer, glm::ivec2 size) override
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

  virtual bool OnMouseButton(int button, int action, int modifier) override
  {
		return false; 
  }

  void Test1()
  {
    int i = 0;

    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return;

    boost::filesystem::path configuration_path = application->GetResourcesPath() / "test1.json";

    nlohmann::json c1;
    chaos::JSONTools::LoadJSONFile(configuration_path, c1, true);

	nlohmann::json const * ar = chaos::JSONTools::GetStructure(c1, "value_array");

	if (ar != nullptr)
	{
		int a,b;
		chaos::JSONTools::GetAttributeByIndex(*ar, 0, a);
		chaos::JSONTools::GetAttributeByIndex(*ar, 1, b);

		nlohmann::json const * ar2 = chaos::JSONTools::GetStructureByIndex(*ar, 3);
	
		bool c;
		chaos::JSONTools::GetAttributeByIndex(*ar, 4, c);

		ar2 = ar2;
	}



	ar =ar;



#if 0
    nlohmann::json c2;
    chaos::JSONTools::LoadJSONFile(configuration_path, c2, false);

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

    c1["INCORPORATION_C2"] = c2; // deep copy ?
	
	c2["APRES_INSERTION_DANS_C1"] = 123456; // won't be visible in the generated file => means previous line is a deep copy
#endif

    boost::filesystem::path dir_path = chaos::JSONTools::DumpConfigFile(c1);
		chaos::WinTools::ShowFile(dir_path);
  }


  virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
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


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 500;
  params.height = 500;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

  return 0;
}


