#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	/**
	* Window
	*/

	Window::~Window()
	{
		assert(glfw_window == nullptr);
		assert(imgui_context == nullptr);
		assert(implot_context == nullptr);
	}

	void Window::Destroy()
	{
		if (WindowApplication* window_application = Application::GetInstance())
		{
			auto it = std::ranges::find_if(window_application->windows, [this](shared_ptr<Window> const& w)
			{
				return (w == this);
			});

			if (it != window_application->windows.end())
			{
				shared_ptr<Window> prevent_destruction = this;
				window_application->windows.erase(it);
				if (GetWindowDestructionGuard() == 0) // can destroy immediatly the window or must wait until no current operation ?
					DoWindowDestruction();
			}
		}
	}

	void Window::DoWindowDestruction() // window is not more handled by WindowApplication
	{
		StorePersistentProperties(true);
		Finalize();
		DestroyImGuiContext();
		DestroyRenderContext();
		DestroyGLFWWindow();
		if (WindowApplication* window_application = Application::GetInstance())
			window_application->OnWindowDestroyed(this);
	}

	bool Window::CreateRenderContext(GPUDevice * in_gpu_device)
	{
		assert(render_context == nullptr);
		render_context = in_gpu_device->CreateRenderContext(this);
		if (render_context == nullptr)
			return false;
		return true;
	}

	void Window::DestroyRenderContext()
	{
		assert(render_context != nullptr);
		render_context->Destroy();
		render_context = nullptr;
	}
		
	void Window::DestroyImGuiContext()
	{
		// destroy implot context
		if (implot_context != nullptr)
		{
			ImPlotContext* previous_implot_context = ImPlot::GetCurrentContext();
			ImPlot::SetCurrentContext(implot_context);

			ImPlot::DestroyContext(implot_context);

			ImPlot::SetCurrentContext((previous_implot_context != implot_context) ? previous_implot_context : nullptr); // if there was another context, restore it

			implot_context = nullptr;
		}

		// destroy ImGui context (must happen before the windows destruction because some GLFW callbacks rely on the existence of the ImGui context)
		if (imgui_context != nullptr)
		{
#if _WIN32
			SetImGuiWindowProc(false); // remove our WndProc layer before ImGui may remove its layer
#endif // #if _WIN32
			ImGuiContext* previous_imgui_context = ImGui::GetCurrentContext();
			ImGui::SetCurrentContext(imgui_context);

			GetImGuiManager()->FinalizeWindowImGuiContext(this);

			ImGui_ImplGlfw_Shutdown();
			ImGui_ImplOpenGL3_Shutdown();

			ImGui::DestroyContext();
			ImGui::SetCurrentContext((previous_imgui_context != imgui_context) ? previous_imgui_context : nullptr); // if there was another context, restore it

			imgui_context = nullptr;
		}
	}

	void Window::DestroyGLFWWindow()
	{
		// destroy GLFW window
		if (glfw_window != nullptr)
		{
			GLFWwindow * previous_context = glfwGetCurrentContext();
			glfwMakeContextCurrent(nullptr);
			glfwDestroyWindow(glfw_window);
			glfwMakeContextCurrent((previous_context != glfw_window)? previous_context : nullptr); // if there was another context, restore it

			glfw_window = nullptr;
		}
	}

	namespace GlobalVariables
	{
		CHAOS_GLOBAL_VARIABLE(bool, UnlimitedFPS, false);
	};

	void Window::ShowWindow(bool visible)
	{
		if (glfw_window == nullptr)
			return;
		if (visible)
			glfwShowWindow(glfw_window);
		else
			glfwHideWindow(glfw_window);
	}

	bool Window::IsWindowVisible() const
	{
		if (glfw_window == nullptr)
			return false;
		return glfwGetWindowAttrib(glfw_window, GLFW_VISIBLE) != 0;
	}

	void Window::SetWindowPlacement(WindowPlacementInfo placement_info)
	{
		// compute the monitor over which the window will be
		GLFWmonitor* monitor = placement_info.monitor;
		if (monitor == nullptr)
		{
			// explicitly given monitor
			if (placement_info.monitor_index.has_value())
				monitor = MonitorTools::GetMonitorByIndex(placement_info.monitor_index.value());
			// position is an absolute position. get the corresponding monitor and transform position into monitor local
			else if (placement_info.position.has_value()) 
			{
				monitor = MonitorTools::GetNearestMonitor(placement_info.position.value());
				placement_info.position = MonitorTools::AbsolutePositionToMonitor(placement_info.position.value(), monitor);
			}
			// fallback on primary monitor
			else
				monitor = glfwGetPrimaryMonitor();

			if (monitor == nullptr) // all monitors are off
				return;
		}

		// XXX: at this point, if placement_info.position is defined, it is relative to the monitor

		// retrieve the position and size of the monitor
		GLFWvidmode const* mode = glfwGetVideoMode(monitor);
		if (mode == nullptr)
			return;
		glm::ivec2 monitor_size = { mode->width , mode->height };

		// compute the position and size of the window
		bool pseudo_fullscreen = true;
		if (!placement_info.fullscreen)
			if (placement_info.size.has_value())
				if (placement_info.size.value().x > 0 && placement_info.size.value().y > 0) // a unset size is equivalent to a fullscreen
					pseudo_fullscreen = false;

		// compute window size and position
		glm::ivec2 window_position = { 0, 0 };
		glm::ivec2 window_size = { 0, 0 };

		bool decorated = false;
		bool toplevel  = false;
#if _DEBUG
		bool toplevel_enabled = false; // prevent toplevel in DEBUG because it's more complicated for debugging
#else
		bool toplevel_enabled = true;
#endif

		if (pseudo_fullscreen) // full-screen, the window use the full-size
		{
			// update internals
			if (placement_info.fullscreen) // no decorator
			{
				fullscreen_monitor = monitor;
				decorated = false;
				toplevel = true;
			}
			else // decorators ? it depends of initial settings (this is a window that takes the whole monitor, but maybe with decorators)
			{
				fullscreen_monitor = nullptr;
				decorated = initial_decorated;
				toplevel = initial_toplevel;
			}
			// compute size and position
			window_size = monitor_size;
			window_position = { 0, 0 };
			// set position in absolute coordinates
			window_position = MonitorTools::MonitorPositionToAbsolute(window_position, monitor);
		}
		else
		{
			assert(placement_info.size.has_value());

			// update internals
			fullscreen_monitor = nullptr;
			decorated = initial_decorated;
			toplevel = initial_toplevel;
			
			// compute size
			window_size = placement_info.size.value();
			// compute position (local to concerned monitor)
			if (placement_info.position.has_value())
				window_position = placement_info.position.value();
			else
				window_position = (monitor_size - window_size) / 2; // use center of screen by default

			// set position in absolute coordinates
			window_position = MonitorTools::MonitorPositionToAbsolute(window_position, monitor);
			// ensure a large enough portion of the window is visible in any monitor (so that it can be displaced by the user)
			window_position = MonitorTools::GetTweakedWindowPositionForMonitors(window_position, window_size);
		}

		glfwSetWindowAttrib(glfw_window, GLFW_DECORATED, (decorated)? 1 : 0);
		glfwSetWindowAttrib(glfw_window, GLFW_FLOATING, (toplevel && toplevel_enabled)? 1 : 0);

		// update placement
		SetWindowSize(window_size, true); // include decorators
		SetWindowPosition(window_position, true); // include decorators
	}

	void Window::UpdatePlacementInfoAccordingToConfig(WindowPlacementInfo & placement_info) const
	{
		// search size and position values
		// 1-saved configuration overrides given parameters
		// 2-use given parameters
		// 3-use fallback configuration
		JSONReadConfiguration config = GetJSONReadConfiguration();

		glm::ivec2 window_size;
		if (JSONTools::GetAttribute(config, "size", window_size))
			placement_info.size = window_size;
		else if (!placement_info.size.has_value())
			JSONTools::GetAttribute(config, "default_size", placement_info.size);

		glm::ivec2 window_position;
		if (JSONTools::GetAttribute(config, "position", window_position))
			placement_info.position = window_position;
		else if (!placement_info.position.has_value())
			JSONTools::GetAttribute(config, "default_position", placement_info.position);

		int monitor_index = 0;
		if (JSONTools::GetAttribute(config, "monitor_index", monitor_index))
			placement_info.monitor_index = monitor_index;

		if (!JSONTools::GetAttribute(config, "fullscreen", placement_info.fullscreen))
			JSONTools::GetAttribute(config, "default_fullscreen", placement_info.fullscreen);
	}

	bool Window::CreateGLFWWindow(GPUDevice * in_gpu_device, WindowPlacementInfo placement_info, WindowCreateParams const &create_params, GLFWwindow* share_context_window, GLFWHints const & glfw_hints)
	{
		assert(glfw_window == nullptr);

		// create the GPURenderContext
		if (!CreateRenderContext(in_gpu_device))
			return false;

		// prepare window creation
		glfwWindowHint(GLFW_RESIZABLE, create_params.resizable);
		glfwWindowHint(GLFW_VISIBLE, create_params.start_visible);
		glfwWindowHint(GLFW_DECORATED, create_params.decorated);
		glfwWindowHint(GLFW_FLOATING, create_params.toplevel);
		glfwWindowHint(GLFW_FOCUSED, create_params.focused);
		glfwWindowHint(GLFW_VISIBLE, 0); // override the initial visibility

		glfw_hints.ApplyHints();

		initial_toplevel  = create_params.toplevel;
		initial_decorated = create_params.decorated;
		double_buffer     = glfw_hints.double_buffer;
	
		// we are doing a pseudo fullscreen => monitor parameters of glfwCreateWindow must be null or it will "capture" the screen
		// use a random size that will be corrected later in SetWindowPlacement call
		glfw_window = glfwCreateWindow(500, 500, create_params.title.c_str(), nullptr, share_context_window);
		if (glfw_window == nullptr)
			return false;
		glfwMakeContextCurrent(glfw_window);

		// set vsync
		if (GlobalVariables::UnlimitedFPS.Get() || glfw_hints.unlimited_fps)
			glfwSwapInterval(0);

		// set the callbacks
		SetGLFWCallbacks();
		// some input mode
		glfwSetInputMode(glfw_window, GLFW_STICKY_KEYS, 1);
		// prepare cursor mode
		SetCursorMode(CursorMode::DISABLED);
		// set the window position
		UpdatePlacementInfoAccordingToConfig(placement_info);
		SetWindowPlacement(placement_info);
		// initialize ImGui
		if (!CreateImGuiContext())
			return false;
		// finalize the creation
		if (!Initialize())
			return false;
		// now that the window is fully placed ... we can show it
		if (create_params.start_visible)
			glfwShowWindow(glfw_window);

		return true;
	}

	bool Window::CreateImGuiContext()
	{
		ImGuiManager* imgui_manager = GetImGuiManager();

		// save imgui context
		ImGuiContext* previous_imgui_context = ImGui::GetCurrentContext();

		// create a new context for this window
		imgui_context = ImGui::CreateContext(imgui_manager->BuildAtlas());
		ImGui::SetCurrentContext(imgui_context);

		imgui_manager->InitializeWindowImGuiContext(this);

		// initialize the context
		//
		// note: we don't install callbacks. we will manually call the functions ourselves
		//       under normal circumstances, we have the following call chain
		//
		//       Window Message Handler (or NOT) -> GLFW call -> IMGUI callbacks -> our callbacks
		//                ^
		//                +-- settings the IMGUI context here is not enought
		//
		//       due to our multiple-window system, inside the IMGUI callbacks, the current IMGUI context may not be set causing a crash
		//       -> for example, push both LEFT and RIGHT shift and release one of theses keys
		//
		//       we can change the Window Message Function to ensure the IMG context is properly set, but some time the GLFW call is not made from inside the Window Message Handler
		//
		//       By not installing IMGUI callbacks we have the following chain
		//
		//       Window Message Handler or (NOT) -> GLFW call -> our callbacks -> set IMGUI context -> manually call IMGUI callbacks
		//
		//       see in ImGui_ImplGlfw_InstallCallbacks (IMGUI source code) which callback are concerned

		ImGui_ImplGlfw_InitForOpenGL(GetGLFWHandler(), false); // do not install callbacks !!
		ImGui_ImplOpenGL3_Init("#version 130");

		// substitute our own window proc (now that ImGui has inserted its own WindowProc */
#if _WIN32
		SetImGuiWindowProc(true);
#endif // #if _WIN32
		// the context is ready for rendering
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// create a new context for implot
		implot_context = ImPlot::CreateContext();

		// restore previous imgui context
		ImGui::SetCurrentContext(previous_imgui_context);

		return true;
	}

	bool Window::CreateRootWidget()
	{
		// create the root widget
		root_widget = new WindowRootWidget;
		if (root_widget != nullptr)
		{
			root_widget->window = this;
			return true;
		}
		return false;
	}

	void Window::SetCursorMode(CursorMode mode)
	{
		cursor_mode = mode;
		UpdateCursorMode();
	}

	void Window::UpdateCursorMode()
	{
		glfwSetInputMode(glfw_window, GLFW_CURSOR, (int)GetEffectiveCursorMode()); // do not effectively change cursor mode, during imgui_menu_enabled
	}

	CursorMode Window::GetCursorMode() const
	{
		return cursor_mode;
	}

	CursorMode Window::GetEffectiveCursorMode() const
	{
		if (WindowApplication::IsImGuiMenuEnabled())
			return CursorMode::NORMAL;
		return cursor_mode;
	}
	
	ImGuiManager* Window::GetImGuiManager() const
	{
		if (WindowApplication* window_application = Application::GetInstance())
			return window_application->GetImGuiManager();
		return nullptr;
	}

	void Window::OnImGuiMenuEnabledChanged(bool enabled)
	{
		UpdateCursorMode();
		GetImGuiManager()->OnWindowImGuiMenuEnabledChanged(this, enabled);
	}

	glm::ivec2 Window::GetWindowPosition(bool include_decorators) const
	{
		glm::ivec2 result = { 0, 0 };
		if (glfw_window != nullptr)
		{
			// get client top-left coner
			glfwGetWindowPos(glfw_window, &result.x, &result.y);

			// include decorators
			if (include_decorators)
			{
				WindowFrameSizeInfo framesize_info = GetWindowFrameSizeInfo(glfw_window);
				result.x -= framesize_info.left;
				result.y -= framesize_info.top;
			}
		}	
		return result;
	}

	glm::ivec2 Window::GetWindowSize(bool include_decorators) const
	{
		glm::ivec2 result = { 0, 0 };
		if (glfw_window != nullptr)
		{
			// get the client size
			glfwGetWindowSize(glfw_window, &result.x, &result.y);

			// include decorators
			if (include_decorators)
			{
				WindowFrameSizeInfo framesize_info = GetWindowFrameSizeInfo(glfw_window);
				result.x += framesize_info.left + framesize_info.right;
				result.y += framesize_info.top  + framesize_info.bottom;
			}
		}
		return result;
	}

	void Window::SetWindowPosition(glm::ivec2 position, bool include_decorators)
	{
		if (glfw_window == nullptr)
			return;

		if (include_decorators) // client area is displaced to the right and the bottom
		{
			WindowFrameSizeInfo framesize_info = GetWindowFrameSizeInfo(glfw_window);
			position.x += framesize_info.left;
			position.y += framesize_info.top;
		}
		glfwSetWindowPos(glfw_window, position.x, position.y); // glfwSetWindowPos(...) receives client information
	}

	void Window::SetWindowSize(glm::ivec2 size, bool include_decorators)
	{
		if (glfw_window == nullptr)
			return;
		if (include_decorators) // client aera is smaller than the given size
		{
			WindowFrameSizeInfo framesize_info = GetWindowFrameSizeInfo(glfw_window);
			size.x -= (framesize_info.left + framesize_info.right);
			size.y -= (framesize_info.top  + framesize_info.bottom);
		}
		glfwSetWindowSize(glfw_window, size.x, size.y); // glfwSetWindowSize(...) receives client information
	}

	GLFWwindow* Window::GetGLFWHandler() const
	{
		return glfw_window;
	}

	bool Window::ShouldClose()
	{
		return (glfw_window != nullptr && glfwWindowShouldClose(glfw_window) != 0);
	}

	void Window::TickRenderer(float real_delta_time)
	{
		assert(glfw_window == glfwGetCurrentContext());

		// cannot tick a non existing window
		if (glfw_window == nullptr || render_context == nullptr)
			return;

		// tick the render_context: it has metrics that rely on the real frame_rate (not modified one due to some of our tweaks)
		render_context->Tick(real_delta_time);
	}

	bool Window::IsMousePositionValid() const
	{
		return mouse_position.has_value();
	}

	glm::vec2 Window::GetMousePosition() const
	{
		if (!IsMousePositionValid())
			return glm::vec2(0.0f, 0.0f);
		return mouse_position.value();
	}

