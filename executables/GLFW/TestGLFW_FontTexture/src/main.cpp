#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GLProgramLoader.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/SkyBoxTools.h>
#include <chaos/BitmapFontTextMeshBuilder.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/StringTools.h>






class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
public:

  MyGLFWWindowOpenGLTest1() : click_count(0){}

protected:

  virtual bool OnDraw(int width, int height) override
  {
    glm::vec4 clear_color(1.0f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, width, height);
    
    
    debug_display.Display(width, height);
    return true;
  }

  virtual void OnMouseButton(int button, int action, int modifier) override 
  {
    if (action == GLFW_PRESS)
    {
      if (button == 1)
        debug_display.Clear();  
      else
        debug_display.AddLine(chaos::StringTools::Printf("LINE FROM CLICKED [%d]", click_count++).c_str(), 5.0f); 
      RequireWindowRefresh();    
    }
  }

  virtual void Finalize() override
  {
    debug_display.Finalize();
  }

  virtual bool Tick(double delta_time) override
  {
    return debug_display.Tick(delta_time);
  }

  virtual bool Initialize() override
  {   
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    boost::filesystem::path resources_path = application->GetApplicationPath() / "resources";
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

  virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);
    hints.toplevel  = 0;
    hints.decorated = 1;
  }

protected:

  chaos::GLDebugOnScreenDisplay debug_display;

  int click_count;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();
    
  FreeImage_Initialise(); // glew will be initialized 
  glfwInit();

  chaos::MyGLFWSingleWindowApplicationParams params;
  params.monitor       = nullptr;
  params.width         = 500;
  params.height        = 500;
  params.monitor_index = 0;
  chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

  glfwTerminate();
  FreeImage_DeInitialise();

  return 0;
}


