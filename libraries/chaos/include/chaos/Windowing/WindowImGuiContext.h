namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	class WindowImGuiContext;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API WindowImGuiContext : public InputEventReceiverInterface, public ImGuiInterface
	{
		friend class Window;

	public:

		/** constructor */
		WindowImGuiContext(Window * in_window);
		/** destructor */
		~WindowImGuiContext();

	protected:

		/** check whether the input event should be handled */
		bool ShouldCaptureInputEvent() const;

		/** override */
		virtual bool OnMouseMoveImpl(glm::vec2 const& delta) override;
		/** override */
		virtual bool OnMouseButtonImpl(int button, int action, int modifier) override;
		/** override */
		virtual bool OnMouseWheelImpl(double scroll_x, double scroll_y) override;
		/** override */
		virtual bool OnKeyEventImpl(KeyEvent const& key_event) override;
		/** override */
		virtual bool OnCharEventImpl(unsigned int c) override;

		/** override */
		virtual void OnDrawImGuiMenu(Window* window, BeginImGuiMenuFunc begin_menu_func) override;

		/** called whenever a monitor is connected or disconnected */
		void OnMonitorEvent(GLFWmonitor* monitor, int monitor_state);
		/** called menu is being enabled or disabled */
		void OnImGuiMenuEnabledChanged(bool enabled);

		/** gets the imgui manager */
		ImGuiManager* GetImGuiManager() const;

		/** create the context */
		bool CreateContext();
		/** destroy the context */
		void DestroyContext();

		/** using imgui context, call functor, then restore previous */
		template<typename FUNC>
		decltype(auto) WithImGuiContext(FUNC const& func)
		{
			using L = meta::LambdaInfo<FUNC>;

			// set the ImGui context as current
			ImGuiContext* previous_imgui_context = ImGui::GetCurrentContext();
			ImGuiContext* new_imgui_context = imgui_context;
			ImGui::SetCurrentContext(new_imgui_context);

			// set the ImPlot context as current
			ImPlotContext* previous_implot_context = ImPlot::GetCurrentContext();
			ImPlotContext* new_implot_context = implot_context;
			ImPlot::SetCurrentContext(new_implot_context);

			auto restore_contexts = [=]()
			{
				if (new_implot_context != previous_implot_context) // maybe previous context was same then window's context and window's context has been deleted
					ImPlot::SetCurrentContext(previous_implot_context);

				if (new_imgui_context != previous_imgui_context) // maybe previous context was same then window's context and window's context has been deleted
					ImGui::SetCurrentContext(previous_imgui_context);
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
		}

	protected:

		/** the window owning this context */
		Window * window = nullptr;
		/** the context for ImGui */
		ImGuiContext* imgui_context = nullptr;
		/** the context for ImPlot */
		ImPlotContext* implot_context = nullptr;
	};


	#if 0


	/**
	* Window: a binding class between chaos and GLFW to handle window (beware the prefix "My")
	*/

	class CHAOS_API Window : public Object, public WindowInterface, public ConfigurationUserInterface, public NamedInterface
	{
		friend class WindowApplication;

		CHAOS_DECLARE_OBJECT_CLASS(Window, Object);

	public:

		using CreateImGuiObjectFunc = LightweightFunction<ImGuiObject* ()>;
		using EnumerateKnownImGuiObjectFunc = LightweightFunction<bool(char const*, CreateImGuiObjectFunc)>;

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
				using L = meta::LambdaInfo<FUNC>;

				// save GLFW context
				GLFWwindow* previous_glfw_window = glfwGetCurrentContext();
				glfwMakeContextCurrent(glfw_window);
				// save ImGui context
				ImGuiContext* previous_imgui_context = ImGui::GetCurrentContext();
				ImGuiContext* imgui_context_to_set = imgui_context;
				ImGui::SetCurrentContext(imgui_context_to_set);

				if constexpr (std::is_same_v<void, typename L::result_type>)
				{
					// call delegate
					func();
					// restore GLFW context (if different, because maybe the context has been destroy inside the func() call
					if (previous_glfw_window != glfw_window)
						glfwMakeContextCurrent(previous_glfw_window);
					// restore ImGui context (if different, because maybe the context has been destroy inside the func() call
					if (previous_imgui_context != imgui_context_to_set)
						ImGui::SetCurrentContext(previous_imgui_context);
				}
				else
				{
					// call delegate
					decltype(auto) result = func();
					// restore GLFW context (if different, because maybe the context has been destroy inside the func() call
					if (previous_glfw_window != glfw_window)
						glfwMakeContextCurrent(previous_glfw_window);
					// restore ImGui context (if different, because maybe the context has been destroy inside the func() call
					if (previous_imgui_context != imgui_context_to_set)
						ImGui::SetCurrentContext(previous_imgui_context);
					return result;
				}
			});
		}

		/** gets the ImGui context */
		ImGuiContext* GetImGuiContext() const { return imgui_context; }

		/** create and add a new imgui object */
		template<typename IMGUIOBJECT, typename... PARAMS>
		IMGUIOBJECT* AddNewImGuiObject(char const* title, PARAMS... params)
		{
			static_assert(std::is_base_of_v<ImGuiObject, IMGUIOBJECT>);

			// ensure an object with same name does not already exist
			if (FindImGuiObject(title) != nullptr)
			{
				ImGuiLog::Warning("Window::AddNewImGuiObject(...) failure: an ImGuiObject named [%s] already exists", title);
				return nullptr;
			}

			// create and add the object
			IMGUIOBJECT* result = new IMGUIOBJECT(std::forward<PARAMS>(params)...);
			if (result != nullptr)
			{
				result->SetName(title);
				AddImGuiObject(result);
			}
			return result;
		}

		/** create and add a new imgui popup modal */
		template<typename IMGUIOBJECT, typename... PARAMS>
		IMGUIOBJECT* AddNewImGuiPopupModal(char const* title, PARAMS... params)
		{
			IMGUIOBJECT* result = AddNewImGuiObject<IMGUIOBJECT>(title, std::forward<PARAMS>(params)...);
			if (result != nullptr)
			{
				ImGuiObjectFlags flags = result->GetImGuiObjectFlags(); // ensure the ImGuiObject is marked as a modal popup whilst keeping flags it could naturaly have
				flags |=  ImGuiObjectFlags::PopupModalWindow;
				flags &= ~ImGuiObjectFlags::PopupWindow;
				flags &= ~ImGuiObjectFlags::FloatingWindow;
				flags &= ~ImGuiObjectFlags::FullViewport;
				result->SetImGuiObjectFlags(flags);
			}
			return result;
		}


		/** create and add a new imgui popup */
		template<typename IMGUIOBJECT, typename... PARAMS>
		IMGUIOBJECT* AddNewImGuiPopup(char const* title, PARAMS... params)
		{
			IMGUIOBJECT* result = AddNewImGuiObject<IMGUIOBJECT>(title, std::forward<PARAMS>(params)...);
			if (result != nullptr)
			{
				ImGuiObjectFlags flags = result->GetImGuiObjectFlags(); // ensure the ImGuiObject is marked as a modal popup whilst keeping flags it could naturaly have
				flags &= ~ImGuiObjectFlags::PopupModalWindow;
				flags |=  ImGuiObjectFlags::PopupWindow;
				flags &= ~ImGuiObjectFlags::FloatingWindow;
				flags &= ~ImGuiObjectFlags::FullViewport;
				result->SetImGuiObjectFlags(flags);
				result->SetWindowPlacement(ImGuiWindowPlacement::GetCenterOnCursorPlacement());
			}
			return result;
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
		bool IsApplicationImGuiMenuPluggedIn() const;
		/** enable or disable application imgui menu to be plugged into this window */
		void SetApplicationImGuiMenuPluggedIn(bool enabled);

		/** search whether a given ImGui object exists */
		bool IsKnownImGuiObjectVisible(char const* name) const;
		/** create a ImGuiObject from its name */
		bool SetKnownImGuiObjectVisibility(char const* name, bool visible);

		/** search an ImGui object by name */
		AutoCastable<ImGuiObject> FindImGuiObject(ObjectRequest request);
		/** search an ImGui object by name */
		AutoConstCastable<ImGuiObject> FindImGuiObject(ObjectRequest request) const;

		/** insert an ImGui object into list */
		void AddImGuiObject(ImGuiObject* imgui_object);
		/** remove an ImGui object from list */
		void RemoveImGuiObject(ImGuiObject* imgui_object);

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

	protected:

		/** override */
		virtual bool DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const override;
		/** override */
		virtual bool OnKeyEventImpl(KeyEvent const& key_event) override;
		/** override */
		virtual bool OnMouseMoveImpl(glm::vec2 const& delta) override;
		/** override */
		virtual bool OnMouseButtonImpl(int button, int action, int modifier) override;
		/** override */
		virtual bool OnMouseWheelImpl(double scroll_x, double scroll_y) override;
		/** override */
		virtual bool OnCharEventImpl(unsigned int c) override;
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

		/** set or restore WndProc for ImGui usage */
		void SetImGuiWindowProc(bool set_proc);
		/** a dedicated WndProc for ImGui */
		static LRESULT CALLBACK ImGuiWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		/** special handlers */
		virtual bool HookedWindowProc(UINT msg, WPARAM wParam, LPARAM lParam);
		/** called whenever the language is being changed */
		virtual void OnInputLanguageChanged();

#endif // #if _WIN32

		/** get the destruction guard count */
		int GetWindowDestructionGuard() const { return window_destruction_guard; }

		/** override */
		virtual bool OnStorePersistentProperties(JSONWriteConfiguration config) const override;

		/** enumerate ImGuiObjects that the window can create */
		virtual bool EnumerateKnownImGuiObjects(EnumerateKnownImGuiObjectFunc func) const;
		/** create or destroy an ImGuiObject */
		void SetImGuiObjectInternalVisibility(bool visible, char const* name, CreateImGuiObjectFunc create_func);
		/** draw all ImGui objects */
		void DrawImGuiObjects();

		/** called to destroy the window (window is already unknown from WindowApplication) */
		void DoWindowDestruction();
		/** checks whether the window is inside the application windows array */
		bool IsWindowHandledByApplication() const;

		/** prevent window destruction during inner call */
		template<typename FUNC>
		decltype(auto) PreventWindowDestruction(FUNC func)
		{
			using L = meta::LambdaInfo<FUNC>;
			
			shared_ptr<Window> prevent_destruction = this;
	
			++window_destruction_guard;
			
			if constexpr (L::convertible_to_bool)
			{
				// call functor
				decltype(auto) result = func();
				// check whether window is to be destroyed
				assert(window_destruction_guard > 0);
				if (--window_destruction_guard == 0)
					if (!IsWindowHandledByApplication())
						DoWindowDestruction();
				return result;
			}
			else
			{
				// call functor
				func();
				// check whether window is to be destroyed
				assert(window_destruction_guard > 0);
				if (--window_destruction_guard == 0)
					if (!IsWindowHandledByApplication())
						DoWindowDestruction();
			}
		}

	private:

		/** binding function with GLFW library */
		static void DoOnWindowClosed(GLFWwindow* in_glfw_window);
		/** binding function with GLFW library */
		static void DoOnWindowResize(GLFWwindow* in_glfw_window, int width, int height);
		/** binding function with GLFW library */
		static void DoOnMouseMove(GLFWwindow* in_glfw_window, double x, double y);
		/** binding function with GLFW library */
		static void DoOnMouseButton(GLFWwindow* in_glfw_window, int button, int action, int modifier);
		/** binding function with GLFW library */
		static void DoOnMouseWheel(GLFWwindow* in_glfw_window, double scroll_x, double scroll_y);
		/** binding function with GLFW library */
		static void DoOnKeyEvent(GLFWwindow* in_glfw_window, int keycode, int scancode, int action, int modifier);
		/** binding function with GLFW library */
		static void DoOnCharEvent(GLFWwindow* in_glfw_window, unsigned int c);
		/** binding function with GLFW library */
		static void DoOnDropFile(GLFWwindow* in_glfw_window, int count, char const** paths);
		/** binding function with GLFW library */
		static void DoOnIconifiedStateChange(GLFWwindow* in_glfw_window, int iconified);
		/** binding function with GLFW library */
		static void DoOnFocusStateChange(GLFWwindow* in_glfw_window, int gain_focus);


	};

	#endif


#endif

}; // namespace chaos