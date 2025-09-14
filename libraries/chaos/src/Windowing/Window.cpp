#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace GlobalVariables
	{
		CHAOS_GLOBAL_VARIABLE(bool, UnlimitedFPS, false);
	};

	Window::Window():
		window_imgui_context(this)
	{
	}

	Window::~Window()
	{
		assert(glfw_window == nullptr);
	}

	void Window::Destroy()
	{
		if (WindowApplication* window_application = Application::GetInstance())
			window_application->RemoveWindowFromHandling(this);
	}

	void Window::CompleteWindowDestruction() // window is not more handled by WindowApplication
	{
		assert(!IsWindowHandledByApplication());

		StorePersistentProperties(true);
		Finalize();
		UpdateWindowProc(false);
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

	bool Window::CreateImGuiContext()
	{
		return window_imgui_context.CreateContext(GetImGuiManager());
	}
		
	void Window::DestroyImGuiContext()
	{
		window_imgui_context.DestroyContext(GetImGuiManager());
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
		glfwWindowHint(GLFW_RESIZABLE, (create_params.resizable)? 1 : 0);
		glfwWindowHint(GLFW_VISIBLE, (create_params.start_visible)? 1 : 0);
		glfwWindowHint(GLFW_DECORATED, (create_params.decorated)? 1 : 0);
		glfwWindowHint(GLFW_FLOATING, (create_params.toplevel)? 1 : 0);
		glfwWindowHint(GLFW_FOCUSED, (create_params.focused)? 1 : 0);
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
		UpdateWindowProc(true);
		// start rendering into ImGui
		StartImGuiNewFrame();
		// finalize the creation
		if (!Initialize())
			return false;
		// now that the window is fully placed ... we can show it
		if (create_params.start_visible)
			glfwShowWindow(glfw_window);

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
		window_imgui_context.OnImGuiMenuEnabledChanged(enabled);
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

	// Note on ImGui and WndProc
	//
	//   first GLFW install its own WndProc to make a bind between WIN32API and GLFW
	//
	//   then ImGui install its own WndProc on top of that (see ImGui_ImplGlfw_WndProc)
	//
	//   The issue is that ImGui WndProc relies on the current ImGui context to dispatch events.
	//   The current ImGui context does not necessary correspond the the window whose message is being processed
	//
	//   That's why we install our own WndProc on top of all to get a ImGui from a HWND (and restore it to nullptr at the end)
	//
	//   Thoses layers of WndProc must be removed in reverse order
	//
	//   Our new WndProc does not use glfwMakeCurrentContext(...) to make current GLFW context active because:
	//   - this doesn't seem to be necessary
	//   - this function is much more costly than ImGui::SetCurrentContext(...)
	//   - There is a huge number of window events and we are only interrested to have a GLFW context in some glfw callback

	void Window::UpdateWindowProc(bool set_proc)
	{
		HWND hWnd = glfwGetWin32Window(GetGLFWHandler());

		if (set_proc) // set the WndProc
		{
			assert(GetPropW(hWnd, L"CHAOS_WINDOW") == nullptr);
			assert(GetPropW(hWnd, L"CHAOS_PREVIOUS_WNDPROC") == nullptr);
			SetPropW(hWnd, L"CHAOS_WINDOW", this);
			SetPropW(hWnd, L"CHAOS_PREVIOUS_WNDPROC", (WNDPROC)::GetWindowLongPtr(hWnd, GWLP_WNDPROC));
			::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&Window::InternalWindowProc);
		}		
		else // restore the WndProc
		{
			assert(GetPropW(hWnd, L"CHAOS_WINDOW") != nullptr);
			assert(GetPropW(hWnd, L"CHAOS_PREVIOUS_WNDPROC") != nullptr);
			::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)GetPropW(hWnd, L"CHAOS_PREVIOUS_WNDPROC"));
			RemovePropW(hWnd, L"CHAOS_WINDOW");
			RemovePropW(hWnd, L"CHAOS_PREVIOUS_WNDPROC");
		}
	}

	LRESULT CALLBACK Window::InternalWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (Window* window = (Window*)GetPropW(hWnd, L"CHAOS_WINDOW"))
		{
			if (WNDPROC previous_wndproc = (WNDPROC)GetPropW(hWnd, L"CHAOS_PREVIOUS_WNDPROC"))
			{
				return window->PreventWindowDestruction([window, previous_wndproc, hWnd, msg, wParam, lParam]()
				{
					return window->window_imgui_context.WithImGuiContext([&]()
					{
						// call special treatments
						window->HookedWindowProc(msg, wParam, lParam);
						// call "super" window proc
						return CallWindowProc(previous_wndproc, hWnd, msg, wParam, lParam); // call "super" WndProc
					});
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

	Window * Window::GetWindowFromGLFWContext(GLFWwindow* in_glfw_window)
	{
		return (Window*)glfwGetWindowUserPointer(in_glfw_window);
	}

	void Window::DoOnIconifiedStateChange(GLFWwindow* in_glfw_window, int value)
	{
		GetWindowAndProcessWithContext(in_glfw_window, [value](Window * my_window)
		{
			my_window->OnIconifiedStateChange(value == GL_TRUE);
		});
	}

	void Window::DoOnFocusStateChange(GLFWwindow* in_glfw_window, int value)
	{
		GetWindowAndProcessWithContext(in_glfw_window, [=](Window * my_window)
		{
			if (my_window->window_imgui_context.IsImGuiContextCurrent())
				ImGui_ImplGlfw_WindowFocusCallback(in_glfw_window, value);

			my_window->OnFocusStateChange(value == GL_TRUE);
		});
	}

	void Window::DoOnWindowClosed(GLFWwindow* in_glfw_window)
	{
		GetWindowAndProcessWithContext(in_glfw_window, [](Window* my_window)
		{
			if (my_window->OnWindowClosed())
				my_window->RequireWindowClosure();
		});
	}

	void Window::DoOnWindowResize(GLFWwindow* in_glfw_window, int width, int height)
	{
		GetWindowAndProcessWithContext(in_glfw_window, [width, height](Window* my_window)
		{
			my_window->OnWindowResize(glm::ivec2(width, height));
		});
	}

	bool Window::TraverseInputEventReceiverHierarchy(TraverseInputEventReceiverHierarchyFunction event_func)
	{
		// try window client
		if (window_client != nullptr)
			if (window_client->TraverseInputEventReceiverHierarchy(event_func))
				return true;
		// try super call
		return WindowInterface::TraverseInputEventReceiverHierarchy(event_func);
	}

	bool Window::DoDispatchInputEventWithContext(Window * in_window, TraverseInputEventReceiverHierarchyFunction event_func)
	{
		// try imgui context
		if (in_window->window_imgui_context.TraverseInputEventReceiverHierarchy(event_func))
			return true;
		// try window
		if (in_window->TraverseInputEventReceiverHierarchy(event_func))
			return true;
		// try application
		if (WindowApplication* window_application = Application::GetInstance())
			if (window_application->TraverseInputEventReceiverHierarchy(event_func))
				return true;
		return false;
	}

	void Window::DoOnMouseMove(GLFWwindow* in_glfw_window, double x, double y)
	{
		// notify the application of the mouse state
		WindowApplication::SetApplicationInputMode(InputMode::MOUSE);

		// dispatch the event (a window is mandatory to have a delta)
		if (Window * my_window = GetWindowFromGLFWContext(in_glfw_window))
		{
			glm::vec2 position = { float(x), float(y) };

			glm::vec2 delta = my_window->IsMousePositionValid()?
				position - my_window->mouse_position.value():
				glm::vec2(0.0f, 0.0f);

			GetWindowAndDispatchInputEventWithContext(in_glfw_window, &InputEventReceiverInterface::OnMouseMove, delta);

			my_window->mouse_position = position;
		}
	}

	static KeyAction GetKeyActionFromGLFW(int action)
	{
		if (action == GLFW_PRESS)
			return KeyAction::Press;
		if (action == GLFW_REPEAT)
			return KeyAction::Repeat;

		return KeyAction::Release; // = 0
	}

	static KeyModifier GetKeyModifiersFromGLFW(int modifier)
	{
		KeyModifier result = KeyModifier::None;

		if ((modifier & GLFW_MOD_SHIFT) != 0)
			result |= KeyModifier::Shift;
		if ((modifier & GLFW_MOD_CONTROL) != 0)
			result |= KeyModifier::Control;
		if ((modifier & GLFW_MOD_ALT) != 0)
			result |= KeyModifier::Alt;

		return result;
	}

	void Window::DoOnMouseButton(GLFWwindow* in_glfw_window, int button, int action, int modifiers)
	{
		// update global state
		MouseButton mouse_button = (MouseButton)button;

		if (KeyboardAndMouseState * keyboard_and_mouse_state = KeyboardAndMouseState::GetInstance())
		{
			bool key_value = (action == GLFW_PRESS || action == GLFW_REPEAT);
			keyboard_and_mouse_state->SetKeyValue(mouse_button, key_value);
		}

		// notify the application of the mouse state
		WindowApplication::SetApplicationInputMode(InputMode::MOUSE);

		// dispatch event
		MouseButtonEvent mouse_button_event;
		mouse_button_event.key = MouseButton(button);
		mouse_button_event.action = GetKeyActionFromGLFW(action);
		mouse_button_event.modifiers = GetKeyModifiersFromGLFW(modifiers);

		GetWindowAndDispatchInputEventWithContext(in_glfw_window, &InputEventReceiverInterface::OnMouseButton, mouse_button_event);
	}

	void Window::DoOnMouseWheel(GLFWwindow* in_glfw_window, double scroll_x, double scroll_y)
	{
		// notify the application of the mouse state
		WindowApplication::SetApplicationInputMode(InputMode::MOUSE);

		// dispatch event
		GetWindowAndDispatchInputEventWithContext(in_glfw_window, &InputEventReceiverInterface::OnMouseWheel, scroll_x, scroll_y);
	}

	void Window::DoOnKeyEvent(GLFWwindow* in_glfw_window, int keycode, int scancode, int action, int modifiers)
	{
		// GLFW keycode corresponds to the character that would be produced on a QWERTY layout
		// we have to make a conversion to know the character is to be produced on CURRENT layout
		keycode = KeyboardLayoutConversion::ConvertGLFWKeycode(keycode, KeyboardLayoutType::QWERTY, KeyboardLayoutType::CURRENT);

		// update global keyboard state
		KeyboardButton keyboard_button = KeyboardButton(keycode);
		if (KeyboardAndMouseState * keyboard_and_mouse_state = KeyboardAndMouseState::GetInstance())
		{
			bool key_value = (action == GLFW_PRESS || action == GLFW_REPEAT);
			keyboard_and_mouse_state->SetKeyValue(keyboard_button, key_value);
		}

		// notify the application of the keyboard state
		WindowApplication::SetApplicationInputMode(InputMode::KEYBOARD);

		// dispatch the event
		KeyEvent key_event;
		key_event.key = keyboard_button;
		key_event.scancode = scancode;
		key_event.action = GetKeyActionFromGLFW(action);
		key_event.modifiers = GetKeyModifiersFromGLFW(modifiers);

		GetWindowAndDispatchInputEventWithContext(in_glfw_window, &InputEventReceiverInterface::OnKeyEvent, key_event);
	}

	void Window::DoOnCharEvent(GLFWwindow* in_glfw_window, unsigned int c)
	{
		// notify the application of the keyboard button state
		WindowApplication::SetApplicationInputMode(InputMode::KEYBOARD);

		// dispatch the event
		GetWindowAndDispatchInputEventWithContext(in_glfw_window, &InputEventReceiverInterface::OnCharEvent, c);
	}

	void Window::DoOnDropFile(GLFWwindow* in_glfw_window, int count, char const** paths)
	{
		GetWindowAndProcessWithContext(in_glfw_window, [count, paths](Window* my_window)
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

	bool Window::EnumerateKeyActions(KeyActionEnumerator & in_action_enumerator)
	{
		if (in_action_enumerator(RequestKeyPressed(KeyboardButton::F9) , "Screen Capture", [this]()
		{
			ScreenCapture();
		}))
		{
			return true;
		}

		if (in_action_enumerator(RequestKeyPressed(KeyboardButton::F10) , "Toggle Fullscreen", [this]()
		{
			ToggleFullscreen();
		}))
		{
			return true;
		}

		return WindowInterface::EnumerateKeyActions(in_action_enumerator);
	}

	bool Window::IsStandardImGuiMenuPluggedIn() const
	{
		return standard_imgui_menu_plugged_in;
	}

	void Window::SetStandardImGuiMenuPluggedIn(bool enabled)
	{
		standard_imgui_menu_plugged_in = enabled;
	}

	void Window::DrawImGui()
	{
		// draw the content
		OnDrawImGuiContent();
		// draw the menu
		if (WindowApplication::IsImGuiMenuEnabled())
			OnDrawImGuiMenu(ImGuiTools::BeginMainMenuBar());
		// draw the ImGui objects
		OnDrawImGuiObjectOwnerContent();
	}

	void Window::OnDrawImGuiContent()
	{
	}

	void Window::OnDrawImGuiMenu(BeginImGuiMenuFunc begin_menu_func)
	{
		if (IsStandardImGuiMenuPluggedIn())
		{
			// display application menu items
			if (WindowApplication* window_application = Application::GetInstance())
				window_application->OnDrawApplicationImGuiMenu(this, begin_menu_func);
			// display the imgui context
			window_imgui_context.OnDrawImGuiMenu(this, begin_menu_func);
			// display menu items for widgets
			OnDrawImGuiObjectOwnerMenu(begin_menu_func);
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
		StartImGuiNewFrame();

		return true;
	}

	void Window::StartImGuiNewFrame()
	{
		window_imgui_context.WithImGuiContext([]()
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		});
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
		window_imgui_context.OnMonitorEvent(monitor, monitor_state);

		if (monitor == fullscreen_monitor && monitor_state == GLFW_DISCONNECTED)
			fullscreen_monitor = nullptr;
		if (window_client != nullptr)
			window_client->OnMonitorEvent(monitor, monitor_state);
	}

	void Window::Finalize()
	{
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
