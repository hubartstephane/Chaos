#include <chaos/MyGLFWWindow.h>

namespace chaos
{
  namespace MyGLFW
  {

    /**
    * WindowHints
    */

    void WindowHints::ApplyHints()
    {
      glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debug_context);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version);
      glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate); // only usefull in fullscreen mode
      glfwWindowHint(GLFW_OPENGL_PROFILE, opengl_profile);
      glfwWindowHint(GLFW_RESIZABLE, resizable);
      glfwWindowHint(GLFW_VISIBLE, false);          // XXX : we don't use start_visible yet because we may do some small window displacement (probably never visible but ...)
      glfwWindowHint(GLFW_DECORATED, decorated);
      glfwWindowHint(GLFW_FLOATING, toplevel);
      glfwWindowHint(GLFW_SAMPLES, samples);
      glfwWindowHint(GLFW_DOUBLEBUFFER, double_buffer);
      glfwWindowHint(GLFW_DEPTH_BITS, depth_bits);
      glfwWindowHint(GLFW_STENCIL_BITS, stencil_bits);
      glfwWindowHint(GLFW_RED_BITS, red_bits);
      glfwWindowHint(GLFW_GREEN_BITS, green_bits);
      glfwWindowHint(GLFW_BLUE_BITS, blue_bits);
      glfwWindowHint(GLFW_ALPHA_BITS, alpha_bits);
      glfwWindowHint(GLFW_FOCUSED, focused);
    }

    /**
    * Window
    */

    GLFWwindow * Window::GetGLFWHandler()
    {
      return glfw_window;
    }

    bool Window::ShouldClose()
    {
      return (glfwWindowShouldClose(glfw_window) != 0);
    }

    void Window::MainTick(double delta_time)
    {
      if (Tick(delta_time))
        RequireWindowRefresh();

      if (refresh_required)
      {
        DoOnDraw(glfw_window);
        refresh_required = false;
      }
    }

    void Window::BindGLFWWindow(GLFWwindow * in_glfw_window)
    {
      assert(glfw_window == nullptr); // ensure not already bound
      assert(in_glfw_window != nullptr);

      glfw_window = in_glfw_window;

      glfwSetWindowUserPointer(in_glfw_window, this);

      glfwSetCursorPosCallback(in_glfw_window, DoOnMouseMove);
      glfwSetMouseButtonCallback(in_glfw_window, DoOnMouseButton);
      glfwSetScrollCallback(in_glfw_window, DoOnMouseWheel);
      glfwSetWindowSizeCallback(in_glfw_window, DoOnWindowResize);
      glfwSetKeyCallback(in_glfw_window, DoOnKeyEvent);
      glfwSetWindowCloseCallback(in_glfw_window, DoOnWindowClosed);
      glfwSetWindowRefreshCallback(in_glfw_window, DoOnDraw);
      glfwSetDropCallback(in_glfw_window, DoOnDropFile);
      glfwSetWindowFocusCallback(in_glfw_window, DoOnFocusStateChange);
      glfwSetWindowIconifyCallback(in_glfw_window, DoOnIconifiedStateChange);
    }

    void Window::DoOnIconifiedStateChange(GLFWwindow * in_glfw_window, int value)
    {
      Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
      if (my_window != nullptr)
        my_window->OnIconifiedStateChange(value == GL_TRUE);
    }

    void Window::DoOnFocusStateChange(GLFWwindow * in_glfw_window, int value)
    {
      Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
      if (my_window != nullptr)
        my_window->OnFocusStateChange(value == GL_TRUE);
    }

    void Window::DoOnWindowClosed(GLFWwindow * in_glfw_window)
    {
      Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
      if (my_window != nullptr)
        if (my_window->OnWindowClosed())
          my_window->RequireWindowClosure();
    }

    void Window::DoOnWindowResize(GLFWwindow * in_glfw_window, int width, int height)
    {
      Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
      if (my_window != nullptr)
        my_window->OnWindowResize(glm::ivec2(width, height));
    }

    void Window::DoOnDraw(GLFWwindow * in_glfw_window)
    {
      Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
      if (my_window != nullptr)
      {
        int width = 0;
        int height = 0;

        glfwMakeContextCurrent(in_glfw_window);
        glfwGetFramebufferSize(in_glfw_window, &width, &height); // framebuffer size is in pixel ! (not glfwGetWindowSize)

        if (width <= 0 || height <= 0) // some crash to expect in drawing elsewhere
          return;

        if (my_window->OnDraw(glm::ivec2(width, height)))
          if (my_window->double_buffer)
            glfwSwapBuffers(in_glfw_window);
      }
    }

    void Window::DoOnMouseMove(GLFWwindow * in_glfw_window, double x, double y)
    {
      Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
      if (my_window != nullptr)
        my_window->OnMouseMove(x, y);
    }

    void Window::DoOnMouseButton(GLFWwindow * in_glfw_window, int button, int action, int modifier)
    {
      Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
      if (my_window != nullptr)
        my_window->OnMouseButton(button, action, modifier);
    }

    void Window::DoOnMouseWheel(GLFWwindow * in_glfw_window, double scroll_x, double scroll_y)
    {
      Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
      if (my_window != nullptr)
        my_window->OnMouseWheel(scroll_x, scroll_y);
    }

    void Window::DoOnKeyEvent(GLFWwindow * in_glfw_window, int key, int scan_code, int action, int modifier)
    {
      Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
      if (my_window != nullptr)
        my_window->OnKeyEvent(key, scan_code, action, modifier);
    }

    void Window::DoOnDropFile(GLFWwindow * in_glfw_window, int count, char const ** paths)
    {
      Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
      if (my_window != nullptr)
        my_window->OnDropFile(count, paths);
    }

    void Window::RequireWindowClosure()
    {
      glfwSetWindowShouldClose(glfw_window, 1);
    }

    void Window::RequireWindowRefresh()
    {
      //  refresh_required = true;
      HWND hWnd = glfwGetWin32Window(glfw_window);
      if (hWnd != NULL)
        InvalidateRect(hWnd, NULL, false); // this cause flickering
    }

    void Window::TweakHints(WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const
    {
      // retrieve the mode of the monitor to deduce pixel format
      GLFWvidmode const * mode = glfwGetVideoMode(monitor);

      // the pixel format
      hints.red_bits = mode->redBits;
      hints.green_bits = mode->greenBits;
      hints.blue_bits = mode->blueBits;
      hints.alpha_bits = (hints.red_bits == 8 && hints.green_bits == 8 && hints.blue_bits == 8) ? 8 : 0; // alpha only if RGBA 32bits

      hints.refresh_rate = mode->refreshRate;

      if (pseudo_fullscreen) // full-screen, the window use the full-size
      {
        hints.decorated = 0;
        hints.resizable = 0;
        hints.toplevel = 1;
        hints.focused = 1;
      }
    }

    bool Window::PrepareWindow(GLFWwindow * in_glfw_window, bool in_double_buffer, nlohmann::json const & config, boost::filesystem::path const & config_path)
    {
      BindGLFWWindow(in_glfw_window);
      double_buffer = in_double_buffer;
      return Initialize(config, config_path);
    }

  }; // namespace MyGLFW

}; // namespace chaos
