#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	/**
	* WindowCreateParams
	*/

	bool SaveIntoJSON(nlohmann::json& json, WindowCreateParams const& src)
	{
		if (!json.is_object())
			json = nlohmann::json::object();
		JSONTools::SetAttribute(json, "monitor_index", src.monitor_index);
		JSONTools::SetAttribute(json, "width", src.width);
		JSONTools::SetAttribute(json, "height", src.height);
		JSONTools::SetAttribute(json, "resizable", src.resizable);
		JSONTools::SetAttribute(json, "start_visible", src.start_visible);
		JSONTools::SetAttribute(json, "decorated", src.decorated);
		JSONTools::SetAttribute(json, "toplevel", src.toplevel);
		JSONTools::SetAttribute(json, "focused", src.focused);
		JSONTools::SetAttribute(json, "samples", src.samples);
		JSONTools::SetAttribute(json, "double_buffer", src.double_buffer);
		JSONTools::SetAttribute(json, "depth_bits", src.depth_bits);
		JSONTools::SetAttribute(json, "stencil_bits", src.stencil_bits);
		return true;
	}

	bool LoadFromJSON(nlohmann::json const& json, WindowCreateParams& dst)
	{
		if (!json.is_object())
			return false;
		JSONTools::GetAttribute(json, "monitor_index", dst.monitor_index);
		JSONTools::GetAttribute(json, "width", dst.width);
		JSONTools::GetAttribute(json, "height", dst.height);
		JSONTools::GetAttribute(json, "resizable", dst.resizable);
		JSONTools::GetAttribute(json, "start_visible", dst.start_visible);
		JSONTools::GetAttribute(json, "decorated", dst.decorated);
		JSONTools::GetAttribute(json, "toplevel", dst.toplevel);
		JSONTools::GetAttribute(json, "focused", dst.focused);
		JSONTools::GetAttribute(json, "samples", dst.samples);
		JSONTools::GetAttribute(json, "double_buffer", dst.double_buffer);
		JSONTools::GetAttribute(json, "depth_bits", dst.depth_bits);
		JSONTools::GetAttribute(json, "stencil_bits", dst.stencil_bits);
		return true;
	}

	/**
	* Window
	*/

	Window::Window()
	{
		// create the renderer
		renderer = new GPURenderer(this);
	}

	Window::~Window()
	{
		DestroyGLFWWindow();
	}

	void Window::DestroyGLFWWindow()
	{
		if (glfw_window != nullptr)
		{
			GLFWwindow * previous_context = glfwGetCurrentContext();
			glfwMakeContextCurrent(nullptr);
			glfwDestroyWindow(glfw_window);
			if (previous_context != nullptr && previous_context != glfw_window)
				glfwMakeContextCurrent(previous_context);
			glfw_window = nullptr;
		}
	}

	namespace Arguments
	{
#if !_DEBUG
		CHAOS_APPLICATION_ARG(bool, UnlimitedFPS);
#endif
	};

	bool Window::CreateGLFWWindow(WindowCreateParams create_params, GLFWwindow* share_context_window)
	{
		// resource already existing
		if (glfw_window != nullptr)
			return false;

		// compute the monitor upon which the window will be : use it for pixel format
		if (create_params.monitor == nullptr)
			create_params.monitor = GLFWTools::GetMonitorByIndex(create_params.monitor_index);

		// retrieve the position of the monitor
		int monitor_x = 0;
		int monitor_y = 0;
		glfwGetMonitorPos(create_params.monitor, &monitor_x, &monitor_y);

		// compute the position and size of the window
		bool pseudo_fullscreen = (create_params.width <= 0 && create_params.height <= 0);

		// prepare window creation
		glfwWindowHint(GLFW_RESIZABLE, create_params.resizable);
		glfwWindowHint(GLFW_VISIBLE, create_params.start_visible);
		glfwWindowHint(GLFW_DECORATED, create_params.decorated);
		glfwWindowHint(GLFW_FLOATING, create_params.toplevel);
		glfwWindowHint(GLFW_FOCUSED, create_params.focused);
		glfwWindowHint(GLFW_SAMPLES, create_params.samples);
		glfwWindowHint(GLFW_DOUBLEBUFFER, create_params.double_buffer);
		glfwWindowHint(GLFW_DEPTH_BITS, create_params.depth_bits);
		glfwWindowHint(GLFW_STENCIL_BITS, create_params.stencil_bits);
		glfwWindowHint(GLFW_RED_BITS, 8);
		glfwWindowHint(GLFW_GREEN_BITS, 8);
		glfwWindowHint(GLFW_BLUE_BITS, 8);
		glfwWindowHint(GLFW_ALPHA_BITS, 8);
		glfwWindowHint(GLFW_VISIBLE, 0); // override the initial visibility

		// compute window size and position
		GLFWvidmode const* mode = glfwGetVideoMode(create_params.monitor);

		int x = 0;
		int y = 0;
		if (pseudo_fullscreen) // full-screen, the window use the full-size
		{
			create_params.width = mode->width;
			create_params.height = mode->height;

			x = monitor_x;
			y = monitor_y;
		}
		else
		{
			if (create_params.width <= 0)
				create_params.width = mode->width;
			else
				create_params.width = std::min(mode->width, create_params.width);

			if (create_params.height <= 0)
				create_params.height = mode->height;
			else
				create_params.height = std::min(mode->height, create_params.height);

			x = monitor_x + (mode->width - create_params.width) / 2;
			y = monitor_y + (mode->height - create_params.height) / 2;
		}

		// title cannot be null
		if (create_params.title == nullptr)
			create_params.title = "";

		// we are doing a pseudo fullscreen => monitor parameters of glfwCreateWindow must be null or it will "capture" the screen
		glfw_window = glfwCreateWindow(create_params.width, create_params.height, create_params.title, nullptr, share_context_window);
		if (glfw_window == nullptr)
			return false;

		// vsync ?
#if !_DEBUG
		if (Arguments::UnlimitedFPS.Get())
			create_params.unlimited_fps = true;
#else
		create_params.unlimited_fps = true;
#endif

		if (create_params.unlimited_fps)
		{
			WithGLContext<void>([]()
			{
				glfwSwapInterval(0);
			});
		}

		// set the callbacks
		SetGLFWCallbacks(create_params.double_buffer ? true : false);

		// x and y are the coordinates of the client area : when there is a decoration, we want to tweak the window size / position with that
		int left, top, right, bottom;
		glfwGetWindowFrameSize(glfw_window, &left, &top, &right, &bottom);
		if (left != 0 || top != 0 || right != 0 || bottom != 0)
		{
			x += left;
			y += top;
			create_params.width = create_params.width - left - right;
			create_params.height = create_params.height - top - bottom;
			glfwSetWindowSize(glfw_window, create_params.width, create_params.height);
		}

		glfwSetWindowPos(glfw_window, x, y);
		//glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//  glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		glfwSetInputMode(glfw_window, GLFW_STICKY_KEYS, 1);

		// now that the window is fully placed ... we can show it
		if (create_params.start_visible)
			glfwShowWindow(glfw_window);
		return true;
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

	void Window::SetWindowPosition(glm::ivec2 const& position)
	{
		if (glfw_window == nullptr)
			return;
		glfwSetWindowPos(glfw_window, position.x, position.y);

	}

	void Window::SetWindowSize(glm::ivec2 const& size)
	{
		if (glfw_window == nullptr)
			return;
		glfwSetWindowSize(glfw_window, size.x, size.y);
	}

	GLFWwindow* Window::GetGLFWHandler()
	{
		return glfw_window;
	}

	bool Window::ShouldClose()
	{
		return (glfw_window != nullptr && glfwWindowShouldClose(glfw_window) != 0);
	}

	void Window::TickRenderer(float real_delta_time)
	{
		// cannot tick a non existing window
		if (glfw_window == nullptr || renderer == nullptr)
			return;

		WithGLContext<void>([this, real_delta_time]()
		{
			// tick the renderer: it has metrics that rely on the real frame_rate (not modified one due to some of our tweaks)
			renderer->Tick(real_delta_time);
		});
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

	void Window::SetGLFWCallbacks(bool in_double_buffer)
	{
		glfwSetWindowUserPointer(glfw_window, this);

		glfwSetCursorPosCallback(glfw_window, DoOnMouseMove);
		glfwSetMouseButtonCallback(glfw_window, DoOnMouseButton);
		glfwSetScrollCallback(glfw_window, DoOnMouseWheel);
		glfwSetWindowSizeCallback(glfw_window, DoOnWindowResize);
		glfwSetKeyCallback(glfw_window, DoOnKeyEvent);

		glfwSetCharCallback(glfw_window, DoOnCharEvent);

		glfwSetWindowCloseCallback(glfw_window, DoOnWindowClosed);
		glfwSetWindowRefreshCallback(glfw_window, DoOnDraw);
		glfwSetDropCallback(glfw_window, DoOnDropFile);
		glfwSetWindowFocusCallback(glfw_window, DoOnFocusStateChange);
		glfwSetWindowIconifyCallback(glfw_window, DoOnIconifiedStateChange);

		double_buffer = in_double_buffer;
	}

	void Window::DoOnIconifiedStateChange(GLFWwindow* in_glfw_window, int value)
	{
		if (Window* my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window))
		{
			my_window->WithGLContext<void>([my_window, value]()
			{
				my_window->OnIconifiedStateChange(value == GL_TRUE);
			});
		}
	}

	void Window::DoOnFocusStateChange(GLFWwindow* in_glfw_window, int value)
	{
		if (Window* my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window))
		{
			my_window->WithGLContext<void>([my_window, value]()
			{
				my_window->OnFocusStateChange(value == GL_TRUE);
			});
		}
	}

	void Window::DoOnWindowClosed(GLFWwindow* in_glfw_window)
	{
		if (Window* my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window))
		{
			my_window->WithGLContext<void>([my_window]()
			{
				if (my_window->OnWindowClosed())
					my_window->RequireWindowClosure();
			});
		}
	}

	void Window::DoOnWindowResize(GLFWwindow* in_glfw_window, int width, int height)
	{
		if (Window* my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window))
		{
			my_window->WithGLContext<void>([my_window, width, height]()
			{
				my_window->OnWindowResize(glm::ivec2(width, height));
			});
		}
	}

	void Window::DoOnDraw(GLFWwindow* in_glfw_window)
	{
		if (Window* my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window))
		{
			my_window->WithGLContext<void>([my_window]()
			{
				my_window->OnWindowDraw();
			});
		}
	}

	void Window::DoOnMouseMove(GLFWwindow* in_glfw_window, double x, double y)
	{
		Application::SetApplicationInputMode(InputMode::MOUSE);

		if (Window* my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window))
		{
			my_window->WithGLContext<void>([my_window, x, y]()
			{
				if (!my_window->IsMousePositionValid())
					my_window->OnMouseMove(0.0, 0.0);
				else
					my_window->OnMouseMove(x - my_window->mouse_position.x, y - my_window->mouse_position.y);

				my_window->mouse_position.x = (float)x;
				my_window->mouse_position.y = (float)y;
			});
		}
	}

	void Window::DoOnMouseButton(GLFWwindow* in_glfw_window, int button, int action, int modifier)
	{
		MouseButton mouse_button = (MouseButton)button;

		// notify the application of the keyboard button state
		WindowApplication* application = Application::GetInstance();
		if (application != nullptr)
		{
			application->SetInputMode(InputMode::MOUSE);
			application->SetMouseButtonState(mouse_button, action);
		}

		if (Window* my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window))
		{
			my_window->WithGLContext<void>([my_window, button, action, modifier]()
			{
				my_window->OnMouseButton(button, action, modifier);
			});
		}
	}

	void Window::DoOnMouseWheel(GLFWwindow* in_glfw_window, double scroll_x, double scroll_y)
	{
		Application::SetApplicationInputMode(InputMode::MOUSE);

		if (Window* my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window))
		{
			my_window->WithGLContext<void>([my_window, scroll_x, scroll_y]()
			{
				my_window->OnMouseWheel(scroll_x, scroll_y);
			});
		}
	}

	void Window::DoOnKeyEvent(GLFWwindow* in_glfw_window, int key, int scan_code, int action, int modifier)
	{
		KeyboardButton keyboard_button = (KeyboardButton)key;

		// notify the application of the keyboard button state
		WindowApplication* application = Application::GetInstance();
		if (application != nullptr)
		{
			application->SetInputMode(InputMode::KEYBOARD);
			application->SetKeyboardButtonState(keyboard_button, action);
		}
		// handle the message
		KeyEvent event;
		event.key = key;
		event.scan_code = scan_code;
		event.action = action;
		event.modifier = modifier;

		if (Window* my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window))
		{
			my_window->WithGLContext<void>([my_window, event]()
			{
				my_window->OnKeyEvent(event);
			});
		}
	}

	void Window::DoOnCharEvent(GLFWwindow* in_glfw_window, unsigned int c)
	{
		Application::SetApplicationInputMode(InputMode::KEYBOARD);

		if (Window* my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window))
		{
			my_window->WithGLContext<void>([my_window, c]()
			{
				my_window->OnCharEvent(c);
			});
		}
	}

	void Window::DoOnDropFile(GLFWwindow* in_glfw_window, int count, char const** paths)
	{
		if (Window* my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window))
		{
			my_window->WithGLContext<void>([my_window, count, paths]()
			{
				my_window->OnDropFile(count, paths);
			});
		}
	}

	void Window::OnWindowDraw()
	{
		if (glfw_window != nullptr && renderer != nullptr)
		{
			assert(glfw_window == glfwGetCurrentContext());

			// XXX : not sure whether i should call glfwGetWindowSize(..) or glfwGetFramebufferSize(..)
			glm::ivec2 window_size = { 0, 0 };
			glfwGetWindowSize(glfw_window, &window_size.x, &window_size.y); // framebuffer size is in pixel ! (not glfwGetWindowSize)

			if (window_size.x <= 0 || window_size.y <= 0) // some crash to expect in drawing elsewhere
				return;

			renderer->BeginRenderingFrame();

			// compute viewport
			WindowDrawParams draw_params;
			draw_params.viewport = GetRequiredViewport(window_size);
			draw_params.full_size = window_size;
			GLTools::SetViewport(draw_params.viewport);

			// data provider
			GPUProgramProviderCommonTransforms common_transforms; // some common deduction rules

			GPUProgramProviderInterface const* application_provider_interface = Application::GetInstance(); // cast the application into a provider_interface

			GPUProgramProviderChain provider(this, application_provider_interface, common_transforms); // order: window, application, common deduction rules

			// render
			if (OnDraw(renderer.get(), draw_params, &provider))
			{
				if (double_buffer)
					glfwSwapBuffers(glfw_window);
				else
					glFlush(); // seems useless ?
			}
			renderer->EndRenderingFrame();
		}
	}

	void Window::RequireWindowClosure()
	{
		glfwSetWindowShouldClose(glfw_window, 1);
	}

	void Window::RequireWindowRefresh()
	{
		HWND hWnd = glfwGetWin32Window(glfw_window);
		if (hWnd != NULL)
			InvalidateRect(hWnd, NULL, false); // this cause flickering
	}

	GLFWmonitor* Window::GetFullscreenMonitor() const
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
		GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
		if (monitor_count > 0 && monitors != nullptr)
		{
			for (int i = 0; i < monitor_count; ++i)
			{
				// get the monitor
				GLFWmonitor* monitor = monitors[i];
				if (monitor == nullptr)
					continue;
				// get the monitor position
				glm::ivec2 monitor_position = glm::vec2(0, 0);
				glfwGetMonitorPos(monitor, &monitor_position.x, &monitor_position.y);
				// compare with the window position
				if (window_position != monitor_position)
					continue;
				// get the monitor size
				GLFWvidmode const* mode = glfwGetVideoMode(monitor);
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

	GLFWmonitor* Window::GetPreferredMonitor() const
	{
		glm::ivec2 window_center = GetWindowPosition() + GetWindowSize() / 2;

		return GLFWTools::GetNearestMonitor(window_center);
	}

	void Window::ToggleFullscreen()
	{
		if (glfw_window == nullptr)
			return;

		if (GLFWmonitor* fullscreen_monitor = GetFullscreenMonitor())
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
				if (GLFWmonitor * preferred_monitor = GetPreferredMonitor())
				{
					if (GLFWvidmode const* mode = glfwGetVideoMode(preferred_monitor))
					{
						glm::ivec2 monitor_position = glm::ivec2(0, 0);
						glfwGetMonitorPos(preferred_monitor, &monitor_position.x, &monitor_position.y);

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
			if (GLFWmonitor* preferred_monitor = GetPreferredMonitor())
			{
				if (GLFWvidmode const* mode = glfwGetVideoMode(preferred_monitor))
				{
					// change window size and position
					glm::ivec2 monitor_position;
					glfwGetMonitorPos(preferred_monitor, &monitor_position.x, &monitor_position.y);
					SetWindowPosition(monitor_position);
					SetWindowSize(glm::ivec2(mode->width, mode->height));
				}
			}
		}
	}

	bool Window::ScreenCapture()
	{
		// get renderer
		if (glfw_window == nullptr || renderer == nullptr)
			return false;

		return WithGLContext<bool>([this]()
		{
			// this call may take a while causing a jump in 'delta_time'
			WindowApplication* application = Application::GetInstance();
			if (application == nullptr)
				return false;
			application->FreezeNextFrameTickDuration();

			// compute rendering size
			// in normal case, we work with the window_size then apply a viewport cropping
			// here we want exactly to work with no cropping
			ViewportPlacement viewport = GetRequiredViewport(GetWindowSize());
			glm::ivec2 framebuffer_size = viewport.size;

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
			GPUProgramProviderCommonTransforms common_transforms;
			GPUProgramProviderChain provider(this, (WindowApplication*)Application::GetInstance(), common_transforms);

			renderer->BeginRenderingFrame();
			renderer->PushFramebufferRenderContext(framebuffer.get(), false);

			WindowDrawParams draw_params;
			draw_params.viewport.position = { 0, 0 };
			draw_params.viewport.size = framebuffer_size;
			draw_params.full_size = framebuffer_size;

			GLTools::SetViewport(draw_params.viewport); // use the draw_params' viewport because its position is {0, 0} and that's what we want for drawing on Render Target
			OnDraw(renderer.get(), draw_params, &provider);

			renderer->PopFramebufferRenderContext();
			renderer->EndRenderingFrame();

			// extract the texture
			GPUFramebufferAttachmentInfo const* attachment = framebuffer->GetColorAttachment(0);
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
		});
	}

	ViewportPlacement Window::GetRequiredViewport(glm::ivec2 const& size) const
	{
		ViewportPlacement result;
		result.position = { 0, 0 };
		result.size = size;
		return GLTools::ShrinkViewportToAspect(result, 16.0f / 9.0f);
	}

	CHAOS_HELP_TEXT(SHORTCUTS, "F9  : ScreenCapture");
	CHAOS_HELP_TEXT(SHORTCUTS, "F10 : ToggleFullscreen");

	bool Window::OnKeyEventImpl(KeyEvent const& event)
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
		if (Application* application = Application::GetInstance())
			if (application->OnKeyEvent(event))
				return true;
		// super method
		if (InputEventReceiverInterface::OnKeyEventImpl(event))
			return true;
		return false;
	}

	bool Window::InitializeFromConfiguration(nlohmann::json const& config)
	{
		return true;
	}

	bool Window::DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const
	{
		return false;
	}

}; // namespace chaos
