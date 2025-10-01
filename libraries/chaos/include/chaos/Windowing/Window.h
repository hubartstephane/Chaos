namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	class NonFullScreenWindowData;
	class Window;

	enum class CursorMode;
	enum class WindowCategory;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * some aliases
	 */
	 
	using DoDispatchInputEventFunction = LightweightFunction<bool(InputReceiverInterface*)>;

	/**
	* CursorMode
	*/

	enum class CHAOS_API CursorMode : int
	{
		NORMAL = GLFW_CURSOR_NORMAL,
		DISABLED = GLFW_CURSOR_DISABLED,
		HIDDEN = GLFW_CURSOR_HIDDEN
	};

	/**
	* NonFullScreenWindowData : a binding class between chaos and GLFW to handle window (beware the prefix "My")
	*/

	class NonFullScreenWindowData
	{
	public:

		/** the position of window */
		glm::ivec2 position = { 0, 0 };
		/** the size of window */
		glm::ivec2 size = { 0, 0 };
	};

	/**
	* WindowCategory: Indicate window closing behaviour relative to others (windows)
	*/

	enum class CHAOS_API WindowCategory
	{
		MAIN_WINDOW, // once last main window has been close, all other windows should be closed and application quit
		OPTIONAL_WINDOW
	};

	/**
	* Window: a binding class between chaos and GLFW to handle window (beware the prefix "My")
	*/

	class CHAOS_API Window : public Object, public WindowInterface, public ConfigurationUserInterface, public NamedInterface, public ImGuiObjectOwnerInterface
	{
		friend class WindowApplication;

		CHAOS_DECLARE_OBJECT_CLASS(Window, Object);

	public:

		/** constructor */
		Window();
		/** destructor */
		virtual ~Window();

		/** Get the GPUDevice */
		GPUDevice * GetGPUDevice() const;

		/** require the destruction of the window (at least its GLFW resource). this will remove it from the WindowApplication */
		void Destroy();

		/** called to require the window to close */
		void RequireWindowClosure();
		/** getter on the handler */
		GLFWwindow* GetGLFWHandler() const;
		/** returns whether the window has a pending GLFW close message */
		bool ShouldClose();

		/** toggle to fullscreen mode */
		void ToggleFullscreen();
		/** returns whether the screen is fullscreen */
		bool IsFullscreen() const { return fullscreen_monitor != nullptr; }

		/** returns the monitor where the window is centered */
		GLFWmonitor* GetPreferredMonitor() const;
		/** set window to fullscreen or remove */
		void SetFullscreen(GLFWmonitor* monitor);

		/** returns the position of the window */
		glm::ivec2 GetWindowPosition(bool include_decorators = false) const;
		/** returns the size of the window */
		glm::ivec2 GetWindowSize(bool include_decorators = false) const;

		/** change the window position */
		void SetWindowPosition(glm::ivec2 position, bool include_decorators);
		/** change the window size */
		void SetWindowSize(glm::ivec2 size, bool include_decorators);
		/** change the placement of the window */
		void SetWindowPlacement(WindowPlacementInfo placement_info);

		/** show or hide the window */
		void ShowWindow(bool visible);
		/** get visibility status */
		bool IsWindowVisible() const;

		/** require a screen capture */
		bool ScreenCapture();

		/** getting the required viewport for given window */
		virtual aabox2 GetRequiredViewport(glm::ivec2 const& size) const;

		/** getting the render_context */
		GPURenderContext* GetGPURenderContext() const { return render_context.get(); }

		/** get the root widget */
		WindowRootWidget* GetRootWidget() { return root_widget.get(); }
		/** get the root widget */
		WindowRootWidget const* GetRootWidget() const { return root_widget.get(); }

		/** update the whole widget hierarchy placement */
		virtual void UpdateWidgetPlacementHierarchy();

		/** using window context, call functor, then restore previous */
		template<typename FUNC>
		decltype(auto) WithWindowContext(FUNC const& func)
		{
			return PreventWindowDestruction([this, &func]()
			{
				return window_imgui_context.WithImGuiContext([this, &func]()
				{
					using L = meta::LambdaInfo<FUNC>;

					// set the GLFW context as current
					GLFWwindow* previous_glfw_window = glfwGetCurrentContext();
					GLFWwindow* new_glfw_window = glfw_window;
					glfwMakeContextCurrent(glfw_window);

					auto restore_contexts = [=]()
					{
						if (new_glfw_window != previous_glfw_window) // maybe previous context was same then window's context and window's context has been deleted
							glfwMakeContextCurrent(previous_glfw_window);
					};

					if constexpr (std::is_same_v<void, typename L::result_type>)
					{
						func();
						restore_contexts();
					}
					else
					{
						decltype(auto) result = func();
						restore_contexts();
						return result;
					}
				});
			});
		}

		/** change the cursor mode */
		void SetCursorMode(CursorMode mode);
		/** get the cursor mode */
		CursorMode GetCursorMode() const;
		/** get the final cursor mode (taking user conditions into consideration) */
		virtual CursorMode GetEffectiveCursorMode() const;
		/** update cursor mode, GLFW side */
		void UpdateCursorMode();

		/** gets the category of the window */
		WindowCategory GetWindowCategory() const { return window_category; };
		/** gets the category of the window */
		void SetWindowCategory(WindowCategory in_window_category){ window_category = in_window_category; };

		/** returns whether window wants application imgui menu to be plugged into its menu */
		bool IsStandardImGuiMenuPluggedIn() const;
		/** enable or disable application imgui menu to be plugged into this window */
		void SetStandardImGuiMenuPluggedIn(bool enabled);

		/** gets the imgui manager */
		ImGuiManager* GetImGuiManager() const;

		/** draw ImGui */
		virtual void DrawImGui();
		/** draw ImGui content */
		virtual void OnDrawImGuiContent();
		/** draw ImGui menu */
		virtual void OnDrawImGuiMenu(BeginImGuiMenuFunc begin_menu_func);

		/** get the owned client */
		AutoCastable<WindowClient> GetWindowClient() { return window_client.get(); }
		/** get the owning window */
		AutoConstCastable<WindowClient> GetWindowClient() const { return window_client.get(); }

		/** set the client */
		void SetWindowClient(WindowClient * in_client);

		/** override */
		virtual bool TraverseInputReceiver(InputReceiverTraverser & in_traverser, InputDeviceInterface const* in_input_device) override;
		/** override */
		virtual bool EnumerateInputActions(InputActionEnumerator & in_action_enumerator, EnumerateInputActionContext in_context) override;

		/** special input receiver traversal for event dispatching (handle imgui and application) */
		bool TraverseInputReceiverFull(InputReceiverTraverser& in_traverser, InputDeviceInterface const * in_input_device = KeyboardAndMouseState::GetInstance());

	protected:

		/** override */
		virtual bool DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const override;
		
		/** override */
		virtual bool OnDraw(GPURenderContext* render_context, GPUProgramProviderInterface const* uniform_provider, WindowDrawParams const& draw_params) override;
		/** override */
		virtual bool DoTick(float delta_time) override;

		/** create the root widget */
		virtual bool CreateRootWidget();

		/** display both the window content and the widget overlay */
		virtual bool DrawInternal(GPUProgramProviderInterface const* uniform_provider);

		/** bind Window with GLFW */
		virtual void SetGLFWCallbacks();

		/** create a render context */
		bool CreateRenderContext(GPUDevice * in_gpu_device);
		/** destroy the context */
		void DestroyRenderContext();

		/** create an ImGui context */
		bool CreateImGuiContext();
		/** destroying the ImGui context */
		void DestroyImGuiContext();
		/** start rendering into imgui */
		void StartImGuiNewFrame();

		/** update the placement info according to information stored in window config */
		void UpdatePlacementInfoAccordingToConfig(WindowPlacementInfo & placement_info) const;

		/** create the internal window */
		bool CreateGLFWWindow(GPUDevice* in_gpu_device, WindowPlacementInfo placement_info, WindowCreateParams const &create_params, GLFWwindow* share_context, GLFWHints const & glfw_hints);
		/** destroying the window */
		void DestroyGLFWWindow();

		/** called menu is being enabled or disabled */
		void OnImGuiMenuEnabledChanged(bool enabled);

		/** called whenever the window is redrawn (entry point) */
		virtual void DrawWindow();

		/** get a provider and process the function */
		virtual bool GetProgramProviderAndProcess(LightweightFunction<bool(GPUProgramProviderBase*)> func);

		/** called at window creation */
		virtual bool Initialize();
		/** called at window destruction */
		virtual void Finalize();
		/** called at window creation (returns false if the window must be killed) */
		virtual bool OnInitialize(JSONReadConfiguration config);

		/** get the mouse position */
		glm::vec2 GetMousePosition() const;
		/** returns true if the mouse position is valid (very first frame) */
		bool IsMousePositionValid() const;

		/** tick the render_context of the window with the real framerate (with no time scale) */
		void TickRenderer(float real_delta_time);

		/** called whenever the window is resized */
		virtual void OnWindowResize(glm::ivec2 size);
		/** called whenever the user try to close window */
		virtual bool OnWindowClosed();
		/** called whenever a file is dropped */
		virtual void OnDropFile(int count, char const** paths);
		/** called whenever the window becomes iconified or is restored */
		virtual void OnIconifiedStateChange(bool iconified);
		/** called whenever the window gain or loose focus */
		virtual void OnFocusStateChange(bool gain_focus);
		/** called whenever a monitor is connected or disconnected */
		virtual void OnMonitorEvent(GLFWmonitor* monitor, int monitor_state);

#if _WIN32

		/** plug new WindowProc */
		void UpdateWindowProc(bool set_proc);
		/** a dedicated WndProc  */
		static LRESULT CALLBACK InternalWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		/** special handlers */
		virtual bool HookedWindowProc(UINT msg, WPARAM wParam, LPARAM lParam);
		/** called whenever the language is being changed */
		virtual void OnInputLanguageChanged();

#endif // #if _WIN32

		/** get the destruction guard count */
		int GetWindowDestructionGuard() const { return window_destruction_guard; }

		/** override */
		virtual bool OnStorePersistentProperties(JSONWriteConfiguration config) const override;

		/** called to destroy the window (window is already unknown from WindowApplication) */
		void CompleteWindowDestruction();
		/** checks whether the window is inside the application windows array */
		bool IsWindowHandledByApplication() const;

		/** get the window from GLFW context */
		static Window * GetWindowFromGLFWContext(GLFWwindow* in_glfw_window);

		/** get Window from a GLFW context, set appropriate IMGUI/GLFW context and call callback */
		template<typename FUNC>
		static decltype(auto) GetWindowAndProcessWithContext(GLFWwindow* in_glfw_window, FUNC const & func)
		{
			using L = meta::LambdaInfo<FUNC, Window *>;

			if (Window* my_window = GetWindowFromGLFWContext(in_glfw_window))
			{
				return my_window->WithWindowContext([my_window, &func]()
				{
					return func(my_window);
				});
			}

			if constexpr (L::convertible_to_bool)
				return typename L::result_type {};
		}

		/** prevent window destruction during inner call */
		template<typename FUNC>
		decltype(auto) PreventWindowDestruction(FUNC const & func)
		{
			using L = meta::LambdaInfo<FUNC>;
			
			auto release_destruction_guard = [this]()
			{
				assert(window_destruction_guard > 0);
				if (--window_destruction_guard == 0)
					if (!IsWindowHandledByApplication())
						CompleteWindowDestruction();
			};

			shared_ptr<Window> prevent_destruction = this;

			++window_destruction_guard;
			
			if constexpr (std::is_same_v<void, typename L::result_type>)
			{
				func();
				release_destruction_guard();
			}
			else
			{
				decltype(auto) result = func();
				release_destruction_guard();
				return result;
			}
		}

		/** dispatch an input event through the ImGui/WindowClient/Window/Application hierarchy */
		template<typename FUNC, typename ...PARAMS>
		bool DispatchInputEventWithContext(FUNC const & func, PARAMS&& ...params)
		{
			auto process_function = [&](InputReceiverInterface * in_input_receiver, InputDeviceInterface const* in_input_device)  // XXX: mandatory to have a VARIABLE lambda so that the underlying DelegateTraverser's LightweightFunction does not point on a deleted object
			{
				return (in_input_receiver->*func)(std::forward<PARAMS>(params)...);
			};

			return WithWindowContext([this, &process_function]()
			{
				DelegateInputReceiverTraverser traverser(process_function);
				return TraverseInputReceiverFull(traverser);
			});
		}

		/** gets the window imgui context */
		WindowImGuiContext * GetWindowImGuiContext() { return &window_imgui_context;}

		/** returns whether the window has the focus */
		bool HasFocus() const;

	private:

		/** binding function with GLFW library */
		static void DoOnWindowClosed(GLFWwindow* in_glfw_window);
		/** binding function with GLFW library */
		static void DoOnWindowResize(GLFWwindow* in_glfw_window, int width, int height);
		/** binding function with GLFW library */
		static void DoOnMouseMove(GLFWwindow* in_glfw_window, double x, double y);
		/** binding function with GLFW library */
		static void DoOnMouseButton(GLFWwindow* in_glfw_window, int button, int action, int modifiers);
		/** binding function with GLFW library */
		static void DoOnMouseWheel(GLFWwindow* in_glfw_window, double scroll_x, double scroll_y);
		/** binding function with GLFW library */
		static void DoOnKeyEvent(GLFWwindow* in_glfw_window, int keycode, int scancode, int action, int modifiers);
		/** binding function with GLFW library */
		static void DoOnCharEvent(GLFWwindow* in_glfw_window, unsigned int c);
		/** binding function with GLFW library */
		static void DoOnDropFile(GLFWwindow* in_glfw_window, int count, char const** paths);
		/** binding function with GLFW library */
		static void DoOnIconifiedStateChange(GLFWwindow* in_glfw_window, int iconified);
		/** binding function with GLFW library */
		static void DoOnFocusStateChange(GLFWwindow* in_glfw_window, int gain_focus);

	protected:

		/** the ImGui/Implot context */
		WindowImGuiContext window_imgui_context;

		/** the window in GLFW library */
		GLFWwindow* glfw_window = nullptr;
		/** is the window with double buffer */
		bool double_buffer = true;
		/** whether the window should be toplevel when non-fullscreen */
		bool initial_toplevel = false;
		/** whether the window should be decorated when non-fullscreen */
		bool initial_decorated = false;
		/** the root widget for the window */
		shared_ptr<WindowRootWidget> root_widget;
		/** the render_context */
		shared_ptr<GPURenderContext> render_context;
		/** previous mouse position */
		std::optional<glm::vec2> mouse_position;
		/** used to store data when toggling fullscreen */
		std::optional<NonFullScreenWindowData> non_fullscreen_data;
		/** if the window is fullscreen, this points to the concerned monitor */
		GLFWmonitor* fullscreen_monitor = nullptr;
		/** the current cursor mode */
		CursorMode cursor_mode = CursorMode::NORMAL;
		/** a counter that prevent destruction of the window resources */
		int window_destruction_guard = 0;
		/** the window category */
		WindowCategory window_category = WindowCategory::MAIN_WINDOW;
		/** whether application is enabled to be plugged into the window */
		bool standard_imgui_menu_plugged_in = true;

		/** the window client */
		shared_ptr<WindowClient> window_client;
	};

#endif

}; // namespace chaos