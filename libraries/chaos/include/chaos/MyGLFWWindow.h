#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/InputEventReceiver.h>
#include <chaos/ReferencedObject.h>
#include <chaos/GeometryFramework.h>
#include <chaos/GPURenderer.h>

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
			/** whether we want the fps to be unlimited */
			bool unlimited_fps = false;
		};

		/**
		* Window : a binding class between chaos and GLFW to handle window (beware the prefix "My")
		*/

		class Window : public ReferencedObject, public InputEventReceiver
		{
			friend class SingleWindowApplication;

		public:

			/** constructor */
			Window();

			/** entry point from Application */
			void MainTick(double delta_time);
			/** called to require the window to close */
			void RequireWindowClosure();
			/** called to require the window to refresh */
			void RequireWindowRefresh();
			/** getter on the handler */
			GLFWwindow * GetGLFWHandler();
			/** returns whether the window has a pending GLFW close message */
			bool ShouldClose();

			/** toggle to fullscreen mode */
			void ToggleFullscreen();
			/** returns whether the screen is fullscreen */
			bool IsFullscreen() const { return GetFullscreenMonitor() != nullptr;}
			/** returns the monitor for which the window is fullscreen (nullptr otherwise) */
			GLFWmonitor * GetFullscreenMonitor() const;

			/** returns the position of the window */
			glm::ivec2 GetWindowPosition() const;
			/** returns the size of the window */
			glm::ivec2 GetWindowSize() const;

			/** change the window position */
			void SetWindowPosition(glm::ivec2 const & position);
			/** change the window size */
			void SetWindowSize(glm::ivec2 const & size);

			/** require a screen capture */
			bool ScreenCapture();

			/** getting the required viewport for given window */
			virtual chaos::box2 GetRequiredViewport(glm::ivec2 const & size) const;

			/** override */
			virtual bool OnKeyEvent(int key, int scan_code, int action, int modifier) override;

		protected:

			/** get the hints for new GLFW window */
			virtual void TweakHints(WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const;
			/** bind Window with GLFW */
			virtual void BindGLFWWindow(GLFWwindow * in_glfw_window, bool in_double_buffer);
			/** called every Tick (returns true whenever we want to redraw the window) */
			virtual bool Tick(double delta_time) { return true; }
			/** called at window creation (returns false if the window must be killed) */
			virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) { return true; }
			/** called at window destruction */
			virtual void Finalize() { }

			/** called whenever the user try to close window */
			virtual bool OnWindowClosed() { return true; }
			/** called whenever the window is resized */
			virtual void OnWindowResize(glm::ivec2 size) {}
			/** called whenever the window is redrawn */
			virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::box2 const & viewport, glm::ivec2 window_size) { return true; }

			/** called whenever a file is dropped */
			virtual void OnDropFile(int count, char const ** paths) {}
			/** called whenever the window becomes iconified or is restored */
			virtual void OnIconifiedStateChange(bool iconified) {}
			/** called whenever the window gain or loose focus */
			virtual void OnFocusStateChange(bool gain_focus) {}

			/** called by application when the input mode changes */
			virtual void OnInputModeChanged(int new_mode, int old_mode){}

			/** get the mouse position */
			glm::vec2 GetMousePosition() const;
			/** returns true if the mouse position is valid (very first frame) */
			bool IsMousePositionValid() const;

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
			static void DoOnCharEvent(GLFWwindow * in_glfw_window, unsigned int c);
			/** binding function with GLFW library */
			static void DoOnDropFile(GLFWwindow * in_glfw_window, int count, char const ** paths);
			/** binding function with GLFW library */
			static void DoOnIconifiedStateChange(GLFWwindow * in_glfw_window, int iconified);
			/** binding function with GLFW library */
			static void DoOnFocusStateChange(GLFWwindow * in_glfw_window, int gain_focus);

		protected:

			/** the window in GLFW library */
			GLFWwindow * glfw_window = nullptr;
			/** is a refresh required */
			bool refresh_required = false;
			/** is the window with double buffer */
			bool double_buffer = true;

			/** previous mouse position */
			glm::vec2 mouse_position;

			/** store window position for fullscreen */
			glm::ivec2 non_fullscreen_window_position = glm::ivec2(0, 0);
			/** store window size for fullscreen (-1, -1) for non initialized */
			glm::ivec2 non_fullscreen_window_size = glm::ivec2(-1, -1);
			/** whether the window had decorations before toggling fullscreen */
			bool non_fullscreen_window_decorated = true;
		};

	}; // namespace MyGLFW

}; // namespace chaos

