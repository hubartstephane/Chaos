
#include <chaos/MyGLFWWindow.h>
#include <chaos/Application.h>
#include <chaos/InputMode.h>
#include <chaos/MyGLFWTools.h>
#include <chaos/GPUFramebufferGenerator.h>
#include <chaos/GLTextureTools.h>
#include <chaos/MyGLFWSingleWindowApplication.h>
#include <chaos/GPUTextureLoader.h>

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

		glm::ivec2 Window::GetWindowPosition() const
		{
			glm::ivec2 result = glm::vec2(0, 0);
			if (glfw_window != nullptr)
				glfwGetWindowPos(glfw_window, &result.x, &result.y);
			return result;
		}

		glm::ivec2 Window::GetWindowSize() const
		{
			glm::ivec2 result = glm::vec2(0, 0);
			if (glfw_window != nullptr)
				glfwGetWindowSize(glfw_window, &result.x, &result.y);
			return result;
		}
		
		void Window::SetWindowPosition(glm::ivec2 const & position)
		{
			if (glfw_window == nullptr)
				return;
			glfwSetWindowPos(glfw_window, position.x, position.y);
		
		}
		
		void Window::SetWindowSize(glm::ivec2 const & size)
		{
			if (glfw_window == nullptr)
				return;
			glfwSetWindowSize(glfw_window, size.x, size.y);
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
				glfwMakeContextCurrent(in_glfw_window);

				// XXX : not sure whether i should call glfwGetWindowSize(..) or glfwGetFramebufferSize(..)
				int width = 0;
				int height = 0;
				glfwGetWindowSize(in_glfw_window, &width, &height); // framebuffer size is in pixel ! (not glfwGetWindowSize)

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
						{					
							if (my_window->double_buffer)
								glfwSwapBuffers(in_glfw_window);
							// XXX : seems useless
							//else
							//	glFlush(); 
						}
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

		GLFWmonitor * Window::GetFullscreenMonitor() const
		{
			if (glfw_window == nullptr)
				return nullptr;

			// fullscreen window is not decorated
			int decorated = glfwGetWindowAttrib(glfw_window, GLFW_DECORATED);
			if (decorated)
				return nullptr;

			// get the window information
			glm::ivec2 window_position = GetWindowPosition();

			glm::ivec2 window_size = GetWindowSize();

			// search for all monitors if there is a match
			int monitor_count = 0;
			GLFWmonitor ** monitors = glfwGetMonitors(&monitor_count);
			if (monitor_count > 0 && monitors != nullptr)
			{
				for (int i = 0 ; i < monitor_count ; ++i)
				{
					// get the monitor
					GLFWmonitor * monitor = monitors[i];
					if (monitor == nullptr)
						continue;
					// get the monitor position
					glm::ivec2 monitor_position = glm::vec2(0, 0);
					glfwGetMonitorPos(monitor, &monitor_position.x, &monitor_position.y);
					// compare with the window position
					if (window_position != monitor_position)
						continue;
					// get the monitor size
					GLFWvidmode const * mode = glfwGetVideoMode(monitor);
					if (mode == nullptr)
						continue;
					glm::ivec2 monitor_size = glm::ivec2(mode->width, mode->height);
					// compare with the window size
					if (window_size != monitor_size)
						continue;

					return monitor;
				}
			}
			return nullptr;
		}

		void Window::ToggleFullscreen()
		{
			if (glfw_window == nullptr)
				return;

			GLFWmonitor * fullscreen_monitor = GetFullscreenMonitor();

			if (fullscreen_monitor != nullptr)
			{
				// data properly initialized
				if (non_fullscreen_window_size.x >= 0 && non_fullscreen_window_size.y >= 0)
				{
					glfwSetWindowAttrib(glfw_window, GLFW_DECORATED, non_fullscreen_window_decorated != 0);

					glfwSetWindowPos(glfw_window, non_fullscreen_window_position.x, non_fullscreen_window_position.y);

					glfwSetWindowSize(glfw_window, non_fullscreen_window_size.x, non_fullscreen_window_size.y);
				}
				// window probably started fullscreen
				else
				{
					glm::ivec2 window_center = GetWindowPosition() + GetWindowSize() / 2;

					GLFWmonitor * nearest_monitor = MyGLFW::Tools::GetNearestMonitor(window_center);
					if (nearest_monitor != nullptr)
					{
						GLFWvidmode const * mode = glfwGetVideoMode(nearest_monitor);
						if (mode != nullptr)
						{
							glm::ivec2 monitor_position = glm::ivec2(0, 0);
							glfwGetMonitorPos(nearest_monitor, &monitor_position.x, &monitor_position.y);

							glfwSetWindowAttrib(glfw_window, GLFW_DECORATED, 1); // default choice

							glm::ivec2 window_size = glm::ivec2(mode->width, mode->height) / 2;
							SetWindowSize(window_size);

							glm::ivec2 window_position = monitor_position + window_size / 2;
							SetWindowPosition(window_position);
						}
					}
				}
			}
			else
			{
				// store current information
				non_fullscreen_window_decorated = glfwGetWindowAttrib(glfw_window, GLFW_DECORATED) != 0;

				glfwGetWindowPos(glfw_window, &non_fullscreen_window_position.x, &non_fullscreen_window_position.y);

				glfwGetWindowSize(glfw_window, &non_fullscreen_window_size.x, &non_fullscreen_window_size.y);

				// remove decoration
				glfwSetWindowAttrib(glfw_window, GLFW_DECORATED, 0);
				// search the monitor that contains the center of the window
				glm::ivec2 window_center = GetWindowPosition() + GetWindowSize() / 2;

				GLFWmonitor * nearest_monitor = MyGLFW::Tools::GetNearestMonitor(window_center);
				if (nearest_monitor  != nullptr)
				{
					GLFWvidmode const * mode = glfwGetVideoMode(nearest_monitor);
					if (mode != nullptr)
					{
						// change window size and position
						glm::ivec2 monitor_position;
						glfwGetMonitorPos(nearest_monitor, &monitor_position.x, &monitor_position.y);
						SetWindowPosition(monitor_position);
						SetWindowSize(glm::ivec2(mode->width, mode->height));
					}				
				}
			}
		}

		bool Window::ScreenCapture()
		{
			// get renderer
			MyGLFW::SingleWindowApplication * application = MyGLFW::SingleWindowApplication::GetGLFWApplicationInstance();
			if (application == nullptr)
				return false;

			Renderer * renderer = application->GetRenderer();
			if (renderer == nullptr)
				return false;

			// compute rendering size
			int width = 512;
			int height = 512;

			glm::ivec2 framebuffer_size = glm::ivec2(width, height);

			// generate a framebuffer
			chaos::GPUFramebufferGenerator framebuffer_generator;
			framebuffer_generator.AddColorAttachment(0, chaos::PixelFormat::GetPixelFormat<chaos::PixelBGRA>(), framebuffer_size, "scene");
			framebuffer_generator.AddDepthStencilAttachment(framebuffer_size, "depth");

			chaos::shared_ptr<chaos::GPUFramebuffer> framebuffer = framebuffer_generator.GenerateFramebuffer(framebuffer_size);
			if (framebuffer == nullptr)
				return false;
			if (!framebuffer->CheckCompletionStatus())
				return false;

			// render in the frame buffer
			framebuffer->BeginRendering();
			glViewport(0, 0, framebuffer_size.x, framebuffer_size.y);
				
			renderer->BeginRenderingFrame();					
			OnDraw(renderer, framebuffer_size);
			renderer->EndRenderingFrame();

			framebuffer->EndRendering(false);

			// extract the texture
			chaos::GPUFramebufferAttachmentInfo const * attachment = framebuffer->GetColorAttachment(0);
			if (attachment == nullptr || attachment->texture == nullptr)
				return false;



			chaos::ImageDescription desc;
			char * pixels = chaos::GLTextureTools::GetTextureImage(attachment->texture->GetResourceID(), 0, desc);
			if (pixels == nullptr)
				return false;

			delete[](pixels);

			bitmap_ptr img = bitmap_ptr(FreeImage_Allocate(width, height, 32));
			if (img == nullptr)
				return false;

			BYTE * img_data = FreeImage_GetBits(img.get());
			if (img_data == nullptr)
				return false;

			memcpy(img_data, pixels, width * height * 4);

			// create the directory
			boost::filesystem::path capture_directory_path = application->GetUserLocalTempPath() / "Captures";
			if (!boost::filesystem::is_directory(capture_directory_path))
				if (!boost::filesystem::create_directories(capture_directory_path))
					return false;

			// save the image




#if 0

			

			boost::filesystem::path filepath = application->GetUserLocalTempPath() / "toto.png";

			bool b = FreeImage_Save(FIF_PNG, img.get(), filepath.string().c_str(), 0);

#else

		bitmap_ptr bb = bitmap_ptr(FreeImage_ConvertTo8Bits(img.get()));

			//bitmap_ptr bb = bitmap_ptr(FreeImage_Pimg.get(), FIT_F_GIF));

			boost::filesystem::path filepath = application->GetUserLocalTempPath() / "toto.gif";

			bool b = FreeImage_Save(FIF_GIF, bb.get(), filepath.string().c_str(), 0);

			

#endif


			return true;
		}

	}; // namespace MyGLFW

}; // namespace chaos
