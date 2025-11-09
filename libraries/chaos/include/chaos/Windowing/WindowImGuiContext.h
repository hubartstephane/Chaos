namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	class WindowImGuiContext;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API WindowImGuiContext : public InputReceiverInterface, public ImGuiInterface
	{
		friend class Window;

	public:

		/** constructor */
		WindowImGuiContext(Window * in_window);
		/** destructor */
		~WindowImGuiContext();

		/** check whether the context is current */
		bool IsImGuiContextCurrent() const;

		/** override */
		virtual bool EnumerateInputActions(InputActionEnumerator& in_action_enumerator, EnumerateInputActionContext in_context) override;
		/** override */
		virtual char const* GetInputReceiverName() const override;

	protected:
	
		/** called whenever the menu becomes enabled or disabled */
		void OnImGuiMenuEnabledChanged(bool enabled);
		/** initialize imgui context flags */
		void InitializeImGuiContextFlags(ImGuiManager* imgui_manager);
		/** set mouse flags for ImGui */
		void SetImGuiContextMouseFlag(bool enabled);

		/** check whether the input event should be handled */
		bool ShouldCaptureInputEvent() const;

		/** override */
		virtual bool OnMouseMoveImpl(glm::vec2 const& delta) override;
		/** override */
		virtual bool OnMouseButtonImpl(MouseButtonEvent const &mouse_button_event) override;
		/** override */
		virtual bool OnMouseWheelImpl(double scroll_x, double scroll_y) override;
		/** override */
		virtual bool OnKeyEventImpl(KeyEvent const& key_event) override;
		/** override */
		virtual bool OnCharEventImpl(unsigned int c) override;

		/** override */
		virtual void OnDrawImGuiMenu(Window* window, ImGuiMenuBuilder const & menu_builder) override;

		/** called whenever a monitor is connected or disconnected */
		void OnMonitorEvent(GLFWmonitor* monitor, int monitor_state);

		/** create the context */
		bool CreateContext(ImGuiManager* imgui_manager);
		/** destroy the context */
		void DestroyContext(ImGuiManager* imgui_manager);

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

#endif

}; // namespace chaos