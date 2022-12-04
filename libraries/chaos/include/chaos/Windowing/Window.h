namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class WindowCreateParams;
	class NonFullScreenWindowData;
	class Window;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* WindowCreateParams : parameters for playing single window application
	*/

	class CHAOS_API WindowCreateParams
	{
	public:

		/** the title */
		char const* title = nullptr;
		/** the wanted monitor */
		GLFWmonitor* monitor = nullptr;
		/** the monitor index */
		int monitor_index = 0;
		/** window width */
		int width = 0;
		/** window height */
		int height = 0;
		/** true if the window can be resized */
		int resizable = 1;
		/** true if the window starts visible */
		int start_visible = 1;
		/** true if the window has some decoration */
		int decorated = 1;
		/** true if the window is toplevel */
		int toplevel = 0;
		/** self description */
		int focused = 0;
		/** whether we want the fps to be unlimited */
		bool unlimited_fps = false;
		/** number of samples in multisamples (0 for none) */
		int samples = 0;
		/** self description */
		int double_buffer = 1;
		/** self description */
		int depth_bits = 24;
		/** self description */
		int stencil_bits = 8;
	};

	CHAOS_API bool SaveIntoJSON(nlohmann::json& json, WindowCreateParams const& src);

	CHAOS_API bool LoadFromJSON(nlohmann::json const& json, WindowCreateParams& dst);

	/**
	* NonFullScreenWindowData : a binding class between chaos and GLFW to handle window (beware the prefix "My")
	*/

	class NonFullScreenWindowData
	{
	public:

		/** the position of window */
		glm::ivec2 position = { 0, 0 };
		/** the size of window */
		glm::ivec2 size = { 0, -1 };
		/** whether the window is decorated */
		bool decorated = true;
	};

	/**
	* Window : a binding class between chaos and GLFW to handle window (beware the prefix "My")
	*/

	class CHAOS_API Window : public Object, public WindowInterface
	{
		friend class WindowApplication;

		CHAOS_DECLARE_OBJECT_CLASS(Window, Object);

	public:

		/** constructor */
		Window();
		/** destructor */
		virtual ~Window();

		/** called to require the window to close */
		void RequireWindowClosure();
		/** called to require the window to refresh */
		void RequireWindowRefresh();
		/** getter on the handler */
		GLFWwindow* GetGLFWHandler();
		/** returns whether the window has a pending GLFW close message */
		bool ShouldClose();

		/** destroying the window */
		void DestroyGLFWWindow();
		/** create the internal window */
		bool CreateGLFWWindow(WindowCreateParams create_params, GLFWwindow* share_context);

		/** toggle to fullscreen mode */
		void ToggleFullscreen();
		/** returns whether the screen is fullscreen */
		bool IsFullscreen() const { return GetFullscreenMonitor() != nullptr; }

		/** returns the monitor for which the window is fullscreen (nullptr otherwise) */
		GLFWmonitor* GetFullscreenMonitor() const;
		/** returns the monitor where the window is centered */
		GLFWmonitor* GetPreferredMonitor() const;
		/** set window to fullscreen or remove */
		void SetFullscreen(GLFWmonitor* monitor);

		/** returns the position of the window */
		glm::ivec2 GetWindowPosition() const;
		/** returns the size of the window */
		glm::ivec2 GetWindowSize() const;

		/** change the window position */
		void SetWindowPosition(glm::ivec2 const& position);
		/** change the window size */
		void SetWindowSize(glm::ivec2 const& size);

		/** require a screen capture */
		bool ScreenCapture();

		/** getting the required viewport for given window */
		virtual WidgetPlacement GetRequiredViewport(glm::ivec2 const& size) const;

		/** getting the renderer */
		GPURenderer* GetRenderer() { return renderer.get(); }

		/** get the root widget */
		WindowRootWidget* GetRootWidget() { return root_widget.get(); }
		/** get the root widget */
		WindowRootWidget const* GetRootWidget() const { return root_widget.get(); }

		/** update the whole widget hierarchy placement */
		virtual void UpdateWidgetPlacementHierarchy();

		/** using window context, call functor, then restore previous */
		template<typename T>
		T WithGLContext(std::function<T()> const & func)
		{
			GLFWwindow* previous_context = glfwGetCurrentContext();
			glfwMakeContextCurrent(glfw_window);
			T result = func();
			glfwMakeContextCurrent(previous_context);
			return result;
		}

		template<>
		void WithGLContext(std::function<void()> const & func)
		{
			GLFWwindow* previous_context = glfwGetCurrentContext();
			glfwMakeContextCurrent(glfw_window);
			func();
			glfwMakeContextCurrent(previous_context);
		}

	protected:

		/** override */
		virtual bool DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const override;
		/** override */
		virtual bool OnKeyEventImpl(KeyEvent const& event) override;
		/** override */
		virtual bool DoTick(float delta_time) override;

		/** display both the window content and the widget overlay */
		virtual bool OnDrawInternal(GPURenderer* renderer, WindowDrawParams const& draw_params, GPUProgramProviderInterface const* uniform_provider);

		/** bind Window with GLFW */
		virtual void SetGLFWCallbacks(bool in_double_buffer);

		/** called whenever the window is redrawn (entry point) */
		virtual void OnWindowDraw();

		/** called at window creation (returns false if the window must be killed) */
		virtual bool InitializeFromConfiguration(nlohmann::json const& config);
		/** called at window destruction */
		virtual void Finalize() { }

		/** get the mouse position */
		glm::vec2 GetMousePosition() const;
		/** returns true if the mouse position is valid (very first frame) */
		bool IsMousePositionValid() const;

		/** tick the renderer of the window with the real framerate (with no time scale) */
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

	private:

		/** binding function with GLFW library */
		static void DoOnWindowClosed(GLFWwindow* in_glfw_window);
		/** binding function with GLFW library */
		static void DoOnWindowResize(GLFWwindow* in_glfw_window, int width, int height);
		/** binding function with GLFW library */
		static void DoOnDraw(GLFWwindow* in_glfw_window);
		/** binding function with GLFW library */
		static void DoOnMouseMove(GLFWwindow* in_glfw_window, double x, double y);
		/** binding function with GLFW library */
		static void DoOnMouseButton(GLFWwindow* in_glfw_window, int button, int action, int modifier);
		/** binding function with GLFW library */
		static void DoOnMouseWheel(GLFWwindow* in_glfw_window, double scroll_x, double scroll_y);
		/** binding function with GLFW library */
		static void DoOnKeyEvent(GLFWwindow* in_glfw_window, int key, int scan_code, int action, int modifier);
		/** binding function with GLFW library */
		static void DoOnCharEvent(GLFWwindow* in_glfw_window, unsigned int c);
		/** binding function with GLFW library */
		static void DoOnDropFile(GLFWwindow* in_glfw_window, int count, char const** paths);
		/** binding function with GLFW library */
		static void DoOnIconifiedStateChange(GLFWwindow* in_glfw_window, int iconified);
		/** binding function with GLFW library */
		static void DoOnFocusStateChange(GLFWwindow* in_glfw_window, int gain_focus);

	protected:

		/** the window in GLFW library */
		GLFWwindow* glfw_window = nullptr;
		/** is the window with double buffer */
		bool double_buffer = true;

		/** the root widget for the window */
		shared_ptr<WindowRootWidget> root_widget;

		/** the renderer */
		shared_ptr<GPURenderer> renderer;

		/** previous mouse position */
		glm::vec2 mouse_position = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };

		/** used to store data when toggling fullscreen */
		std::optional<NonFullScreenWindowData> non_fullscreen_data;
	};

#endif

}; // namespace chaos