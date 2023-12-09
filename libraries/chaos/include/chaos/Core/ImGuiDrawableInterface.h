namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiDrawableInterface;
	class ImGuiDrawableObject;

	enum class ImGuiDrawMenuMode;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	enum class ImGuiDrawMenuMode : int
	{
		FullWindow,
		ImGuiWindow
	};

	class CHAOS_API ImGuiDrawableInterface
	{
	public:

		/** destructor */
		virtual ~ImGuiDrawableInterface() = default;
		/** draw both ImGui and Menu */
		virtual void DrawImGui(ImGuiDrawMenuMode menu_mode);

		/** conditionally detect whether a flag for window menu is necessary */
		static int GetConditionalMainMenuFlag(ImGuiDrawMenuMode menu_mode);

		/** start a fullscreen window */
		template<typename FUNC>
		static void FullscreenWindow(char const* name, bool menu_bar, FUNC const & content_func)
		{
			int window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground;
			if (menu_bar)
				window_flags |= GetConditionalMainMenuFlag(ImGuiDrawMenuMode::ImGuiWindow);

			ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x, ImGui::GetMainViewport()->WorkPos.y });
			ImGui::SetNextWindowSize({ ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y });
			if (ImGui::Begin(name, nullptr, window_flags))
			{
				content_func();
				ImGui::End();
			}
		}

		/** start a menu */
		template<typename FUNC>
		static bool MenuBar(ImGuiDrawMenuMode menu_mode, FUNC const& func)
		{
			bool show_menu = true;
			if (WindowApplication* window_application = Application::GetInstance())
				show_menu = window_application->GetImGuiMenuMode();

			if (show_menu)
			{
				switch (menu_mode)
				{
				case ImGuiDrawMenuMode::FullWindow:
					if (ImGui::BeginMainMenuBar())
					{
						func();
						ImGui::EndMainMenuBar();
						return true;
					}
					break;
				case ImGuiDrawMenuMode::ImGuiWindow:
					if (ImGui::BeginMenuBar())
					{
						func();
						ImGui::EndMenuBar();
						return true;
					}
					break;
				default:
					assert(0);
					break;
				}
			}
			return false;
		}

		/** request to close this interface */
		void RequestClosing()
		{
			closing_request = true;
		}
		/** check whether closing is requested */
		bool IsClosingRequested() const
		{
			return closing_request;
		}
	
	protected:

		/** indicates whether closing this UI is requested */
		bool closing_request = false;
	};

	class CHAOS_API ImGuiDrawableObject : public Object, public ImGuiDrawableInterface
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiDrawableObject, Object);
	};

#endif

}; // namespace chaos