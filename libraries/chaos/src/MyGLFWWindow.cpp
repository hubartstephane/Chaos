
#include <chaos/MyGLFWWindow.h>
#include <chaos/Application.h>
#include <chaos/InputMode.h>
#include <chaos/MyGLFWSingleWindowApplication.h>

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

		Window::Window()
		{
			mouse_position.x = mouse_position.y = std::numeric_limits<float>::max();		
		}

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

		bool Window::IsMousePositionValid() const
		{
			if (mouse_position.x == std::numeric_limits<float>::max())
				return false;
			if (mouse_position.y == std::numeric_limits<float>::max())
				return false;
			return true;
		}

		glm::vec2 Window::GetMousePosition() const
		{
			if (!IsMousePositionValid())
				return glm::vec2(0.0f, 0.0f);
			return mouse_position;		
		}

		void Window::BindGLFWWindow(GLFWwindow * in_glfw_window, bool in_double_buffer)
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

			glfwSetCharCallback(in_glfw_window, DoOnCharEvent);

			glfwSetWindowCloseCallback(in_glfw_window, DoOnWindowClosed);
			glfwSetWindowRefreshCallback(in_glfw_window, DoOnDraw);
			glfwSetDropCallback(in_glfw_window, DoOnDropFile);
			glfwSetWindowFocusCallback(in_glfw_window, DoOnFocusStateChange);
			glfwSetWindowIconifyCallback(in_glfw_window, DoOnIconifiedStateChange);

			double_buffer = in_double_buffer;
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


				glfwGetWindowSize(in_glfw_window, &width, &height); // framebuffer size is in pixel ! (not glfwGetWindowSize)

				int l, r, t, b;
				glfwGetWindowFrameSize(in_glfw_window, &l, &t, &r, &b); // framebuffer size is in pixel ! (not glfwGetWindowSize)

				auto ww = r - l;
				auto hh = t - b;

				if (width <= 0 || height <= 0) // some crash to expect in drawing elsewhere
					return;

				MyGLFW::SingleWindowApplication * application = MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
				if (application != nullptr)
				{
					Renderer * renderer = application->GetRenderer();
					if (renderer != nullptr)
					{
						renderer->BeginRenderingFrame();
						if (my_window->OnDraw(renderer, glm::ivec2(width, height)))
							if (my_window->double_buffer)
								glfwSwapBuffers(in_glfw_window);
						renderer->EndRenderingFrame();
					}
				}
			}
		}

		void Window::DoOnMouseMove(GLFWwindow * in_glfw_window, double x, double y)
		{
			Application::SetApplicationInputMode(InputMode::Mouse);

			Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
			if (my_window != nullptr)
			{
				if (!my_window->IsMousePositionValid())
					my_window->OnMouseMove(0.0, 0.0);
				else
					my_window->OnMouseMove(x - my_window->mouse_position.x, y - my_window->mouse_position.y);

				my_window->mouse_position.x = (float)x;
				my_window->mouse_position.y = (float)y;
			}				
		}

		void Window::DoOnMouseButton(GLFWwindow * in_glfw_window, int button, int action, int modifier)
		{
			Application::SetApplicationInputMode(InputMode::Mouse);

			Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
			if (my_window != nullptr)
				my_window->OnMouseButton(button, action, modifier);
		}

		void Window::DoOnMouseWheel(GLFWwindow * in_glfw_window, double scroll_x, double scroll_y)
		{
			Application::SetApplicationInputMode(InputMode::Mouse);

			Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
			if (my_window != nullptr)
				my_window->OnMouseWheel(scroll_x, scroll_y);
		}

		void Window::DoOnKeyEvent(GLFWwindow * in_glfw_window, int key, int scan_code, int action, int modifier)
		{
			Application::SetApplicationInputMode(InputMode::Keyboard);

			Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
			if (my_window != nullptr)
				my_window->OnKeyEvent(key, scan_code, action, modifier);
		}

		void Window::DoOnCharEvent(GLFWwindow * in_glfw_window, unsigned int c)
		{
			Application::SetApplicationInputMode(InputMode::Keyboard);

			Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
			if (my_window != nullptr)
				my_window->OnCharEvent(c);
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

		void Window::ToggleFullscreen()
		{
			if (glfw_window == nullptr)
				return;

			bool decorated = (bool)glfwGetWindowAttrib(glfw_window, GLFW_DECORATED);
			if (decorated)
			{


			}
			else
			{
			
			
			}
			glfwSetWindowAttrib(glfw_window, GLFW_DECORATED, !decorated);

			//glfwSetWindowAttrib(glfw_window, GLFW_DECORATED, !glfwGetWindowAttrib(glfw_window, GLFW_DECORATED)); // shuxxx


			int left, top, right, bottom;
			glfwGetWindowFrameSize(glfw_window, &left, &top, &right, &bottom);
			if (left != 0 || top != 0 || right != 0 || bottom != 0)
			{
#if 0
				x += left;
				y += top;
				params.width = params.width - left - right;
				params.height = params.height - top - bottom;
				glfwSetWindowSize(glfw_window, params.width, params.height);
#endif
				left = left;
			}





			
			glfwMaximizeWindow(glfw_window);
			

			int i = 0;
			++i;
		}

	}; // namespace MyGLFW

}; // namespace chaos