#if _WIN32

	LRESULT CALLBACK Window::ImGuiWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (Window* window = (Window*)GetPropW(hWnd, L"CHAOS_WINDOW"))
		{
			if (WNDPROC previous_wndproc = (WNDPROC)GetPropW(hWnd, L"CHAOS_PREVIOUS_WNDPROC"))
			{
				return window->PreventWindowDestruction([window, previous_wndproc, hWnd, msg, wParam, lParam]()
				{
					LRESULT result = 0;

					// set the ImGui context as current
					ImGuiContext* previous_imgui_context = ImGui::GetCurrentContext();
					ImGuiContext* toset_imgui_context = window->imgui_context;
					ImGui::SetCurrentContext(toset_imgui_context);

					// set the ImPlot context as current
					ImPlotContext* previous_implot_context = ImPlot::GetCurrentContext();
					ImPlotContext* toset_implot_context = window->implot_context;
					ImPlot::SetCurrentContext(toset_implot_context);

					// call special treatments
					window->HookedWindowProc(msg, wParam, lParam);
					// call "super" window proc
					result = ::CallWindowProc(previous_wndproc, hWnd, msg, wParam, lParam); // call "super" WndProc
					// restore the previous ImPlot context
					if (toset_implot_context != previous_implot_context) // maybe previous context was same then window's context and window's context has been deleted
						ImPlot::SetCurrentContext(previous_implot_context);
					// restore the previous ImGui context
					if (toset_imgui_context != previous_imgui_context) // maybe previous context was same then window's context and window's context has been deleted
						ImGui::SetCurrentContext(previous_imgui_context);

					return result;
				});
			}
		}
		return 0;
	}

	bool Window::HookedWindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_INPUTLANGCHANGE) // note: WM_INPUTLANGCHANGE is sent only to the topmost window of the EXE. That's exactly what we want
		{
			if (WindowApplication* window_application = Application::GetInstance())
				window_application->OnInputLanguageChanged(); // the application will be responsible to propage the change to all windows
			return true;
		}
		return false;
	}

	void Window::OnInputLanguageChanged()
	{
		if (window_client != nullptr)
			window_client->OnInputLanguageChanged();
	}

	// Note on ImGui and WndProc
	//
	//   first GLFW install its own WndProc to make a bind between WIN32API and GLFW
	//
	//   then ImGui install its own WndProc on top of that (see ImGui_ImplGlfw_WndProc)
	//
	//   The issue is that ImGui WndProc relies on the current ImGui context to dispatch events. The current ImGui context does not necessary correspond the the window whose message is being processed
	//
	//   That's why we install our own WndProc on top of all to get a ImGui from a HWND (and restore it to nullptr at the end)
	//
	//   Thoses layers of WndProc must be removed in reverse order
	//
	//   Our new WndProc does not use glfwMakeCurrentContext(...) to make current GLFW context active because:
	//   - this doesn't seem to be necessary
	//   - this function is much more costly than ImGui::SetCurrentContext(...)
	//   - There is a huge number of window events and we are only interrested to have a GLFW context in some glfw callback

	void Window::SetImGuiWindowProc(bool set_proc)
	{
		HWND hWnd = glfwGetWin32Window(GetGLFWHandler());
		// set the WndProc
		if (set_proc)
		{
			assert(GetPropW(hWnd, L"CHAOS_WINDOW") == nullptr);
			assert(GetPropW(hWnd, L"CHAOS_PREVIOUS_WNDPROC") == nullptr);
			SetPropW(hWnd, L"CHAOS_WINDOW", this);
			SetPropW(hWnd, L"CHAOS_PREVIOUS_WNDPROC", (WNDPROC)::GetWindowLongPtr(hWnd, GWLP_WNDPROC));
			::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&Window::ImGuiWindowProc);
		}
		// restore the WndProc
		else
		{
			assert(GetPropW(hWnd, L"CHAOS_WINDOW") != nullptr);
			assert(GetPropW(hWnd, L"CHAOS_PREVIOUS_WNDPROC") != nullptr);
			::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)GetPropW(hWnd, L"CHAOS_PREVIOUS_WNDPROC"));
			RemovePropW(hWnd, L"CHAOS_WINDOW");
			RemovePropW(hWnd, L"CHAOS_PREVIOUS_WNDPROC");
		}
	}

