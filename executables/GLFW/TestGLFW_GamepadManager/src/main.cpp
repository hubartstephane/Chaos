
#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>

class MyGamepadManager : public chaos::MyGLFWGamepadManager
{
protected:

  virtual bool OnGamepadConnected(chaos::MyGLFWGamepad * gamepad) override
  {
    std::cout << "OnGamepadConnected : " << gamepad->GetGamepadIndex() << std::endl;
    return true;
  }

  virtual bool OnGamepadDisconnected(chaos::MyGLFWGamepad * gamepad) override
  {
    std::cout << "OnGamepadDisconnected : " << gamepad->GetGamepadIndex() << std::endl;
    return true;
  }
};

class MyGLFWWindowGamepadTest : public chaos::MyGLFWWindow
{
public:

  MyGLFWWindowGamepadTest() : main_gamepad(nullptr){}

protected:

  virtual bool OnDraw(int width, int height) override
  {
    glm::vec4 clear_color(0.1f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);
    return true;
  }

  virtual bool Initialize() override
  {
    main_gamepad = gamepad_manager.AllocateGamepad();
    return true;
  }

  virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);

    hints.toplevel  = 1;
    hints.decorated = 1;
  }

  virtual bool Tick(double delta_time) override
  {
    gamepad_manager.Tick(0.0f);
#if 0
    if (main_gamepad->IsPresent())
    {
      if (main_gamepad->IsAnyAction())
        std::cout << "IsAnyAction()" << std::endl;

      glm::vec2 l = main_gamepad->GetXBOXStickDirection(chaos::MyGLFWGamepad::XBOX_LEFT_AXIS);
      if (l.x != 0.0f || l.y != 0.0f)
        std::cout << "LEFT x : " << l.x << "    y : " << l.y << std::endl;

      glm::vec2 r = main_gamepad->GetXBOXStickDirection(chaos::MyGLFWGamepad::XBOX_RIGHT_AXIS);
      if (r.x != 0.0f || r.y != 0.0f)
        std::cout << "RIGHT x : " << r.x << "    y : " << r.y << std::endl;
    }
#endif
    return true; // no redraw
  }


protected:

  MyGamepadManager gamepad_manager;

  chaos::MyGLFWGamepad * main_gamepad;
};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  glfwInit();

  chaos::MyGLFWSingleWindowApplicationParams params;
  params.monitor       = nullptr;
  params.width         = 500;
  params.height        = 500;
  params.monitor_index = 0;
  chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowGamepadTest>(params);

  glfwTerminate();

  return 0;
}

