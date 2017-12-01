
#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>

class MyGLFWWindowTest : public chaos::MyGLFW::Window
{

protected:

  virtual bool OnDraw(glm::ivec2 size) override
  {    
    glm::vec4 clear_color(0.1f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);
    return true;
  }

  virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override
  {
    if (key == GLFW_KEY_ESCAPE && action ==  GLFW_PRESS)
      RequireWindowClosure();
  }

  virtual void TweakSingleWindowApplicationHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFW::Window::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);

    hints.toplevel  = 1;
    hints.decorated = 1;
  }

  virtual void OnIconifiedStateChange(bool iconified)
  {
    std::cout << "OnIconifiedStateChange : " << iconified << std::endl;
  }  

  virtual void OnFocusStateChange(bool gain_focus)
  {
    std::cout << "OnFocusStateChange : " << gain_focus << std::endl;
  }  
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor       = nullptr;
  params.width         = 500;
  params.height        = 500;
  params.monitor_index = 0;
  chaos::MyGLFW::Window::RunSingleWindowApplication<MyGLFWWindowTest>(params);

  chaos::Application::Finalize();

  return 0;
}