#endif // #if _WIN32

	void Window::SetGLFWCallbacks()
	{
		glfwSetWindowUserPointer(glfw_window, this);

		// note: glfwSetWindowRefreshCallback(..) is not called to set a refresh function. The draw function is manually called from main loop
		glfwSetCursorPosCallback(glfw_window, DoOnMouseMove);
		glfwSetMouseButtonCallback(glfw_window, DoOnMouseButton);
		glfwSetScrollCallback(glfw_window, DoOnMouseWheel);
		glfwSetWindowSizeCallback(glfw_window, DoOnWindowResize);
		glfwSetKeyCallback(glfw_window, DoOnKeyEvent);
		glfwSetCharCallback(glfw_window, DoOnCharEvent);
		glfwSetWindowCloseCallback(glfw_window, DoOnWindowClosed);
		glfwSetDropCallback(glfw_window, DoOnDropFile);
		glfwSetWindowFocusCallback(glfw_window, DoOnFocusStateChange);
		glfwSetWindowIconifyCallback(glfw_window, DoOnIconifiedStateChange);
	}

	static void GetWindowAndProcess(GLFWwindow* in_glfw_window, LightweightFunction<void(Window*)> func)
	{
		if (Window* my_window = (Window*)glfwGetWindowUserPointer(in_glfw_window))
		{
			my_window->WithWindowContext([my_window, &func]()
			{
				func(my_window);
			});
		}
	}

	void Window::DoOnIconifiedStateChange(GLFWwindow* in_glfw_window, int value)
	{
		GetWindowAndProcess(in_glfw_window, [value](Window * my_window)
		{
			my_window->OnIconifiedStateChange(value == GL_TRUE);
		});
	}

	void Window::DoOnFocusStateChange(GLFWwindow* in_glfw_window, int value)
	{
		GetWindowAndProcess(in_glfw_window, [=](Window * my_window)
		{
			// XXX: manually call IMGUI callbacks. Check for context because this could be called even before IMGUI is fully bound to the window
			if (ImGui::GetCurrentContext() != nullptr)
				ImGui_ImplGlfw_WindowFocusCallback(in_glfw_window, value);

			my_window->OnFocusStateChange(value == GL_TRUE);
		});
	}

	void Window::DoOnWindowClosed(GLFWwindow* in_glfw_window)
	{
		GetWindowAndProcess(in_glfw_window, [](Window* my_window)
		{
			if (my_window->OnWindowClosed())
				my_window->RequireWindowClosure();
		});
	}

	void Window::DoOnWindowResize(GLFWwindow* in_glfw_window, int width, int height)
	{
		GetWindowAndProcess(in_glfw_window, [width, height](Window* my_window)
		{
			my_window->OnWindowResize(glm::ivec2(width, height));
		});
	}

	void Window::DoOnMouseMove(GLFWwindow* in_glfw_window, double x, double y)
	{
		GetWindowAndProcess(in_glfw_window, [=](Window* my_window)
		{
			// XXX: manually call IMGUI callbacks. Check for context because this could be called even before IMGUI is fully bound to the window
			if (ImGui::GetCurrentContext() != nullptr && WindowApplication::IsImGuiMenuEnabled())
			{
				ImGui_ImplGlfw_CursorPosCallback(in_glfw_window, x, y);

				ImGuiIO& io = ImGui::GetIO();
				if (io.WantCaptureMouse)
					return;
			}
			// notify the application of the mouse state
			WindowApplication::SetApplicationInputMode(InputMode::MOUSE);

			glm::vec2 position = { float(x), float(y) };
			if (!my_window->IsMousePositionValid())
				my_window->OnMouseMove({ 0.0f, 0.0f });
			else
				my_window->OnMouseMove(position - my_window->mouse_position.value());
			my_window->mouse_position = position;
		});
	}

	void Window::DoOnMouseButton(GLFWwindow* in_glfw_window, int button, int action, int modifier)
	{
		GetWindowAndProcess(in_glfw_window, [=](Window* my_window)
		{
			// XXX: manually call IMGUI callbacks. Check for context because this could be called even before IMGUI is fully bound to the window
			if (ImGui::GetCurrentContext() != nullptr && WindowApplication::IsImGuiMenuEnabled())
			{
				ImGui_ImplGlfw_MouseButtonCallback(in_glfw_window, button, action, modifier);

				ImGuiIO& io = ImGui::GetIO();
				if (io.WantCaptureMouse)
					return;
			}

			MouseButton mouse_button = (MouseButton)button;
			KeyboardState::SetMouseButtonState(mouse_button, action);

			// notify the application of the mouse state
			WindowApplication::SetApplicationInputMode(InputMode::MOUSE);

			my_window->OnMouseButton(button, action, modifier);
		});
	}

	void Window::DoOnMouseWheel(GLFWwindow* in_glfw_window, double scroll_x, double scroll_y)
	{
		GetWindowAndProcess(in_glfw_window, [=](Window* my_window)
		{
			// XXX: manually call IMGUI callbacks. Check for context because this could be called even before IMGUI is fully bound to the window
			if (ImGui::GetCurrentContext() != nullptr && WindowApplication::IsImGuiMenuEnabled())
			{
				ImGui_ImplGlfw_ScrollCallback(in_glfw_window, scroll_x, scroll_y);

				ImGuiIO& io = ImGui::GetIO();
				if (io.WantCaptureMouse)
					return;
			}
			// notify the application of the mouse state
			WindowApplication::SetApplicationInputMode(InputMode::MOUSE);

			my_window->OnMouseWheel(scroll_x, scroll_y);
		});
	}

	void Window::DoOnKeyEvent(GLFWwindow* in_glfw_window, int keycode, int scancode, int action, int modifier)
	{
		GetWindowAndProcess(in_glfw_window, [=](Window* my_window)
		{
			// XXX: manually call IMGUI callbacks. Check for context because this could be called even before IMGUI is fully bound to the window
			if (ImGui::GetCurrentContext() != nullptr && WindowApplication::IsImGuiMenuEnabled())
			{
				ImGui_ImplGlfw_KeyCallback(in_glfw_window, keycode, scancode, action, modifier);

				ImGuiIO& io = ImGui::GetIO();
				if (io.WantCaptureKeyboard)
					return;
			}

			// GLFW keycode corresponds to the character that would be produced on a QWERTY layout
			// we have to make a conversion to know the character is to be produced on CURRENT layout

			KeyboardButton keyboard_button = KeyboardButton(KeyboardLayoutConversion::ConvertGLFWKeycode(keycode, KeyboardLayoutType::QWERTY, KeyboardLayoutType::CURRENT));
			KeyboardState::SetKeyboardButtonState(keyboard_button, action);

			// notify the application of the keyboard button state
			WindowApplication::SetApplicationInputMode(InputMode::KEYBOARD);

			// handle the message
			KeyEvent event;
			event.button = keyboard_button;
			event.scancode = scancode;
			event.action = action;
			event.modifier = modifier;

			my_window->OnKeyEvent(event);
		});
	}

	void Window::DoOnCharEvent(GLFWwindow* in_glfw_window, unsigned int c)
	{
		GetWindowAndProcess(in_glfw_window, [=](Window* my_window)
		{
			// XXX: manually call IMGUI callbacks. Check for context because this could be called even before IMGUI is fully bound to the window
			if (ImGui::GetCurrentContext() != nullptr && WindowApplication::IsImGuiMenuEnabled())
			{
				ImGui_ImplGlfw_CharCallback(in_glfw_window, c);

				ImGuiIO& io = ImGui::GetIO();
				if (io.WantCaptureKeyboard)
					return;
			}
			// notify the application of the keyboard button state
			WindowApplication::SetApplicationInputMode(InputMode::KEYBOARD);

			my_window->OnCharEvent(c);
		});
	}

	void Window::DoOnDropFile(GLFWwindow* in_glfw_window, int count, char const** paths)
	{
		GetWindowAndProcess(in_glfw_window, [count, paths](Window* my_window)
		{
			my_window->OnDropFile(count, paths);
		});
	}



	bool Window::GetProgramProviderAndProcess(LightweightFunction<bool(GPUProgramProviderBase*)> func)
	{
		GPUProgramProviderCommonTransforms common_transforms; // some common deduction rules

		WindowApplication* window_application = Application::GetInstance(); // application is a provider_interface

		GPUProgramProviderChain provider(this, window_application, common_transforms); // order: window, application, common deduction rules

		return func(&provider);
	}

	void Window::DrawWindow()
	{
		if (glfw_window != nullptr && render_context != nullptr)
		{
			assert(glfw_window == glfwGetCurrentContext());

			render_context->RenderFrame([this]()
			{
				GetProgramProviderAndProcess([this](GPUProgramProviderBase* provider)
				{
					return DrawInternal(provider);
				});

				if (double_buffer)
					glfwSwapBuffers(glfw_window);
				else
					glFlush();

				return true;
			});
		}
	}

	void Window::RequireWindowClosure()
	{
		glfwSetWindowShouldClose(glfw_window, 1);
	}

	GLFWmonitor* Window::GetPreferredMonitor() const
	{
		glm::ivec2 top_left_corner = GetWindowPosition(true); // include decorators
		return MonitorTools::GetNearestMonitor(top_left_corner);
	}

	void Window::SetFullscreen(GLFWmonitor* monitor)
	{
		if (glfw_window == nullptr)
			return;

		WindowPlacementInfo placement_info;

		if (monitor == nullptr) // want to "unfull screen"
		{
			if (fullscreen_monitor != nullptr) // is currently fullscreen
			{
				if (non_fullscreen_data.has_value())
				{
					placement_info.position   = non_fullscreen_data->position; // absolute placement of the window (monitor not defined)
					placement_info.size       = non_fullscreen_data->size;
					placement_info.fullscreen = false;
					SetWindowPlacement(placement_info);
				}
				else
				{
					if (GLFWmonitor* preferred_monitor = GetPreferredMonitor())
					{
						if (GLFWvidmode const* mode = glfwGetVideoMode(preferred_monitor))
						{
							glm::ivec2 monitor_size = { mode->width, mode->height}; // half monitor size

							placement_info.monitor    = preferred_monitor;
							placement_info.size       = monitor_size / 2;
							placement_info.position   = (monitor_size - placement_info.size.value()) / 2; // relative placement to given monitor
							placement_info.fullscreen = false;
							SetWindowPlacement(placement_info);
						}
					}
				}
			}
		}
		else // monitor not null
		{
			if (fullscreen_monitor != monitor) // is not currently fullscreen or for another monitor
			{
				// store current information (if not already fullscreen)
				if (fullscreen_monitor == nullptr)
				{
					NonFullScreenWindowData data;
					data.position = GetWindowPosition(true); // include decorators
					data.size     = GetWindowSize(true);
					non_fullscreen_data = data;
				}
				placement_info.monitor = monitor; // fullscreen placement
				placement_info.fullscreen = true;
				SetWindowPlacement(placement_info);
			}
		}
	}

	void Window::ToggleFullscreen()
	{
		if (fullscreen_monitor != nullptr)
			SetFullscreen(nullptr);
		else if (GLFWmonitor* preferred_monitor = GetPreferredMonitor())
			SetFullscreen(preferred_monitor);
	}

	bool Window::ScreenCapture()
	{
		// get render_context
		if (glfw_window == nullptr || render_context == nullptr)
			return false;

		return WithWindowContext([this]()
		{
			// this call may take a while causing a jump in 'delta_time'
			WindowApplication* window_application = Application::GetInstance();
			if (window_application == nullptr)
				return false;
			window_application->FreezeNextFrameTickDuration();

			// compute rendering size
			glm::ivec2 window_size = GetWindowSize(false); // only interrested in client area

			// generate a framebuffer
			GPUFramebufferGenerator framebuffer_generator(GetGPURenderContext());
			framebuffer_generator.AddColorAttachment(0, PixelFormat::BGRA, window_size, "scene");
			framebuffer_generator.AddDepthStencilAttachment(window_size, "depth");

			shared_ptr<GPUFramebuffer> framebuffer = framebuffer_generator.GenerateFramebuffer(window_size);
			if (framebuffer == nullptr)
				return false;
			if (!framebuffer->CheckCompletionStatus())
				return false;

			// render into the framebuffer
			render_context->RenderIntoFramebuffer(framebuffer.get(), false, [this]()
			{
				GetProgramProviderAndProcess([this](GPUProgramProviderBase* provider)
				{
					DrawInternal(provider);
					return true;
				});
				return true;
			});

			// extract the texture
			GPUFramebufferAttachmentInfo const* attachment = framebuffer->GetColorAttachment(0);
			if (attachment == nullptr || attachment->texture == nullptr)
				return false;

			bitmap_ptr img = bitmap_ptr(ImageTools::GenFreeImage(attachment->texture->GetResourceID(), 0));
			if (img == nullptr)
				return false;

			// create the directory
			boost::filesystem::path capture_directory_path = window_application->GetUserLocalTempPath() / "Captures";
			if (!boost::filesystem::is_directory(capture_directory_path))
				if (!boost::filesystem::create_directories(capture_directory_path))
					return false;

			// save the image
			std::string format = StringTools::Printf(
				"capture_%s_%%d.png",
				StringTools::TimeToString(std::chrono::system_clock::now(), TimeToStringFormatType::FILENAME).c_str());

			boost::filesystem::path file_path = FileTools::GetUniquePath(capture_directory_path, format.c_str(), true);
			if (file_path.empty())
				return false;

			bitmap_ptr img_without_alpha = bitmap_ptr(FreeImage_ConvertTo24Bits(img.get())); // suppress alpha channel before saving

			return (FreeImage_Save(FIF_PNG, img_without_alpha.get(), file_path.string().c_str(), 0) != 0);
		});
	}

	aabox2 Window::GetRequiredViewport(glm::ivec2 const& size) const
	{
		aabox2 result;
		result.position = { 0, 0 };
		result.size = size;
		return SetBoxAspect(result, 16.0f / 9.0f, SetBoxAspectMethod::SHRINK_BOX);
	}

	CHAOS_HELP_TEXT(SHORTCUTS, "F9  : ScreenCapture");
	CHAOS_HELP_TEXT(SHORTCUTS, "F10 : ToggleFullscreen");

	bool Window::OnKeyEventImpl(KeyEvent const& key_event)
	{
		// screen capture
		// CMD F9 : ScreenCapture(...)
		if (key_event.IsKeyPressed(KeyboardButton::F9))
		{
			ScreenCapture();
			return true;
		}
		// try to go fullscreen
		// CMD F10 : ToggleFullscreen(...)
		if (key_event.IsKeyPressed(KeyboardButton::F10))
		{
			ToggleFullscreen();
			return true;
		}
		// give opportunity to application
		if (WindowApplication* window_application = Application::GetInstance())
			if (window_application->OnKeyEvent(key_event))
				return true;
		// then client
		if (window_client != nullptr)
			if (window_client->OnKeyEvent(key_event))
				return true;
		// then fallback to super
		return WindowInterface::OnKeyEventImpl(key_event);
	}

	bool Window::OnMouseMoveImpl(glm::vec2 const& delta)
	{
		// give opportunity to application
		if (WindowApplication* window_application = Application::GetInstance())
			if (window_application->OnMouseMove(delta))
				return true;
		// then client
		if (window_client != nullptr)
			if (window_client->OnMouseMove(delta))
				return true;
		// then fallback to super
		return WindowInterface::OnMouseMoveImpl(delta);
	}

	bool Window::OnMouseButtonImpl(int button, int action, int modifier)
	{
		// give opportunity to application
		if (WindowApplication* window_application = Application::GetInstance())
			if (window_application->OnMouseButton(button, action, modifier))
				return true;
		// then client
		if (window_client != nullptr)
			if (window_client->OnMouseButton(button, action, modifier))
				return true;
		// then fallback to super
		return WindowInterface::OnMouseButtonImpl(button, action, modifier);
	}

	bool Window::OnMouseWheelImpl(double scroll_x, double scroll_y)
	{
		// give opportunity to application
		if (WindowApplication* window_application = Application::GetInstance())
			if (window_application->OnMouseWheel(scroll_x, scroll_y))
				return true;
		// then client
		if (window_client != nullptr)
			if (window_client->OnMouseWheel(scroll_x, scroll_y))
				return true;
		// then fallback to super
		return WindowInterface::OnMouseWheelImpl(scroll_x, scroll_y);
	}

	bool Window::OnCharEventImpl(unsigned int c)
	{
		// give opportunity to application
		if (WindowApplication* window_application = Application::GetInstance())
			if (window_application->OnCharEvent(c))
				return true;
		// then client
		if (window_client != nullptr)
			if (window_client->OnCharEvent(c))
				return true;
		// then fallback to super
		return WindowInterface::OnCharEventImpl(c);
	}

	bool Window::IsApplicationImGuiMenuPluggedIn() const
	{
		return application_imgui_menu_plugged_in;
	}

	void Window::SetApplicationImGuiMenuPluggedIn(bool enabled)
	{
		application_imgui_menu_plugged_in = enabled;
	}

	void Window::DrawImGui()
	{
		// draw the content
		OnDrawImGuiContent();
		// draw the menu
		if (WindowApplication::IsImGuiMenuEnabled())
			OnDrawImGuiMenu(ImGuiTools::BeginMainMenuBar());
		// draw the ImGui objects
		DrawImGuiObjects();
	}

	void Window::OnDrawImGuiContent()
	{
	}

	void Window::OnDrawImGuiMenu(BeginImGuiMenuFunc begin_menu_func)
	{
		// display application menu items
		if (IsApplicationImGuiMenuPluggedIn())
			if (WindowApplication* window_application = Application::GetInstance())
				window_application->OnDrawApplicationImGuiMenu(this, begin_menu_func);

		// display menu items for widgets
		EnumerateKnownImGuiObjects([this, &begin_menu_func](char const* name, CreateImGuiObjectFunc create_func)
		{
			begin_menu_func([this, &create_func, name]()
			{
				if (ImGui::BeginMenu("Widgets"))
				{
					bool imgui_object_exists = (FindImGuiObject(name) != nullptr);
					if (ImGui::MenuItem(name, nullptr, imgui_object_exists, true))
						SetImGuiObjectInternalVisibility(!imgui_object_exists, name, create_func);
					ImGui::EndMenu();
				}
			});
			return false; // don't stop the loop
		});
	}

	void Window::DrawImGuiObjects()
	{
		// create a weak copy of imgui objects to avoid 
		// bugs if some elements are destroyed during the loop
		std::vector<weak_ptr<ImGuiObject>> weak_imgui_objects;
		weak_imgui_objects.reserve(imgui_objects.size());
		for (shared_ptr<ImGuiObject> const& imgui_object : imgui_objects)
			weak_imgui_objects.emplace_back(imgui_object.get());

		// draw all imgui objects
		for (weak_ptr<ImGuiObject> imgui_object : weak_imgui_objects)
		{
			if (imgui_object != nullptr)
			{
				shared_ptr<ImGuiObject> prevent_destruction = imgui_object;
				imgui_object->DrawImGui(this);
				if (imgui_object->IsClosingRequested()) // closing requested ?
					RemoveImGuiObject(imgui_object.get());
			}
		}
	}

	bool Window::DrawInternal(GPUProgramProviderInterface const* uniform_provider)
	{
		glm::ivec2 window_size = GetWindowSize(false); // only interrested in client area
		if (window_size.x <= 0 || window_size.y <= 0) // some crash to expect in drawing elsewhere
			return false;

		// some parameters
		WindowDrawParams draw_params;
		draw_params.viewport = GetRequiredViewport(window_size);

		// draw the viewport
		if (!IsGeometryEmpty(draw_params.viewport))
		{
			GLTools::SetViewport(draw_params.viewport);
			OnDraw(render_context.get(), uniform_provider, draw_params);
		}
		// draw the root widget
		if (root_widget != nullptr)
		{
			if (root_widget->IsUpdatePlacementHierarchyRequired())
				UpdateWidgetPlacementHierarchy();
			root_widget->OnDraw(render_context.get(), uniform_provider, draw_params);
		}
		// draw ImGui
		DrawImGui();

		// finalize the rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// prepare rendering until we come back into this function
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		return true;
	}

	void Window::UpdateWidgetPlacementHierarchy()
	{
		if (root_widget != nullptr)
		{
			aabox2 placement;
			placement.position = { 0, 0 };
			placement.size = GetWindowSize(false); // only interrested in client area
			root_widget->SetPlacement(placement);
		}
	}

	bool Window::OnDraw(GPURenderContext* render_context, GPUProgramProviderInterface const* uniform_provider, WindowDrawParams const& draw_params)
	{
		if (window_client != nullptr)
		{
			window_client->OnDraw(render_context, uniform_provider, draw_params);
		}
		else // fallback
		{
			glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
			glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);
		}

		return true;
	}

	bool Window::DoTick(float delta_time)
	{
		bool result = TickableInterface::DoTick(delta_time);
		if (root_widget != nullptr)
		{
			if (root_widget->IsUpdatePlacementHierarchyRequired())
				UpdateWidgetPlacementHierarchy();
			result |= root_widget->Tick(delta_time);
		}

		if (window_client != nullptr)
			window_client->Tick(delta_time);

		return result;
	}

	bool Window::Initialize()
	{
		if (!ReadConfigurableProperties(ReadConfigurablePropertiesContext::Initialization, false))
			return false;
		return OnInitialize(GetJSONReadConfiguration());
	}

	bool Window::OnInitialize(JSONReadConfiguration config)
	{
		// create the root widget
		if (!CreateRootWidget())
			return false;
		return true;
	}

	bool Window::OnStorePersistentProperties(JSONWriteConfiguration config) const
	{
		if (glfw_window != nullptr)
		{
			if (GLFWmonitor* preferred_monitor = GetPreferredMonitor())
			{
				// gather information
				glm::ivec2 window_position = GetWindowPosition(true); // include decorators
				window_position = MonitorTools::AbsolutePositionToMonitor(window_position, preferred_monitor); // relative to preferred monitor

				glm::ivec2 window_size = GetWindowSize(true); // include decorators

				int monitor_index = MonitorTools::GetMonitorIndex(preferred_monitor);
				// store information
				JSONTools::SetAttribute(config, "position", window_position);
				JSONTools::SetAttribute(config, "size", window_size);
				JSONTools::SetAttribute(config, "monitor_index", monitor_index);
				JSONTools::SetAttribute(config, "fullscreen", IsFullscreen());
			}
		}
		return true;
	}

	bool Window::DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const
	{
		return false;
	}

	void Window::OnWindowResize(glm::ivec2 size)
	{
		UpdateWidgetPlacementHierarchy();

		if (window_client != nullptr)
			window_client->OnWindowResize(size);
	}

	bool Window::OnWindowClosed()
	{
		if (window_client != nullptr)
			return window_client->OnWindowClosed();
		return true;
	}

	void Window::OnDropFile(int count, char const** paths)
	{
		if (window_client != nullptr)
			window_client->OnDropFile(count, paths);
	}

	void Window::OnIconifiedStateChange(bool iconified)
	{
		if (window_client != nullptr)
			window_client->OnIconifiedStateChange(iconified);
	}

	void Window::OnFocusStateChange(bool gain_focus)
	{
		if (window_client != nullptr)
			window_client->OnFocusStateChange(gain_focus);
	}

	void Window::OnMonitorEvent(GLFWmonitor* monitor, int monitor_state)
	{
		if (monitor == fullscreen_monitor && monitor_state == GLFW_DISCONNECTED)
			fullscreen_monitor = nullptr;
		if (window_client != nullptr)
			window_client->OnMonitorEvent(monitor, monitor_state);
	}

	void Window::Finalize()
	{
	}

	AutoCastable<ImGuiObject> Window::FindImGuiObject(ObjectRequest request)
	{
		return request.FindObject(imgui_objects);
	}

	AutoConstCastable<ImGuiObject> Window::FindImGuiObject(ObjectRequest request) const
	{
		return request.FindObject(imgui_objects);
	}

	bool Window::IsKnownImGuiObjectVisible(char const* name) const
	{
		return (FindImGuiObject(name) != nullptr);
	}

	bool Window::SetKnownImGuiObjectVisibility(char const* name, bool visible)
	{
		return EnumerateKnownImGuiObjects([this, name, visible](char const* imgui_object_name, CreateImGuiObjectFunc create_func)
		{
			// is it the window we are searching?
			if (StringTools::Stricmp(name, imgui_object_name) != 0)
				return false;
			SetImGuiObjectInternalVisibility(visible, imgui_object_name, create_func);
			return true; // stop the search
		});
	}

	bool Window::EnumerateKnownImGuiObjects(EnumerateKnownImGuiObjectFunc func) const
	{
		if (func("FPS", ImGuiRenderingFPSStatObject::GetStaticClass()))
			return true;

		if (func("Draw calls", ImGuiRenderingDrawCallsStatObject::GetStaticClass()))
			return true;

		if (func("Vertices", ImGuiRenderingVerticesStatObject::GetStaticClass()))
			return true;

		if (func("System Information", ImGuiSystemInformationObject::GetStaticClass()))
			return true;

		if (func("Window Information", ImGuiWindowInformationObject::GetStaticClass()))
			return true;

		if (func("Help", ImGuiHelpObject::GetStaticClass()))
			return true;

		return false;
	}

	void Window::SetImGuiObjectInternalVisibility(bool visible, char const* name, CreateImGuiObjectFunc create_func)
	{
		ImGuiObject* existing_imgui_object = FindImGuiObject(name);

		if (visible)
		{
			if (existing_imgui_object == nullptr)
			{
				if (ImGuiObject* imgui_object = create_func())
				{
					imgui_object->SetName(name);
					imgui_object->SetImGuiObjectFlags(ImGuiObjectFlags::FloatingWindow | ImGuiObjectFlags::CloseWithCross);
					AddImGuiObject(imgui_object);
				}
			}
		}
		else
		{
			if (existing_imgui_object != nullptr)
				RemoveImGuiObject(existing_imgui_object);
		}
	}

	void Window::AddImGuiObject(ImGuiObject* imgui_object)
	{
		assert(imgui_object != nullptr);
		assert(FindImGuiObject(imgui_object->GetName()) == nullptr);

#if _DEBUG
		// check whether the object is not already in list here
		auto it = std::ranges::find_if(imgui_objects, [imgui_object](shared_ptr<ImGuiObject> const& ptr)
		{
			return (ptr == imgui_object);
		});
		assert(it == imgui_objects.end());
#endif
		assert(!StringTools::IsEmpty(imgui_object->GetName())); // window mush have a name !
		imgui_objects.push_back(imgui_object);
		imgui_object->OnAddedToWindow(this);
	}

	void Window::RemoveImGuiObject(ImGuiObject* imgui_object)
	{
		assert(imgui_object != nullptr);
	
		auto it = std::ranges::find_if(imgui_objects, [imgui_object](shared_ptr<ImGuiObject> const& ptr)
		{
			return (ptr == imgui_object);
		});

		if (it != imgui_objects.end())
		{
			shared_ptr<ImGuiObject> keep_alive = imgui_object;
			imgui_objects.erase(it);
			keep_alive->OnRemovedFromWindow(this);
		}
	}

	void Window::SetWindowClient(WindowClient* in_client)
	{
		if (window_client == in_client)
			return;
		if (window_client != nullptr)
		{
			window_client->window = nullptr;
			window_client->OnUnpluggedFromWindow(this);
		}
		window_client = in_client;
		if (window_client != nullptr)
		{
			window_client->window = this;
			window_client->OnPluggedIntoWindow(this);
		}
	}
	
	bool Window::IsWindowHandledByApplication() const
	{
		if (WindowApplication* window_application = Application::GetInstance())
		{
			auto it = std::ranges::find_if(window_application->windows, [this](shared_ptr<Window> const& w)
			{
				return (w == this);
			});
			return (it != window_application->windows.end());
		}
		return false;
	}

	GPUDevice* Window::GetGPUDevice() const
	{
		return WindowApplication::GetGPUDeviceInstance();
	}

}; // namespace chaos
