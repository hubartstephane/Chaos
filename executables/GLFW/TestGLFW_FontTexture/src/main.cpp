#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GPUProgramGenerator.h>
#include <chaos/Application.h>
#include <chaos/GPUSimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/BitmapFontTextMeshBuilder.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/StringTools.h>






class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

  virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size) override
  {
    glm::vec4 clear_color(1.0f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		debug_display.Display((int)(2.0f * viewport.half_size.x), (int)(2.0f * viewport.half_size.y));
    return true;
  }

  virtual bool OnMouseButton(int button, int action, int modifier) override
  {
    if (action == GLFW_PRESS)
    {
      if (button == 1)
        debug_display.Clear();  
      else
        debug_display.AddLine(chaos::StringTools::Printf("LINE FROM CLICKED [%d]", click_count++).c_str(), 5.0f); 
      RequireWindowRefresh();    
			return true;
    }
		return false;
  }

  virtual void Finalize() override
  {
    debug_display.Finalize();
		chaos::MyGLFW::Window::Finalize();
  }

  virtual bool Tick(double delta_time) override
  {
    return debug_display.Tick(delta_time);
  }

  virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
  {   
		if (!chaos::MyGLFW::Window::InitializeFromConfiguration(config, config_path))
			return false;

    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    boost::filesystem::path resources_path = application->GetResourcesPath();
    boost::filesystem::path image_path     = resources_path / "font.png";

    chaos::GLDebugOnScreenDisplay::Params debug_params;
    debug_params.texture_path               = image_path;
    debug_params.font_characters            = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    debug_params.font_characters_per_line   = 10;
    debug_params.font_characters_line_count = 10;
    debug_params.character_width            = 20;
    debug_params.spacing                    = glm::ivec2( 0, 0);
    debug_params.crop_texture               = glm::ivec2(15, 7);

    if (!debug_display.Initialize(debug_params))
      return false;
   
    debug_display.AddLine("line1 gdjkhfghhbb xxxxx", 2.0f);
    debug_display.AddLine("line2 yyyyyyyy xxxxx");
    debug_display.AddLine("line3 yyyyyyyy xxxxx", 5.0f);
    debug_display.AddLine("line4 yyyyyyyy xxxxx");
    debug_display.AddLine("line5 yyyyyyyy xxxxx", 0.0f);
    debug_display.AddLine("line6 yyyyyyyy xxxxx");

    debug_display.AddLine(debug_display.GetFontCharacters().c_str());
      
    return true;
  }

  virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);
    hints.toplevel  = 0;
    hints.decorated = 1;
  }

protected:

  chaos::GLDebugOnScreenDisplay debug_display;

  int click_count = 0;
};

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


