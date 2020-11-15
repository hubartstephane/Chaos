#include <chaos/Chaos.h>

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
			// mouse position
			mouse_position.x = mouse_position.y = std::numeric_limits<float>::max();		
			// create the renderer
			renderer = new GPURenderer;
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

		void Window::MainTick(float delta_time, float real_delta_time)
		{
			// tick the renderer: it has metrics that rely on the real frame_rate (not modified one due to some of our tweaks)
			if (renderer != nullptr)
				renderer->Tick(real_delta_time); 
			// special tick method
			if (Tick(delta_time))
				RequireWindowRefresh();
			// refresh the window
			if (refresh_required)
			{
				OnWindowDraw();
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

		void Window::DoOnDraw(GLFWwindow* in_glfw_window)
		{
			Window* my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
			if (my_window != nullptr)
				my_window->OnWindowDraw();
		}

		void Window::DoOnMouseMove(GLFWwindow * in_glfw_window, double x, double y)
		{
			Application::SetApplicationInputMode(InputMode::MOUSE);

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
			Application::SetApplicationInputMode(InputMode::MOUSE);

			Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
			if (my_window != nullptr)
				my_window->OnMouseButton(button, action, modifier);
		}

		void Window::DoOnMouseWheel(GLFWwindow * in_glfw_window, double scroll_x, double scroll_y)
		{
			Application::SetApplicationInputMode(InputMode::MOUSE);

			Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
			if (my_window != nullptr)
				my_window->OnMouseWheel(scroll_x, scroll_y);
		}

		void Window::DoOnKeyEvent(GLFWwindow * in_glfw_window, int key, int scan_code, int action, int modifier)
		{
			Application::SetApplicationInputMode(InputMode::KEYBOARD);

			KeyEvent event;
			event.key       = key;
			event.scan_code = scan_code;
			event.action    = action;
			event.modifier  = modifier;

			Window * my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window);
			if (my_window != nullptr)
				my_window->OnKeyEvent(event);
		}

		void Window::DoOnCharEvent(GLFWwindow * in_glfw_window, unsigned int c)
		{
			Application::SetApplicationInputMode(InputMode::KEYBOARD);

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

		void Window::OnWindowDraw()
		{
			if (renderer != nullptr)
			{
				glfwMakeContextCurrent(glfw_window);

				// XXX : not sure whether i should call glfwGetWindowSize(..) or glfwGetFramebufferSize(..)
				int width = 0;
				int height = 0;
				glfwGetWindowSize(glfw_window, &width, &height); // framebuffer size is in pixel ! (not glfwGetWindowSize)

				if (width <= 0 || height <= 0) // some crash to expect in drawing elsewhere
					return;

				renderer->BeginRenderingFrame();
				// compute viewport
				glm::vec2 window_size = glm::ivec2(width, height);

				box2 viewport = GetRequiredViewport(window_size);
				GLTools::SetViewport(viewport);
				// render
				if (OnDraw(renderer.get(), viewport, window_size))
				{
					if (double_buffer)
						glfwSwapBuffers(glfw_window);
					// XXX : seems useless
					//else
					//	glFlush(); 
				}
				renderer->EndRenderingFrame();
			}
		}

		bool Window::OnDraw(GPURenderer* renderer, box2 const& viewport, glm::ivec2 window_size)
		{
			return true;
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

					GLFWmonitor * nearest_monitor = MyGLFW::GetNearestMonitor(window_center);
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

				GLFWmonitor * nearest_monitor = MyGLFW::GetNearestMonitor(window_center);
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
			if (renderer == nullptr)
				return false;

			// this call may take a while causing a jump in 'delta_time'
			MyGLFW::SingleWindowApplication* application = Application::GetInstance();
			if (application == nullptr)
				return false;
			application->FreezeNextFrameTickDuration();

			// compute rendering size
			// in normal case, we work with the window_size then apply a viewport cropping
			// here we want exactly to work with no cropping
			box2 viewport = GetRequiredViewport(GetWindowSize());
			viewport.position = viewport.half_size;
			glm::ivec2 framebuffer_size = auto_cast_vector(viewport.half_size * 2.0f);
			
			// generate a framebuffer
			GPUFramebufferGenerator framebuffer_generator;
			framebuffer_generator.AddColorAttachment(0, PixelFormat::BGRA, framebuffer_size, "scene");
			framebuffer_generator.AddDepthStencilAttachment(framebuffer_size, "depth");

			shared_ptr<GPUFramebuffer> framebuffer = framebuffer_generator.GenerateFramebuffer(framebuffer_size);
			if (framebuffer == nullptr)
				return false;
			if (!framebuffer->CheckCompletionStatus())
				return false;

			// render in the frame buffer
			renderer->BeginRenderingFrame();
			renderer->PushFramebufferRenderContext(framebuffer.get(), false);
			GLTools::SetViewport(viewport);
			OnDraw(renderer.get(), viewport, framebuffer_size);
			renderer->PopFramebufferRenderContext();
			renderer->EndRenderingFrame();

			// extract the texture
			GPUFramebufferAttachmentInfo const * attachment = framebuffer->GetColorAttachment(0);
			if (attachment == nullptr || attachment->texture == nullptr)
				return false;

			bitmap_ptr img = bitmap_ptr(ImageTools::GenFreeImage(attachment->texture->GetResourceID(), 0));
			if (img == nullptr)
				return false;

			// create the directory
			boost::filesystem::path capture_directory_path = application->GetUserLocalTempPath() / "Captures";
			if (!boost::filesystem::is_directory(capture_directory_path))
				if (!boost::filesystem::create_directories(capture_directory_path))
					return false;

			// save the image
			std::string format = StringTools::Printf(
				"capture_%s_%%d.png",
				StringTools::TimeToString(true).c_str());

			boost::filesystem::path file_path = FileTools::GetUniquePath(capture_directory_path, format.c_str(), true);
			if (file_path.empty())
				return false;

			return (FreeImage_Save(FIF_PNG, img.get(), file_path.string().c_str(), 0) != 0);
		}

		box2 Window::GetRequiredViewport(glm::ivec2 const & size) const
		{
			box2 viewport = box2(std::make_pair(
				glm::vec2(0.0f, 0.0f),
				RecastVector<glm::vec2>(size)
			));
			return ShrinkBoxToAspect(viewport, 16.0f / 9.0f);
		}

		CHAOS_HELP_TEXT(SHORTCUTS, "F9  : ScreenCapture");
		CHAOS_HELP_TEXT(SHORTCUTS, "F10 : ToggleFullscreen");

		bool Window::OnKeyEventImpl(KeyEvent const & event)
		{
			// kill the window (use ALT+F4 for the moment)
#if 0
			if (event.IsKeyPressed(GLFW_KEY_ESCAPE, GLFW_MOD_SHIFT))
			{
				RequireWindowClosure();
				return true;
			}
#endif
			// screen capture
			// CMD GLFW_KEY_F9 : ScreenCapture(...)
			if (event.IsKeyPressed(GLFW_KEY_F9))
			{
				ScreenCapture();
				return true;
			}
			// try to go fullscreen
			// CMD GLFW_KEY_F10 : ToggleFullscreen(...)
			if (event.IsKeyPressed(GLFW_KEY_F10))
			{
				ToggleFullscreen();
				return true;
			}
			// give opportunity to application
			Application * application = Application::GetInstance();
			if (application != nullptr)
				if (application->OnKeyEvent(event))
					return true;
			// super method
			if (InputEventReceiver::OnKeyEventImpl(event))
				return true;
			return false;
		}

		bool Window::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
		{ 
			return true; 
		}

	}; // namespace MyGLFW

}; // namespace chaos
