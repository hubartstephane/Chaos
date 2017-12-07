#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ClockManager.h>
#include <chaos/SoundManager.h>

namespace chaos
{
  namespace MyGLFW
  {
    /**
    * WindowHints : this represents hint for GLFWwindow creation
    */

    class WindowHints
    {
    public:

      /** gives set hints to GLFW */
      void ApplyHints();

    public:

      /** true if we use an opengl debug context */
      int debug_context = 1;
      /** the major version of opengl */
      int major_version = 4;
      /** the major version of opengl */
      int minor_version = 4;
      /** the refresh rate (only usefull in fullscreen mode) */
      int refresh_rate = 60;
      /** the opengl profile */
      int opengl_profile = GLFW_OPENGL_CORE_PROFILE;
      /** true if the window can be resized */
      int resizable = 1;
      /** true if the window starts visible */
      int start_visible = 1;
      /** true if the window has some decoration */
      int decorated = 1;
      /** true if the window is toplevel */
      int toplevel = 0;
      /** number of samples in multisamples (0 for none) */
      int samples = 0;
      /** self description */
      int double_buffer = 1;
      /** self description */
      int depth_bits = 24;
      /** self description */
      int stencil_bits = 8;
      /** self description */
      int red_bits = 8;
      /** self description */
      int green_bits = 8;
      /** self description */
      int blue_bits = 8;
      /** self description */
      int alpha_bits = 8;
      /** self description */
      int focused = 0;
    };

    /**
    * SingleWindowApplicationParams : parameters for playing single window application
    */

    class SingleWindowApplicationParams
    {
    public:

      /** the title */
      char const * title = nullptr;
      /** the wanted monitor */
      GLFWmonitor * monitor = nullptr;
      /** the monitor index */
      int monitor_index = 0;
      /** window width */
      int width = 0;
      /** window height */
      int height = 0;
      /** the hints */
      WindowHints hints;
    };

    /**
    * Window : a binding class between chaos and GLFW to handle window (beware the prefix "My")
    */

    class Window
    {
    public:

      /** create and run single screen application (if width == height == 0 use pseudo fullscreen mode) */
      template<typename WINDOW_TYPE>
      static bool RunSingleWindowApplication(SingleWindowApplicationParams const & params)
      {
        bool result = false;

        WINDOW_TYPE * my_window = new WINDOW_TYPE();
        if (my_window != nullptr)
        {
          result = my_window->DoRunSingleWindowApplication(params);
          delete(my_window);
        }
        return result;
      }

      /** get the hints for new GLFW window */
      virtual void TweakSingleWindowApplicationHints(WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const;

      /** Get a list of all monitors sorted left to right */
      static std::vector<GLFWmonitor *> GetSortedMonitors();
      /** Get monitor by its index (negative for LEFT, positive for RIGHT, 0 for primary */
      static GLFWmonitor * GetMonitorByIndex(int monitor_index);

      /** gets the main clock */
      Clock * GetMainClock() { return main_clock.get(); }
      /** gets the main clock */
      Clock const * GetMainClock() const { return main_clock.get(); }

      /** load a configuration file */
      nlohmann::json LoadConfigurationFile();

      /** gets the sound manager */
      SoundManager * GetSoundManager() { return sound_manager.get(); }
      /** gets the sound manager */
      SoundManager const * GetSoundManager() const { return sound_manager.get(); }

    protected:

      /** an error callback */
      static void OnError(int code, const char* msg);
      /** the main function for single window application */
      virtual bool DoRunSingleWindowApplication(SingleWindowApplicationParams params);
      /** the main loop for single window application */
      virtual void DoRunSingleWindowMainLoop(GLFWwindow * glfw_window);
      /** bind Window with GLFW */
      virtual void BindGLFWWindow(GLFWwindow * in_glfw_window);
      /** called every Tick (returns true whenever we want to redraw the window) */
      virtual bool Tick(double delta_time) { return true; }
      /** called at window creation (returns false if the window must be killed) */
      virtual bool Initialize() { return true; }
      /** called at window destruction */
      virtual void Finalize() { }

      /** called whenever the user try to close window */
      virtual bool OnWindowClosed() { return true; }
      /** called whenever the window is resized */
      virtual void OnWindowResize(glm::ivec2 size) {}
      /** called whenever the window is redrawn */
      virtual bool OnDraw(glm::ivec2 size) { return true; }
      /** called whenever the mouse is moved */
      virtual void OnMouseMove(double x, double y) {}
      /** called whenever the mouse button is down / up */
      virtual void OnMouseButton(int button, int action, int modifier) {}
      /** called whenever the mouse wheel is changed */
      virtual void OnMouseWheel(double scroll_x, double scroll_y) {}
      /** called whenever a key is pressed */
      virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) {}
      /** called whenever a file is dropped */
      virtual void OnDropFile(int count, char const ** paths) {}
      /** called whenever the window becomes iconified or is restored */
      virtual void OnIconifiedStateChange(bool iconified) {}
      /** called whenever the window gain or loose focus */
      virtual void OnFocusStateChange(bool gain_focus) {}

      /** called to require the window to close */
      void RequireWindowClosure();
      /** called to require the window to refresh */
      void RequireWindowRefresh();

      /** tweak the application params */
      void TweakApplicationParamsFromConfiguration(nlohmann::json configuration, SingleWindowApplicationParams & params);

    private:

      /** binding function with GLFW library */
      static void DoOnWindowClosed(GLFWwindow * in_glfw_window);
      /** binding function with GLFW library */
      static void DoOnWindowResize(GLFWwindow * in_glfw_window, int width, int height);
      /** binding function with GLFW library */
      static void DoOnDraw(GLFWwindow * in_glfw_window);
      /** binding function with GLFW library */
      static void DoOnMouseMove(GLFWwindow * in_glfw_window, double x, double y);
      /** binding function with GLFW library */
      static void DoOnMouseButton(GLFWwindow * in_glfw_window, int button, int action, int modifier);
      /** binding function with GLFW library */
      static void DoOnMouseWheel(GLFWwindow * in_glfw_window, double scroll_x, double scroll_y);
      /** binding function with GLFW library */
      static void DoOnKeyEvent(GLFWwindow * in_glfw_window, int key, int scan_code, int action, int modifier);
      /** binding function with GLFW library */
      static void DoOnDropFile(GLFWwindow * in_glfw_window, int count, char const ** paths);
      /** binding function with GLFW library */
      static void DoOnIconifiedStateChange(GLFWwindow * in_glfw_window, int iconified);
      /** binding function with GLFW library */
      static void DoOnFocusStateChange(GLFWwindow * in_glfw_window, int gain_focus);

    protected:

      /** the main clock of the manager */
      boost::intrusive_ptr<Clock> main_clock;
      /** the sound manager */
      boost::intrusive_ptr<SoundManager> sound_manager;
      /** the window in GLFW library */
      GLFWwindow * glfw_window = nullptr;
      /** is a refresh required */
      bool refresh_required = false;
      /** is the window with double buffer */
      bool double_buffer = true;
    };

  }; // namespace MyGLFW

}; // namespace chaos

